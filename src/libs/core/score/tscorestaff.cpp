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

#include "tscorestaff.h"
#include "tscorescene.h"
#include "tscoreclef.h"
#include "tscorenote.h"
#include "tscorekeysignature.h"
#include "tscorecontrol.h"
#include "tscorescordature.h"
#include <music/tnote.h>
#include <animations/tcombinedanim.h>
#include <QApplication>
#include <QPalette>

#include <QDebug>


TnoteOffset::TnoteOffset(int noteOff, int octaveOff) :
  note(noteOff),
  octave(octaveOff)
{}



TscoreStaff::TscoreStaff(TscoreScene* scene, int notesNr, TscoreStaff::Ekind kindOfStaff) :
  TscoreItem(scene),
  m_scoreControl(0),
  m_kindOfStaff(kindOfStaff),
  m_offset(TnoteOffset(3, 2)),
  m_keySignature(0),
  m_scordature(0),
  m_externWidth(0.0),
	m_enableScord(false),
	m_lower(0),
	m_accidAnim(0), m_flyAccid(0),
	m_index(0)
{
	setZValue(10);
  if (m_kindOfStaff == e_normal) {
    m_height = 40;
    m_upperLinePos = 16;
  } else {
    if (m_kindOfStaff == e_upper) {
      m_upperLinePos = 14;
			m_height = 26; 
		} else if (m_kindOfStaff == e_lower) {
      m_upperLinePos = 4;
			m_height = 22; 
		}
  }
  setAcceptHoverEvents(true);
// Clef
  Tclef cl = Tclef();
  if (kindOfStaff == e_lower) {
    cl = Tclef(Tclef::e_bass_F);
    m_offset = TnoteOffset(5, 0);
  }
  m_clef = new TscoreClef(scene, this, cl);
  connect(m_clef, SIGNAL(clefChanged()), this, SLOT(onClefChanged()));
	m_clef->setZValue(55);
	connect(m_clef, SIGNAL(switchPianoStaff(Tclef)), this, SLOT(onPianoStaffChanged(Tclef)));
// Notes
  for (int i = 0; i < notesNr; i++) {
			m_notes << new Tnote(0, 0, 0);
      m_scoreNotes << new TscoreNote(scene, this, i);
      m_scoreNotes[i]->setPos(7.0 + i * m_scoreNotes[i]->boundingRect().width(), 0);
			m_scoreNotes[i]->setZValue(50);
      connect(m_scoreNotes[i], SIGNAL(noteWasClicked(int)), this, SLOT(onNoteClicked(int)));
			connect(m_scoreNotes[i], SIGNAL(accidWasChanged(int)), this, SLOT(noteChangedAccid(int)));
  }
  
  if (m_scoreNotes.size())
		m_width = m_clef->boundingRect().width() + m_scoreNotes.size() * m_scoreNotes[0]->boundingRect().width() + 3;
	else 
		m_width = m_clef->boundingRect().width() + 3;
// Staff lines
  for (int i = 0; i < 5; i++) {
    m_lines[i] = new QGraphicsLineItem();
    registryItem(m_lines[i]);
    m_lines[i]->setPen(QPen(qApp->palette().text().color(), 0.15));
    m_lines[i]->setLine(1, upperLinePos() + i * 2, boundingRect().width() - 2, upperLinePos() + i * 2);
    m_lines[i]->setZValue(5);
  }
  
  for (int i = 0; i < 7; i++)
    accidInKeyArray[i] = 0;
  
}


TscoreStaff::~TscoreStaff() {
	for (int i = 0; i < m_notes.size(); i++)
		delete m_notes[i];
	m_notes.clear();
}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void TscoreStaff::setScoreControler(TscoreControl* scoreControl) {
	if (scoreControl) {
		m_scoreControl = scoreControl;
		connect(scoreControl, SIGNAL(accidButtonPressed(int)), this, SLOT(onAccidButtonPressed(int)));
	}
}


