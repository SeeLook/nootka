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

#ifndef TBLINKINGITEM_H
#define TBLINKINGITEM_H

#include <QGraphicsObject>


/** T*/
class TblinkingItem : public QObject
{
		Q_OBJECT
    
public:
	
		explicit TblinkingItem(QGraphicsItem *item, QObject* parent = 0);
		
		
signals:
		void blinkingFinished();
		
public slots:
				/** Starts blinking animation. */
		void startBlinking(int count);
		
		
protected slots:
		void blinkingSlot();

private:
		QGraphicsItem 	*m_item;
		int	m_maxCount, m_blinkPhase; // identifies phases of strike blinking animation
		

};

#endif // TBLINKINGITEM_H
