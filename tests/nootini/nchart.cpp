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
#include <QtCore/QDebug>


Nchart::Nchart(QWidget* parent) :
  Tchart(parent),
  m_pitchF(0),
  m_volume(0.0), m_pitchDiff(100.0),
  totalXnr(0),
  m_pitchGrad(0.5, 0, 0.5, 1),
  xSc(4)
{
  yAxis->hide();
  xAxis->hide();
  yAxis->setMaxValue(200, false);

  progresItem = new QGraphicsTextItem;
  scene->addItem(progresItem);
  progresItem->setZValue(250);
  progresItem->setScale(2);
  progresItem->hide();
  progresItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

  m_pitchGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
  const int al = 255;
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

  m_staff = new TscoreStaff(scene, 0);
  m_staff->setScale((yAxis->mapValue(0) - yAxis->mapValue(60)) / m_staff->height());
  m_staff->setViewWidth(400 / m_staff->scale());
  m_staff->setPos(52, yAxis->mapValue(50));
  m_staff->onClefChanged(Tcore::gl()->S->clef);

  m_drawObject = new NdrawObject(this, 0);
}


Nchart::~Nchart()
{
//   Tcore::gl()->S->clef = m_staff->scoreClef()->clef().type();
  m_drawObject->deleteLater();
  qDebug() << "Nchart deleted";
}


void Nchart::setXnumber(int xN) {
  m_xLine->setLine(52, yAxis->boundingRect().height(), (xN + 15) * xSc, yAxis->boundingRect().height());
  totalXnr = xN;
  m_staff->setViewWidth(m_xLine->line().length() / m_staff->scale());
}


void Nchart::setPitchFinder(TpitchFinder* pf) {
  m_pitchF = pf;
  m_drawObject->reset();
  dl.clear();
  currentChunk = 0;
  m_lastDrawnChunk = 0;
//   connect(m_pitchF, &TpitchFinder::chunkProcessed, this, &Nchart::chunkSlot);
  connect(m_pitchF, &TpitchFinder::chunkProcessed, this, &Nchart::runThread, Qt::DirectConnection);
  m_staff->setDisabled(true);
  progresItem->show();
}


void Nchart::allDataLoaded() {
  ajustChartHeight();
  progresItem->hide();
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void Nchart::runThread(AnalysisData* ad, NoteData* nd) {
  m_drawObject->collect(ad, nd);
}


void Nchart::chunkSlot() {
  qDebug() << currentChunk << m_lastDrawnChunk;
  int firstNoteChunk = totalXnr, lastNoteChunk = 0;
  QPolygonF pg;
  int minDurChunk = 0;
  for (int i = currentChunk; i >= m_lastDrawnChunk; i--) {
    if (dl[i].vol && dl[i - 1].vol) {
      QGraphicsLineItem *volLine = new QGraphicsLineItem();
      scene->addItem(volLine);
      volLine->setPen(QPen(QColor(120, 255, 0), 2));
      volLine->setLine(xMap(i - 1), yAxis->mapValue(150 + dl[i - 1].vol * 50.0),
                       xMap(i), yAxis->mapValue(150 + dl[i].vol * 50.0));
      if (dl[i].pitch) {
        firstNoteChunk = qMin<int>(firstNoteChunk, i - 1);
        lastNoteChunk = qMax<int>(lastNoteChunk, i);
//         pg << QPointF(xMap(i), yAxis->mapValue(75) - (dl[i].pitch - (float)qRound(dl[i].pitch)) * 50);
      }
    }
    if (dl[i].dur > Tcore::gl()->A->minDuration) {
      minDurChunk = i + 1;
    }
    if (dl[i].pitch)
      pg << QPointF(xMap(i), yAxis->mapValue(75) - (dl[i].pitch - (float)qRound(dl[i].pitch)) * 50);
  }
  if (firstNoteChunk < totalXnr && lastNoteChunk > 0) {
    QGraphicsLineItem *minVolLine = new QGraphicsLineItem();
      scene->addItem(minVolLine);
      minVolLine->setPen(QPen(QColor(0, 0, 255), 1));
      minVolLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(150 + Tcore::gl()->A->minimalVol * 50.0),
                       xMap(lastNoteChunk), yAxis->mapValue(150 + Tcore::gl()->A->minimalVol * 50.0));
    QGraphicsLineItem *xVolLine = new QGraphicsLineItem();
      scene->addItem(xVolLine);
      xVolLine->setPen(QPen(palette().text().color(), 1));
      xVolLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(151), xMap(lastNoteChunk), yAxis->mapValue(151));
    QGraphicsLineItem *yVolLine = new QGraphicsLineItem();
      scene->addItem(yVolLine);
      yVolLine->setPen(QPen(palette().text().color(), 1));
      yVolLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(199), xMap(firstNoteChunk), yAxis->mapValue(151));
    QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
      scene->addItem(bgPitch);
      bgPitch->setZValue(10);
      bgPitch->setPen(Qt::NoPen);
      bgPitch->setBrush(m_pitchGrad);
      bgPitch->setRect(xMap(firstNoteChunk), yAxis->mapValue(100), (lastNoteChunk - firstNoteChunk) * xSc, yAxis->mapValue(50) - yAxis->mapValue(100));
    if (minDurChunk) {
      QGraphicsLineItem *minDurLine = new QGraphicsLineItem;
      scene->addItem(minDurLine);
      minDurLine->setZValue(11);
      minDurLine->setPen(QPen(Qt::blue, 1));
      minDurLine->setLine(xMap(minDurChunk), yAxis->mapValue(99), xMap(minDurChunk), yAxis->mapValue(51));
    }
    QGraphicsLineItem *pitchLine = new QGraphicsLineItem;
      scene->addItem(pitchLine);
      pitchLine->setZValue(12);
      pitchLine->setPen(QPen(Qt::darkGreen, 1));
      pitchLine->setLine(xMap(firstNoteChunk), yAxis->mapValue(75), xMap(lastNoteChunk), yAxis->mapValue(75));
    QGraphicsLineItem *averpitchLine = new QGraphicsLineItem;
      scene->addItem(averpitchLine);
      averpitchLine->setZValue(16);
      averpitchLine->setPen(QPen(Qt::gray, 1));
      qreal avY = yAxis->mapValue(75) - (dl[lastNoteChunk].avPitch - (float)qRound(dl[lastNoteChunk].avPitch)) * 50;
      averpitchLine->setLine(xMap(firstNoteChunk), avY, xMap(lastNoteChunk), avY);
    QPainterPath pp;
      pp.addPolygon(pg);
    QGraphicsPathItem *pitchDiffLine = new QGraphicsPathItem(pp);
      scene->addItem(pitchDiffLine);
      pitchDiffLine->setZValue(15);
      pitchDiffLine->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
    Tnote n(qRound(dl[lastNoteChunk].avPitch) - 47);
    m_staff->addNote(n, true);
      m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(firstNoteChunk), 0).x(), 0);
      m_staff->noteSegment(m_staff->count() - 1)->showNoteName();
  }
  m_lastDrawnChunk = currentChunk + 1;

  emit chunkDone();
}

