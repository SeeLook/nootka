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

#ifndef TSCORESCORDATURE_H
#define TSCORESCORDATURE_H

#include "tscoreitem.h"
#include "tnote.h"

class TscoreScene;

/**
 * This class represents scordature in a score.
 * Scordatute is set by @p setTune() with tune and note name style in params.
*/
class TscoreScordature : public TscoreItem
{

public:
    TscoreScordature(TscoreScene* scene);
    
          /** Sets tune and displays it in given style. */
    void setTune(Ttune &tune, Tnote::EnameStyle nameStyle);

    
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {};
    virtual QRectF boundingRect();

private:
    QGraphicsTextItem *m_scordText; // dispalyed scordature text

};

#endif // TSCORESCORDATURE_H
