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

#include "tgraphicsstrikeitem.h"
#include <QPen>
#include <QTimer>


TgraphicsStrikeItem::TgraphicsStrikeItem(QGraphicsItem* parent) :
	QGraphicsObject(parent)
{
// 		m_rectF = parent->boundingRect().adjusted(-2.0, -2.0, 4.0, 4.0); // 4 pixels bigger
		m_rectF = parent->boundingRect();
		m_line1 = new QGraphicsLineItem();
			m_line1->setLine(-1.0, -1.0, m_rectF.width() + 1.0, m_rectF.height() + 1.0); // line \ (top to bottom)
			m_line1->setParentItem(this);
		m_line2 = new QGraphicsLineItem();
			m_line2->setLine(-1.0, m_rectF.height() + 1.0, m_rectF.width() + 1.0, -1.0); // line / (bottom to top)
			m_line2->setParentItem(this);
// 		setPos(parent->pos().x() - 2.0, parent->pos().y() - 2.0);
// 			setPos(parent->pos());
}


void TgraphicsStrikeItem::setPen(const QPen& pen) {
		m_line1->setPen(pen);
		m_line2->setPen(pen);
}


void TgraphicsStrikeItem::setColor(const QColor& penColor) {
		qreal pw = m_line1->pen().widthF();
		m_line1->setPen(QPen(penColor, pw));
		m_line2->setPen(QPen(penColor, pw));
}


int TgraphicsStrikeItem::alpha() {
		return m_line1->pen().color().alpha();
}


void TgraphicsStrikeItem::setAlpha(int alp) {
		QColor cc = m_line1->pen().color();
		cc.setAlpha(alp);
		setColor(cc);
}


void TgraphicsStrikeItem::startBlinking() {
// 		if (m_blinkPhase == 0)
				m_blinkPhase = 0;
				QTimer::singleShot(100, this, SLOT(strikeBlinking()));
}


void TgraphicsStrikeItem::strikeBlinking() {
		m_blinkPhase++;
		if (m_blinkPhase < 6) {
			if (m_blinkPhase % 2) { // phase 1 & 3
				m_line1->hide();
				m_line2->hide();
			} else { // phase 2 & 4
				m_line1->show();
				m_line2->show();
			}
			QTimer::singleShot(100, this, SLOT(strikeBlinking()));
		} else {
			emit blinkingFinished();
// 			m_blinkPhase = 0;
		}
}










