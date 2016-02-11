/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#ifndef TROUNDEDLABEL_H
#define TROUNDEDLABEL_H

#include <nootkacoreglobal.h>
#include <QLabel>


/** 
 * This is subclass of QLabel with defined background and rounded rectangle shape. 
 * By default a background color is a variation of window color
 * but it can be set by calling setBackroundColor() 
 */
class NOOTKACORE_EXPORT TroundedLabel : public QLabel
{

public:
	explicit TroundedLabel(QWidget* parent = 0);
	explicit TroundedLabel(QString txt, QWidget* parent = 0);
	
	QString backgroundText();
	
	void setBackroundColor(const QColor& bgColor);
	QColor backgroundColor() { return m_bgColor; }
	void setDefaultBackground();
	
	void setStyleSheet(const QString &styleSheet);
	
private:
	QColor 			m_bgColor;
	QString 		m_styleText;
	
			/** Called by constructors  */
	void initBgColor();


};

#endif // TROUNDEDLABEL_H
