/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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

#ifndef TNOOTKALABEL_H
#define TNOOTKALABEL_H

#include "nootkamiscglobal.h"
#include <QWidget>
#include <QGraphicsView>

class QGraphicsColorizeEffect;

/** 
 * This is QGraphicsView with given image (and its size)
 * colored with given color (palette().window().color() by default).
 * When @setHoverColor() is called then given color is set when mouse is over
 */
class NOOTKAMISC_EXPORT TnootkaLabel : public QGraphicsView
{

	Q_OBJECT
	
public:
	TnootkaLabel(const QString& pixmapPath, QWidget *parent = 0, QColor bgColor= -1, const QString& version = "");
	
	void setHoverColor(const QColor& hoverCol) { m_hoverColor = hoverCol; } /** Color when mouse is over */

signals:
	void clicked();
		
protected:
	virtual void resizeEvent(QResizeEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual bool event(QEvent* event);
    
private:
	QGraphicsPixmapItem 					*m_pixItem;
	QGraphicsColorizeEffect 			*m_effect;
	QColor												 m_bgColor, m_hoverColor;
	
};

#endif // TNOOTKALABEL_H
