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
		if (isChecked()) {
			bg = Qt::red;
		} else {
			bg = Qt::darkGreen;
		}
	}
	painter.setBrush(bg);
// 	painter.setPen(QPen(isChecked() ? Qt::red : Qt::green, 2));
	painter.drawEllipse(1, 1, width() - 1, height() - 1);
	painter.setPen(QPen(Qt::lightGray));
	painter.drawText(1, 1, width() - 1, height() - 1, Qt::AlignCenter, text());
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
	f.setPointSizeF(20.0 * (((qreal)height() * 0.85) / (qreal)fm.boundingRect(text()).height()));
	setFont(f);
	setFixedWidth(height());
}