int TscoreStaff::noteToPos(Tnote& note)	{ 
	return m_offset.octave * 7 + m_offset.note + upperLinePos() - 1 - (note.octave * 7 + (note.note - 1)); 
}

		/** Calculation of note position works As folow:
		 * 1) expr: m_offset.octave * 7 + m_offset.note + upperLinePos() - 1 returns y position of note C in offset octave
		 * 2) (note.octave * 7 + (note.note - 1)) is number of note to be set.
		 * 3) Subtraction of them gives position of the note on staff with current clef and it is displayed 
		 * when this value is in staff scale. */
void TscoreStaff::setNote(int index, Tnote& note) {
	if (index >= 0 && index < m_scoreNotes.size()) {
		if (note.note)
				m_scoreNotes[index]->setNote(noteToPos(note), (int)note.acidental);
		else
				m_scoreNotes[index]->setNote(0, 0);
		if (m_scoreNotes[index]->notePos()) // store note in the list
				*(m_notes[index]) = note;
		else
				*(m_notes[index]) = Tnote(0, 0, 0);
		m_index = index;
	}
}


void TscoreStaff::insertNote(int index, Tnote& note, bool disabled) {
	index = qBound(0, index, m_scoreNotes.size()); // 0 - adds at the begin, size() adds at the end
	m_scoreNotes.insert(index, new TscoreNote(scoreScene(), this, index));
	m_notes.insert(index, new Tnote());
	updateWidth();
	setNote(index, note);
	setNoteDisabled(index, disabled);
	if (!disabled)
		m_index = index;
}


void TscoreStaff::addNote(Tnote& note, bool disabled) {
	insertNote(m_scoreNotes.size(), note, disabled);
}


void TscoreStaff::removeNote(int index) {
	if (index >= 0 && index < m_scoreNotes.size()) {
		m_scoreNotes.removeAt(index);
		m_notes.removeAt(index);
		updateWidth();
	}
}



void TscoreStaff::setNoteDisabled(int index, bool isDisabled) {
	if (index >=0 && index < m_scoreNotes.size())
		m_scoreNotes[index]->setReadOnly(isDisabled);
}


void TscoreStaff::setEnableKeySign(bool isEnabled) {
	if (isEnabled != (bool)m_keySignature) {
		if (isEnabled) {
			m_keySignature = new TscoreKeySignature(scoreScene(), this);
			m_keySignature->setPos(7.0, 0.0);
			m_keySignature->setClef(m_clef->clef());
			m_keySignature->setZValue(30);
			connect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
			m_flyAccid = new QGraphicsSimpleTextItem;
			registryItem(m_flyAccid);
			m_flyAccid->setFont(TscoreNote::getAccidFont());
			m_flyAccid->setScale(TscoreNote::accidScale());
			m_flyAccid->hide();
			if (m_scoreNotes.size())				
					m_flyAccid->setBrush(m_scoreNotes[0]->mainNote()->brush());
			m_accidAnim = new TcombinedAnim(m_flyAccid, this);
			connect(m_accidAnim, SIGNAL(finished()), this, SLOT(accidAnimFinished()));
			m_accidAnim->setDuration(300);
			m_accidAnim->setScaling(m_flyAccid->scale(), m_flyAccid->scale() * 3.0);
// 			m_accidAnim->scaling()->setEasingCurveType(QEasingCurve::OutQuint);
			m_accidAnim->setMoving(QPointF(), QPointF()); // initialize moving
			m_accidAnim->moving()->setEasingCurveType(QEasingCurve::OutBack);
			for (int i = 0; i < m_scoreNotes.size(); i++) {
				connect(m_scoreNotes[i], SIGNAL(fromKeyAnim(QString,QPointF,int)), this, SLOT(fromKeyAnimSlot(QString,QPointF,int)));
				connect(m_scoreNotes[i], SIGNAL(toKeyAnim(QString,QPointF,int)), this, SLOT(toKeyAnimSlot(QString,QPointF,int)));
				connect(m_accidAnim, SIGNAL(finished()), m_scoreNotes[i], SLOT(keyAnimFinished()));
			}
			if (m_scoreControl && !m_scoreControl->isEnabled()) {
					/** This is in case when score/staff is disabled and key signature is added.
					 * TscoreControl::isEnabled() determines availableness state. */
					m_keySignature->setReadOnly(true);
					m_keySignature->setAcceptHoverEvents(false);
			}
		} else {
					delete m_keySignature;
					m_keySignature = 0;
					m_accidAnim->deleteLater();
					m_accidAnim = 0;
					delete m_flyAccid;
					m_flyAccid = 0;
		}
		updateWidth();
	}
}


