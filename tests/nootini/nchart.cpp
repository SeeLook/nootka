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
#include <tyaxis.h>
#include <txaxis.h>
#include <tartini/notedata.h>
#include <tartini/channel.h>
#include <tpitchfinder.h>
#include <music/tnote.h>
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <score/tscorescene.h>
#include <score/tscorestaff.h>
#include <score/tscoreclef.h>
#include <tscoreparams.h>
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


Nchart::Nchart(QWidget* parent) :
  Tchart(parent),
  m_pitchF(0),
  m_totalXnr(0),
  m_pitchGrad(0.5, 0, 0.5, 1),
  xSc(4), hSc(2)
{
  yAxis->hide();
  xAxis->hide();
  yAxis->setMaxValue(200, false);

  m_progresItem = new QGraphicsTextItem;
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

  m_xLine = new QGraphicsLineItem;
  scene->addItem(m_xLine);
  m_xLine->setPen(QPen(palette().text().color(), 2));
  m_xLine->setLine(52, yAxis->boundingRect().height(), 400, yAxis->boundingRect().height());
  m_xLine->setZValue(2);

  m_staff = new TscoreStaff(scene, 0);
  m_staff->setScale((yAxis->mapValue(0) - yAxis->mapValue(60)) / m_staff->height());
  m_staff->setViewWidth(400 / m_staff->scale());
  m_staff->setPos(22, yAxis->mapValue(50));
  m_staff->onClefChanged(Tcore::gl()->S->clef);
  connect(m_staff, &TscoreStaff::clefChanged, this, &Nchart::clefChanged);
  QTimer::singleShot(1, this, SLOT(adjustHeight()));
}


Nchart::~Nchart()
{
}


void Nchart::setXnumber(int xN) {
  m_xLine->setLine(52, yAxis->boundingRect().height(), (xN + 15) * xSc, yAxis->boundingRect().height());
  m_totalXnr = xN;
  m_staff->setViewWidth(m_xLine->line().length() / m_staff->scale());
  qreal secondStep = xSc * (qreal)m_pitchF->aGl()->rate / (qreal)m_pitchF->aGl()->framesPerChunk;
  for (int i = 0; i <= m_xLine->line().length() / secondStep; ++i) {
    scene->addLine(QLineF(52 + i * secondStep, yAxis->mapValue(0), 52 + i * secondStep, yAxis->mapValue(0) + 3), m_xLine->pen());
    QGraphicsTextItem *ticText = scene->addText(QString("%1").arg(i));
    ticText->setPos(52 + i * secondStep - ticText->boundingRect().width() / 2, yAxis->mapValue(0) + 3);
    QGraphicsLineItem *l = scene->addLine(52 + i * secondStep, yAxis->mapValue(1), 52 + i * secondStep, yAxis->mapValue(205),
                   QPen(Qt::lightGray, 1, Qt::DashLine));
    l->setZValue(1);
  }
  QGraphicsTextItem *timeText = scene->addText(tr("time [s]") + "  ");
    timeText->setPos(52  + m_xLine->line().length(), yAxis->mapValue(0) + 3);
}


void Nchart::setPitchFinder(TpitchFinder* pf) {
  m_pitchF = pf;
  dl.clear();
  m_chunkNr = -1;
  connect(m_pitchF, &TpitchFinder::chunkProcessed, this, &Nchart::copyChunk, Qt::DirectConnection);
  m_staff->setDisabled(true);
  m_progresItem->show();

  horizontalScrollBar()->setValue(0);
}


