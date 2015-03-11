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
  m_nootkaIndexing(false)
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
  m_pitchGrad.setColorAt(0.15, QColor(255, 0, 0, al));
  m_pitchGrad.setColorAt(0.4, QColor(255, 255, 0, al));
  m_pitchGrad.setColorAt(0.5, QColor(0, 255, 0, al));
//   m_pitchGrad.setColorAt(0.55, QColor(0, 255, 0, al));
  m_pitchGrad.setColorAt(0.6, QColor(255, 255, 0, al));
  m_pitchGrad.setColorAt(0.85, QColor(255, 0, 0, al));
  m_pitchGrad.setColorAt(1, QColor(117, 21, 86, al));

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
    QGraphicsTextItem *minVolText = scene->addText(tr("min\nvolume"));
      minVolText->setDefaultTextColor(minVolumeColor);
      minVolText->setPos(xMap(1) - minVolText->boundingRect().width(),
                      yMap(101 + Tcore::gl()->A->minimalVol * 50.0) - minVolText->boundingRect().height() / 2);
    QGraphicsTextItem *noteVolText = scene->addText((tr("note\nvolume")));
      noteVolText->setDefaultTextColor(noteVolumeColor);
      noteVolText->setPos(xMap(1) - minVolText->boundingRect().width(), yMap(150));
    scene->addLine(xMap(1), yMap(101), xMap(m_totalXnr) , yMap(101), QPen(palette().text().color(), 1)); // X axis of volume chart
    QGraphicsTextItem *pcmSigText = scene->addText(tr("PCM\nvolume"));
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
                        "<span style=\"color: " + averPitchColor.name() + "\";>" + tr("average pitch") + "</span>");
    pitchTexts->setPos(xMap(0) - pitchTexts->boundingRect().width(), yMap(75) - pitchTexts->boundingRect().height() / 2);
  QGraphicsTextItem *settingsTexts = new QGraphicsTextItem();
    scene->addItem(settingsTexts);
    QString pdm = "MPM";
    if (Tcore::gl()->A->detectMethod == 1)
      pdm = "autocorelation";
    else if (Tcore::gl()->A->detectMethod == 2)
      pdm = "mod. cepstrum";
    settingsTexts->setHtml(tr("method") + ": <b>" + pdm +"</b><br>" +
      tr("filter") + " :" + yOrN(m_pitchF->aGl()->equalLoudness) + "<br>" +
      QString("noise-floor: <b>%1</b>").arg(yOrN(m_pitchF->aGl()->doingAutoNoiseFloor)) + "<br>" +
      tr("threshold") + QString(": <b>%1 %</b>").arg(m_pitchF->aGl()->threshold) + "<br>" +
      "<span style=\"color: " + noteVolumeColor.name() + "\";>" + QString("split vol.: <b>%1</b>").
          arg((m_minVolToSplit == 0.0 ? "no" : QString("%1").arg(m_minVolToSplit))) + "</span><br>" +
      QString("dbFoor: <b>%1</b>").arg(m_pitchF->aGl()->dBFloor) + "<br>" +
      QString("middle A: <b>%1 Hz</b>").arg(pitch2freq(freq2pitch(440) + Tcore::gl()->A->a440diff)) + "<br>" +
      "<span style=\"color: " + minVolumeColor.name() + "\";>" + QString("min. volume: <b>%1</b>").arg(Tcore::gl()->A->minimalVol * 100) + "</span><br>"
      "<span style=\"color: " + minDurColor.name() + "\";>" + QString("min. duration: <b>%1 [ms]</b>").arg(Tcore::gl()->A->minDuration * 1000) + "</span>");
    settingsTexts->setPos(xMap(xN - 2), yMap(100));
}


void Nchart::setAudioLoader(NaudioLoader* loader) {
  m_pass++;
  m_loader = loader;
  m_pitchF = loader->finder();
  m_chunkNr = -1;
  m_hiVol = 0;
  m_loVol = 0;
  connect(m_pitchF, &TpitchFinder::chunkProcessed, this, &Nchart::copyChunk, Qt::DirectConnection);
  m_progresItem->show();
  m_chunkDuration = 1.0 / ((qreal)m_pitchF->aGl()->rate / (qreal)m_pitchF->aGl()->framesPerChunk);
  m_minChunkDur = qRound(Tcore::gl()->A->minDuration / m_chunkDuration);
  qDebug() << "chunk duration" << m_chunkDuration << "min chunks number" << m_minChunkDur;
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
//     m_staff->setViewWidth(oldStaff->width());
    m_staff->setPos(oldStaff->x(), yMap(50));
    m_staff->onClefChanged(oldStaff->scoreClef()->clef());
  }
  setXnumber(m_loader->totalChunks() + 1);
  m_staff->setDisabled(true);
}


