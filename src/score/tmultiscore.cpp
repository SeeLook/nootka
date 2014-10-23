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

#include "tmultiscore.h"
#include <score/tscorestaff.h>
#include <score/tscorekeysignature.h>
#include <score/tscorescene.h>
#include <score/tscorenote.h>
#include <score/tscoreclef.h>
#include <score/tnotecontrol.h>
#include <QtWidgets>



#define SENDER_TO_STAFF static_cast<TscoreStaff*>(sender())



TmultiScore::TmultiScore(QMainWindow* mw, QWidget* parent) :
	TsimpleScore(1, parent),
	m_mainWindow(mw),
	m_inMode(e_multi),
	m_scale(1.0),
	m_clickedOff(0), m_currentIndex(-1),
	m_useAinim(true),
	m_addNoteAnim(true)
{
	setObjectName("m_mainScore");
	setStyleSheet("TsimpleScore#m_mainScore { background: transparent }");
	setContentsMargins(2, 2, 2, 2);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	staff()->setZValue(11); // to be above next staves - TnoteControl requires it
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	setMaximumWidth(QWIDGETSIZE_MAX); // revert what TsimpleScore 'broke'
	setAlignment(Qt::AlignCenter);
}

TmultiScore::~TmultiScore()
{}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void TmultiScore::setInsertMode(TmultiScore::EinMode mode) {
	if (mode != m_inMode) {
		bool ignoreThat = false;
		if ((mode == e_record && m_inMode == e_multi) || (mode == e_multi && m_inMode == e_record))
			ignoreThat = true;
		m_inMode = mode;
		if (ignoreThat)
			return;
		if (mode == e_single) {
				deleteNotes();
				staff()->setStafNumber(-1);
				staff()->setViewWidth(0.0);
				m_addNoteAnim = false;
				staff()->insertNote(1, true);
				m_addNoteAnim = false;
				staff()->insertNote(2, true);
				setControllersEnabled(true, false);
				setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
				scoreScene()->left()->enableToAddNotes(false);
				scoreScene()->right()->enableToAddNotes(false);
				m_currentIndex = 0;
				setAlignment(Qt::AlignLeft);
				setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
				resizeEvent(0);
		} else {
				staff()->setStafNumber(0);
				deleteNotes();
				setControllersEnabled(true, true);
				scoreScene()->left()->enableToAddNotes(true);
				scoreScene()->right()->enableToAddNotes(true);
				setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
				setMaximumWidth(QWIDGETSIZE_MAX); // revert what TsimpleScore 'broke'
				setAlignment(Qt::AlignCenter);
				setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
				resizeEvent(0);
		}
	}
}


void TmultiScore::setNote(const Tnote& note) {
	if (insertMode() != e_single) {
			if (currentIndex() == -1)
				changeCurrentIndex(0);
			TscoreStaff *thisStaff = currentStaff();
			if (insertMode() == e_record) {
					changeCurrentIndex(currentIndex() + m_clickedOff);
					thisStaff = currentStaff();
					m_clickedOff = 1;
			}
			thisStaff->setNote(currentIndex() % staff()->maxNoteCount(), note);
			if (staffCount() > 1)
					ensureVisible(thisStaff, 0, 0);
	} else {
			TsimpleScore::setNote(0, note);
	}
}


Tnote* TmultiScore::getNote(int index) {
	if (index >= 0 && index < notesCount())
		return noteFromId(index)->note();
	else
		return 0;
}


void TmultiScore::setEnableKeySign(bool isEnabled) {
	if (isEnabled != (bool)staff()->scoreKey()) {
		for (int i = 0; i < m_staves.size(); ++i) {
			staves(i)->setEnableKeySign(isEnabled);
			if (isEnabled)
				connect(staves(i)->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedSlot()));
		}
		if (isEnabled)
				staff()->scoreKey()->showKeyName(true);
// 		resizeEvent(0);
	}
}


void TmultiScore::setScoreDisabled(bool disabled) {
	for (int i = 0; i < m_staves.size(); ++i) {
		m_staves[i]->setDisabled(disabled);
	}
	if (disabled) {
		scoreScene()->left()->hide();
		scoreScene()->right()->hide();
	}
}


