/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#include <nootkacoreglobal.h>
#include "tscoreitem.h"


class Tnote;
class TnoteControl;
class TcombinedAnim;
class TcrossFadeTextAnim;
class TscoreControl;
class TscoreScene;

typedef QList<QGraphicsLineItem*> TaddLines; /** List of graphics lines  */

/*!
 * This class represents single note on a score. 
 * It is a rectangle area over the staff with handling mouse move event to display working note cursor.
 * It also grabs wheel event to manipulate accidentals
 * It can be set to read-only mode through setReadOnly() then mouse events are ignored.
 */
class NOOTKACORE_EXPORT TscoreNote : public TscoreItem
{
  Q_OBJECT
  
public:
    TscoreNote(TscoreScene *scene, TscoreStaff *staff, int index);
    ~TscoreNote();
    
				/** Index of this note instance. It is connected with note number in the list */
		int index() { return m_index; }
		void changeIndex(int newIndex) { m_index = newIndex; }
		
		void setReadOnly(bool ro);
		bool isReadOnly() { return m_readOnly; }
		
				/** Points to Tnote instance.
				 * TscoreNote is not aware of it. It is just container.
				 * This value has to be set through setNote() */
		Tnote* note() { return m_note; }
		
		void adjustSize(); /** Grabs height from staff and adjust to it. */
		
				/** Adjust note cursor and TnoteControl to new staff size. 
				 * For performance reason it has to be called once for all adjustSize() of TscoreNote
				 * because there is only one instance of note cursor and TnoteControl */
		static void adjustCursor(); 
		
        /** Sets color of main note. */
    void setColor(QColor color);
		
        /** Sets color of pointing (work) note. */
    static void setPointedColor(QColor color);
		
				/** It sets background of the note segment. When sets to -1 means transparent - no background. */
		void setBackgroundColor(QColor bg) { m_bgColor = bg; update(); }
    
        /** Adds blur effect to main note. If blurColor is -1 deletes the effect. */
    void markNote(QColor blurColor);
		
    void moveNote(int posY);
		void moveWorkNote(const QPointF& newPos);
		
				/** Returns true if note was selected */
		bool isSelected() { return m_selected; }
		void selectNote(bool sel);
		
				/** Sets note-head at given position and given accidental accidental. Also puts Tnote of it. */
		void setNote(int notePos, int accNr, const Tnote& n);
		
				/** Returns pointer to main note QGraphicsEllipseItem.  */
		QGraphicsEllipseItem* mainNote() { return m_mainNote; }
		
				/** Returns pointer to main accidental QGraphicsSimpleTextItem.  */
		QGraphicsSimpleTextItem *mainAccid() { return m_mainAccid; }
    
				/** Min and Max values of Y coefficient on the staff */
    void setAmbitus(int lo, int hi);

        /** This return value of -2 is bb,  1 is #,  etc... */ 
    int accidental() {return m_accidental;}
    int ottava() { return m_ottava; } /** NOTE: for this moment it is unused and set to 0 */
    int notePos() { return m_mainPosY; }  /** Y Position of note head */

    static QString getAccid(int accNr); /** Returns QString with accidental symbol*/
		
		static qreal accidYoffset() { return m_accidYoffset; }
		static qreal accidScale() { return m_accidScale; } /** Scale of accidental text item */
		
		
				/** It paints string number symbol. Automatically determines above or below staff. */
    void setString(int realNr);
    void removeString(); /** Removes string number */
		int stringNumber() { return m_stringNr; } /** Displayed string number or 0 if not. */
		
				/** Starts displaying note name of this note. 
				 * Name will change appropriate to moved note until removeNoteName() will be invoked */
		void showNoteName();
		void removeNoteName();
		QGraphicsTextItem* noteName() { return m_nameText; } /** Graphics item of note name text */
		
		
				/** Changes accidental of a working note cursor. */
		static void setWorkAccid(int accNr);
		
				/** Enables moving note animation during its position (pitch) change.
				 * In fact, when accidental is visible it is animated as well. */
		void enableNoteAnim(bool enable, int duration = 150);
		bool isNoteAnimEnabled() { return (bool)m_noteAnim; }
		
