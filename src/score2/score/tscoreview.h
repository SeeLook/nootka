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

#ifndef TSCOREVIEW_H
#define TSCOREVIEW_H

#include <QGraphicsView>


/** Main purpose of this clas (and only one for this moment)
 * is to cover scrollContentsBy() mwthod and disable scroling by wheel. */
class TscoreView : public QGraphicsView
{
public:
    TscoreView(QWidget* parent = 0);

protected:
    virtual void scrollContentsBy(int dx, int dy) {};

};

#endif // TSCOREVIEW_H
