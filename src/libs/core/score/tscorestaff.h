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


#ifndef TSCORESTAFF_H
#define TSCORESTAFF_H

#include <nootkacoreglobal.h>
#include "tscoreitem.h"
#include <music/tclef.h>
#include <QPointer>

class TcombinedAnim;
class Tnote;
class Ttune;
class TscoreControl;
class TscoreKeySignature;
class TscoreScordature;
class TscoreNote;
class TscoreClef;
class TscoreScene;


/** 
 * Describes offset of a note. 
 */
class NOOTKACORE_EXPORT TnoteOffset
{
public:
  TnoteOffset(int noteOff, int octaveOff);
  
  int note;
  int octave;
  int total() { return octave * 7 + note; }
};

/** 
 * @class TscoreStaff manages score items on the staff.
 * It has got:
 * - clef - @p TscoreClef - accessing by @p scoreClef()
 * - key signature - @p TscoreKeySignature - scoreKey()
 * - notes (in QList) - @p TscoreNote - @p noteSegment(int nr)
 * 
 * If TscoreControl is set (@p setScoreControler(TscoreControl)) it manages of accidentals of notes
 */
class NOOTKACORE_EXPORT TscoreStaff : public TscoreItem
{
    Q_OBJECT

public:
    
    TscoreStaff(TscoreScene* scene, int notesNr);
    virtual ~TscoreStaff();
    
				/** Returns pointer to TscoreNote element in the score. */
		TscoreNote* noteSegment(int nr) { return m_scoreNotes[nr]; }
		
		TscoreKeySignature* scoreKey() { return m_keySignature; }
		
		TscoreClef* scoreClef() { return m_clef; }
		
		void setPianoStaff(bool isPiano);
		bool isPianoStaff() { return m_isPianoStaff; }
		
				/** Returns current @p index note or Tnot(0,0,0) if not set. */
		Tnote* getNote(int index) { return m_notes[index]; }
		virtual void setNote(int index, const Tnote& note);
		virtual void setNoteDisabled(int index, bool isDisabled);
		
		int count() { return m_scoreNotes.size(); } /** Number of notes on the score */
		
        /** adds note at the end of the staff 
         * Empty Tnote creates new instance of TscoreNote item. */
    void addNote(Tnote& note, bool disabled = false);
				/** Inserts note in given position (index). 
				 * When @p index is out of scope adds it at the end. */
		void insertNote(int index, const Tnote& note, bool disabled = false);
		void insertNote(int index, bool disabled = false); /** Insert empty note */
		void removeNote(int index); /** Deletes given note from staff */
		int currentIndex() { return m_index; }
		
		virtual void setEnableKeySign(bool isEnabled);
		
        /** This array keeps values (-1, 0 or 1) for accidentals in key sign.
         * It is common for TscoreKeySignature and all TscoreNote. 
         * TscoreKeySignature::setAccidInKeyPointer and TscoreNote::setAccidInKeyPointer
         * have to be set to connect them. 
         * When TscoreKeySignature is deleted it should be set to 0. */
    char accidInKeyArray[7];
		
		    /** Sets scordature according to given tune.
				 * To delete it just call this with Ttune::standardTune.*/
    virtual void setScordature(Ttune& tune);
		virtual bool hasScordature() { return (bool)m_scordature; } /** @p TRUE when staff has got scordature. */
		virtual void removeScordatute();
		
    qreal upperLinePos() const { return m_upperLinePos; } /** Y position of upper line of a staff. */
    qreal lowerLinePos() const { return m_lowerStaffPos; } /** Y position of lower line of a lower staff. */
    qreal height() const { return m_height; } // staff height
    qreal width() const { return m_width; } // staff width
				
				/** Updates rectangle of QGraphicsScene to staff bounding rectangle. */
    void updateSceneRect();
    
				/** Returns number of a note. upperLinePos() is note nr 0 but it depends on octave (clef).  */
    int notePosRelatedToClef(int pos, TnoteOffset off) {
                    return off.octave * 7 - (pos + 1 - (int)upperLinePos() - off.note);  }
      
