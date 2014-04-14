/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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


#ifndef TNOTECONTROL_H
#define TNOTECONTROL_H

#include "tscoreitem.h"

class TscoreNote;
class TscoreStaff;



class TnoteControl : public TscoreItem
{

	Q_OBJECT
	
public:
	TnoteControl(TscoreStaff* staff, TscoreScene* scene);
	virtual ~TnoteControl();

	TscoreNote* scoreNote() { return m_scoreNote; }
	void setScoreNote(TscoreNote* sn);
	
	void adjustSize(); /** Grabs height from staff and adjust to it. */
	
	bool hasMouseCursor() { return m_hasMouse; }
	void hideWithDelay(int delay = 300);
	
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	virtual QRectF boundingRect() const;
		
protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	
protected slots:
	void hideDelayed();
		
private:
		TscoreNote											*m_scoreNote;
		qreal						 								 m_height;
		bool						 								 m_hasMouse;
		QGraphicsSimpleTextItem					*m_plus, *m_minus, *m_name;
};

#endif // TNOTECONTROL_H
