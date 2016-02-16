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


#ifndef TBAR_H
#define TBAR_H


#include "ttiphandler.h"
#include "tstatisticstip.h"

class QGraphicsSceneHoverEvent;
class TgroupedQAunit;

class Tbar : public TtipHandler
{

public:
  
    Tbar(qreal height, TgroupedQAunit* qaGroup, TstatisticsTip::Ekind tipType = TstatisticsTip::e_full);
    virtual ~Tbar();
    
    
protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QRectF boundingRect() const;
    
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
  
private:
    qreal m_height;
    TgroupedQAunit *m_qaGroup;
    qreal m_wrongAt, m_notBadAt; // Keeps position of color gradient for mistakes
        /** Static type of a tip. Described in TstatisticsTip class. */
    static TstatisticsTip::Ekind m_tipType;
    bool m_isUnderMouse;
};

#endif // TBAR_H
