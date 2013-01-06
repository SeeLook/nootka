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


#ifndef TBAR_H
#define TBAR_H

#include <QGraphicsItem>

class TgraphicsTextTip;
class QGraphicsSceneHoverEvent;
class TgroupedQAunit;

class Tbar : public QGraphicsItem
{

public:
  
    Tbar(qreal height, TgroupedQAunit* qaGroup);
    QString getTipText();
    
    
protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QRectF boundingRect() const;
    
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* );
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* );
  
private:
    qreal m_height;
    TgroupedQAunit *m_qaGroup;
    qreal m_wrongAt, m_notBadAt; // Keeps position of color gradient for mistakes
    TgraphicsTextTip *m_tip;
};

#endif // TBAR_H
