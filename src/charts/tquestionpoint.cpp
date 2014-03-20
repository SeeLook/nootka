/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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
#include "ttipchart.h"
#include <exam/tqaunit.h>
#include <graphics/tdropshadoweffect.h>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QGraphicsScene>


/* static */
void TquestionPoint::setColors(QColor goodColor, QColor wrongColor, QColor notBadColor, QColor shadowColor, QColor bgColor) {
    m_goodColor = goodColor;
    m_wrongColor = wrongColor;
    m_notBadColor = notBadColor;
    m_shadowColor = shadowColor;
    m_bgColor = bgColor;
}
QColor TquestionPoint::m_goodColor = Qt::darkGreen;
QColor TquestionPoint::m_wrongColor = Qt::red;
QColor TquestionPoint::m_notBadColor = QColor("#FF8000");
QColor TquestionPoint::m_shadowColor = QColor(63, 63, 63, 180); // gray with transparency
QColor TquestionPoint::m_bgColor = Qt::white;





TquestionPoint::TquestionPoint(TqaPtr qaPtr) :
  TtipHandler(),
  m_qaPtr(qaPtr)
{
    setColor();
}
  
 

void TquestionPoint::setColor() {
  if (m_qaPtr.qaPtr->isCorrect())
    m_color = m_goodColor;
  else {
    if (m_qaPtr.qaPtr->isWrong())
      m_color = m_wrongColor;
    else
      m_color = m_notBadColor;
  }
  TdropShadowEffect *shadow = new TdropShadowEffect(shadowColor());
  setGraphicsEffect(shadow);
}

TquestionPoint::~TquestionPoint() {}

  
void TquestionPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  
  QRectF rect = boundingRect();
#if defined(Q_OS_MAC)
  painter->setFont(QFont("nootka", 35));
#else
  painter->setFont(QFont("nootka", 25));
#endif
  painter->setPen(m_color);
  rect.translate(-1, -1);
//   if (m_question->isWrong())
//     setRotation(180);
  QString glyph = "n";
  if (m_qaPtr.qaPtr->isWrong())
    glyph = "N";
  painter->drawText(rect, Qt::AlignCenter, glyph);
}
  
QRectF TquestionPoint::boundingRect() const {
//   QFontMetrics metrics = QFont("nootka", 25);
//   QRectF rect = metrics.boundingRect("n");
  QRectF rect(-9, -29, 24, 41); // values calculated from above, hardcoded for speedy
  if (m_qaPtr.qaPtr->isWrong())
    rect.setRect(-11, -10, 24, 41);;
  return rect;
}
  
void TquestionPoint::hoverEnterEvent(QGraphicsSceneHoverEvent* event ) {
    if (tip) {
        if (tip == initObject())
          return;
        else
          deleteTip();
		}
    tip = new TtipChart(this);
    handleTip(event->scenePos());
}


