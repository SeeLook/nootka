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
#include <tpitchfinder.h>
#include <music/tnote.h>
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <score/tscorescene.h>
#include <score/tscorestaff.h>
#include <score/tscoreclef.h>
#include <tscoreparams.h>
#include <QtCore/QDebug>


double m_volume = 0, m_pitchDiff = 100.0;
QGraphicsTextItem *m_progresItem;
int m_totalXnr = 0;
int m_prevNoteIndex = -1;
QLinearGradient m_pitchGrad(0.5, 0, 0.5, 1);
const int xSc = 4;
QGraphicsLineItem *xLine;

int xMap(int xx) {
  return xLine->line().x1() + (xx + 1) * xSc;
}

Nchart::Nchart(QWidget* parent) :
  Tchart(parent),
  m_pitchF(0)
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
  const int al = 255;
  m_pitchGrad.setColorAt(0, QColor(117, 21, 86, al));
  m_pitchGrad.setColorAt(0.2, QColor(255, 0, 0, al));
  m_pitchGrad.setColorAt(0.35, QColor(255, 255, 0, al));
  m_pitchGrad.setColorAt(0.5, QColor(0, 255, 0, al));
  m_pitchGrad.setColorAt(0.65, QColor(255, 255, 0, al));
  m_pitchGrad.setColorAt(0.8, QColor(255, 0, 0, al));
  m_pitchGrad.setColorAt(1, QColor(117, 21, 86, al));

  xLine = new QGraphicsLineItem;
  scene->addItem(xLine);
  xLine->setPen(QPen(palette().text().color(), 2));
  xLine->setLine(52, yAxis->boundingRect().height(), 400, yAxis->boundingRect().height());

  m_staff = new TscoreStaff(scene, 0);
  m_staff->setScale((yAxis->mapValue(0) - yAxis->mapValue(60)) / m_staff->height());
  m_staff->setViewWidth(400 / m_staff->scale());
  m_staff->setPos(52, yAxis->mapValue(50));
  m_staff->onClefChanged(Tcore::gl()->S->clef);
}


void Nchart::setXnumber(int xN) {
  xLine->setLine(52, yAxis->boundingRect().height(), (xN + 15) * xSc, yAxis->boundingRect().height());
  m_totalXnr = xN;
  m_staff->setViewWidth(xLine->line().length() / m_staff->scale());
}


void Nchart::setPitchFinder(TpitchFinder* pf) {
  m_pitchF = pf;
  m_chunkNr = 0;
  connect(m_pitchF, &TpitchFinder::chunkProcessed, this, &Nchart::chunkSlot);
//   m_staff->setDisabled(true);
  m_progresItem->show();
}


void Nchart::allDataLoaded() {
  ajustChartHeight();
  m_progresItem->hide();
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void Nchart::chunkSlot(AnalysisData* ad, NoteData* nd) {
  m_chunkNr++;
  if (nd && ad && ad->noteIndex >=0) {
    if (m_volume) {
      QGraphicsLineItem *volLine = new QGraphicsLineItem();
      scene->addItem(volLine);
      volLine->setPen(QPen(QColor(120, 255, 0), 2));
      volLine->setLine(xMap(m_chunkNr - 1), yAxis->mapValue(150 + m_volume),
                       xMap(m_chunkNr), yAxis->mapValue(150 + dB2Normalised(ad->logrms()) * 50.0));
    }
    m_volume = dB2Normalised(ad->logrms()) * 50.0;
    if (m_prevNoteIndex != ad->noteIndex) {
//       QGraphicsTextItem *pitchLab = new QGraphicsTextItem();
//       pitchLab->setFlag(QGraphicsItem::ItemIgnoresTransformations);
//       scene->addItem(pitchLab);
      Tnote n(qRound(ad->pitch) - 47);
//       pitchLab->setHtml(n.toRichText());
//       pitchLab->setDefaultTextColor(Qt::blue);
//       pitchLab->setPos(xMap(m_chunkNr), yAxis->mapValue(50));
      m_prevNoteIndex = ad->noteIndex;
      m_staff->addNote(n, true);
      m_staff->noteSegment(m_staff->count() - 1)->setPos(m_staff->mapFromScene(xMap(m_chunkNr), 0).x(), 0);
      m_staff->noteSegment(m_staff->count() - 1)->showNoteName();
    }
    if (m_pitchDiff < 100.0) {
      QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
      scene->addItem(bgPitch);
      bgPitch->setPen(Qt::NoPen);
      bgPitch->setBrush(m_pitchGrad);
      bgPitch->setRect(xMap(m_chunkNr - 1), yAxis->mapValue(100), xSc + 0.5, yAxis->mapValue(50) - yAxis->mapValue(100));
      QGraphicsLineItem *pitchLine = new QGraphicsLineItem;
      scene->addItem(pitchLine);
      pitchLine->setPen(QPen(Qt::darkGreen, 1));
      pitchLine->setLine(xMap(m_chunkNr - 1), yAxis->mapValue(75),xMap(m_chunkNr), yAxis->mapValue(75));
      QGraphicsLineItem *pdifLine = new QGraphicsLineItem;
      scene->addItem(pdifLine);
      pdifLine->setPen(QPen(Qt::white, 2));
      pdifLine->setLine(xMap(m_chunkNr - 1), yAxis->mapValue(75) - m_pitchDiff * 50,
                        xMap(m_chunkNr), yAxis->mapValue(75) - (ad->pitch - (float)qRound(ad->pitch)) * 50);

    }
    m_pitchDiff = ad->pitch - (float)qRound(ad->pitch);
//     qDebug() << ad->pitch << m_pitchDiff;
  } else { // no note - silence or so
    m_volume = 0;
    m_pitchDiff = 100.0;
  }
  m_progresItem->setHtml(tr("Detecting...") + QString("<big><b> %1%</b></big>").arg(int(((qreal)m_chunkNr / (qreal)(m_totalXnr)) * 100)));
  m_progresItem->update();
}






