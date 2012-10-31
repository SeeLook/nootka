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

#ifndef TSCORDATUREVIEW_H
#define TSCORDATUREVIEW_H

#include <QGraphicsView>
#include <tnote.h>

/**
 * This class represents scordature in a score.
 * Its position has to be set externaly by @fn setGeometry()
 * and by heigt() is the fonts size are determined
 * Scordatute is set by @fn setTune()
 * with tune and note name style in params.
  @author Tomasz Bojczuk  <tomaszbojczuk@gmail.com>
*/

class QResizeEvent;
class QGraphicsTextItem;
class Ttune;

class TscordatureView : public QGraphicsView
{

public:
    TscordatureView(QWidget *parent = 0);
    virtual ~TscordatureView();
      /** Sets tune and displays in given style. */
    void setTune(Ttune tune, Tnote::EnameStyle nameStyle);
  
  
protected:
    virtual void resizeEvent(QResizeEvent*);
    
private:
    QGraphicsScene *m_scene;
    QGraphicsTextItem *m_text; // dispalyed scoedature text
    bool m_hasScord;
};

#endif // TSCORDATUREVIEW_H