void Nchart::allDataLoaded() {
  m_progresItem->hide();
}


void Nchart::setNootkaIndexing(bool yes) {
  m_nootkaIndexing = yes;
  if (m_nootkaIndexing) {
    m_newNote.basePitch = 0;
    m_prevNote.basePitch = 0;
    m_nootkaIndex = 0;
  }
}
//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void Nchart::copyChunk(TnoteStruct* ad) {
  m_chunkNr++;
  m_pitchF->lastData().signalStrenght = m_loader->volume();
  m_pitchF->lastData().pitchF = m_pitchF->lastData().pitchF - Tcore::gl()->A->a440diff; // adjust middle a offset
}


void Nchart::drawChunk() {
  int firstNoteChunk;
  int lowestChunk;
  for (int c = 1; c < m_totalXnr - 4; c++) {
    while (c >= m_pitchF->dataSize()) { // wait for Tartini processing
      qApp->thread()->msleep(1);
//       SLEEP(1);
//       qDebug() << "sleep during drawing" << c;
      qApp->processEvents();
    }
    if (c % 30 == 0)
      m_progresItem->setHtml(tr("Detecting...") + QString("<big><b> %1%</b></big>").arg(int(((qreal)c / (qreal)(m_totalXnr)) * 100)));
    if (m_drawVolume)
      scene->addLine(xMap(c) + hSc, yMap(101 + m_pitchF->dl(c - 1).signalStrenght * 50.0), // PCM signal volume line
                   xMap(c + 1) + hSc, yMap(101 + m_pitchF->dl(c).signalStrenght * 50.0), QPen(pcmVolumeColor, 1, Qt::SolidLine, Qt::RoundCap));
      // Line is shifted one chunk forward due to it represents data of will be processed chunk

    if (m_pitchF->dl(c).index != -1 && m_pitchF->dl(c - 1).index != m_pitchF->dl(c).index)
      firstNoteChunk = c;
    if (m_pitchF->dl(c - 1).index != -1 && m_pitchF->dl(c - 1).index != m_pitchF->dl(c).index) { // note was finished in previous chunk - so process its data
      if (m_minVolToSplit > 0.0) {
        int fnc = firstNoteChunk;
        for (int i = firstNoteChunk; i <= c - 1; i++) {
          if (i == c - 1)
            drawNoteSegment(fnc, i);
          else {
            if (i - fnc >= m_minChunkDur && m_pitchF->dl(i).volume - m_pitchF->dl(i - 2).volume > m_minVolToSplit) {
              drawNoteSegment(fnc, i);
              qDebug() << fnc << i << "Volume changed" << m_pitchF->dl(i).volume - m_pitchF->dl(i - 2).volume;
              fnc = i + 1;
            }
          }
        }
      } else {
        drawNoteSegment(firstNoteChunk, c - 1);
      }
    }

    if (m_pitchF->dl(c).index == -1) { // no data in chunk
      emptyRect(c, xSc);
      m_hiVol = 0.0;
      m_loVol = 1.0;
    }
  }
  emit chunkDone();
  allDataLoaded();
}


void Nchart::drawNoteSegment(int firstNoteChunk, int lastNoteChunk) {
  int minDurChunk = 0;
  qreal pitchSum = 0;
  bool loudEnough = false;
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++) {
    if (m_drawVolume) {
      if (m_pitchF->dl(i).volume && m_pitchF->dl(i - 1).volume) {
        QGraphicsLineItem *volLine = new QGraphicsLineItem(); // line with volumes of every chunk
        scene->addItem(volLine);
        volLine->setPen(QPen(noteVolumeColor, 2));
        volLine->setLine(xMap(i - 1) + hSc, yMap(101 + m_pitchF->dl(i - 1).volume * 50.0),
                        xMap(i) + hSc, yMap(101 + m_pitchF->dl(i).volume * 50.0));
      }
    }
    if (m_pitchF->dl(i).volume >= Tcore::gl()->A->minimalVol)
      loudEnough = true;
    if (!minDurChunk && i - firstNoteChunk >= m_minChunkDur)
      minDurChunk = i;
    if (i - firstNoteChunk < m_minChunkDur) // average pitch only during minimal time
      pitchSum += m_pitchF->dl(i).pitchF;
  }
  qreal averPitch = pitchSum / qMin<int>(lastNoteChunk - firstNoteChunk + 1, m_minChunkDur);
  QPolygonF pitchGon;
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++)
    pitchGon << QPointF(xMap(i) + hSc, yMap(75) - qBound<qreal>(-50.0, (m_pitchF->dl(i).pitchF - (float)qRound(averPitch)) * 50.0, 50.0));

