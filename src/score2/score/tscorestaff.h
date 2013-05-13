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
  
    enum Ekind {
      e_normal, // normal staff placed in the centre of score
      e_upper, // placed up - right hand of piano staff
      e_lower // placed down - left hand of piano staff
    }; // Kind of staff (normal or upper (right hand) or lower(left hand))
      
    TscoreStaff(TscoreScene *scene, Ekind kindOfStaff = e_normal);
    virtual ~TscoreStaff();
    
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {};
    virtual QRectF boundingRect() const;
        /** This array keeps values (-1, 0 or 1) for accidentals in key sign.
         * It is common for TscoreKeySignature and all TscoreNote. 
         * TscoreKeySignature::setAccidInKeyPointer and TscoreNote::setAccidInKeyPointer
         * have to be set to connect them. 
         * When TscoreKeySignature is deleted it should be set to 0. */
    char accidInKeyArray[7];
        /** Y position of upper line of a staff. */
    qreal upperLinePos() { return m_upperLinePos; }
    qreal height() { return m_height; }
        /** Kind of staff (normal or upper (right hand) or lower(left hand)) */
    Ekind kindOfStaff() { return m_kindOfStaff; }
    
protected slots:
    void onClefChanged(Tclef::Etype);
    
private:    
    QGraphicsLineItem       *m_lines[5];
    TscoreClef              *m_clef;
    TscoreKeySignature      *m_keySignature;
    QList<TscoreNote*>      m_notes;
    qreal                   m_upperLinePos;
    qreal                   m_height;
    Ekind                   m_kindOfStaff;
};

#endif // TSCORESTAFF_H
