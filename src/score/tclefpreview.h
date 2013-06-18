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

#ifndef TCLEFPREVIEW_H
#define TCLEFPREVIEW_H

#include "tscorestaff.h"

/** This is subclass of TscoreStaff to display previevs of clef.
* it hasn't got any notes, just staff and clef. */
class TclefPreview : public TscoreStaff
{
    Q_OBJECT

public:
    TclefPreview(TscoreScene *scene, Tclef clef);
    ~TclefPreview();
				
				/** Highlights a preview */
		void mark();
		
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect() const;

		
signals:
		void clicked(Tclef);
		
protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

		
private:
		QColor											m_bgColor;
		QGraphicsSimpleTextItem 		*m_desc; // a clef description
		bool 												m_marked;
		TscoreStaff									*m_lower;

};

#endif // TCLEFPREVIEW_H
