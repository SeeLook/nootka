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
#include <tnote.h>
#include <QFont>
#include <qfontmetrics.h>


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
	connect(m_lower, SIGNAL(pianoStaffSwitch(Tclef)), this, SLOT(lowerStaffClefChanged(Tclef)));
// brace (Akolada)
	QGraphicsSimpleTextItem *brace = new QGraphicsSimpleTextItem();
	registryItem(brace);
	brace->setParentItem(this);
	QFont ff = QFont("nootka");
	ff.setPointSizeF(24.5);
	QFontMetrics fMetr(ff);
	qreal fact = ff.pointSizeF() / fMetr.boundingRect(QChar(0xe16c)).height();
	ff.setPointSizeF(ff.pointSizeF() * fact);
	brace->setFont(ff);
	brace->setText(QString(QChar(0xe16c)));
	brace->setPos(0.8, 7);
}

TscorePianoStaff::~TscorePianoStaff() {}


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


QRectF TscorePianoStaff::boundingRect() const
{
    return QRectF(0, 0, width(), 40);
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
	m_lower->noteSegment(noteIndex)->hideNote();
	// no nessessary to emit noteChanged - this noteChanged is emited by ancesor class
}

void TscorePianoStaff::lowerNoteChanged(int noteIndex) {
	noteSegment(noteIndex)->hideNote();
	getNote(noteIndex)->note = m_lower->getNote(noteIndex)->note;
	getNote(noteIndex)->acidental = m_lower->getNote(noteIndex)->acidental;
	getNote(noteIndex)->acidental = m_lower->getNote(noteIndex)->acidental;
	emit noteChanged(noteIndex);
}

void TscorePianoStaff::lowerStaffClefChanged(Tclef clef) {
  emit pianoStaffSwitch(clef);
}







