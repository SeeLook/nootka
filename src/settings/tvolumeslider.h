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


#ifndef TVOLUMESLIDER_H
#define TVOLUMESLIDER_H

#include <QWidget>

class QSpinBox;
class QSlider;

class TvolumeSlider : public QWidget
{
  
  Q_OBJECT
  
public:
    TvolumeSlider(QWidget * parent = 0);
    
    void setValue(int val);
    int value();
    
    
private slots:
//     void spinSlot(int val);
    void sliderSlot(int val);
    
private:
    QSlider     *m_slider;
    QSpinBox    *m_spinBox;
  
};

#endif // TVOLUMESLIDER_H
