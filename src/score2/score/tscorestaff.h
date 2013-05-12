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


#ifndef TSCORESTAFF_H
#define TSCORESTAFF_H

#include "tscoreitem.h"
#include <tclef.h>

class TscoreKeySignature;
class TscoreNote;
class TscoreClef;
class TscoreScene;


class TscoreStaff : public TscoreItem
{
    Q_OBJECT

public:
    TscoreStaff(TscoreScene *scene);
    virtual ~TscoreStaff();
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {};
    virtual QRectF boundingRect() const;
    
    char accidInKeyArray[7];
    
protected slots:
    void onClefChanged(Tclef::Etype);
    
private:    
    QGraphicsLineItem       *m_lines[5];
    TscoreClef              *m_clef;
    TscoreKeySignature      *m_keySignature;
    QList<TscoreNote*>      m_notes;
    

};

#endif // TSCORESTAFF_H
