/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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

#ifndef TGRAPHICSLINE_H
#define TGRAPHICSLINE_H

#include <qgraphicsitem.h>

class TgraphicsTextTip;


/** This class represents a line on QGraphicsScene 
 * but it captures hover events and shows description text 
 * in TgraphicsTextTip. 
 * When text is empty, events are ignored. */
class TgraphicsLine : QObject, public QGraphicsLineItem
{
  Q_OBJECT

public:
  TgraphicsLine(QString text = "");
  virtual ~TgraphicsLine();
  void setText(QString text) { m_text = text; }
  
protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
  
protected slots:
  void delayedDelete();
  
private:
  QString m_text;
  TgraphicsTextTip *m_tip;
  QTimer *m_delTimer;
};

#endif // TGRAPHICSLINE_H
