/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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

#include "tgraphicstexttip.h"
#include "tquestionpoint.h"
#include <QTextBlockFormat>
#include <QTextCursor>


/* static */
void TgraphicsTextTip::alignCenter(TgraphicsTextTip* tip) {
    tip->setTextWidth(tip->boundingRect().width());
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    QTextCursor cursor = tip->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    cursor.clearSelection();
    tip->setTextCursor(cursor);
}



TgraphicsTextTip::TgraphicsTextTip(QString text, QColor bgColor) :
  QGraphicsTextItem(),
  m_bgColor(bgColor)
{

}

TgraphicsTextTip::~TgraphicsTextTip() {

}

void TgraphicsTextTip::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  if (m_bgColor != -1) {
    QRectF rect = boundingRect();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(TquestionPoint::bgColor()));
    painter->drawRoundedRect(rect, 5, 5);
    QColor startColor = m_bgColor;
    startColor.setAlpha(25);
    QColor endColor = startColor;
    endColor.setAlpha(75);
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0.1, startColor);
    grad.setColorAt(0.9, endColor);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 5, 5);
  }

    QGraphicsTextItem::paint(painter, option, widget);

}
