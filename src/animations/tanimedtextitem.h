/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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

#ifndef TANIMEDTEXTITEM_H
#define TANIMEDTEXTITEM_H

#include <QObject>
#include <QGraphicsSimpleTextItem>


class TanimedTextItem : public QObject, public QGraphicsSimpleTextItem
{
    Q_OBJECT
    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    
public:
  TanimedTextItem();
  virtual ~TanimedTextItem();
  int alpha();
  void setAlpha(int al);
  
protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
};

#endif // TANIMEDTEXTITEM_H