void TscoreStaff::setScordature(Ttune& tune) {
	if (!hasScordature()) {
		m_scordature = new TscoreScordature(scoreScene(), this);
		m_scordature->setParentItem(this);
		m_scordature->setZValue(35); // above key signature
		if (kindOfStaff() == e_upper)
			m_scordature->hide();
	}
	m_scordature->setTune(tune);
	if (m_scordature->isScordatured())	{ 
			m_enableScord = true;
	} else { // nothing to show - standard tune
			delete m_scordature;
			m_scordature = 0;
			m_enableScord = false;
	}
		updateWidth();
}


void TscoreStaff::removeScordatute() {
	delete m_scordature; 
	m_scordature = 0; 
	m_enableScord = false;
	updateWidth();	
}


void TscoreStaff::setDisabled(bool disabled) {
	scoreClef()->setReadOnly(disabled);
	if (scoreKey()) {
		scoreKey()->setAcceptHoverEvents(!disabled); // stops displaying status tip
		scoreKey()->setReadOnly(disabled);
	}
	for (int i = 0; i < m_scoreNotes.size(); i++) {
		m_scoreNotes[i]->setReadOnly(disabled);
		m_scoreNotes[i]->hideWorkNote();
	}
}


QRectF TscoreStaff::boundingRect() const {
  return QRectF(0, 0, m_width, m_height);
}


