/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TCLEFVIEW_H
#define TCLEFVIEW_H

#include <QGraphicsView>
#include "tscorewidgetsimple.h"

class QGraphicsScene;
class QGraphicsSimpleTextItem;
    /** @short This class represents clef on the score
    *
    * @author Tomasz Bojczuk  <tomaszbojczuk@gmail.com>
    */
class TclefView : public QGraphicsView
{
    friend class TscoreWidgetSimple;

    Q_OBJECT
public:
    explicit TclefView(TscoreWidgetSimple *parent = 0);

signals:

public slots:

protected:
        /** @class TscoreWidgetSimple as a parent has to take care about resizing process
        * so it calls this function in his resizeEvent and put @param coeff parameter */
    void resize(int co);

private:
    QGraphicsScene *m_scene;
    QGraphicsSimpleTextItem *m_clefText;
//    int m_coeff;

};

#endif // TCLEFVIEW_H
