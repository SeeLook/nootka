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

#ifndef TANIMEDITEM_H
#define TANIMEDITEM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"




/**
 * This class implements moving animation of QGraphicsItem.
 * Default duration is 150 ms and item is moving every 30 ms.
 */
class NOOTKACORE_EXPORT TmovedAnim : public TabstractAnim
{
    Q_OBJECT

public:
  explicit TmovedAnim(QGraphicsItem* item, QObject* parent = 0);

  bool isMoving() { return !(bool)m_currStep; }


public slots:
  void startMoving(const QPointF& start, const QPointF& stop);


protected slots:
  void animationRoutine();

private:
  QPointF                     m_startPos, m_endPos;
  int                         m_step, m_currStep;
  QGraphicsLineItem         *m_line;


};

#endif // TANIMEDITEM_H
