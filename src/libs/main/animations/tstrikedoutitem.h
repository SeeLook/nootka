/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#ifndef TGRAPHICSSTRIKEITEM_H
#define TGRAPHICSSTRIKEITEM_H

#include <QGraphicsObject>

class TblinkingItem;
class QPen;

/** This is QGraphicsObject element looks like striking cross over the given rectangle. 
 * Lines of striking are a bit longer than given rectangle size.
 * It recognizes a shape of item and strike can be 'X' or '--/---/---/---/---/---' */
class TstrikedOutItem : public QGraphicsObject
{
		Q_OBJECT
    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	
		TstrikedOutItem(QGraphicsItem *parent);
		TstrikedOutItem(const QRectF &rect, QGraphicsItem *parent);
		
				/** Defines a pen of striking lines. */
		void setColor(const QColor &color);
		void setPen(const QPen &pen);
		
		int alpha();
		void setAlpha(int alp);
		
				/** Starts blinking animation. */
		void startBlinking(int count = 2);
		
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {};
		virtual QRectF boundingRect() const { return m_rectF; }
		
signals:
		void strikedFInished();
		
protected slots:
		void blinkingSlot();

private:
		void prepareLines(QGraphicsItem *parent);
		
private:
    QList<QGraphicsLineItem*>		 		m_lines;
		QRectF							 					 	m_rectF;
		TblinkingItem										*m_blinking;


};

#endif // TGRAPHICSSTRIKEITEM_H
