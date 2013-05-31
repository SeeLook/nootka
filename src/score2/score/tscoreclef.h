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

class TclefSelector;
class TscoreClef : public TscoreItem
{
    Q_OBJECT    

public:
    TscoreClef(TscoreScene *scene, TscoreStaff *staff, Tclef clef);
    ~TscoreClef();

    Tclef clef() { return m_clef; }
    void setClef(Tclef clef);
    
    
    static QChar clefToChar(Tclef clef);
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QRectF boundingRect() const;
    
    void setReadOnly(bool ro) { m_readOnly = ro; } // hover mouse events are ignored 
    bool readOnly() { return m_readOnly; } // when TTRUE, hover mouse events are ignored
    bool isClickable() { return m_isClickable; } // mouse pressing are ignored
		void setIsClickable(bool isIt) { m_isClickable = isIt; } // mouse pressing are ignored
    
signals:
    void clefChanged();
		void switchPianoStaff(Tclef);
    
protected:
    void wheelEvent(QGraphicsSceneWheelEvent* event);
		void mousePressEvent(QGraphicsSceneMouseEvent* event);
		
protected slots:
		void clefSelected(Tclef clef);
    
private:
    int getYclefPos(Tclef clef);
    int getClefPosInList(Tclef clef);

private:
    Tclef                             m_clef;
    QGraphicsSimpleTextItem           *m_textClef;
		TclefSelector											*m_selector;
    
    int                               m_currClefInList;    
        /** List of all clef types exept empty (none clef) */
    static QList<Tclef::Etype>        m_typesList;
    bool                              m_readOnly; // hover mouse events are ignored
		bool 															m_isClickable; // mouse pressing are ignored

};

#endif // TSCORECLEF_H