void TmultiScore::setScoreScale(qreal sc) {
	if (sc != m_scale) {
		m_scale = sc;
		resizeEvent(0);
	}
}


int TmultiScore::notesCount() {
	return (m_staves.size() - 1) * staff()->maxNoteCount() + m_staves.last()->count();
}


void TmultiScore::selectNote(int id) {
	if (id >= -1 && id < notesCount())
		changeCurrentIndex(id);
}


//####################################################################################################
//#################################    PUBLIC SLOTS       ############################################
//####################################################################################################

void TmultiScore::noteWasClicked(int index) {
	TscoreStaff *st = SENDER_TO_STAFF;
	Tnote note = *(st->getNote(index));
	changeCurrentIndex(index + st->number() * st->maxNoteCount());
	m_clickedOff = 0;
	emit noteWasChanged(index, note);
	st->noteSegment(index)->update();
  checkAndAddNote(st, index);
}


void TmultiScore::noteWasSelected(int index) {
	m_clickedOff = 0;
	TscoreStaff *st = SENDER_TO_STAFF;
	changeCurrentIndex(index + st->number() * st->maxNoteCount());
	emit noteWasChanged(index, *st->getNote(index));
}


void TmultiScore::onClefChanged(Tclef cl) {
	if (staffCount() > 1) {
			int staffNr = SENDER_TO_STAFF->number();
			for (int i = 0; i < staffCount(); i++)
				if (staves(i)->number() != staffNr) {
					staves(i)->disconnect(SIGNAL(clefChanged(Tclef)));
					staves(i)->onClefChanged(cl);
					connect(staves(i), SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
				}
	}
	TsimpleScore::onClefChanged(cl);
}


void TmultiScore::removeCurrentNote() {
	if (currentIndex() > 0 || (currentIndex() == 0 && staff()->count() > 1))
		currentStaff()->removeNote(currentIndex() % staff()->maxNoteCount());
	else if (currentIndex() == 0) { // just clear first note when only one
		m_clickedOff = 0;
		setNote(Tnote());
		m_clickedOff = 0; // in record mode this is increased after setNote()
		emit noteWasChanged(0, Tnote());
	}
	staff()->noteSegment(0)->hideWorkNote();
}


void TmultiScore::deleteNotes() {
	if (!staff()->count())
			return; // nothing to delete
	m_currentIndex = 0;
	while (staffCount() > 1)
		deleteLastStaff();
	if (staff()->count() > 1) {
		QList<TscoreNote*> notesToDel;
		staff()->takeNotes(notesToDel, 1, staff()->count() - 1);
		for (int i = 0; i <notesToDel.size(); i++)
			delete notesToDel[i];
	}
	removeCurrentNote();
	updateSceneRect();
	m_currentIndex = -1;
}

//####################################################################################################
//###################################   PROTECTED   ##################################################
//####################################################################################################
/** To call TsimpleScore::resizeEvent twice solves problem 
 * with adjusting score size to scene (staff) in single note mode. */
void TmultiScore::resizeSlot() {
	TsimpleScore::resizeEvent(0);
}


void TmultiScore::resizeEvent(QResizeEvent* event) {
  int hh = height(), ww = width();
	if (event) {
		hh = event->size().height();
		ww = event->size().width();
	}
	if (ww < 300)
      return;
	if (m_inMode == e_single) {
		TsimpleScore::resizeEvent(event);
		QTimer::singleShot(10, this, SLOT(resizeSlot()));
	} else {
		QList<TscoreNote*> allNotes;
		for (int i = 0; i < m_staves.size(); i++) { // grab all TscoreNote
			m_staves[i]->takeNotes(allNotes, 0, m_staves[i]->count() - 1);
		}
		qreal staffOff = 0.0;
		if (staff()->isPianoStaff())
			staffOff = 1.1;
		qreal factor = (((qreal)hh / (staff()->height() + 2.0)) / transform().m11()) / m_scale;
		scale(factor, factor);
		int stavesNumber; // how many staves are needed
		for (int i = 0; i < m_staves.size(); i++) {
			adjustStaffWidth(m_staves[i]);
			if (i == 0) { // first loop - make preparations for new amount of staves
				stavesNumber = allNotes.size() / m_staves[0]->maxNoteCount(); // needed staves for this amount of notes
				if (allNotes.size() % m_staves[0]->maxNoteCount())
						stavesNumber++;
				if (stavesNumber > m_staves.size()) { // create new staff(staves)
						int stavesToAdd = stavesNumber - m_staves.size();
						for (int s = 0; s < stavesToAdd; s++) {
							addStaff();
						}
				} else if (stavesNumber < m_staves.size()) { // or delete unnecessary staves
						int stavesToDel = m_staves.size() - stavesNumber;
						for (int s = 0; s < stavesToDel; s++) {
							deleteLastStaff();
						}
				}
			}
			if (allNotes.size() > i * m_staves[i]->maxNoteCount()) {
					QList<TscoreNote*> stNotes = allNotes.mid(i * m_staves[i]->maxNoteCount(), m_staves[i]->maxNoteCount());
					m_staves[i]->addNotes(0, stNotes);
			}
			
			if (i == 0)
				m_staves[i]->setPos(staffOff, 0.05);
			else {
				qreal yOff = 4.0;
				if (staff()->hasScordature() && i == 1)
						yOff += 3.0;
				m_staves[i]->setPos(staffOff, m_staves[i - 1]->pos().y() + m_staves[i - 1]->loNotePos() - m_staves[i]->hiNotePos() + yOff);
			}
		}
		updateSceneRect();
	}
}


void TmultiScore::updateSceneRect() {
	qreal sh;
	if (m_staves.size() == 1)
		sh = (staff()->height() + 0.1) * m_scale;
	else
		sh = m_staves.last()->pos().y() + m_staves.last()->height();
	QRectF scRec = staff()->mapToScene(QRectF(0.0, 0.0, 
								staff()->width() + (staff()->isPianoStaff() ? 1.1 : 0.0),	sh)).boundingRect();
	scoreScene()->setSceneRect(0.0, 0.0, scRec.width(), scRec.height());
	qDebug() << "updateSceneRect" << scoreScene()->sceneRect() << m_staves.size();
}


void TmultiScore::changeCurrentIndex(int newIndex) {
	if (m_inMode != e_single && newIndex != m_currentIndex) {
			int prevIndex = m_currentIndex;
			if (m_currentIndex >= 0) { // deselect previous note
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->selectNote(false);
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->setBackgroundColor(-1);
			}
			m_currentIndex = newIndex;
			if (m_currentIndex >= 0) {
				if (m_currentIndex / staff()->maxNoteCount() == m_staves.size()) // add new staff with single note
						staffHasNoSpace(m_currentIndex / staff()->maxNoteCount() - 1);
				else if (m_currentIndex % staff()->maxNoteCount() == currentStaff()->count())
						checkAndAddNote(currentStaff(), m_currentIndex % staff()->maxNoteCount() - 1);
				else if (m_currentIndex / staff()->maxNoteCount() > m_staves.size() ||
								m_currentIndex % staff()->maxNoteCount() > currentStaff()->count()) {
										qDebug() << "Something wrong with current index" << m_currentIndex; 
										return;
				}
			}
			if (m_currentIndex >= 0) { // select a new note
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->setBackgroundColor(palette().highlight().color());
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->selectNote(true);
				if (prevIndex / staff()->maxNoteCount() != m_currentIndex / staff()->maxNoteCount())
				ensureVisible(currentStaff(), 0, 0);
			}
	}
}


TscoreStaff* TmultiScore::currentStaff() {
	return m_staves[m_currentIndex / staff()->maxNoteCount()];
}


/**  In record mode  add new 'empty' note segment at the end off the staff when index is on its last note 
 * but ignore last possible note on the staff - new staff is already created with a new single note */
void TmultiScore::checkAndAddNote(TscoreStaff* sendStaff, int noteIndex) {
  if (insertMode() == e_record && noteIndex == sendStaff->count() - 1 && noteIndex != sendStaff->maxNoteCount() - 1) {
      Tnote nn(0, 0, 0);
			m_addNoteAnim = false; // do not show adding note animation when note is added here
			sendStaff->addNote(nn);
			if (staff()->noteSegment(0)->noteName())
				sendStaff->noteSegment(sendStaff->count() - 1)->showNoteName();
  }
}



void TmultiScore::adjustStaffWidth(TscoreStaff* st) {
	int scrollOff = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
	st->setViewWidth((width() - 25 - scrollOff) / transform().m11());
}


void TmultiScore::addStaff(TscoreStaff* st) {
	if (st == 0) { // create new staff at the end of a list
		m_staves << new TscoreStaff(scoreScene(), 1);
		m_staves.last()->onClefChanged(m_staves.first()->scoreClef()->clef());
		m_staves.last()->scoreClef()->setReadOnly(m_staves.first()->scoreClef()->readOnly());
		lastStaff()->setEnableKeySign(staff()->scoreKey());
		if (m_staves.last()->scoreKey())
			m_staves.last()->scoreKey()->setKeySignature(m_staves.first()->scoreKey()->keySignature());
		connect(m_staves.last(), SIGNAL(hiNoteChanged(int,qreal)), this, SLOT(staffHiNoteChanged(int,qreal))); // ignore for first
} else { // staff of TsimpleScore is added this way
		st->enableToAddNotes(true);
		st->disconnect(SIGNAL(noteChanged(int)));
		st->disconnect(SIGNAL(clefChanged(Tclef)));
		m_staves << st;
	}
	m_staves.last()->setStafNumber(m_staves.size() - 1);
	connect(m_staves.last(), SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
	connect(m_staves.last(), SIGNAL(noteSelected(int)), this, SLOT(noteWasSelected(int)));
	connect(m_staves.last(), SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
	connect(m_staves.last(), SIGNAL(noMoreSpace(int)), this, SLOT(staffHasNoSpace(int)));
	connect(m_staves.last(), SIGNAL(freeSpace(int,int)), this, SLOT(staffHasFreeSpace(int,int)));
	connect(m_staves.last(), SIGNAL(noteToMove(int,TscoreNote*)), this, SLOT(noteGetsFree(int,TscoreNote*)));
	connect(m_staves.last(), SIGNAL(noteIsRemoving(int,int)), this, SLOT(noteRemovingSlot(int,int)));
	connect(m_staves.last(), SIGNAL(noteIsAdding(int,int)), this, SLOT(noteAddingSlot(int,int)));
	connect(m_staves.last(), SIGNAL(loNoteChanged(int,qreal)), this, SLOT(staffLoNoteChanged(int,qreal)));
	if (m_staves.last()->scoreKey())
		connect(m_staves.last()->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedSlot()));
}


void TmultiScore::deleteLastStaff() {
	delete m_staves.last();
	m_staves.removeLast();
	qDebug() << "staff deleted";
}


//####################################################################################################
//#################################    PROTECTED SLOTS    ############################################
//####################################################################################################

void TmultiScore::keyChangedSlot() {
	if (m_staves.size() == 1)
		return;
	TscoreKeySignature *key = static_cast<TscoreKeySignature*>(sender());
	for (int i = 0; i < m_staves.size(); i++)
		if (m_staves[i]->scoreKey() != key) {
			disconnect(m_staves[i]->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedSlot()));
			m_staves[i]->scoreKey()->setKeySignature(key->keySignature());
			connect(m_staves[i]->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedSlot()));
		}
}


