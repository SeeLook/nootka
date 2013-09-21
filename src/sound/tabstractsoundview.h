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


#ifndef TABSTRACTSOUNDVIEW_H
#define TABSTRACTSOUNDVIEW_H

#include <QtGui/QWidget>


class TabstractSoundView : public QWidget {
  
public:
  
    explicit TabstractSoundView(QWidget* parent = 0);
    
    QColor gradColorAtPoint(float lineX1, float lineX2, QColor startC, QColor endC, float posC);
    
    static QColor startColor, middleColor, endColor, totalColor;
    QColor tc;
    void setDisabled(bool isDisabled);
    
protected:
    virtual void resizeIt(int myHeight);
    
protected:
    QFont           nootFont;
    QRect           noteBound;
  
};



#endif // TABSTRACTSOUNDVIEW_H