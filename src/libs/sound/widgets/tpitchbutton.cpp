/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tpitchbutton.h"
#include <QPainter>



TpitchButton::TpitchButton(const QString& text, QWidget* parent) :
	QAbstractButton(parent),
	m_entered(false)
{
	setText(text);
	setMinimumSize(24, 17);
	setCheckable(true);
}


void TpitchButton::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
	painter.setPen(Qt::NoPen);
	QColor bg;
	if (m_entered)
		bg = palette().color(palette().currentColorGroup(), QPalette::Highlight);
	else {
		bg = palette().color(QPalette::Disabled, QPalette::Text);
		bg.setAlpha(150);
	}
	painter.setBrush(bg);
	qreal radius = qMax(height() / 6.0, 3.0);
	painter.drawRoundedRect(1, 1, width() - 1, height() - 1, radius, radius);
	radius *= 1.5;
	if (isChecked()) {
			painter.setPen(QPen(Qt::red, radius, Qt::SolidLine, Qt::RoundCap));
			painter.drawLine(QLineF(width() / 2.0 - radius * 1.2, radius, width() / 2.0 - radius  * 1.2, height() - radius));
			painter.drawLine(QLineF(width() / 2.0 + radius  * 0.7, radius, width() / 2.0 + radius  * 0.7, height() - radius));
	} else {
			painter.setBrush(Qt::red);
			painter.drawEllipse(4, 3, width() - 6, height() - 6);
	}
}


QSize TpitchButton::sizeHint() const {
	return QAbstractButton::sizeHint();
}

void TpitchButton::enterEvent(QEvent* ) {
	m_entered = true;
	update();
}

void TpitchButton::leaveEvent(QEvent* ) {
	m_entered = false;
	update();
}

void TpitchButton::resizeEvent(QResizeEvent* ) {
	QFont f("nootka");
	f.setPointSizeF(20.0);
	QFontMetrics fm(f);
	f.setPointSizeF(20.0 * (((qreal)height() * 0.9) / (qreal)fm.boundingRect(text()).height()));
	setFont(f);
	setFixedWidth(height());
}