    int notePosRelatedToClef(int pos) {
      return notePosRelatedToClef(pos, m_offset); }
      
        /** Returns offset of a y coefficient of a note related to current clef. */
    int noteOffset() { return m_offset.note; }
    
				/** octave offset related to middle (one-line) octave. */
    int octaveOffset() { return m_offset.octave; }
			
				/** Returns number of accidental in key signature, fe.: F# - 0, C# - 1 or Bb - 0, Eb - 1 */
    int accidNrInKey(int noteNr, char key);
		
				/** Return Y position of given note */
		int noteToPos(const Tnote& note); 
		
				/** Checks is note position on grand staff and adds 2 */
		int fixNotePos(int pianoPos);
		
				/** Width of a staff set by external function. 
				 * It is preferred when it is bigger than width calculated by updateWidth() */
		void setExternalWidth(qreal w) { m_externWidth = w; updateWidth(); }
		qreal externalWidth() { return m_externWidth; }
		
				/** Switches when note segments have colored background after their note are set */
		void setSelectableNotes(bool selectable) { m_selectableNotes = selectable; }
		bool selectableNotes() { return m_selectableNotes; }
    
    virtual void setScoreControler(TscoreControl *scoreControl);
		
				/** Stops/starts capturing any mouse events. */
		virtual void setDisabled(bool disabled);		
		
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {};
    virtual QRectF boundingRect() const;
	
signals:
		void pianoStaffSwitched();
		void noteChanged(int index);
		void clefChanged(Tclef);
		void staffSizeChanged(); /** when piano staff is changed or staff width */
		
public slots:
				/** It is connected with clef, but also refresh m_offset appropriate to current clef. */
		void onClefChanged(Tclef clef);
		void setCurrentIndex(int index);
		void noteChangedAccid(int accid); // TscoreNote wheel event - changes accidental
		
protected:
				/** Creates staff lines at first call, sets lines width, creates lower staff lines as well.
				 * It also calls createBrace().  */
	  void prepareStaffLines();
		
				/** It doesn't add scordature like setScordature() method, 
				 * just make place (re-sizes staff width if necessary) for scordature. 
				 * setScordature calls it itself. */
		void setEnableScordtature(bool enable);
		
				/** Calculates current width of a staff depends on is key sign. enabled. */
		void updateWidth();
		
				/** Iterates through all notes, sets theirs indexes and updates staff index of selected note.
				 * It must to be invoked after inserting or removing a note. */
		void updateIndex();
		
				/** Protected method that creates new TscoreNote note instance and inserts it to m_scoreNotes.
				 * It doesn't perform any checks */
		void insert(int index);
		
protected slots:
    void onKeyChanged();
    void onNoteClicked(int noteIndex);
		void onAccidButtonPressed(int accid); // TscoreControl accid button pressed
		void onPianoStaffChanged(Tclef clef); // clef demands piano staff
		void toKeyAnimSlot(QString accidText, QPointF accidPos, int notePos);
		void fromKeyAnimSlot(QString accidText, QPointF accidPos, int notePos);
		void accidAnimFinished();
		
private:    
    QGraphicsLineItem       					*m_lines[5], *m_lowLines[5]; // five staff lines
    TscoreClef              					*m_clef;
		QGraphicsSimpleTextItem 					*m_brace;
    TscoreKeySignature								*m_keySignature; 
    QList<TscoreNote*>       					 m_scoreNotes;
		QList<Tnote*>						 					 m_notes;
    qreal                    					 m_upperLinePos, m_lowerStaffPos;
    qreal                    					 m_height, m_width;
		qreal										 					 m_externWidth; // width set from outside
    TnoteOffset              					 m_offset;
		bool 										 					 m_isPianoStaff;
		QPointer<TscoreControl>						 m_scoreControl;
		TscoreScordature				 					*m_scordature;
		bool										 					 m_enableScord;
		TcombinedAnim											*m_accidAnim;
		QGraphicsSimpleTextItem 					*m_flyAccid;
		int											 					 m_index; // index of currently selected note
		bool 										 					 m_selectableNotes;
		
private:
		void createBrace();

};

#endif // TSCORESTAFF_H
