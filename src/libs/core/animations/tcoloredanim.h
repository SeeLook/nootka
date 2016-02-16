/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
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

#ifndef TCOLOREDANIM_H
#define TCOLOREDANIM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"

/** 
 * This class performs transforming of colors.
 * Alpha channel is supported as well.
 * WARRING! For QGraphicsTextItem items, start color is taken from 
 * QGraphicsTextItem::defaultTextColor(), so declare it first
 */
class NOOTKACORE_EXPORT TcoloredAnim : public TabstractAnim
{
    Q_OBJECT
    
public:
	
	explicit TcoloredAnim(QGraphicsItem* item = 0, QObject* parent = 0);

	void startColoring(const QColor &endColor, const QColor &midColor = -1);
	
	
protected slots:
	void animationRoutine();
	
private:
			/** Grabs color of given item by examining its type. */
	QColor getColorFromItem();
	
    
private:
    QColor 											 m_startColor, m_endColor, m_midColor, m_colorToGo;
		QGraphicsLineItem						*m_line;
		QGraphicsTextItem						*m_richText;
		QGraphicsSimpleTextItem			*m_text;
		QGraphicsEllipseItem				*m_ellipse;
};

#endif // TCOLOREDANIM_H
