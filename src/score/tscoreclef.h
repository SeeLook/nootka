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

#ifndef TSCORECLEF_H
#define TSCORECLEF_H

#include "tscoreitem.h"
#include "tclef.h"

class TclefMenu;


/** It implements clef on the score.
*	Clef can be changed by mouse click. It displays @class TclefMenu (with clefs to select).
* Piano staff can be selected or deselected then. In this case @p switchPianoStaff(Tclef) is emited,
* with selected clef as a parameter.
* @p readOnly() controls when @class TscoreClef can work that way.
* Static @p clefToChar(Tclef) method converts Tclef::type() to appropirate glyph in nootka font.
*/
class TscoreClef : public TscoreItem
{
    Q_OBJECT    

public:
    TscoreClef(TscoreScene *scene, TscoreStaff *staff, Tclef clef);
    ~TscoreClef();

    Tclef clef() { return m_clef; }
    void setClef(Tclef clef);
    
				/** Converts Tclef to appropirate QChar with clef glyph in nootka font. */
    static QChar clefToChar(Tclef clef);
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QRectF boundingRect() const;
    
    void setReadOnly(bool ro) { m_readOnly = ro; getStatusTip(); } // when TRUE clef is locked
    bool readOnly() { return m_readOnly; } // when TRUE clef is locked
    
signals:
    void clefChanged();
		void switchPianoStaff(Tclef);
    
protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* event);
		
private:
    int getYclefPos(Tclef clef);
    int getClefPosInList(Tclef clef);
				/** Generates and refresh status tip depends on readOnly() and isClickable() state. */
		void getStatusTip();

private:
    Tclef                             m_clef;
    QGraphicsSimpleTextItem           *m_textClef;
		TclefMenu													*m_clefMenu;
    
    int                               m_currClefInList;    
        /** List of all clef types exept empty (none clef) and piano staff. */
    static QList<Tclef::Etype>        m_typesList;
    bool                              m_readOnly; // when TRUE clef is locked

};

#endif // TSCORECLEF_H
