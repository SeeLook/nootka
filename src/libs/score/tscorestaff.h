/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <QtCore/qpointer.h>

class QTimer;
class TcombinedAnim;
class Tnote;
class Ttune;
class TscoreKeySignature;
class TscoreScordature;
class TscoreNote;
class TscoreClef;
class TscoreScene;
class Tscore5lines;
class TscoreMeter;


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
 * - scordature - below clef through @p setScordature()
 */
class NOOTKACORE_EXPORT TscoreStaff : public TscoreItem
{
    Q_OBJECT

public:

	TscoreStaff(TscoreScene* scene, int notesNr);
	virtual ~TscoreStaff();

			/** Determines index of this staff in staff list in multi-staff mode.
				* Otherwise (default) returns -1 */
	void setStafNumber(int nr) { m_staffNr = nr; }
	int number() { return m_staffNr; }

			/** Returns pointer to TscoreNote element in the score. */
	TscoreNote* noteSegment(int nr) { return m_scoreNotes[nr]; }

	TscoreKeySignature* scoreKey() { return m_keySignature; }

	TscoreClef* scoreClef() { return m_clef; }

	void setPianoStaff(bool isPiano);
	bool isPianoStaff() const { return m_isPianoStaff; }

			/** Returns current @p index note or Tnote(0, 0, 0) if not set. */
	Tnote* getNote(int index);
	void setNote(int index, const Tnote& note);
	void setNoteDisabled(int index, bool isDisabled);

	int count() { return m_scoreNotes.size(); } /**< Number of notes on the score */

			/** adds note at the end of the staff
				* Empty Tnote creates new instance of TscoreNote item. */
	void addNote(Tnote& note, bool disabled = false);

			/** Adds notes from the list to this staff, starting from given @p index.
				* Notes number in the list can not be bigger than available space on the staff */
	void addNotes(int index, QList<TscoreNote*>& nList);
	void addNote(int index, TscoreNote* freeNotet);

			/** Inserts note in given position (index).
				* When @p index is out of scope adds it at the end. */
	void insertNote(int index, const Tnote& note, bool disabled = false);
	void insertNote(int index, bool disabled = false); /**< Insert empty note */
	void removeNote(int index); /**< Deletes given note from the staff */

			/** Removes all note segments from @p from to @p to
				* and puts those TscoreNote pointers to given @p nList.
				* To grab all notes from a staff just invoke:
				* takeNotes(smoeList, 0, count() - 1); */
	void takeNotes(QList<TscoreNote*>& nList, int from, int to);

	void setEnableKeySign(bool isEnabled);

			/** This array keeps values (-1, 0 or 1) for accidentals in key sign.
				* It is common for TscoreKeySignature and all TscoreNote.
				* TscoreKeySignature::setAccidInKeyPointer and TscoreNote::setAccidInKeyPointer
				* have to be set to connect them.
				* When TscoreKeySignature is deleted it should be set to 0. */
	char accidInKeyArray[7];

			/** Sets scordature according to given tune.
				* To delete it just call this with Ttune::standardTune.*/
	void setScordature(Ttune& tune);
	bool hasScordature() { return (bool)m_scordature; } /**< @p TRUE when staff has got scordature. */
	void removeScordatute();

  void setEnableMetrum(bool isEnabled);
  TscoreMeter* scoreMetrum() { return m_scoreMeter; }

	qreal upperLinePos() const { return m_upperLinePos; } /**< Y position of upper line of a staff. */
	qreal lowerLinePos() const { return m_lowerStaffPos; } /**< Y position of lower line of a lower staff. */
	qreal height() const { return m_height; } // staff height
	qreal width() const { return m_width; } // staff width

	qreal loNotePos() const { return m_loNotePos; } /**< Y position of lowest note on the staff */
	qreal hiNotePos() const { return m_hiNotePos; } /**< Y position of highest note on the staff */

			/** Minimal height of the staff to display all its notes. */
	qreal minHight() const { return m_loNotePos - m_hiNotePos; }

			/** Checks positions of all notes to find lowest and highest.
				* @p doEmit determines whether this method sends appropriate signals */
	void checkNoteRange(bool doEmit = true);

			/** Updates rectangle of QGraphicsScene to staff bounding rectangle. */
	void updateSceneRect();

			/** Returns number of a note. upperLinePos() is note nr 0 but it depends on octave (clef).  */
	int notePosRelatedToClef(int pos, TnoteOffset off) {
									return off.octave * 7 - (pos + 1 - (int)upperLinePos() - off.note);  }

	int notePosRelatedToClef(int pos) { return notePosRelatedToClef(pos, m_offset); }

			/** Returns offset of a y coefficient of a note related to current clef. */
	int noteOffset() { return m_offset.note; }

			/** octave offset related to middle (one-line) octave. */
	int octaveOffset() { return m_offset.octave; }

			/** Returns number of accidental in key signature, fe.: F# - 0, C# - 1 or Bb - 0, Eb - 1 */
	int accidNrInKey(int noteNr, char key);

	int noteToPos(const Tnote& note); /**< Return Y position of given note. */
	int fixNotePos(int pianoPos); /**< Checks is note position on grand staff and adds 2 to it. */
	qreal notesOffset(); /**< X Position of first TscoreNote on the staff (depends on clef, key and scordature) */

			/** Informs a staff about QGraphicsView width displaying this staff.
				* With this value the staff determines maximal lines width and maximal notes count.
				* If not set (0.0) - single staff, If set - m_externWidth is ignored.
				* This is very important for multi-system view (vertical staves) */
	void setViewWidth(qreal viewW);

