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

#include "troundedlabel.h"


TroundedLabel::TroundedLabel(QWidget* parent) : 
	QLabel(parent)
{
	setContentsMargins(5, 5, 5, 5);
	initBgColor();
}


TroundedLabel::TroundedLabel(QString txt, QWidget* parent) :
	QLabel(txt, parent)
{
	setContentsMargins(5, 5, 5 , 5);
	initBgColor();
}


void TroundedLabel::setStyleSheet(const QString& styleSheet) {
		QLabel::setStyleSheet(backgroundText() + styleSheet);
		m_styleText = styleSheet;
}


void TroundedLabel::setBackroundColor(const QColor &bgColor) {
		m_bgColor = bgColor;
		QLabel::setStyleSheet(backgroundText() + m_styleText);
}


void TroundedLabel::setDefaultBackground() {
		QString tmpStyle = m_styleText;
		initBgColor();
		setStyleSheet(tmpStyle);
}



QString TroundedLabel::backgroundText() {
    return QString("background-color: rgba(%1, %2, %3, %4); border-radius: 10px; ")
				.arg(m_bgColor.red()).arg(m_bgColor.green()).arg(m_bgColor.blue()).arg(m_bgColor.alpha());
}

//######################################### PRIVATE ######################################################
void TroundedLabel::initBgColor() {
		if (parentWidget()) {
#if defined(Q_OS_WIN32)
				m_bgColor = parentWidget()->palette().window().color().lighter(101);
#else
				m_bgColor = parentWidget()->palette().window().color().lighter(105);
#endif
		} else {
#if defined(Q_OS_WIN32)
				m_bgColor = palette().window().color().lighter(101);
#else
				m_bgColor = palette().window().color().lighter(105);
#endif
		}
		QLabel::setStyleSheet(backgroundText());
		m_styleText = "";
}



