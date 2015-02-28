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
#include <QtCore/QDebug>

double m_volume = 0, m_pitchDiff = 100.0;
QGraphicsTextItem *m_progresItem;
int m_totalXnr = 0;
int m_prevNoteIndex = -1;
QLinearGradient m_pitchGrad(0.5, 0, 0.5, 1);

Nchart::Nchart(QWidget* parent) :
  Tchart(parent),
  m_pitchF(0)
{
  yAxis->hide();
  scale(0.05, 1.0);
  yAxis->setMaxValue(200, false);
//   xAxis->setQuestionWidth(1);
  m_progresItem = new QGraphicsTextItem;
  scene->addItem(m_progresItem);
  m_progresItem->setZValue(250);
  m_progresItem->setScale(2);
  m_progresItem->hide();
  m_progresItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
  m_pitchGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
  m_pitchGrad.setColorAt(0, QColor(117, 21, 86, 50));
  m_pitchGrad.setColorAt(0.2, QColor(255, 0, 0, 50));
  m_pitchGrad.setColorAt(0.35, QColor(255, 255, 0, 50));
  m_pitchGrad.setColorAt(0.5, QColor(0, 255, 0, 50));
  m_pitchGrad.setColorAt(0.65, QColor(255, 255, 0, 50));
  m_pitchGrad.setColorAt(0.8, QColor(255, 0, 0, 50));
  m_pitchGrad.setColorAt(1, QColor(117, 21, 86, 50));
}


void Nchart::setXnumber(int xN) {
  xAxis->setLength(xAxis->questWidth() * xN);
  m_totalXnr = xN;
}


void Nchart::setPitchFinder(TpitchFinder* pf) {
  m_pitchF = pf;
  m_chunkNr = 0;
  connect(m_pitchF, &TpitchFinder::chunkProcessed, this, &Nchart::chunkSlot);
  m_progresItem->show();
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
      volLine->setLine(xAxis->pos().x() + xAxis->mapValue(m_chunkNr - 1),
                       yAxis->mapValue(100 + m_volume),
                       xAxis->pos().x() + xAxis->mapValue(m_chunkNr),
                       yAxis->mapValue(100 + dB2Normalised(ad->logrms()) * 100.0));
    }
    m_volume = dB2Normalised(ad->logrms()) * 100.0;
    if (m_prevNoteIndex != ad->noteIndex) {
      QGraphicsTextItem *pitchLab = new QGraphicsTextItem();
      pitchLab->setFlag(QGraphicsItem::ItemIgnoresTransformations);
      scene->addItem(pitchLab);
      pitchLab->setHtml(Tnote(qRound(ad->pitch) - 47).toRichText());
      pitchLab->setDefaultTextColor(Qt::blue);
      pitchLab->setPos(xAxis->pos().x() + xAxis->mapValue(m_chunkNr), yAxis->mapValue(50) - pitchLab->boundingRect().height());
      m_prevNoteIndex = ad->noteIndex;
    }
    if (m_pitchDiff < 100.0) {
      QGraphicsRectItem *bgPitch = new QGraphicsRectItem;
      scene->addItem(bgPitch);
      bgPitch->setPen(Qt::NoPen);
      bgPitch->setBrush(m_pitchGrad);
      bgPitch->setRect(xAxis->pos().x() + xAxis->mapValue(m_chunkNr - 1), yAxis->mapValue(100), xAxis->questWidth() + 1,
                       yAxis->mapValue(50) - yAxis->mapValue(100));
      QGraphicsLineItem *pitchLine = new QGraphicsLineItem;
      scene->addItem(pitchLine);
      pitchLine->setPen(QPen(palette().text().color(), 1));
      pitchLine->setLine(xAxis->pos().x() + xAxis->mapValue(m_chunkNr - 1), yAxis->mapValue(75),
                        xAxis->pos().x() + xAxis->mapValue(m_chunkNr), yAxis->mapValue(75));
      QGraphicsLineItem *pdifLine = new QGraphicsLineItem;
      scene->addItem(pdifLine);
      pdifLine->setPen(QPen(Qt::blue, 2));
      pdifLine->setLine(xAxis->pos().x() + xAxis->mapValue(m_chunkNr - 1),
                        yAxis->mapValue(75) - m_pitchDiff * 50,
                        xAxis->pos().x() + xAxis->mapValue(m_chunkNr),
                        yAxis->mapValue(75) - (ad->pitch - (float)qRound(ad->pitch)) * 50);

    }
    m_pitchDiff = ad->pitch - (float)qRound(ad->pitch);
//     qDebug() << ad->pitch << m_pitchDiff;
  } else { // no note - silence or so
    m_volume = 0;
    m_pitchDiff = 100.0;
  }
  if (m_chunkNr < m_totalXnr - 1) {
    m_progresItem->setHtml(tr("Detecting...") + QString("<big><b> %1%</b></big>").arg(int(((qreal)m_chunkNr / (qreal)(m_totalXnr)) * 100)));
    m_progresItem->update();
  } else {
    m_progresItem->hide();
    ajustChartHeight();
  }
}






