/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TTIPMELODY_H
#define TTIPMELODY_H

#include "ttipchart.h"

class QLabel;
class TmelodyView;
class QGraphicsWidget;

/** 
 * This is a tip displaying by charts when question/answers are melodies.
 * It has QWidget inside with TmultiScore and others widgets to represent required data.
 */
class TtipMelody : public TtipChart
{

  Q_OBJECT
  
public:
  TtipMelody(TquestionPoint* point);

  virtual ~TtipMelody();
  
  virtual QRectF boundingRect() const;
    
protected slots:
  void attemptChanged(int attNr);
  
private:
  QGraphicsWidget                 *m_widget;
  TmelodyView                     *m_score;
  QLabel                          *m_attemptLabel, *m_resultLabel;
  QWidget                         *m_w; /** content widget inside QGraphicsWidget */
  
};

#endif // TTIPMELODY_H
