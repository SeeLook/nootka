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

#ifndef TSCORENOTE_H
#define TSCORENOTE_H

#include "tscoreitem.h"

class TcrossFadeTextAnim;
class TanimedItem;
class TscoreControl;
class TscoreScene;


class TscoreNote : public TscoreItem
{
  Q_OBJECT
  
public:
    TscoreNote(TscoreScene *scene, TscoreStaff *staff, int index);
    ~TscoreNote();
    
				/** Index of this note instance. */
		int index() { return m_index; }
        /** Hides main note */
    void hideNote();
        /** Hides pointing (work) note */
    void hideWorkNote();
        /** Sets color of main note. */
    void setColor(QColor color);
        /** Sets color of pointing (work) note. */
    void setPointedColor(QColor color);
				/** It sets background of the note segment. When sets to -1 means transparent - no background. */
		void setBackgroundColor(QColor bg) { m_bgColor = bg; update(); }
    
        /** Adds blur effect to main note. If blurColor is -1 deletes the effect. */
    void markNote(QColor blurColor);    
    void moveNote(int pos);
				/** Sets noteHead at given position and given accidental accidental. */
		void setNote(int notePos, int accNr);
				/** Returns bounding rectangle of selected note.  */
		QGraphicsEllipseItem* mainNote() { return m_mainNote; }
		QGraphicsSimpleTextItem *mainAccid() { return m_mainAccid; }
    
				/** Min and Max values of Y coefficient on the staff */
    void setAmbitus(int min, int max){ m_ambitMin = qMax(min, 1); m_ambitMax = qMin(max, (int)m_height - 1); }

        /** This return value of @li -2 is bb @li 1 is #
         * @li etc... */ 
    int accidental() {return m_accidental;}
    int ottava() { return m_ottava; } // NOTE: for this moment it is unused and set to 0
    int notePos() { return m_mainPosY; }
//     int noteNumber() { return m_noteNr; } // note number depends on octave.
        /** Returns QString with accidental symbol*/
    static QString getAccid(int accNr);
				/** nootka font with well scaled accidental glyph. */
    static QFont getAccidFont();
		static qreal accidYoffset() { return m_accidYoffset; }
		static qreal accidScale() { return m_accidScale; }
		
		
				/** It paints string number symbol.
        * Automaticaly determines above or below staff. */
    void setString(int realNr);
    void removeString(); /** Removes string number */
		int stringNumber() { return m_stringNr; } /* Displayed string number or 0 if not. */
		
		void setReadOnly(bool ro);
		bool isReadOnly() { return m_readOnly; }
		
		void setWorkAccid(int accNr);
		void enableAnimation(bool enable, int duration = 150);
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect() const;
    
signals:
    void noteWasClicked(int);
		void accidWasChanged(int);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

private:
    QGraphicsEllipseItem          *m_workNote, *m_mainNote;
    QGraphicsSimpleTextItem       *m_workAccid, *m_mainAccid;
		QGraphicsSimpleTextItem 			*m_stringText;
    QList<QGraphicsLineItem*>     m_upLines, m_mainUpLines, m_mainDownLines, m_downLines;
    QColor                        m_workColor, m_mainColor;
		TanimedItem										*m_noteAnim;
		TcrossFadeTextAnim 						*m_accidAnim;
    
    int                           m_workPosY, m_mainPosY;
    int                           m_curentAccid, m_accidental;
    int                           m_index; // note index in externl list
//     int 													m_noteNr; // note number depends on octave
    int 													m_ottava; // values from -2 (two octaves down), to 2 (two octaves up)
                  /** Represents range (ambitus) of notes on score */
    int                           m_ambitMin, m_ambitMax;
		int 													m_stringNr;
    qreal                         m_height;
		bool													m_readOnly;
		QColor                        m_bgColor;
		static qreal 									m_accidYoffset; // difference to note y position.
    static qreal									m_accidScale;
    
private:
        /** Prepares noteHead (elipse) */
    QGraphicsEllipseItem* createNoteHead();
    QGraphicsLineItem*    createNoteLine(int yPos);
    void hideLines(QList<QGraphicsLineItem*> &linesList);
				/** Determines and set string numer position (above or below the staff) depends on note pos */
		void setStringPos();
// 		void checkOctavation();
		
		    
};

#endif // TSCORENOTE_H
