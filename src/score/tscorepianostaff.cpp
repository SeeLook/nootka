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
#include <QFontMetrics>
#include <QApplication>
#include <QPalette>
#include <qrawfont.h>

// #include <QDebug>


TscorePianoStaff::TscorePianoStaff(TscoreScene* scene, int notesNr) :
	TscoreStaff(scene, notesNr, e_upper)
{	
	addLowerStaff();
	lower()->setParentItem(this);
	lower()->setPos(0, TscoreStaff::boundingRect().height());
	m_lowerHeight = lower()->height(); // Clumsy, but this way it can be put as boundingRect() param
	for (int i = 0; i < notesNr; i++) {
		connect(noteSegment(i), SIGNAL(noteWasClicked(int)), this, SLOT(upperNoteChanged(int)));
		connect(lower()->noteSegment(i), SIGNAL(noteWasClicked(int)), this, SLOT(lowerNoteChanged(int)));
	}
	connect(lower(), SIGNAL(pianoStaffSwitched(Tclef)), this, SLOT(lowerStaffClefChanged(Tclef)));
// brace (Akolada)
	QGraphicsSimpleTextItem *brace = new QGraphicsSimpleTextItem();
	registryItem(brace);
	QFont ff = QFont("nootka");
  ff.setPointSizeF(25.5);
  QFontMetrics fm(ff);
  ff.setPointSizeF(ff.pointSizeF() * (ff.pointSizeF() / fm.boundingRect(QChar(0xe16c)).height()));
  brace->setFont(ff);
	brace->setText(QString(QChar(0xe16c)));
	qreal distance = lower()->pos().y() + lower()->upperLinePos() + 8 - upperLinePos();
	qreal fact = (distance + 0.2) / brace->boundingRect().height();
	brace->setScale(fact);
	brace->setBrush(qApp->palette().text().color());
	brace->setPos(-2.0, upperLinePos() + distance / 2 - (brace->boundingRect().height() * brace->scale()) / 2 + 0.4);
}

TscorePianoStaff::~TscorePianoStaff() {}


void TscorePianoStaff::setNote(int index, Tnote& note) {
	bool inScale = true;
	Tnote emptyNote = Tnote(0, 0, 0);
	if (note.note) {
			if ((note.octave * 7 + note.note) > 7) {
					if (noteSegment(index)->isNoteAnimEnabled() && lower()->noteSegment(index)->notePos())
							noteSegment(index)->mainNote()->setPos(3.0, height() + lower()->noteSegment(index)->mainNote()->pos().y());
					TscoreStaff::setNote(index, note); // set a note
					if (noteSegment(index)->notePos() == 0) // and check is it in staff scale
						inScale = false;
					else // or reset lower staff
						lower()->setNote(index, emptyNote);
			} else {
					if (noteSegment(index)->isNoteAnimEnabled() && noteSegment(index)->notePos())
							lower()->noteSegment(index)->mainNote()->setPos(3.0, -height() + noteSegment(index)->mainNote()->pos().y());
					lower()->setNote(index, note);
					if (lower()->noteSegment(index)->notePos() == 0)
						inScale = false;
					else // or reset upper staff
						TscoreStaff::setNote(index, emptyNote);
			}
	} else {
		inScale = false;
		TscoreStaff::setNote(index, emptyNote);
		lower()->setNote(index, emptyNote);
	}
	if (inScale)
			*(getNote(index)) = note; // store new note when was set
	else
			*(getNote(index)) = emptyNote; // or store empty note
}


void TscorePianoStaff::setNoteDisabled(int index, bool isDisabled) {
		TscoreStaff::setNoteDisabled(index, isDisabled);
		lower()->setNoteDisabled(index, isDisabled);
}



void TscorePianoStaff::setEnableKeySign(bool isEnabled)
{
	TscoreStaff::setEnableKeySign(isEnabled);
	lower()->setEnableKeySign(isEnabled);
	if (isEnabled) {
			scoreKey()->showKeyName(true);
			connect(scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(upperStaffChangedKey()));
			connect(lower()->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(lowerStaffChangedKey()));
	}
}


void TscorePianoStaff::setScoreControler(TscoreControl* scoreControl) {
	TscoreStaff::setScoreControler(scoreControl);
	lower()->setScoreControler(scoreControl);
}


void TscorePianoStaff::setDisabled(bool disabled) {
    TscoreStaff::setDisabled(disabled);
		lower()->setDisabled(disabled);
}

		/** It overrides that method from TscoreStaff. 
		 * In piano staff the lower displays scordature and upper has got just increased width. */
void TscorePianoStaff::setScordature(Ttune& tune) {
		lower()->setScordature(tune);
		setEnableScordtature(lower()->hasScordature());
}



QRectF TscorePianoStaff::boundingRect() const {
    return QRectF(0, 0, width() + 9, TscoreStaff::height() + m_lowerHeight);
}

//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscorePianoStaff::upperStaffChangedKey() {
	disconnect(lower()->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(lowerStaffChangedKey()));
	lower()->scoreKey()->setKeySignature(scoreKey()->keySignature());
	connect(lower()->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(lowerStaffChangedKey()));
}

void TscorePianoStaff::lowerStaffChangedKey() {
	disconnect(scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(upperStaffChangedKey()));
	scoreKey()->setKeySignature(lower()->scoreKey()->keySignature());
	connect(scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(upperStaffChangedKey()));
}

void TscorePianoStaff::upperNoteChanged(int noteIndex) {
	if (lower()->noteSegment(noteIndex)->stringNumber()) // move string number up
			noteSegment(noteIndex)->setString(lower()->noteSegment(noteIndex)->stringNumber());
	lower()->noteSegment(noteIndex)->hideNote();
	lower()->noteSegment(noteIndex)->removeString();
	// no necessary to emit noteChanged - this noteChanged is emited by ancesor class
}

void TscorePianoStaff::lowerNoteChanged(int noteIndex) {
	if (noteSegment(noteIndex)->stringNumber()) // move string number down
			lower()->noteSegment(noteIndex)->setString(noteSegment(noteIndex)->stringNumber());
	noteSegment(noteIndex)->hideNote();
	noteSegment(noteIndex)->removeString();
	*(getNote(noteIndex)) = *(lower()->getNote(noteIndex));
	emit noteChanged(noteIndex);
}

void TscorePianoStaff::lowerStaffClefChanged(Tclef clef) {
  emit pianoStaffSwitched(clef);
}







