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
#include <QGraphicsScene>
#include <QTimer>

TgraphicsStrikeItem::TgraphicsStrikeItem(const QRectF& rect, QGraphicsItem* parent) :
	QGraphicsObject(parent),
	m_rectF(rect)
{
	prepareLines(parent);
}



TgraphicsStrikeItem::TgraphicsStrikeItem(QGraphicsItem* parent) :
	QGraphicsObject(parent),
	m_rectF(parent->boundingRect())
{
	prepareLines(parent);
}


void TgraphicsStrikeItem::prepareLines(QGraphicsItem* parent) {
		QGraphicsLineItem *fakeLine = new QGraphicsLineItem();
		if (parent->type() == fakeLine->type()) { // strike of line like -/-/-/-/-/-/-/-/
			QGraphicsLineItem *parentLine = qgraphicsitem_cast<QGraphicsLineItem*>(parent);
			int lineSize = /*parentLine->scene()->sceneRect().height() / 20;*/ parentLine->pen().width() * 7;
			int linesCnt = m_rectF.width() / lineSize + 1;
			for (int i = 2; i < linesCnt - 2; i += 3) { // only horizontal lines without angle are supported
					qreal offset = parentLine->pen().width() * 3.0;
					qreal xx;
					for (int j = 0; j <2; j++) {
							QGraphicsLineItem *line = new QGraphicsLineItem();
							xx = parentLine->line().p1().x() + i * lineSize;
							if (j % 2)
								line->setLine(xx, parentLine->line().p1().y() - offset, xx + 2 * offset, parentLine->line().p1().y() + offset); // line
							else
								line->setLine(xx + 2 * offset, parentLine->line().p1().y() - offset, xx, parentLine->line().p1().y() + offset); // line /
							line->setParentItem(this);
							m_lines << line;
					}
			}
		} else { // strike of other shape like X
				qreal offset = m_rectF.height() / 3.0;
				for (int i = 0; i < 2; i++) {
					QGraphicsLineItem *line = new QGraphicsLineItem();
					if (i == 0)
						line->setLine(-offset, -offset, m_rectF.width() + offset, m_rectF.height() + offset); // line \ (top to bottom)
					else
						line->setLine(-offset, m_rectF.height() + offset, m_rectF.width() + offset, -offset); // line / (bottom to top)
					line->setParentItem(this);
					m_lines << line;
				}
		}
		delete fakeLine;
}


void TgraphicsStrikeItem::setPen(const QPen& pen) {
	for (int i = 0; i < m_lines.size(); i++)
		m_lines[i]->setPen(pen);
}


void TgraphicsStrikeItem::setColor(const QColor& penColor) {
		qreal pw = m_lines[0]->pen().widthF();
		for (int i = 0; i < m_lines.size(); i++)
				m_lines[i]->setPen(QPen(penColor, pw));
}


int TgraphicsStrikeItem::alpha() {
		return m_lines[0]->pen().color().alpha();
}


void TgraphicsStrikeItem::setAlpha(int alp) {
		QColor cc = m_lines[0]->pen().color();
		cc.setAlpha(alp);
		setColor(cc);
}


void TgraphicsStrikeItem::startBlinking() {
		m_blinkPhase = 0;
		QTimer::singleShot(150, this, SLOT(strikeBlinking()));
}


void TgraphicsStrikeItem::strikeBlinking() {
		m_blinkPhase++;
		if (m_blinkPhase < 5) {
			if (m_blinkPhase % 2) { // phase 1 & 3
					for (int i = 0; i < m_lines.size(); i++)
						m_lines[i]->hide();
			} else { // phase 2 & 4
					for (int i = 0; i < m_lines.size(); i++)
						m_lines[i]->show();
			}
			QTimer::singleShot(150, this, SLOT(strikeBlinking()));
		} else {
			emit blinkingFinished();
		}
}










