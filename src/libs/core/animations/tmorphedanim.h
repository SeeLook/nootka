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

#ifndef TMORPHEDANIM_H
#define TMORPHEDANIM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"

/**
 * It imitates transforming ellipse into line and opposite.
 * Ellipse is stretched to imitate line. 
 * Position of ellipse remains untouched. 
 */
class NOOTKACORE_EXPORT TmorphedAnim : public TabstractAnim
{
    Q_OBJECT

public:
	explicit TmorphedAnim(QGraphicsEllipseItem* ellipse = 0, QObject* parent = 0);

			/** Starts animation. When @p toLine is false It transforms line to ellipse 
			 * otherwise ellipse to line */
	void startMorphing(const QLineF& line, qreal width, bool toLine = true);
	
protected slots:
	void animationRoutine();
	
private:
	QLineF											m_line;
	QGraphicsEllipseItem				*m_ellipse;	
	bool 												m_toLine;
	QPointF											m_startPos;
	qreal												m_width;
};

#endif // TMORPHEDANIM_H