int TscoreStaff::accidNrInKey(int noteNr, char key) {
	int accidNr;
	switch ((56 + notePosRelatedToClef(noteNr, m_offset)) % 7 + 1) {
		case 1: accidNr = 1; break;
		case 2: accidNr = 3; break;
		case 3: accidNr = 5; break;
		case 4: accidNr = 0; break;
		case 5: accidNr = 2; break;
		case 6: accidNr = 4; break;
		case 7: accidNr = 6; break;
	}
	if (key < 0)
		accidNr = 6 - accidNr;
	return accidNr;
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreStaff::setEnableScordtature(bool enable) {
	if (enable != m_enableScord) {
		m_enableScord = enable;
		updateWidth();
	}
}


void TscoreStaff::addLowerStaff() {
	if (!m_lower) {
		m_lower = new TscoreStaff(scoreScene(), m_scoreNotes.size(), e_lower);
	}
}

//##########################################################################################################
//####################################### PROTECTED SLOTS  #################################################
//##########################################################################################################

void TscoreStaff::onClefChanged( ) {
	int globalNr;
	if (m_scoreNotes.size())
		globalNr = notePosRelatedToClef(m_scoreNotes[0]->notePos(), m_offset);
  switch(m_clef->clef().type()) {
    case Tclef::e_treble_G:
      m_offset = TnoteOffset(3, 2); break;
    case Tclef::e_treble_G_8down:
      m_offset = TnoteOffset(3, 1); break;
    case Tclef::e_bass_F:
      m_offset = TnoteOffset(5, 0); break;
    case Tclef::e_bass_F_8down:
      m_offset = TnoteOffset(5, -1); break;
    case Tclef::e_alto_C:
      m_offset = TnoteOffset(4, 1); break;
    case Tclef::e_tenor_C:
      m_offset = TnoteOffset(2, 1); break;
		default : break;
  }
  if (m_keySignature)
      m_keySignature->setClef(m_clef->clef());
	if (m_scoreNotes.size()) {
			int newNr = notePosRelatedToClef(m_scoreNotes[0]->notePos(), m_offset);
			for (int i = 0; i < m_scoreNotes.size(); i++) {
				if (m_scoreNotes[i]->notePos()) {
						m_scoreNotes[i]->moveNote(m_scoreNotes[i]->notePos() + m_scoreNotes[i]->ottava() * 7 - (globalNr - newNr));
						if (m_scoreNotes[i]->notePos() == 0) // reset Tnote list to 0 when new note is not on the staff
							*(m_notes[i]) = Tnote(0, 0, 0);
				} 
			}
	}
	emit clefChanged(scoreClef()->clef());
}


void TscoreStaff::onKeyChanged() {
  for (int i = 0; i < m_scoreNotes.size(); i++) {
    if (m_scoreNotes[i]->notePos())
        m_scoreNotes[i]->moveNote(m_scoreNotes[i]->notePos());
  }
}


void TscoreStaff::onNoteClicked(int noteIndex) {
  int globalNr = notePosRelatedToClef(m_scoreNotes[noteIndex]->notePos() + m_scoreNotes[noteIndex]->ottava() * 7, m_offset);
	m_notes[noteIndex]->note = (char)(56 + globalNr) % 7 + 1;
	m_notes[noteIndex]->octave = (char)(56 + globalNr) / 7 - 8;
	m_notes[noteIndex]->acidental = (char)m_scoreNotes[noteIndex]->accidental();
	m_index = noteIndex;
	emit noteChanged(noteIndex);
}


void TscoreStaff::noteChangedAccid(int accid) {
	if (m_scoreControl) {
			m_scoreControl->setAccidental(accid);
	}
}


void TscoreStaff::onAccidButtonPressed(int accid) {
	scoreScene()->setCurrentAccid(accid);
	/** It is enough to do this as long as every TscoreNote handles mouseHoverEvent
	 * which checks value set above and changes accidental symbol if necessary. */
}


void TscoreStaff::fromKeyAnimSlot(QString accidText, QPointF accidPos, int notePos) {
	m_flyAccid->setText(accidText);
	m_accidAnim->setMoving(mapFromScene(m_keySignature->accidTextPos(accidNrInKey(notePos, scoreKey()->keySignature()))),
												 mapFromScene(accidPos));
	m_accidAnim->startAnimations();
	m_flyAccid->show();
}


void TscoreStaff::toKeyAnimSlot(QString accidText, QPointF accidPos, int notePos) {
	m_flyAccid->setText(accidText);
	m_accidAnim->setMoving(mapFromScene(accidPos),
												 mapFromScene(m_keySignature->accidTextPos(accidNrInKey(notePos, scoreKey()->keySignature()))));
	m_accidAnim->startAnimations();
	m_flyAccid->show();
}


void TscoreStaff::accidAnimFinished() {
	m_flyAccid->hide();
}

//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

void TscoreStaff::updateWidth() {
	qreal off = 0.0;
	if (m_keySignature)
			off = KEY_WIDTH + 1.5;
	else if (m_enableScord)
			off = KEY_WIDTH / 2;
	if (m_scoreNotes.size())
			m_width = 10.0 + off + m_scoreNotes.size() * m_scoreNotes[0]->boundingRect().width() + 2.0;
	else
			m_width = 10.0 + off + 2.0;
	if (m_externWidth > m_width)
		m_width = m_externWidth;
	
	for (int i = 0; i < m_scoreNotes.size(); i++) // update positions of the notes
				m_scoreNotes[i]->setPos(7.0 + off + i * m_scoreNotes[0]->boundingRect().width(), 0);
	for (int i = 0; i < 5; i++) // adjust staff lines length
				m_lines[i]->setLine(1, upperLinePos() + i * 2, width() - 2, upperLinePos() + i * 2);
	if (lower())
		lower()->updateWidth();
	scoreScene()->update();
}



