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
#include "tclef.h"

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
class TnoteOffset
{
public:
  TnoteOffset(int noteOff, int octaveOff);
  
  int note;
  int octave;
  int total() { return octave * 7 + note; }
};

/** 
 * @class TscoreStaff manages score items onthe staff.
 * It has got:
 * - clef - @p TscoreClef - accessing by @p scoreClef()
 * - key signature - @p TscoreKeySignature - scoreKey()
 * - notes (in QList) - @p TscoreNote - @p noteSegment(int nr)
 * 
 * If TscoreControl is set (@p setScoreControler(TscoreControl)) it manages of accidentals of notes
 */
class TscoreStaff : public TscoreItem
{
    Q_OBJECT

public:
  
    enum Ekind {
      e_normal, // normal staff placed in the centre of score
      e_upper, // placed up - right hand of piano staff with treble clef
      e_lower // placed down - left hand of piano staff with bass clef
    }; // Kind of staff (normal or upper (right hand) or lower(left hand))
      
    TscoreStaff(TscoreScene *scene, int notesNr, Ekind kindOfStaff = e_normal);
    virtual ~TscoreStaff();
    
				/** Returns pointer to TscoreNote element in the score. 
				 * When it is piano staff - upper notes are returned. */
		TscoreNote* noteSegment(int nr) { return m_scoreNotes[nr]; }
		TscoreKeySignature* scoreKey() { return m_keySignature; }
		TscoreClef* scoreClef() { return m_clef; }
				/** Returns pointer to lower staff if it is piano staff or 0.
				 * Every time You want to invoke this 
				 * CHECK IS IT EXIST - different than 0. */
		TscoreStaff* lower() { return m_lower; }
		
				/** Returns current @p index note or Tnot(0,0,0) if not set. */
		Tnote* getNote(int index) { return m_notes[index]; }
		virtual void setNote(int index, Tnote &note);
		virtual void setNoteDisabled(int index, bool isDisabled);
		
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
		
        /** Y position of upper line of a staff. */
    qreal upperLinePos() const { return m_upperLinePos; }
    qreal height() const { return m_height; } // staff height
    qreal width() const { return m_width; } // staff width
        /** Kind of staff (normal or upper (right hand) or lower(left hand)) */
    Ekind kindOfStaff() { return m_kindOfStaff; }
				/** Returns number of a note. upperLinePos() is note nr 0 but it depends on octave (clef).  */
    int notePosRelatedToClef(int pos, TnoteOffset off) {
      return off.octave * 7 - (pos + 1 - (int)upperLinePos() - off.note);  }
    int notePosRelatedToClef(int pos) {
      return notePosRelatedToClef(pos, m_offset); }
        /** Returns offset of a y coeff. of a note related to current cleff. */
    int noteOffset() { return m_offset.note; }
				/** octave offset related to middle (one-line) octave. */
    int octaveOffset() { return m_offset.octave; }
    
    virtual void setScoreControler(TscoreControl *scoreControl);
				/** Stops/starts capturing any mouse events. */
		virtual void setDisabled(bool disabled);
		
		
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {};
    virtual QRectF boundingRect() const;
		
signals:
		void pianoStaffSwitched(Tclef);
		void noteChanged(int index);
		void clefChanged(Tclef);
		
public slots:
				/** It is connected with clef, but also refresh m_offset appropirate to current clef. */
		void onClefChanged();
		
protected:
				/** It doesn't add scordature like setScordature() method, 
				 * just make place (resizes staff width if necessary) for scordature. 
				 * setScordature calls it itself. */
		void setEnableScordtature(bool enable);
				/** This method adds additional staff under itself end becomes piano staff. */
		void addLowerStaff();
		
protected slots:
    void onKeyChanged();
    void onNoteClicked(int noteIndex);
		void noteChangedAccid(int accid); // TscoreNote wheel event - changes accidental
		void onAccidButtonPressed(int accid); // TscoreControl accid button pressed
		void onPianoStaffChanged(Tclef clef) { emit pianoStaffSwitched(clef); } // clef demands piano staff
    
private:
				/** Calculates current width of a staff depends on is key sign. enabled. */
		void updateWidth();
		
private:    
    QGraphicsLineItem       *m_lines[5]; // five staff lines
    TscoreClef              *m_clef;
    TscoreKeySignature      *m_keySignature;
    QList<TscoreNote*>      m_scoreNotes;
    qreal                   m_upperLinePos;
    qreal                   m_height, m_width;
		qreal										m_extraWidth;
    Ekind                   m_kindOfStaff;
    TnoteOffset             m_offset;
		TscoreControl						*m_scoreControl;
		TscoreScordature				*m_scordature;
		QList<Tnote*>						m_notes;
		bool										m_enableScord;
				/** Grand (left hand) staff. It exist in piano staff only. In normal staff it is 0. */
		TscoreStaff 						*m_lower;
		
};

#endif // TSCORESTAFF_H
