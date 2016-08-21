/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "nchart.h"
#include "naudioloader.h"
#include <txaxis.h>
#include <tartini/notedata.h>
#include <tartini/channel.h>
#include <tpitchfinder.h>
#include <music/tnote.h>
#include <music/tnotestruct.h>
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <score/tscorescene.h>
#include <score/tscorestaff.h>
#include <score/tscoreclef.h>
#include <graphics/tgraphicstexttip.h>
#include <tscoreparams.h>
#include <tcolor.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QScrollBar>
#include <QtCore/QDebug>
#include <QTimer>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #include <unistd.h>
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif

QColor minVolumeColor = QColor(0, 0, 255);
QColor noteVolumeColor = QColor(120, 255, 0);
QColor pcmVolumeColor = Qt::magenta;
QColor perfectPitchColor = Qt::darkGreen;
QColor averPitchColor = Qt::gray;
QColor bestPitchColor = QColor(103, 0, 103);
QColor chunkPitchColor = Qt::cyan;
QColor emptyColor = Qt::darkGray;
QColor minDurColor = Qt::blue;


QString yOrN(bool yn) {
  return yn ? "<b>yes</b>" : "<b>no</b>";
}


Nchart::Nchart(QWidget* parent) :
  Tchart(parent),
  m_pitchF(0),
  m_totalXnr(0),
  m_pitchGrad(0.5, 0, 0.5, 1),
  xSc(4), hSc(2),
  m_pass(-1),
  m_currChunk(0), m_prevChunk(0),
  m_drawnPrev(-1)
{
  yAxis->hide();
  xAxis->hide();
  yAxis->setLength(400);
  yAxis->setMaxValue(140, false);

  m_progresItem = new TgraphicsTextTip("", palette().highlight().color());
  scene->addItem(m_progresItem);
  m_progresItem->setZValue(250);
  m_progresItem->setScale(2);
  m_progresItem->hide();
  m_progresItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

  m_pitchGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
  const int al = 200;
  m_pitchGrad.setColorAt(0, QColor(117, 21, 86, al));
  m_pitchGrad.setColorAt(0.1, QColor(255, 0, 0, al));
  m_pitchGrad.setColorAt(0.3, QColor(255, 255, 0, al));
  m_pitchGrad.setColorAt(0.5, QColor(0, 255, 0, al));
//   m_pitchGrad.setColorAt(0.55, QColor(0, 255, 0, al));
  m_pitchGrad.setColorAt(0.7, QColor(255, 255, 0, al));
  m_pitchGrad.setColorAt(0.9, QColor(255, 0, 0, al));
  m_pitchGrad.setColorAt(1.0, QColor(117, 21, 86, al));

  m_staff = new TscoreStaff(scene, 0);
  m_staff->setScale((yMap(0) - yMap(60)) / m_staff->height());
  m_staff->setViewWidth(400 / m_staff->scale());
  m_staff->setPos(22, yMap(50));
  m_staff->onClefChanged(Tcore::gl()->S->clef);
  connect(m_staff, &TscoreStaff::clefChanged, this, &Nchart::clefChanged);
  QTimer::singleShot(1, this, SLOT(adjustHeight()));
}


Nchart::~Nchart()
{
}


void Nchart::setAudioLoader(NaudioLoader* loader) {
  m_pass++;
  m_loader = loader;
  m_pitchF = loader->finder();
  m_chunkNr = -1;
  connect(m_loader, &NaudioLoader::chunkReady, this, &Nchart::drawChunk, Qt::DirectConnection);
  connect(m_loader, &NaudioLoader::processingFinished, this, &Nchart::allDataLoaded, Qt::DirectConnection);
  connect(m_pitchF, &TpitchFinder::noteStarted, this, &Nchart::noteStartedSlot, Qt::DirectConnection);
  connect(m_loader, &NaudioLoader::noteFinished, this, &Nchart::copyChunk, Qt::DirectConnection);
  m_progresItem->show();
  m_prevNoteIndex = 0;
  m_prevChunk = 0;
  m_ad = 0;
  m_pad = 0;
  horizontalScrollBar()->setValue(0);

  if (m_pass) { // another processing on existing chart
    TYaxis *oldYaxis = yAxis;
    yAxis = new TYaxis;
    scene->addItem(yAxis);
    yAxis->setLength(400);
    yAxis->setMaxValue(140, false);
    yAxis->setZValue(55);
    yAxis->setPos(45, oldYaxis->y() + oldYaxis->length() + 5 - (m_drawVolume ? 0 : 130));
    yAxis->hide();

    TscoreStaff *oldStaff = m_staff;
    m_staff = new TscoreStaff(scene, 0);
    m_staff->setScale(oldStaff->scale());
    m_staff->setPos(oldStaff->x(), yMap(50));
    m_staff->onClefChanged(oldStaff->scoreClef()->clef());
  }
  setXnumber(m_loader->totalChunks() + 1);
  m_staff->setDisabled(true);
  QTimer::singleShot(250, this, SLOT(progressSlot()));
}