//#################################################################################################
//#################################################################################################
//###################              NdrawObject         ############################################
//#################################################################################################

NdrawObject::NdrawObject(Nchart* chart, QObject* parent) :
  QObject(parent),
  m_chart(chart),
  m_mutex(QMutex::Recursive)
{
//   moveToThread(&m_thread);
//   connect(&m_thread, &QThread::started, m_chart, &Nchart::chunkSlot);
//   connect(m_chart, &Nchart::chunkDone, this, &NdrawObject::chunkProcessed);
}


NdrawObject::~NdrawObject()
{
  m_thread.quit();
  m_thread.wait();
}


void NdrawObject::reset() {
  m_chunkNr = 0;
  m_prevNoteIndex = -1;
}


void NdrawObject::collect(AnalysisData* ad, NoteData* nd) {
  bool run = false;
  if (ad && nd) {
    m_chart->dl << NanalyzeData(m_chunkNr, ad->pitch, nd->avgPitch(), dB2Normalised(ad->logrms()), nd->noteLength());
    if (m_prevNoteIndex != ad->noteIndex)
      run = true;
    m_prevNoteIndex = ad->noteIndex;
  } else {
    m_chart->dl << NanalyzeData(m_chunkNr);
    m_prevNoteIndex = -1;
  }
  if (m_chunkNr == m_chart->totalXnr - 3)
    run = true;
  m_chunkNr++;
  if (m_chunkNr && m_chunkNr % 20 == 0) {
    m_chart->progresItem->setHtml(tr("Detecting...") + QString("<big><b> %1%</b></big>").arg(int(((qreal)m_chunkNr / (qreal)(m_chart->totalXnr)) * 100)));
    qApp->processEvents();
  }
  if (run) {
//     m_mutex.lock();
    m_chart->currentChunk = m_chunkNr - 1;
    m_chart->chunkSlot();
//     m_thread.start();
  }
  if (m_chunkNr >= m_chart->totalXnr - 3)
    m_chart->allDataLoaded();
}


void NdrawObject::chunkProcessed() {
//   m_thread.quit();
//   m_mutex.unlock();
//   if (m_chunkNr >= m_chart->totalXnr - 3)
//     m_chart->allDataLoaded();
}




