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

class TcombinedAnim;
class TcrossFadeTextAnim;
class TscoreControl;
class TscoreScene;

/*!
 * This class represents single note n a score. 
 * It is a rectangle area over the staff with handling mouse move event to display working note cursor.
 * It also grabs wheel event to manipulate accidentals
 */
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
    void moveNote(int posY);
		void moveWorkNote(const QPointF& newPos);
		
				/** Sets note-head at given position and given accidental accidental. */
		void setNote(int notePos, int accNr);
		
				/** Returns pointer to main note QGraphicsEllipseItem.  */
		QGraphicsEllipseItem* mainNote() { return m_mainNote; }
		
				/** Returns pointer to main accidental QGraphicsSimpleTextItem.  */
		QGraphicsSimpleTextItem *mainAccid() { return m_mainAccid; }
    
				/** Min and Max values of Y coefficient on the staff */
    void setAmbitus(int lo, int hi);

        /** This return value of -2 is bb,  1 is #,  etc... */ 
    int accidental() {return m_accidental;}
    int ottava() { return m_ottava; } // NOTE: for this moment it is unused and set to 0
    int notePos() { return m_mainPosY; }
    

        /** Returns QString with accidental symbol*/
    static QString getAccid(int accNr);
		
				/** nootka font with well scaled accidental glyph. */
    static QFont getAccidFont();
		static qreal accidYoffset() { return m_accidYoffset; }
		static qreal accidScale() { return m_accidScale; }
		
		
				/** It paints string number symbol.
        * Automatically determines above or below staff. */
    void setString(int realNr);
    void removeString(); /** Removes string number */
		int stringNumber() { return m_stringNr; } /* Displayed string number or 0 if not. */
		
		void setReadOnly(bool ro);
		bool isReadOnly() { return m_readOnly; }
		
				/** Changes accidental of a working note cursor. */
		void setWorkAccid(int accNr);
		
				/** Enables moving note animation during its position (pitch) change.
				 * In fact, when accidental is visible it is animated as well. */
		void enableNoteAnim(bool enable, int duration = 150);
		bool isNoteAnimEnabled() { return (bool)m_noteAnim; }
		
				/** When note accidental has already existed in a key signature animation is performed.  */
		void enableAccidToKeyAnim(bool enable) { m_accidToKeyAnim = enable; }
		bool accidToKeyAnim() { return m_accidToKeyAnim; }
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect() const;
    
signals:
    void noteWasClicked(int);
		void accidWasChanged(int);
		
		void toKeyAnim(QString, QPointF, int notePos); /** Emitted when accidental has been in key already */
		void fromKeyAnim(QString, QPointF, int notePos); /** Emitted when neutral is necessary */
		
public slots:
		void keyAnimFinished();

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
		TcombinedAnim									*m_noteAnim;
		TcrossFadeTextAnim 						*m_accidAnim;
    bool													m_accidToKeyAnim;
    
    int                           m_workPosY, m_mainPosY;
    int                           m_curentAccid, m_accidental;
    int                           m_index; // note index in external list
//     int 													m_noteNr; // note number depends on octave
    int 													m_ottava; /** values from -2 (two octaves down), to 2 (two octaves up) */
    int                           m_ambitMin, m_ambitMax; /** Represents range (ambitus) of notes on score */
		int 													m_stringNr;
    qreal                         m_height;
		bool													m_readOnly;
		QColor                        m_bgColor;
		
		static qreal 									m_accidYoffset; // difference to note y position.
    static qreal									m_accidScale;
    
private:
        /** Prepares note-head (ellipse) */
    QGraphicsEllipseItem* createNoteHead();
    QGraphicsLineItem*    createNoteLine(int yPos);
    void hideLines(QList<QGraphicsLineItem*> &linesList);
		void setStringPos(); /** Determines and set string number position (above or below the staff) depends on note position */
// 		void checkOctavation();
		
		    
};

#endif // TSCORENOTE_H
