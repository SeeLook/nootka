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

#include "tnotenamelabel.h"
#include <QPainter>
#include <QTimer>



TnoteNameLabel::TnoteNameLabel(const QString& text, QWidget* parent) :
	QLabel(text, parent),
	m_currentBlink(0)
{

}


void TnoteNameLabel::blinkCross(const QColor& color, int count, int period) {
	m_color = color;
	m_count = count * 2;
	m_period = period;
	m_currentBlink = 0;
	crossBlinkingSlot();
}


//################################################################################################
//################################# PROTECTED    #################################################
//################################################################################################
void TnoteNameLabel::paintEvent(QPaintEvent* event) {
	QLabel::paintEvent(event);
	if (m_currentBlink % 2) {
		QPainter painter(this);
		painter.setPen(QPen(m_color, painter.viewport().height() / 10, Qt::SolidLine, Qt::RoundCap));
		int gap = painter.viewport().width() / 20;
		painter.drawLine(gap, 4, painter.viewport().width() - gap, painter.viewport().height() - 4);
		painter.drawLine(gap, painter.viewport().height() - 4, painter.viewport().width() - gap / 2, 4);
	}
}


void TnoteNameLabel::crossBlinkingSlot() {
	m_currentBlink++;
	if (m_currentBlink <= m_count) {
		QTimer::singleShot(m_period, this, SLOT(crossBlinkingSlot()));
	} else
		m_currentBlink = 0;
	update();
}






