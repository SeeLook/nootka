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


#ifndef TFINISHTIP_H
#define TFINISHTIP_H
#include <QGraphicsObject>

class TgraphicsTextTip;
class Texam;

QString finishExamText(Texam *exam);


/** QGraphicsTextTip with exam summary like piece of paper. */
class TnootkaCertificate : public QGraphicsObject
{
    Q_OBJECT
    
public:
    TnootkaCertificate(Texam *exam, QGraphicsObject *parent = 0);
		
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {};
    
		
protected:
			/** Can be used to show hints as well. */
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event = 0);
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event = 0);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		
		void hideHints();
		void removeHints();
		void createHints();
	
protected slots:
		void saveSlot();
    
private:
    Texam 									*m_exam;
		TgraphicsTextTip 				*m_cert, *m_saveHint;
		QGraphicsPixmapItem 		*m_saveIcon, *m_closeIcon;
    
};

#endif // TFINISHTIP_H