void Nchart::setXnumber(int xN) {
  m_xLine = new QGraphicsLineItem;
  scene->addItem(m_xLine);
  m_xLine->setPen(QPen(palette().text().color(), 2));
  m_xLine->setZValue(2);
  m_xLine->setLine(52, yAxis->y() + yAxis->boundingRect().height(), (xN + 15) * xSc, yAxis->y() + yAxis->boundingRect().height());
  m_totalXnr = xN;
  m_staff->setViewWidth(m_xLine->line().length() / m_staff->scale());
  qreal secondStep = xSc * (qreal)m_pitchF->aGl()->rate / (qreal)m_pitchF->aGl()->framesPerChunk;
  for (int i = 0; i <= m_xLine->line().length() / secondStep; ++i) {
    scene->addLine(QLineF(52 + i * secondStep, yMap(0), 52 + i * secondStep, yMap(0) + 3), m_xLine->pen());
    QGraphicsTextItem *ticText = scene->addText(QString("%1").arg(i));
    ticText->setPos(52 + i * secondStep - ticText->boundingRect().width() / 2, yMap(0) + 3);
    QGraphicsLineItem *l = scene->addLine(52 + i * secondStep, yMap(1), 52 + i * secondStep, yMap(m_drawVolume ? 152 : 102),
                   QPen(Qt::lightGray, 1, Qt::DashLine));
    l->setZValue(1);
  }
  for (int i = 1; i <= m_xLine->line().length() / xSc; ++i)
    scene->addLine(QLineF(52 + i * xSc, yMap(0), 52 + i * xSc, yMap(0) - 2),
                   QPen(palette().text().color(), 0.5));
  QGraphicsTextItem *timeText = scene->addText(tr("time [s]") + "  ");
    timeText->setPos(52  + m_xLine->line().length(), yMap(0) + 3);
// VOLUME
  if (m_drawVolume) {
    scene->addLine(xMap(1), yMap(101 + Tcore::gl()->A->minimalVol * 50.0), // minimum volume line
                  xMap(m_totalXnr), yMap(101 + Tcore::gl()->A->minimalVol * 50.0), QPen(minVolumeColor, 1));
    QGraphicsTextItem *minVolText = scene->addText(tr("min volume"));
      minVolText->setDefaultTextColor(minVolumeColor);
      minVolText->setPos(xMap(1) - minVolText->boundingRect().width(),
                      yMap(101 + Tcore::gl()->A->minimalVol * 50.0) - minVolText->boundingRect().height() / 2);
    QGraphicsTextItem *noteVolText = scene->addText((tr("note volume")));
      noteVolText->setDefaultTextColor(noteVolumeColor);
      noteVolText->setPos(xMap(1) - minVolText->boundingRect().width(), yMap(150));
    scene->addLine(xMap(1), yMap(101), xMap(m_totalXnr) , yMap(101), QPen(palette().text().color(), 1)); // X axis of volume chart
    QGraphicsTextItem *pcmSigText = scene->addText(tr("PCM volume"));
      pcmSigText->setDefaultTextColor(pcmVolumeColor);
      pcmSigText->setPos(xMap(1) - pcmSigText->boundingRect().width(), yMap(100) - pcmSigText->boundingRect().height());
  }
// PITCH
  QGraphicsRectItem *bgPitch = scene->addRect(xMap(1), yMap(100), (xN - 5) * xSc, yMap(50) - yMap(100),
                 Qt::NoPen, QBrush(m_pitchGrad));
    bgPitch->setZValue(5);
// perfect pitch line in the middle of a rectangle
  QGraphicsLineItem *pitchLine = scene->addLine(xMap(1), yMap(75), xMap(xN - 4), yMap(75), QPen(perfectPitchColor, 1));
    pitchLine->setZValue(12);

  QGraphicsTextItem *pitchTexts = new QGraphicsTextItem();
    scene->addItem(pitchTexts);
    pitchTexts->setHtml("<span style=\"color: " + chunkPitchColor.name() + "\";>" + tr("chunk pitch") + "</span><br>" +
                        "<span style=\"color: " + perfectPitchColor.name() + "\";>" + tr("perfect pitch") + "</span><br>" +
                        "<span style=\"color: " + bestPitchColor.name() + "\";>" + tr("closest pitch") + "</span><br>" +
                        "<span style=\"color: " + averPitchColor.name() + "\";>" + tr("average pitch") + "</span>");
    pitchTexts->setPos(xMap(0) - pitchTexts->boundingRect().width(), yMap(75) - pitchTexts->boundingRect().height() / 2);
  QGraphicsTextItem *settingsTexts = new QGraphicsTextItem();
    scene->addItem(settingsTexts);
    QString pdm = "MPM";
    if (Tcore::gl()->A->detectMethod == 1)
      pdm = "autocorelation";
    else if (Tcore::gl()->A->detectMethod == 2)
      pdm = "mod. cepstrum";
    QString rangeText = "middle (1024)";
    if (NaudioLoader::pitchRange() == TpitchFinder::e_high)
      rangeText = "high (512)";
    else if (NaudioLoader::pitchRange() == TpitchFinder::e_low)
      rangeText = "low (2048)";
    settingsTexts->setHtml(tr("method") + ": <b>" + pdm +"</b><br>" +
      tr("filter") + " :" + yOrN(Tcore::gl()->A->equalLoudness) + "<br>" +
      QString("noise-floor: <b>%1</b>").arg(yOrN(m_pitchF->aGl()->doingAutoNoiseFloor)) + "<br>" +
      QString("pitch range: <b>%1</b>").arg(rangeText) + "<br>" +
      tr("threshold") + QString(": <b>%1 %</b>").arg(m_pitchF->aGl()->threshold) + "<br>" +
      "<span style=\"color: " + noteVolumeColor.name() + "\";>" + QString("split vol.: <b>%1</b>").
          arg((m_pitchF->isSplitByVolume() ? QString("%1%").arg(Tcore::gl()->A->minSplitVol) : "no")) + "</span><br>" +
      "<span style=\"color: " + noteVolumeColor.name() + "\";>" + QString("skip stiller: <b>%1</b>").
          arg((m_pitchF->skipStillerValue() > 0.0 ? QString("%1%").arg(Tcore::gl()->A->skipStillerVal) : "no")) + "</span><br>" +
      QString("dbFoor: <b>%1</b>").arg(m_pitchF->aGl()->dBFloor) + "<br>" +
      QString("middle A: <b>%1 [Hz]</b>").arg(pitch2freq(freq2pitch(440) + Tcore::gl()->A->a440diff)) + "<br>" +
      "<span style=\"color: " + minVolumeColor.name() + "\";>" + QString("min. volume: <b>%1</b>").arg(Tcore::gl()->A->minimalVol * 100) + "</span><br>"
      "<span style=\"color: " + minDurColor.name() + "\";>" + QString("min. duration: <b>%1 [ms]</b>").arg(Tcore::gl()->A->minDuration * 1000) + "</span>" +
      QString("<br>chunk time: <b>%1 [ms]</b>").arg(m_pitchF->chunkTime() * 1000));
    settingsTexts->setPos(xMap(xN - 2), yMap(110));
}


