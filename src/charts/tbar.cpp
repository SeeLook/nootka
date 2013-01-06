/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tbar.h"
#include <QPainter>
#include <QGraphicsColorizeEffect>
#include "tquestionpoint.h"
#include "tgroupedqaunit.h"


#define WIDTH (30) // default width of a bar


Tbar::Tbar(qreal height, TgroupedQAunit* qaGroup) :
    m_height(height),
    m_qaGroup(qaGroup)
{
    m_wrongAt = (qreal)m_qaGroup->mistakes() / (qreal)m_qaGroup->size();
    m_notBadAt = (qreal)m_qaGroup->notBad() / (qreal)m_qaGroup->size();
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setOffset(1, 1);
    shadow->setColor(TquestionPoint::shadowColor());
    setGraphicsEffect(shadow);
}

void Tbar::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    QRectF rect = boundingRect();
    rect.translate(1, -1);
    QLinearGradient grad(WIDTH / 2, -rect.height(), 0, 0);
    if (m_wrongAt)
      grad.setColorAt(0.0, TquestionPoint::wrongColor());
    else if (m_notBadAt)
      grad.setColorAt(0.0, TquestionPoint::notBadColor());
    if (m_qaGroup->notBad())
      grad.setColorAt(m_wrongAt + m_notBadAt, TquestionPoint::notBadColor());
    grad.setColorAt(1.0 - m_wrongAt - m_notBadAt, TquestionPoint::goodColor());
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 3, 3);
    
}


QRectF Tbar::boundingRect() const {
    QRectF rect(WIDTH / -2, m_height * -1, WIDTH, m_height); // orign is in the centre on the bottom
    return rect;
}