/** Managing staves and notes:
 * - all is started from addStaff() method - staff is connected with quite big amount of slots
 * - notes can be added to staff automatically (in record mode) or manually by user
 * - when staff has no space to display note in view visible area it emits noMoreSpace() 
 *     connected to staffHasNoSpace() then next staff is added
 * - when note is inserted into staff and it has note to move at its end, noteToMove is emitted
 *    then noteGetsFree() squeezes the note at beginning on the next staff
 * - notes can be deleted and then staff emits freeSpace() connected to staffHasFreeSpace()
 *    First note from next staff is taken and put at the end of emitting staff.
 * Staves are placed partially one over another so changing note pitch can give collisions.
 * To avoid this staff has loNotePos() and hiNotePos() with appropriate values and emits
 * loNoteChanged() and hiNoteChanged() with difference to previous state.
 * This invokes adjusting of staff (staves) position(s) and scene rectangle size. 
 */

void TmultiScore::staffHasNoSpace(int staffNr) {
	addStaff();
	adjustStaffWidth(m_staves.last());
	m_staves.last()->checkNoteRange(false);
	qreal yOff = 4.0;
	if (staff()->hasScordature() && m_staves.last()->number() == 1)
		yOff += 3.0;
	m_staves.last()->setPos(staff()->pos().x(), 
													m_staves[staffNr]->y() + m_staves[staffNr]->loNotePos() - m_staves.last()->hiNotePos() + yOff);
	updateSceneRect();
}


