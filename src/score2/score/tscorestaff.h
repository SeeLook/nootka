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

class TscoreControl;
class TscoreKeySignature;
class TscoreNote;
class TscoreClef;
class TscoreScene;


class TnoteOffset
{
public:
  TnoteOffset(int noteOff, int octaveOff);
  
  int note;
  int octave;
  int total() { return octave * 7 + note; }
};


class TscoreStaff : public TscoreItem
{
    Q_OBJECT

public:
  
    enum Ekind {
      e_normal, // normal staff placed in the centre of score
      e_upper, // placed up - right hand of piano staff
      e_lower // placed down - left hand of piano staff
    }; // Kind of staff (normal or upper (right hand) or lower(left hand))
      
    TscoreStaff(TscoreScene *scene, int notesNr, Ekind kindOfStaff = e_normal);
    virtual ~TscoreStaff();
    
				/** Returns pointer to TscoreNote element in the score. */
		TscoreNote* noteSegment(int nr) { return m_notes[nr]; }
		TscoreKeySignature* scoreKey() { return m_keySignature; }
		void setEnableKeySign(bool isEnabled);
   
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
    int notePosRelatedToClef(int pos, TnoteOffset off) {
      return off.octave * 7 - (pos + 1 - (int)upperLinePos() - off.note);  }
    int notePosRelatedToClef(int pos) {
      return notePosRelatedToClef(pos, m_offset); }
        /** Returns offset of a y coeff. of a note related to current cleff. */
    int noteOffset() { return m_offset.note; }
    
    void setScoreControler(TscoreControl *scoreControl);
		
		
		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QRectF boundingRect() const;
    
protected slots:
    void onClefChanged();
    void onKeyChanged();
    void onNoteClicked(int noteIndex);
		void noteChangedAccid(int accid);
		void onAccidButtonPressed(int accid);
    
private:
				/** Calculates current width of a staff depends on is key sign. enabled. */
		void updateWidth();
		
private:    
    QGraphicsLineItem       *m_lines[5];
    TscoreClef              *m_clef;
    TscoreKeySignature      *m_keySignature;
    QList<TscoreNote*>      m_notes;
    qreal                   m_upperLinePos;
    qreal                   m_height, m_width;
    Ekind                   m_kindOfStaff;
    TnoteOffset             m_offset;
		TscoreControl						*m_scoreControl;
};

#endif // TSCORESTAFF_H
