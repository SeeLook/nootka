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
	QLabel::setStyleSheet(backgroundText());
}


TroundedLabel::TroundedLabel(QString txt, QWidget* parent) :
	QLabel(txt, parent)
{
	QLabel::setStyleSheet(backgroundText());
}


void TroundedLabel::setStyleSheet(const QString& styleSheet) {
		QLabel::setStyleSheet(backgroundText() + styleSheet);
}


QString TroundedLabel::backgroundText() {
#if defined(Q_OS_WIN32)
    QColor bgLight = palette().window().color().lighter(101);
#else
    QColor bgLight = palette().window().color().lighter(105);
#endif
    return QString("background-color: %1; border-radius: 10px; ").arg(bgLight.name());
}