// ACCURACY BACKGROUND
  QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
    scene->addItem(bgPitch);
    bgPitch->setZValue(10);
    bgPitch->setPen(Qt::NoPen);
  if (!minDurChunk || !loudEnough) {
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
  }
  QGraphicsLineItem *averpitchLine = new QGraphicsLineItem;
    scene->addItem(averpitchLine);
    averpitchLine->setZValue(16);
    averpitchLine->setPen(QPen(averPitchColor, 1));
    qreal avY = yMap(75) - (averPitch - (float)qRound(averPitch)) * 50;
    averpitchLine->setLine(xMap(firstNoteChunk), avY, xMap(lastNoteChunk), avY);
  QPainterPath pp;
    pp.addPolygon(pitchGon);
  QGraphicsPathItem *pitchDiffLine = new QGraphicsPathItem(pp);
    scene->addItem(pitchDiffLine);
    pitchDiffLine->setZValue(15);
    pitchDiffLine->setPen(QPen(chunkPitchColor, 2, Qt::SolidLine, Qt::RoundCap));
  Tnote n(qRound(averPitch) - 47);
  m_staff->addNote(n, true);
    m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(firstNoteChunk), 0).x(), 0);
    m_staff->noteSegment(m_staff->count() - 1)->showNoteName();
    if (!minDurChunk || !loudEnough)
      m_staff->noteSegment(m_staff->count() - 1)->setColor(Qt::gray);
}


void Nchart::emptyRect(int firstChunk, qreal width) {
  QGraphicsRectItem *emptyBg = new QGraphicsRectItem;
    scene->addItem(emptyBg);
    emptyBg->setZValue(10);
    emptyBg->setPen(Qt::NoPen);
    emptyBg->setBrush(emptyColor.lighter(160 + (10 * (firstChunk % 2))));
    emptyBg->setRect(xMap(firstChunk), yMap(100), width, yMap(50) - yMap(100));
}


void Nchart::nootkaMethod(int c) {
  if (m_pitchF->dl(c).pitchF == 0) {
    if (m_prevNote.basePitch != 0) { // draw data
      drawNoteSegment2(c - 1 - m_prevNote.chunkDur, c - 1);
      m_prevNote.basePitch = 0;
    }
    m_hiVol = 0.0;
    m_loVol = 0.0;
  } else {
    m_hiVol = qMax<double>(m_hiVol, m_pitchF->dl(c).volume);
    m_loVol = qMin<double>(m_loVol, m_pitchF->dl(c).volume);
    if (m_pitchF->dl(c).basePitch != m_newNote.basePitch) {
      m_newNote.basePitch = m_pitchF->dl(c).basePitch;
      m_newNote.chunkDur = 1;
      m_nootkaIndex++;
      if (m_prevNote.basePitch != 0) { // draw data
        drawNoteSegment2(c - m_prevNote.chunkDur - 1, c - 1);
        m_prevNote.basePitch = 0;
      }
    } else { // note is continued in this chunk
      m_newNote.chunkDur++;
      if (m_newNote.chunkDur >= m_minChunkDur && m_hiVol >= Tcore::gl()->A->minimalVol) { // new note is long enough and loud enough
        m_prevNote.basePitch = m_newNote.basePitch;
        m_prevNote.chunkDur = m_newNote.chunkDur;
        m_prevNote.duration = m_prevNote.chunkDur * m_chunkDuration;
//         if (m_prevNote.index != m_nootkaIndex) {
//           m_prevNote.index = m_nootkaIndex;
//         }
      }
    }
  }
}


void Nchart::drawNoteSegment2(int firstNoteChunk, int lastNoteChunk) {
  int minDurChunk = 0;
  qreal pitchSum = 0;
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++) {
    if (m_pitchF->dl(i).volume && m_pitchF->dl(i - 1).volume) {
      QGraphicsLineItem *volLine = new QGraphicsLineItem(); // line with volumes of every chunk
      scene->addItem(volLine);
      volLine->setPen(QPen(QColor(120, 255, 0), 2));
      volLine->setLine(xMap(i - 1) + hSc, yMap(150 + m_pitchF->dl(i - 1).volume * 50.0),
                      xMap(i) + hSc, yMap(150 + m_pitchF->dl(i).volume * 50.0));
    }
    if (!minDurChunk && m_pitchF->dl(i).duration > Tcore::gl()->A->minDuration)
      minDurChunk = i;
    pitchSum += m_pitchF->dl(i).pitchF;
  }
  qreal averPitch = pitchSum / (lastNoteChunk - firstNoteChunk + 1);
  QPolygonF pg;
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++)
    pg << QPointF(xMap(i) + hSc, yMap(75) - qBound<qreal>(-50.0, (m_pitchF->dl(i).pitchF - (float)qRound(averPitch)) * 50.0, 50.0));