void Nchart::allDataLoaded() {
  m_progresItem->hide();
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void Nchart::copyChunk(TnoteStruct* ad) {
  for (int i = m_prevChunk; i < ad->startChunk; ++i)
      emptyRect(i, xSc);
  drawNoteSegment(ad->startChunk, ad->endChunk, ad);
  m_prevChunk = ad->endChunk + 1;
}


void Nchart::drawChunk() {
  if (m_pitchF->currentChunk() < 1)
    return;

  int c = m_pitchF->currentChunk() - 1;
  m_pad = m_ad;
  m_ad = m_pitchF->ch()->dataAtChunk(c);

//   qDebug() << m_ad->noteIndex << m_ad->pitch << "ncs:" << m_ad->noteChangeScore() << "done" << m_ad->done << "pl:" << m_ad->notePlaying << m_ad->period;

  if (m_drawVolume && m_ad && m_pad)
      scene->addLine(xMap(c - 1) + hSc, yMap(101 + m_pad->pcmVolume * 50.0), // PCM signal volume line
                   xMap(c) + hSc, yMap(101 + m_ad->pcmVolume * 50.0), QPen(pcmVolumeColor, 1, Qt::SolidLine, Qt::RoundCap));

  if (m_ad) {
      if (m_drawVolume) {
        if (m_ad->normalVolume() && m_pad && m_pad->normalVolume()) {
          QGraphicsLineItem *volLine = new QGraphicsLineItem(); // line with volumes of every chunk
          scene->addItem(volLine);
          volLine->setPen(QPen(noteVolumeColor, 2));
          volLine->setLine(xMap(c - 1) + hSc, yMap(101 + m_pad->normalVolume() * 50.0),
                          xMap(c) + hSc, yMap(101 + m_ad->normalVolume() * 50.0));
        }
      }
  }
  qApp->processEvents();
}


void Nchart::drawNoteSegment(int firstNoteChunk, int lastNoteChunk, TnoteStruct* ns) {
  int minDurChunk = 0;
  qreal shortSum = 0, totalSum = 0;
  int lessDiffChunk;
  bool loudEnough = false;
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++) {
    if (m_pitchF->ch()->dataAtChunk(i)->normalVolume() >= Tcore::gl()->A->minimalVol)
      loudEnough = true;
    if (!minDurChunk && i - firstNoteChunk >= m_pitchF->minChunksNumber())
      minDurChunk = i;
    if (i > firstNoteChunk + 2 && i < lastNoteChunk - 1) // average pitch from 'inner' chunks
      shortSum += m_pitchF->ch()->dataAtChunk(i)->pitch;
    totalSum += m_pitchF->ch()->dataAtChunk(i)->pitch;
  }
  if (ns)
    loudEnough = ns->maxVol >= Tcore::gl()->A->minimalVol;
  qreal shortAverPitch = shortSum / (lastNoteChunk - firstNoteChunk - 4) - Tcore::gl()->A->a440diff;
  qreal averPitch = totalSum / (lastNoteChunk - firstNoteChunk + 1) - Tcore::gl()->A->a440diff;
  if (ns) {
    shortAverPitch = ns->shortAverage() - Tcore::gl()->A->a440diff;
    averPitch = ns->totalAverage() - Tcore::gl()->A->a440diff;
  }
  QPolygonF pitchGon;
  qreal lessDiff = 1.0;
  qreal y25 = yMap(50) - yMap(100);
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++){
    qreal pi = m_pitchF->ch()->dataAtChunk(i)->pitch - Tcore::gl()->A->a440diff;
    pitchGon << QPointF(xMap(i) + hSc, yMap(75) - qBound<qreal>(-y25 / 2, (pi - (qreal)qRound(shortAverPitch)) * y25, y25 / 2));
    if (qAbs<qreal>(pi - (qreal)qRound(shortAverPitch)) < lessDiff) {
      lessDiff = qAbs<qreal>(pi - (qreal)qRound(shortAverPitch));
      lessDiffChunk = i;
    }
  }

