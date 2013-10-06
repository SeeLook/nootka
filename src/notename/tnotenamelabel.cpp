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


QString getBgColorText(const QColor& C) {
	if ( C != -1)
    return QString("background-color: rgba(%1, %2, %3, %4); ").arg(C.red()).arg(C.green()).arg(C.blue()).arg(C.alpha());
  else
    return QString("background-color: transparent; ");
}


/*static*/
QString TnoteNameLabel::borderStyleText() {
	return "border: 1px solid palette(Text); border-radius: 10px;";
}


TnoteNameLabel::TnoteNameLabel(const QString& text, QWidget* parent) :
	QLabel(text, parent),
	m_currentBlink(0)
{
	m_bgColor = palette().base().color();
	m_bgColor.setAlpha(220);
	setBackgroundColor(m_bgColor);
}


void TnoteNameLabel::setStyleSheet(const QString& style) {
	QLabel::setStyleSheet(borderStyleText() + m_bgColorText + style);
	m_styleText = style;
}


void TnoteNameLabel::setBackgroundColor(const QColor& color) {
	m_bgColor = color;
	m_bgColorText = getBgColorText(color);
	QLabel::setStyleSheet(borderStyleText() + m_bgColorText + m_styleText);
	repaint();
}



void TnoteNameLabel::blinkCross(const QColor& color, int count, int period) {
	m_color = color;
	m_count = count * 2;
	m_period = period;
	m_currentBlink = 0;
	strikeBlinkingSlot();
}


void TnoteNameLabel::crossFadeText(const QString& newText, const QColor& newBgColor, int duration) {
		m_newText = newText;
		m_newBgColor = newBgColor;
		m_fadeDuration = duration;
		m_fadePhase = 0;
		m_alphaStepOut = m_bgColor.alpha() / (m_fadeDuration / 60); // 2 x 30ms - half of duration
		m_alphaStepIn = m_newBgColor.alpha() / (m_fadeDuration / 60); // 2 x 30ms - half of duration
		crossFadeSlot();
}


//################################################################################################
//################################# PROTECTED    #################################################
//################################################################################################
void TnoteNameLabel::paintEvent(QPaintEvent* event) {
	QLabel::paintEvent(event);
	if (m_currentBlink % 2) {
		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
		painter.setPen(QPen(m_color, painter.viewport().height() / 10, Qt::SolidLine, Qt::RoundCap));
		int gap = painter.viewport().width() * 0.4;
		painter.drawLine(gap, 4, painter.viewport().width() - gap, painter.viewport().height() - 4);
		painter.drawLine(gap, painter.viewport().height() - 4, painter.viewport().width() - gap, 4);
	}
}


void TnoteNameLabel::strikeBlinkingSlot() {
	m_currentBlink++;
	if (m_currentBlink <= m_count) {
		QTimer::singleShot(m_period, this, SLOT(strikeBlinkingSlot()));
	} else {
		m_currentBlink = 0;
		emit blinkingFinished();
	}
	repaint();
}


void TnoteNameLabel::crossFadeSlot() {
	m_fadePhase++;
	if (m_fadePhase <= (m_fadeDuration / 30)) {
		if (m_fadePhase < (m_fadeDuration / 60)) { // fade out
			QColor newC = m_bgColor;
			newC.setAlpha(m_bgColor.alpha() - m_alphaStepOut);
			setBackgroundColor(newC);
		} else { // fade in
			if (m_alphaStepOut) { // m_alphaStepOut also indicates moment of switching fade out/in
				setText(m_newText); // new text
				m_alphaStepOut = 0;
			}
			QColor newC = m_newBgColor;
			newC.setAlpha((m_fadePhase - (m_fadeDuration / 60)) * m_alphaStepIn);
			setBackgroundColor(newC);
		}
		QTimer::singleShot(30, this, SLOT(crossFadeSlot()));
	} else {
		setBackgroundColor(m_newBgColor);
		emit crossFadeingFinished();
	}
}