				/** When note accidental has already existed in a key signature animation is performed.  */
		void enableAccidToKeyAnim(bool enable) { m_accidToKeyAnim = enable; }
		bool accidToKeyAnim() { return m_accidToKeyAnim; }
    
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect() const;
    
		bool isCursorVisible() { return m_workNote->isVisible(); }
		
				/** Note controllers, appear with cursor. 
				 * There are automatically created with first note instance 
				 * when score scene has a view. */
		static TnoteControl* right() { return m_rightBox; }
		static TnoteControl* left() { return m_leftBox; }
		
signals:
    void noteWasClicked(int);
		void noteWasSelected(int); /** When right button was clicked. */
		
		void toKeyAnim(QString, QPointF, int notePos); /** Emitted when accidental has been in key already */
		void fromKeyAnim(QString, QPointF, int notePos); /** Emitted when neutral is necessary */
		
public slots:
		void keyAnimFinished();
    void hideNote(); /** Hides main note */
    void hideWorkNote(); /** Hides pointing (work) note */

protected:
#if defined (Q_OS_ANDROID)
		virtual void shortTap(const QPointF &cPos);
    virtual void longTap(const QPointF& cPos);
    virtual void touched(const QPointF& cPos);
    virtual void untouched(const QPointF &cPos);
    virtual void touchMove(const QPointF& cPos);
#endif
		
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

private:
    QGraphicsEllipseItem          					*m_mainNote;
    QGraphicsSimpleTextItem       					*m_mainAccid;
    TaddLines      								 					m_mainUpLines, m_mainDownLines, m_mainMidLines;
    QColor                         					m_mainColor;
		TcrossFadeTextAnim 											*m_accidAnim;
		Tnote																	  *m_note;
    
    int                            					m_mainPosY, m_accidental;
    int                            					m_index; /** note index in external list */
//     int 													m_noteNr; // note number depends on octave
    int                            					m_ambitMin, m_ambitMax; /** Represents range (ambitus) of notes on score */
		int 													 					m_stringNr;
		QGraphicsSimpleTextItem 								*m_stringText;
    qreal                          					m_height;
		bool													 					m_readOnly;
		QGraphicsTextItem												*m_nameText;
		int 													 					m_ottava; /** values from -2 (two octaves down), to 2 (two octaves up) */
		QColor                         					m_bgColor;
		TcombinedAnim														*m_noteAnim;
		bool													 					m_accidToKeyAnim;
		bool													 					m_selected;
		
		static qreal 									 					m_accidYoffset; /** difference between y note position. */
    static qreal									 					m_accidScale;
	// static note cursor
		static int                            	m_curentAccid, m_workPosY;
		static QGraphicsEllipseItem          		*m_workNote;
		static QGraphicsSimpleTextItem       		*m_workAccid;
		static TaddLines								      	m_upLines, m_downLines, m_midLines;
		static QColor                         	m_workColor;
		static TnoteControl				  				 		*m_rightBox;
		static TnoteControl									 		*m_leftBox;
		static QString													m_staticTip;
		bool 																		m_touchedToMove; /** Determines whether cursor follows moving finger */
    
private:
        /** Prepares note-head (ellipse) */
    QGraphicsEllipseItem* createNoteHead();
    QGraphicsLineItem*    createNoteLine(int yPos);
		
				/** Common method creating upper and lower staff lines.
				 * It appends new lines to list 
				 * so do not forget to clear list before every next call. */
		void createLines(TaddLines &low, TaddLines &upp, TaddLines &mid);
		void deleteLines(TaddLines &linesList); /** Deletes lines in the list and clears the list */
    void hideLines(TaddLines &linesList);
		void setStringPos(); /** Determines and set string number position (above or below the staff) depends on note position */
		void initNoteCursor(); /** Creates static members of cursor when first TscoreNote instance is created */
		void setCursorParent(TscoreItem* item); /** Sets parent of note cursor to this instance */
				/** Checks whose lines show and hide. @p curPos is current position of note those lines belong to. */
		void checkLines(int curPos, TaddLines &low, TaddLines &upp, TaddLines &mid);
// 		void checkOctavation();
		
		    
};

#endif // TSCORENOTE_H
