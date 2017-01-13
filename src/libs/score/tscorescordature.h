/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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

#ifndef TSCORESCORDATURE_H
#define TSCORESCORDATURE_H

#include <nootkacoreglobal.h>
#include "tscoreitem.h"
#include <music/tnote.h>

class Ttune;
class TscoreScene;

/**
 * This class represents scordature in a score.
 * Scordatute is set by @p setTune() with tune as a parameter.
*/
class NOOTKACORE_EXPORT TscoreScordature : public TscoreItem
{

public:

    TscoreScordature(TscoreScene* scene, TscoreStaff *staff);

        /** Sets tune and displays it in given style. */
    void setTune(Ttune& tune);
        /** Returns @p true when tune was different than standard and something is displayed. */
    bool isScordatured() { return (bool)m_scordText; }

    virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
    virtual QRectF boundingRect() const;

private:
    QGraphicsTextItem   *m_scordText; // displayed scordature text
    qreal               m_height;

};

#endif // TSCORESCORDATURE_H
