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


#define WIDTH (40) // default width of a bar

Tbar::Tbar(qreal height_, TgroupedQAunit *qaGroup) :
    m_height(height_)
{

}

void Tbar::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->setBrush(Qt::red);
    QRectF rect = boundingRect();
    rect.translate(1, -1);
    painter->drawRoundedRect(rect, 3, 3);
    
}


QRectF Tbar::boundingRect() const {
    QRectF rect(WIDTH / -2, m_height * -1, WIDTH, m_height); // orign is in the centre on the bottom
    return rect;
}


