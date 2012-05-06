/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.                                                  *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tquestionpoint.h"
#include "tmainline.h"
#include "tqaunit.h"
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsEffect>


/* static */
void TquestionPoint::setColors(QColor goodColor, QColor wrongColor, QColor notBadColor) {
    m_goodColor = goodColor;
    m_wrongColor = wrongColor;
    m_notBadColor = notBadColor;
}
QColor TquestionPoint::m_goodColor = Qt::darkGreen;
QColor TquestionPoint::m_wrongColor = Qt::red;
QColor TquestionPoint::m_notBadColor = Qt::darkMagenta;





TquestionPoint::TquestionPoint(TmainLine* parent, TQAunit* question):
  QGraphicsItem(),
  m_question(question),
  m_parent(parent)
{
  setAcceptHoverEvents(true);

  if (question->correct())
    m_color = m_goodColor;
  else {
    if (question->wrongNote())
      m_color = m_wrongColor;
    else
      m_color = m_notBadColor;
  }
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
  shadow->setBlurRadius(2);
  shadow->setOffset(2, 2);
  setGraphicsEffect(shadow);
  
}
  
  
  
void TquestionPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  
  QRectF rect = boundingRect();
  painter->setFont(QFont("nootka", 25));
  painter->setPen(m_color);
  rect.translate(-1, -1);
  painter->drawText(rect, Qt::AlignCenter, "n");
}
  
QRectF TquestionPoint::boundingRect() const {
//   QFontMetrics metrics = QFont("nootka", 25);
//   QRectF rect = metrics.boundingRect("n");
  QRectF rect(-11, -29, 24, 41); // values calculated from above, hardcoded for speedy
  return rect;
}
  
void TquestionPoint::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    m_parent->showTip(question(), pos());  
}


void TquestionPoint::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  m_parent->deleteTip();
}