void Nchart::allDataLoaded() {
  m_progresItem->hide();
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void Nchart::copyChunk(AnalysisData* ad, NoteData* nd) {
  m_chunkNr++;
  if (ad && nd)
    dl << NanalyzeData(ad->noteIndex, ad->pitch, nd->avgPitch(), dB2Normalised(ad->logrms()), nd->noteLength());
  else
    dl << NanalyzeData(-1);
}


void Nchart::drawChunk() {
  int firstNoteChunk;
  for (int c = 1; c < m_totalXnr - 4; c++) {
    while (c >= dl.size()) { // wait for Tartini processing
//       qApp->thread()->msleep(1);
      SLEEP(1);
//       qDebug() << "sleep during drawing" << c;
      qApp->processEvents();
    }
    if (c % 20 == 0)
      m_progresItem->setHtml(tr("Detecting...") + QString("<big><b> %1%</b></big>").arg(int(((qreal)c / (qreal)(m_totalXnr)) * 100)));
    if (dl[c -1].index == -1 && dl[c].index != -1) // first chunk with noticed new note
      firstNoteChunk = c;
    if ((dl[c -1].index != -1 && dl[c].index == -1)) { // note was finished in previous chunk - so process its data
      drawNoteSegment(firstNoteChunk, c - 1);

    } else {
    }
    if (dl[c].index == -1) {
      emptyRect(c, xSc);
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
    if (dl[i].vol && dl[i - 1].vol) {
      QGraphicsLineItem *volLine = new QGraphicsLineItem(); // line with volumes of every chunk
      scene->addItem(volLine);
      volLine->setPen(QPen(QColor(120, 255, 0), 2));
      volLine->setLine(xMap(i - 1) + hSc, yAxis->mapValue(150 + dl[i - 1].vol * 50.0),
                      xMap(i) + hSc, yAxis->mapValue(150 + dl[i].vol * 50.0));
    }
    if (dl[i].vol >= Tcore::gl()->A->minimalVol)
      loudEnough = true;
    if (!minDurChunk && dl[i].dur > Tcore::gl()->A->minDuration)
      minDurChunk = i;
    pitchSum += dl[i].pitch;
  }
  qreal averPitch = pitchSum / (lastNoteChunk - firstNoteChunk + 1);
  QPolygonF pg;
  for (int i = firstNoteChunk; i <= lastNoteChunk; i++)
    pg << QPointF(xMap(i) + hSc, yAxis->mapValue(75) - qBound<qreal>(-50.0, (dl[i].pitch - (float)qRound(averPitch)) * 50.0, 50.0));
// VOLUME
  QGraphicsLineItem *minVolLine = new QGraphicsLineItem(); // minimum volume line
    scene->addItem(minVolLine);
    minVolLine->setPen(QPen(QColor(0, 0, 255), 1));
    minVolLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(150 + Tcore::gl()->A->minimalVol * 50.0),
                    xMap(lastNoteChunk) + xSc, yAxis->mapValue(150 + Tcore::gl()->A->minimalVol * 50.0));
  QGraphicsLineItem *xVolLine = new QGraphicsLineItem(); // X axis of volume chart
    scene->addItem(xVolLine);
    xVolLine->setPen(QPen(palette().text().color(), 1));
    xVolLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(151), xMap(lastNoteChunk) + xSc, yAxis->mapValue(151));
  QGraphicsLineItem *yVolLine = new QGraphicsLineItem(); // X axis of volume chart
    scene->addItem(yVolLine);
    yVolLine->setPen(QPen(palette().text().color(), 1));
    yVolLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(199), xMap(firstNoteChunk), yAxis->mapValue(151));
// ACCURACY BACKGROUND
  QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
    scene->addItem(bgPitch);
    bgPitch->setZValue(10);
    bgPitch->setPen(Qt::NoPen);
    if (minDurChunk && loudEnough)
      bgPitch->setBrush(m_pitchGrad);
    else {
      QColor emptyColor = palette().text().color();
      emptyColor.setAlpha(40);
      bgPitch->setBrush(emptyColor);
    }
    bgPitch->setRect(xMap(firstNoteChunk), yAxis->mapValue(100),
                      (lastNoteChunk - firstNoteChunk + 1) * xSc, yAxis->mapValue(50) - yAxis->mapValue(100));
  if (minDurChunk) {
    QGraphicsLineItem *minDurLine = new QGraphicsLineItem;
    scene->addItem(minDurLine);
    minDurLine->setZValue(11);
    minDurLine->setPen(QPen(Qt::blue, 1));
    minDurLine->setLine(xMap(minDurChunk) + hSc, yAxis->mapValue(99), xMap(minDurChunk) + hSc, yAxis->mapValue(51));
  }
  QGraphicsLineItem *pitchLine = new QGraphicsLineItem; // perfect pitch line in the middle of a rectangle
    scene->addItem(pitchLine);
    pitchLine->setZValue(12);
    pitchLine->setPen(QPen(Qt::darkGreen, 1));
    pitchLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(75), xMap(lastNoteChunk), yAxis->mapValue(75));
  QGraphicsLineItem *averpitchLine = new QGraphicsLineItem;
    scene->addItem(averpitchLine);
    averpitchLine->setZValue(16);
    averpitchLine->setPen(QPen(Qt::gray, 1));
    qreal avY = yAxis->mapValue(75) - (averPitch - (float)qRound(averPitch)) * 50;
    averpitchLine->setLine(xMap(firstNoteChunk), avY, xMap(lastNoteChunk), avY);
  QPainterPath pp;
    pp.addPolygon(pg);
  QGraphicsPathItem *pitchDiffLine = new QGraphicsPathItem(pp);
    scene->addItem(pitchDiffLine);
    pitchDiffLine->setZValue(15);
    pitchDiffLine->setPen(QPen(Qt::cyan, 2, Qt::SolidLine, Qt::RoundCap));
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
    QColor emptyColor = palette().text().color();
    emptyColor.setAlpha(40);
    emptyBg->setBrush(emptyColor);
    emptyBg->setRect(xMap(firstChunk), yAxis->mapValue(100), width, yAxis->mapValue(50) - yAxis->mapValue(100));
}


void Nchart::clefChanged(Tclef clef) {
  Tcore::gl()->S->clef = clef.type();
}


void Nchart::adjustHeight() {
  qreal factor = (viewport()->rect().height() / scene->sceneRect().height()) * 0.95;
  scale(factor, factor);
}