void TmultiScore::staffHasFreeSpace(int staffNr, int notesFree) {
	qDebug() << "staffHasFreeSpace" << staffNr << notesFree;
	if (m_staves[staffNr] != m_staves.last()) { // is not the last staff,
		QList<TscoreNote*> notes;
		m_staves[staffNr + 1]->takeNotes(notes, 0, notesFree - 1); // take first note from the next staff
		m_staves[staffNr]->addNotes(m_staves[staffNr]->count(), notes); // and add it to this staff
		if (staffNr + 2 < m_staves.size()) { // call the same method for next staff
			staffHasFreeSpace(staffNr + 1, notesFree);
		} else if (!m_staves[staffNr + 1]->count()) {
				/*delete */m_staves[staffNr + 1]->deleteLater();
				m_staves.removeAt(staffNr + 1);
				updateSceneRect();
		}
	} else if (!m_staves[staffNr]->count()) {
			/*delete*/ m_staves[staffNr]->deleteLater();
			m_staves.removeAt(staffNr);
			updateSceneRect();
	}
}


void TmultiScore::noteGetsFree(int staffNr, TscoreNote* freeNote) {
	qDebug() << "noteGetFree" << staffNr << freeNote->note()->toText();
	if (staffNr + 1 == m_staves.size())
		staffHasNoSpace(staffNr); // add staff
	for (int i = m_staves.size() - 2; i >= staffNr + 1; i--) { // make space in next staves
			QList<TscoreNote*> notes;
			m_staves[i]->takeNotes(notes, m_staves[i]->count() - 1, m_staves[i]->count() - 1);
			m_staves[i + 1]->addNotes(0, notes);
		}			
	m_staves[staffNr + 1]->addNote(0, freeNote);
}


