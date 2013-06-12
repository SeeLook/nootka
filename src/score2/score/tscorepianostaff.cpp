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

#include "tscorepianostaff.h"
#include "tscorekeysignature.h"
#include "tscorenote.h"
#include "tscorescene.h"
#include "tscoreclef.h"
#include "tnote.h"
#include <QFont>
// #include <QFontMetrics>
#include <QGraphicsView>

// #include <QDebug>


TscorePianoStaff::TscorePianoStaff(TscoreScene* scene, int notesNr) :
	TscoreStaff(scene, notesNr, e_upper)
{	
	m_lower = new TscoreStaff(scene, notesNr, e_lower);
	m_lower->setParentItem(this);
	m_lower->setPos(0, TscoreStaff::boundingRect().height());
	for (int i = 0; i < notesNr; i++) {
		connect(noteSegment(i), SIGNAL(noteWasClicked(int)), this, SLOT(upperNoteChanged(int)));
		connect(m_lower->noteSegment(i), SIGNAL(noteWasClicked(int)), this, SLOT(lowerNoteChanged(int)));
	}
	connect(m_lower, SIGNAL(pianoStaffSwitched(Tclef)), this, SLOT(lowerStaffClefChanged(Tclef)));
// brace (Akolada)
	QGraphicsSimpleTextItem *brace = new QGraphicsSimpleTextItem();
	registryItem(brace);
	QFont ff = QFont("nootka", 25);
	brace->setFont(ff);
	brace->setText(QString(QChar(0xe16c)));
	qreal distance = m_lower->pos().y() + m_lower->upperLinePos() + 8 - upperLinePos();
	qreal fact = (distance + 1.2) / brace->boundingRect().height();
	brace->setScale(fact);
	brace->setBrush(scene->views()[0]->palette().text().color());
	brace->setPos(-2.0, upperLinePos() + distance / 2 - (brace->boundingRect().height() * brace->scale()) / 2 + 0.2);
}

TscorePianoStaff::~TscorePianoStaff() {}


void TscorePianoStaff::setNote(int index, Tnote& note) {
	bool inScale = true;
	if ((note.octave * 7 + note.note) > 7) {
		TscoreStaff::setNote(index, note); // set a note
		if (noteSegment(index)->notePos() == 0) // and check isit inscale
			inScale = false;			
	} else {
		m_lower->setNote(index, note);
		if (m_lower->noteSegment(index)->notePos() == 0)
			inScale = false;
	}
	if (inScale)
			*(getNote(index)) = note; // store new note when was set
	else
			*(getNote(index)) = Tnote(0, 0, 0); // or store empty note
}


void TscorePianoStaff::setEnableKeySign(bool isEnabled)
{
	TscoreStaff::setEnableKeySign(isEnabled);
	m_lower->setEnableKeySign(isEnabled);
	if (isEnabled) {
			scoreKey()->showKeyName(true);
			connect(scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(upperStaffChangedKey()));
			connect(m_lower->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(lowerStaffChangedKey()));
	}
}


void TscorePianoStaff::setScoreControler(TscoreControl* scoreControl) {
	TscoreStaff::setScoreControler(scoreControl);
	m_lower->setScoreControler(scoreControl);
}


void TscorePianoStaff::setDisabled(bool disabled) {
    TscoreStaff::setDisabled(disabled);
		m_lower->setDisabled(disabled);
}

		/** It overrides that method from TscoreStaff. 
		 * In piano staff the lower displays scordature and upper has got just increased width. */
void TscorePianoStaff::setScordature(Ttune& tune) {
		m_lower->setScordature(tune);
		setExtraWidth(KEY_WIDTH / 2);
}



QRectF TscorePianoStaff::boundingRect() const {
    return QRectF(0, 0, width() + 3, TscoreStaff::boundingRect().height() + m_lower->boundingRect().height());
}

//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscorePianoStaff::upperStaffChangedKey() {
	disconnect(m_lower->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(lowerStaffChangedKey()));
	m_lower->scoreKey()->setKeySignature(scoreKey()->keySignature());
	connect(m_lower->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(lowerStaffChangedKey()));
}

void TscorePianoStaff::lowerStaffChangedKey() {
	disconnect(scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(upperStaffChangedKey()));
	scoreKey()->setKeySignature(m_lower->scoreKey()->keySignature());
	connect(scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(upperStaffChangedKey()));
}

void TscorePianoStaff::upperNoteChanged(int noteIndex) {
	if (m_lower->noteSegment(noteIndex)->stringNumber()) // move string number up
			noteSegment(noteIndex)->setString(m_lower->noteSegment(noteIndex)->stringNumber());
	m_lower->noteSegment(noteIndex)->hideNote();
	m_lower->noteSegment(noteIndex)->removeString();
	// no necessary to emit noteChanged - this noteChanged is emited by ancesor class
}

void TscorePianoStaff::lowerNoteChanged(int noteIndex) {
	if (noteSegment(noteIndex)->stringNumber()) // move string number down
			m_lower->noteSegment(noteIndex)->setString(noteSegment(noteIndex)->stringNumber());
	noteSegment(noteIndex)->hideNote();
	noteSegment(noteIndex)->removeString();
	*(getNote(noteIndex)) = *(m_lower->getNote(noteIndex));
	emit noteChanged(noteIndex);
}

void TscorePianoStaff::lowerStaffClefChanged(Tclef clef) {
  emit pianoStaffSwitched(clef);
}