// ACCURACY BACKGROUND
  if (!minDurChunk || !loudEnough) {
    QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
    scene->addItem(bgPitch);
    bgPitch->setZValue(10);
    bgPitch->setPen(Qt::NoPen);
    bgPitch->setBrush(emptyColor);
    bgPitch->setRect(xMap(firstNoteChunk), yMap(100),
                    (lastNoteChunk - firstNoteChunk + 1) * xSc, yMap(50) - yMap(100));
  }
  if (minDurChunk) {
    QGraphicsLineItem *minDurLine = new QGraphicsLineItem;
      scene->addItem(minDurLine);
      minDurLine->setZValue(11);
      minDurLine->setPen(QPen(minDurColor, 1));
      minDurLine->setLine(xMap(minDurChunk) + hSc, yMap(99), xMap(minDurChunk) + hSc, yMap(51));
    QGraphicsTextItem *durText =  scene->addText(tr("min\nduration"));
      durText->setDefaultTextColor(minDurColor);
      durText->setScale(0.5);
      durText->setPos(xMap(minDurChunk) + hSc - durText->boundingRect().width() / 4, yMap(51));
    QGraphicsTextItem *sumText = scene->addText("");
      sumText->setHtml(QString("<p style=\"" + Tcolor::bgTag(QColor(255, 255, 255, 50)) + "\">a: %1<br>b: %2</p>").
          arg(averPitch).arg(ns->bestPitch - Tcore::gl()->A->a440diff));
      sumText->setDefaultTextColor(minDurColor);
      sumText->setScale(0.55);
      sumText->setPos(xMap(minDurChunk) + xSc, yMap(99));
      sumText->setZValue(100);
  }
  QGraphicsLineItem *bestPitchLine = new QGraphicsLineItem;
    scene->addItem(bestPitchLine);
    bestPitchLine->setZValue(16);
    bestPitchLine->setPen(QPen(bestPitchColor, 1));
    qreal pi = (m_pitchF->ch()->dataAtChunk(lessDiffChunk)->pitch - Tcore::gl()->A->a440diff) -
              (float)qRound(m_pitchF->ch()->dataAtChunk(lessDiffChunk)->pitch - Tcore::gl()->A->a440diff);
    if (ns)
      pi = (ns->bestPitch - Tcore::gl()->A->a440diff) - (float)ns->basePitch;
    qreal avY = yMap(75) - pi * y25;
    bestPitchLine->setLine(xMap(firstNoteChunk), avY, xMap(lastNoteChunk) + xSc, avY);

  QGraphicsLineItem *averPitchLine = new QGraphicsLineItem;
    scene->addItem(averPitchLine);
    averPitchLine->setZValue(15);
    averPitchLine->setPen(QPen(averPitchColor, 1));
    pi = averPitch - (float)qRound(averPitch);
    avY = yMap(75) - pi * y25;
    averPitchLine->setLine(xMap(firstNoteChunk), avY, xMap(lastNoteChunk) + xSc, avY);

