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

#ifndef TSCORECONTROL_H
#define TSCORECONTROL_H

#include <QWidget>

class QGraphicsProxyWidget;
class TscoreScene;
class TpushButton;


class TscoreControl : public QWidget
{
    Q_OBJECT

public:
  
    TscoreControl(TscoreScene *scene);
    
    QGraphicsProxyWidget* proxy() { return m_proxy;}

protected:
    void setButtons(TpushButton *button);
    

private:
    TpushButton *m_sharpBut, *m_flatBut, *m_dblSharpBut, *m_dblFlatBut;
    QGraphicsProxyWidget *m_proxy;
};

#endif // TSCORECONTROL_H
