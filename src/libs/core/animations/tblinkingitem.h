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

#ifndef TBLINKINGITEM_H
#define TBLINKINGITEM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"



/**
 * This class performs blinking of QGraphicsItem.
 */
class NOOTKACORE_EXPORT TblinkingItem : public TabstractAnim
{
    Q_OBJECT

public:

    explicit TblinkingItem(QGraphicsItem *item, QObject* parent = 0);


public slots:
        /** Starts blinking animation. */
    void startBlinking(int count = 2);


protected slots:
    virtual void animationRoutine();

};

#endif // TBLINKINGITEM_H