// VOLUME
  QGraphicsLineItem *minVolLine = new QGraphicsLineItem(); // minimum volume line
    scene->addItem(minVolLine);
    minVolLine->setPen(QPen(QColor(0, 0, 255), 1));
    minVolLine->setLine(xMap(firstNoteChunk), yMap(150 + Tcore::gl()->A->minimalVol * 50.0),
                    xMap(lastNoteChunk) + xSc, yMap(150 + Tcore::gl()->A->minimalVol * 50.0));
  QGraphicsLineItem *xVolLine = new QGraphicsLineItem(); // X axis of volume chart
    scene->addItem(xVolLine);
    xVolLine->setPen(QPen(palette().text().color(), 1));
    xVolLine->setLine(xMap(firstNoteChunk), yMap(151), xMap(lastNoteChunk) + xSc, yMap(151));
  QGraphicsLineItem *yVolLine = new QGraphicsLineItem(); // X axis of volume chart
    scene->addItem(yVolLine);
    yVolLine->setPen(QPen(palette().text().color(), 1));
    yVolLine->setLine(xMap(firstNoteChunk), yMap(199), xMap(firstNoteChunk), yMap(151));
// ACCURACY BACKGROUND
  QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
    scene->addItem(bgPitch);
    bgPitch->setZValue(10);
    bgPitch->setPen(Qt::NoPen);
    if (minDurChunk)
      bgPitch->setBrush(m_pitchGrad);
    else {
      QColor emptyColor = palette().text().color();
      emptyColor.setAlpha(40);
      bgPitch->setBrush(emptyColor);
    }
    bgPitch->setRect(xMap(firstNoteChunk), yMap(100),
                      (lastNoteChunk - firstNoteChunk + 1) * xSc, yMap(50) - yMap(100));
  if (minDurChunk) {
    QGraphicsLineItem *minDurLine = new QGraphicsLineItem;
    scene->addItem(minDurLine);
    minDurLine->setZValue(11);
    minDurLine->setPen(QPen(Qt::blue, 1));
    minDurLine->setLine(xMap(minDurChunk) + hSc, yMap(99), xMap(minDurChunk) + hSc, yMap(51));
  }
  QGraphicsLineItem *pitchLine = new QGraphicsLineItem; // perfect pitch line in the middle of a rectangle
    scene->addItem(pitchLine);
    pitchLine->setZValue(12);
    pitchLine->setPen(QPen(Qt::darkGreen, 1));
    pitchLine->setLine(xMap(firstNoteChunk), yMap(75), xMap(lastNoteChunk), yMap(75));
  QGraphicsLineItem *averpitchLine = new QGraphicsLineItem;
    scene->addItem(averpitchLine);
    averpitchLine->setZValue(16);
    averpitchLine->setPen(QPen(Qt::gray, 1));
    qreal avY = yMap(75) - (averPitch - (float)qRound(averPitch)) * 50;
    averpitchLine->setLine(xMap(firstNoteChunk), avY, xMap(lastNoteChunk), avY);
  QPainterPath pp;
    pp.addPolygon(pg);
  QGraphicsPathItem *pitchDiffLine = new QGraphicsPathItem(pp);
    scene->addItem(pitchDiffLine);
    pitchDiffLine->setZValue(15);
    pitchDiffLine->setPen(QPen(Qt::cyan, 2, Qt::SolidLine, Qt::RoundCap));
  Tnote n(m_prevNote.basePitch - 47);
  m_staff->addNote(n, true);
    m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(firstNoteChunk), 0).x(), 0);
    m_staff->noteSegment(m_staff->count() - 1)->showNoteName();
    if (!minDurChunk)
      m_staff->noteSegment(m_staff->count() - 1)->setColor(Qt::gray);
}


void Nchart::clefChanged(Tclef clef) {
  Tcore::gl()->S->clef = clef.type();
}


void Nchart::adjustHeight() {
  qreal factor = (viewport()->rect().height() / scene->sceneRect().height()) * 0.95;
  scale(factor, factor);
}



