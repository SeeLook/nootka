/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tcrossfadetextanim.h"
#include <QBrush>
#include <QGraphicsSimpleTextItem>
#include <QTimer>
#include <QEasingCurve>

QEasingCurve easingC(QEasingCurve::InExpo);

TcrossFadeTextAnim::TcrossFadeTextAnim(QGraphicsSimpleTextItem* textItem, QObject* parent) :
  QObject(parent),
  m_item(textItem)
{
  setDuration(150);
}


void TcrossFadeTextAnim::startCrossFading(const QString& newText, const QColor& newColor) {
  m_newText = newText;
  m_newColor = newColor;
  m_currStep = 0;
  m_alpha = m_item->brush().color().alpha();
  crossFadingSlot();
}


void TcrossFadeTextAnim::crossFadingSlot() {
  m_currStep++;
  if (m_currStep <= m_maxStep) {
    if (m_currStep < m_maxStep / 2) { // fade out
        setItemAlpha(m_item->brush().color().alpha() - (m_alpha / (m_maxStep / 2)));
//             easingC.valueForProgress((qreal)m_currStep / (m_maxStep / 2.0)));
    } else { // fade in
      if (m_currStep == m_maxStep / 2) {
          m_item->setText(m_newText);
          if (m_newColor != -1)
            m_item->setBrush(m_newColor);
      }
      setItemAlpha((m_currStep - (m_maxStep / 2)) * (m_alpha / (m_maxStep / 2)));
//         easingC.valueForProgress(((qreal)m_currStep - (m_maxStep / 2.0)) / (m_maxStep / 2.0)));
    }
    QTimer::singleShot(CLIP_TIME, this, SLOT(crossFadingSlot()));
  } else {
      setItemAlpha(m_alpha);
      emit crossFadingFinished();
  }
}


void TcrossFadeTextAnim::setItemAlpha(int alpha) {
    QColor color = m_item->brush().color();
    color.setAlpha(alpha);
    m_item->setBrush(QBrush(color));
}





