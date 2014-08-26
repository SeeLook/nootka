/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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


#ifndef TMULTISCORE_H
#define TMULTISCORE_H

#include <score/tsimplescore.h>
#include <score/tscorenote.h>


class QMainWindow;
class TscoreStaff;



/** 
 * This is subclass of TsimpleScore which adds support for multiple staves/systems
 */
class TmultiScore : public TsimpleScore
{
	Q_OBJECT
	
public:
	
	TmultiScore(QMainWindow* mw, QWidget *parent = 0);
	~TmultiScore();
	
	QMainWindow* mainWindow() { return m_mainWindow; } /** Nootka main window */
	
	enum EinMode {
		e_single, // single note mode
		e_multi, // adding notes and multiple staves
		e_record // calling setNote() from outside switches current index to the next note or adds one
	}; /** Describes insert mode of a score */
	
	EinMode insertMode() { return m_inMode; }
	void setInsertMode(EinMode mode);
		
	int currentIndex() { return m_currentIndex; }
	
	TscoreStaff* staves(int id) { return m_staves[id]; } /** Returns given staff */
	TscoreStaff* lastStaff() { return m_staves.last(); }
	TscoreStaff* firstStaff() { return m_staves.first(); }
	int staffCount() { return m_staves.size(); }
	
	
	virtual void setNote(const Tnote& note);
	
	virtual void setEnableKeySign(bool isEnabled);
	
	void setScoreDisabled(bool disabled);
	
	qreal scoreScale() { return m_scale; } /** Scale factor of user zoom. */
	void setScoreScale(qreal sc);
	
	bool ainmationsEnabled() { return m_useAinim; }
	void setAnimationsEnabled(bool aEnabled) { m_useAinim = aEnabled; }
	
	int notesCount(); /** Total number of notes on the score */
	
	void selectNote(int id); /** Marks given note or cancel selection if @p -1 */
	
public slots:
	void noteWasClicked(int index);
	void noteWasSelected(int index);
	void onClefChanged(Tclef cl);
	void removeCurrentNote(); /** Removes currently selected note, if any. */
	void deleteNotes(); /** Deletes all notes from the score - single one remains */
	void resizeSlot(); /** Invokes resizeEvent() of simple score. */
	
protected:
	virtual void resizeEvent(QResizeEvent* event);
	
	int clickedOffset() { return m_clickedOff; }
	void updateSceneRect(); /** Adjusts score scene to space required by staff(staves) */
	void changeCurrentIndex(int newIndex);
	TscoreStaff* currentStaff();
	void resetClickedOff() { m_clickedOff = 0; }
	void checkAndAddNote(TscoreStaff* sendStaff, int noteIndex);
	
	
protected slots:
	void keyChangedSlot();
	
	void staffHasNoSpace(int staffNr); /** Create new (next) staff */
	void staffHasFreeSpace(int staffNr, int notesFree); /** Move notes to this staff from the next one */
	void noteGetsFree(int staffNr, TscoreNote* freeNote);
	void noteRemovingSlot(int staffNr, int noteToDel);
	void noteAddingSlot(int staffNr, int noteToDel);
	void staffLoNoteChanged(int staffNr, qreal loNoteYoff);
	void staffHiNoteChanged(int staffNr, qreal hiNoteYoff);
	void adjustStaffWidth(TscoreStaff *st); /** Calls TscoreStaff::setViewWidth with score width  */
	
			/** Adds given staff at the end of m_staves list or creates a new one.
			* Sets staff number corresponding to its index in the m_staves list,
			* connects the staff with TmainScore slots */
	virtual void addStaff(TscoreStaff* st = 0);
	void deleteLastStaff();
	
private:
	QMainWindow									*m_mainWindow;
	EinMode											 m_inMode;
	qreal												 m_scale;
	
			/** m_clickedOff indicates whether setNote() is set to m_currentIndex and whether to the next after current */
	int													 m_clickedOff, m_currentIndex;
	bool 												 m_useAinim, m_addNoteAnim;
	
	QList<TscoreStaff*>					 m_staves; // list of staves in page view
	
	
};

#endif // TMULTISCORE_H