//   QGraphicsLineItem *shortAverLine = new QGraphicsLineItem;
//     scene->addItem(shortAverLine);
//     shortAverLine->setZValue(15);
//     shortAverLine->setPen(QPen(Qt::darkYellow, 1));
//     pi = shortAverPitch - (float)qRound(shortAverPitch);
//     avY = yMap(75) - pi * y25;
//     shortAverLine->setLine(xMap(firstNoteChunk + 2), avY, xMap(lastNoteChunk - 2), avY);

  QPainterPath pp;
    pp.addPolygon(pitchGon);
  QGraphicsPathItem *pitchDiffLine = new QGraphicsPathItem(pp);
    scene->addItem(pitchDiffLine);
    pitchDiffLine->setZValue(15);
    pitchDiffLine->setPen(QPen(chunkPitchColor, 2, Qt::SolidLine, Qt::RoundCap));
//   Tnote n(qRound((ns ? ns->bestPitch : m_pitchF->ch()->dataAtChunk(lessDiffChunk)->pitch) - Tcore::gl()->A->a440diff) - 47);
  Tnote shortNote(qRound(shortAverPitch) - 47);
  m_staff->addNote(shortNote, true);
    m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(firstNoteChunk), 0).x(), 0);
    m_staff->noteSegment(m_staff->count() - 1)->showNoteName();
    if (!minDurChunk || !loudEnough)
      m_staff->noteSegment(m_staff->count() - 1)->setColor(Qt::gray);
  if (ns && loudEnough && minDurChunk) {
    Tnote totalNote(qRound(ns->totalAverage() - Tcore::gl()->A->a440diff) - 47);
    Tnote quickNote(qRound(ns->getAverage(3, m_pitchF->minChunksNumber()) - Tcore::gl()->A->a440diff) - 47);
    if (totalNote != shortNote) {
      m_staff->addNote(totalNote, true);
      m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(lastNoteChunk), 0).x() - 2 * m_staff->scale(), 0);
      m_staff->noteSegment(m_staff->count() - 1)->setColor(noteVolumeColor);
    }
    if (quickNote != shortNote) {
      m_staff->addNote(quickNote, true);
      m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(firstNoteChunk), 0).x() + 2 * m_staff->scale(), 0);
      m_staff->noteSegment(m_staff->count() - 1)->setColor(minDurColor);
    }
  }
}


void Nchart::emptyRect(int firstChunk, qreal width) {
  QGraphicsRectItem *emptyBg = new QGraphicsRectItem;
    scene->addItem(emptyBg);
    emptyBg->setZValue(10);
    emptyBg->setPen(Qt::NoPen);
    emptyBg->setBrush(emptyColor.lighter(160 + (10 * (firstChunk % 2))));
    emptyBg->setRect(xMap(firstChunk), yMap(100), width, yMap(50) - yMap(100));
}


void Nchart::noteStartedSlot(qreal pitch, qreal freq, qreal duration) {
//   Tnote n;
//   n.fromMidi(qRound(pitch));
//   qDebug() << "started" << n.toText();
}


void Nchart::progressSlot() {
  if (m_pitchF->currentChunk() < m_totalXnr) {
    m_progresItem->setHtml(tr("Detecting...") + QString("<big><b> %1%</b></big>").
            arg(int(((qreal)m_pitchF->currentChunk() / (qreal)(m_totalXnr)) * 100)));
    QTimer::singleShot(250, this, SLOT(progressSlot()));
  }
}


void Nchart::clefChanged(Tclef clef) {
  Tcore::gl()->S->clef = clef.type();
}


void Nchart::adjustHeight() {
  qreal factor = (viewport()->rect().height() / scene->sceneRect().height()) * 0.95;
  scale(factor, factor);
}