			/** Returns maximal note number which staff can display in single line in view area.
				* or current notes count if staff is in linear mode */
	int maxNoteCount() { return m_maxNotesCount ? m_maxNotesCount : count(); }

//--- note controllers/switches
			/** Switches when note segments have colored background after their note are set */
	void setSelectableNotes(bool selectable) { m_selectableNotes = selectable; }
	bool selectableNotes() { return m_selectableNotes; }

			/** Determines whether note controllers can add/remove note to the staff.
				* Notice, when enabled, 'remove' (minus) is displayed only when staff has more notes than one. */
	void enableToAddNotes(bool alowAdding);

			/** Shows accidentals from key signature also near a note (in brackets) */
	void setExtraAccids(bool extra) { m_extraAccids = extra; }
	bool extraAccids() { return m_extraAccids; }

			/** Stops/starts capturing any mouse events. */
	void setDisabled(bool disabled);

      /** With tidy key - key signature width is adjusted exactly to space occupies by visible accidentals.
       * Calling this invokes notes replacing when @p tidy value really changes,
       * which may be expensive. */
  void setTidyKey(bool tidy);
  bool isTidyKey() { return m_tidyKey; } /**< Is positioning adjusted to accidentals number in key sign. */

      /** When @p m_autoAddedNoteId exists (staff generated temporary note)
       * this method adds it to staff permanently. Emits signal @p noteIsAdding()
       * Stops @p m_addTimer ass well.
       * If not exists - does nothing. */
  void applyAutoAddedNote();

	virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {};
	virtual QRectF boundingRect() const;

signals:
	void pianoStaffSwitched();
	void noteChanged(int index);

			/** Emitted when right button was clicked over note.
				* @p selectableNotes or @p controlledNotes has to be set to @p TRUE */
	void noteSelected(int index);
	void clefChanged(Tclef);

			/** When staff has no more space to display next note segment.
				* Argument is staff number */
	void noMoreSpace(int);

			/** Emitted usually after removing a note. Staff has extra free space.
				* First argument is staff number, the second one is amount of space for new note(s) */
	void freeSpace(int, int);

			/** There was no space for a note emitting as an argument.
				* Usually it was the last note on this staff before inserting another note. */
	void noteToMove(int, TscoreNote*);

			/** Emitting just before note will be removed and deleted.
				* First argument is staff number and second is note number in the list.
				* Signal is emitted when note still exists. */
	void noteIsRemoving(int, int);

	void noteIsAdding(int, int); /**< Emitting when note is added/inserted to a staff */

			/** Signals informing about changing note range on the staff.
				* Sending parameters are the staff number and difference of Y position. */
	void loNoteChanged(int,qreal);
	void hiNoteChanged(int,qreal);


public slots:
	void onClefChanged(Tclef clef); /**< It is connected with clef, but also refresh m_offset appropriate to current clef. */
	void noteChangedAccid(int accid); /**< TscoreNote wheel event - changes accidental */

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

	void updateIndexes(); /**< Iterates through all notes, sets theirs indexes. It must to be invoked after inserting or removing a note. */
	void updateLines(); /**< Updates staff lines  */
	void updateNotesPos(int startId = 0); /**< Replaces (performs pos()) all TscoreNote items. Starts from @p startId */

			/** Protected method that creates new TscoreNote note instance and inserts it to m_scoreNotes.
				* It doesn't perform any checks */
	void insert(int index);

protected slots:
	void onKeyChanged();
	void onNoteClicked(int noteIndex);
	void onNoteSelected(int noteIndex);
	void onAccidButtonPressed(int accid); // TnoteControl accid button pressed
	void onPianoStaffChanged(Tclef clef); // clef demands piano staff
	void toKeyAnimSlot(const QString& accidText, const QPointF& accidPos, int notePos);
	void fromKeyAnimSlot(const QString& accidText, const QPointF& accidPos, int notePos);
	void accidAnimFinished();
	void addNoteTimeOut();
  void noteDestroingSlot(QObject* n); /**< Every note segment call this before it will be deleted */

private:
	int 															 m_staffNr;
	Tscore5lines                      *m_5lines;
	TscoreClef              					*m_clef;
	QGraphicsSimpleTextItem 					*m_brace;
	TscoreKeySignature								*m_keySignature;
	QList<TscoreNote*>       					 m_scoreNotes;
	qreal                    					 m_upperLinePos, m_lowerStaffPos;
	qreal                    					 m_height, m_width;
	qreal															 m_viewWidth; /**< width of QGraphicsView in scene coordinates. */
	TnoteOffset              					 m_offset;
	bool 										 					 m_isPianoStaff;
	TscoreScordature				 					*m_scordature;
	bool										 					 m_enableScord, m_tidyKey;
	TcombinedAnim											*m_accidAnim;
	QGraphicsSimpleTextItem 					*m_flyAccid;
	bool 										 					 m_selectableNotes, m_extraAccids;
	int																 m_maxNotesCount;
	qreal															 m_loNotePos, m_hiNotePos;
	bool															 m_lockRangeCheck; /**< to prevent the checking during clef switching */
	QPointer<QTimer>									 m_addTimer;
	int																 m_autoAddedNoteId; /**< Index of automatically added last note. */
	QPointer<TscoreNote>               m_noteWithAccidAnimed; /**< Pointer to note segment currently invoked to key animation */
	TscoreMeter                       *m_scoreMeter;

private:
	void createBrace();
	int getMaxNotesNr(qreal maxWidth); /**< Calculates notes number from given width */
	void findLowestNote(); /**< Checks all Y positions of staff notes ti find lowest one */
	void findHighestNote(); /**< Checks all Y positions of staff notes ti find highest one */
	void connectNote(TscoreNote *sn); /**< Performs all TscoreNote connections to this staff */

};

#endif // TSCORESTAFF_H