void TmultiScore::noteAddingSlot(int staffNr, int noteToAdd) {
	if (staffNr * staff()->maxNoteCount() + noteToAdd < m_currentIndex) {
// 		qDebug() << "selected note moved forward";
		m_currentIndex++;
	}
	if (staff()->noteSegment(0)->noteName() || staff()->noteSegment(staff()->count() - 1)->noteName())
			m_staves[staffNr]->noteSegment(noteToAdd)->showNoteName();
	m_staves[staffNr]->noteSegment(noteToAdd)->enableAccidToKeyAnim(true);
	if (m_useAinim && m_addNoteAnim)
		m_staves[staffNr]->noteSegment(noteToAdd)->popUpAnim(300);
	m_addNoteAnim = true;
}


void TmultiScore::noteRemovingSlot(int staffNr, int noteToDel) {
	if (staffNr * staff()->maxNoteCount() + noteToDel == m_currentIndex) {
		qDebug() << "current selected note will be removed";
		changeCurrentIndex(-1);
	} else if (staffNr * staff()->maxNoteCount() + noteToDel < m_currentIndex) {
		qDebug() << "selected note moved backward";
		m_currentIndex--;
	}
}



void TmultiScore::staffHiNoteChanged(int staffNr, qreal hiNoteYoff) {
	qDebug() << "staffHiNoteChanged" << hiNoteYoff << m_staves[staffNr]->hiNotePos();
	for (int i = staffNr; i < m_staves.size(); i++) // move every staff with difference
			m_staves[i]->setY(m_staves[i]->y() + hiNoteYoff);
	updateSceneRect();
}


void TmultiScore::staffLoNoteChanged(int staffNr, qreal loNoteYoff) {
	if (m_staves.size() > 1 && staffNr < m_staves.size() - 1) { // more staves and not the last one
		qDebug() << "staffLoNoteChanged" << loNoteYoff << m_staves[staffNr]->loNotePos();
		for (int i = staffNr + 1; i < m_staves.size(); i++) // move every staff with difference
			m_staves[i]->setY(m_staves[i]->y() + loNoteYoff);
		updateSceneRect();
	}
}







