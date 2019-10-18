/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tquestionpoint.h"
#include <exam/tqaunit.h>
#include <QtWidgets/QtWidgets>


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
QColor TquestionPoint::m_notBadColor = QColor(0xFF, 0x80, 0);
QColor TquestionPoint::m_shadowColor = QColor(63, 63, 63, 180); // gray with transparency
QColor TquestionPoint::m_bgColor = Qt::white;


TquestionPoint::TquestionPoint(const TqaPtr& ptr) :
  QGraphicsObject()
{
  p_qaPtr = ptr;
  setColor();
}


TquestionPoint::~TquestionPoint() {}


void TquestionPoint::setColor() {
  if (p_qaPtr.qaPtr->isCorrect())
      p_qaPtr.color = m_goodColor;
  else {
      if (p_qaPtr.qaPtr->isWrong())
        p_qaPtr.color = m_wrongColor;
      else
        p_qaPtr.color = m_notBadColor;
  }
//   auto shadow = new QGraphicsDropShadowEffect();
//   shadow->setBlurRadius(10);
//   shadow->setColor(qApp->palette().text().color());
//   shadow->setOffset(1.0, 1.0);
//   setGraphicsEffect(shadow);
}


void TquestionPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

  QRectF rect = boundingRect();
#if defined(Q_OS_MAC)
  painter->setFont(QFont("nootka", 35));
#else
  painter->setFont(QFont("nootka", 25));
#endif
  painter->setPen(p_qaPtr.color);
  rect.translate(-1, -1);
  QString glyph = QStringLiteral("n");
  if (p_qaPtr.qaPtr->isWrong())
    glyph = QStringLiteral("N");
  if (p_qaPtr.qaPtr->melody())
    glyph = QStringLiteral("m");
  painter->drawText(rect, Qt::AlignCenter, glyph);
}


QRectF TquestionPoint::boundingRect() const {
// QFontMetrics metrics = QFont("nootka", 25);
// QRectF rect = metrics.boundingRect("n");
  QRectF rect(-9, -29, 24, 41); // values calculated from above, hard-coded for speedy
  if (p_qaPtr.qaPtr->melody())
    rect.setRect(-11, -29, 24, 41);
  else if (p_qaPtr.qaPtr->isWrong())
    rect.setRect(-11, -10, 24, 41);
  return rect;
}




