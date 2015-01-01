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
#include "tscorescordature.h"
#include "tnotecontrol.h"
#include <music/tnote.h>
#include <animations/tcombinedanim.h>
#include <tnoofont.h>
#include <QApplication>
#include <QGraphicsView>
#include <QPalette>

#include <QDebug>


TnoteOffset::TnoteOffset(int noteOff, int octaveOff) :
  note(noteOff),
  octave(octaveOff)
{}



TscoreStaff::TscoreStaff(TscoreScene* scene, int notesNr) :
  TscoreItem(scene),
  m_staffNr(-1),
  m_keySignature(0),
  m_upperLinePos(16.0), m_lowerStaffPos(0.0),
  m_height(40.0),
  m_viewWidth(0.0),
  m_offset(TnoteOffset(3, 2)),
  m_isPianoStaff(false),
	m_scordature(0), m_enableScord(false),
	m_accidAnim(0), m_flyAccid(0),
	m_index(0), m_selectableNotes(false), m_extraAccids(false),
	m_maxNotesCount(0),
	m_loNotePos(28.0), m_hiNotePos(12.0),
	m_lockRangeCheck(false)
{
	setFlag(QGraphicsItem::ItemHasNoContents);
	enableTouchToMouse(false); // Do not propagate - hasCursor() is not necessary
	m_lines[0] = 0;
	m_lowLines[0] = 0; // first array item points are the all items exist or not
	setZValue(10);
  setAcceptHoverEvents(true);
// Clef
  Tclef cl = Tclef();
  m_clef = new TscoreClef(scene, this, cl);
  connect(m_clef, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
	m_clef->setZValue(29); // lower than key signature (if any)
// Notes
  for (int i = 0; i < notesNr; i++) {
      m_scoreNotes << new TscoreNote(scene, this, i);
      m_scoreNotes[i]->setPos(7.0 + i * m_scoreNotes[i]->boundingRect().width(), 0);
			m_scoreNotes[i]->setZValue(50);
			connectNote(m_scoreNotes[i]);
  }
  
// Staff lines, it also sets m_width of staff
	prepareStaffLines();
	
  for (int i = 0; i < 7; i++)
    accidInKeyArray[i] = 0;
}


TscoreStaff::~TscoreStaff() {
	if (scoreScene()->right() && scoreScene()->right()->parentItem() == this) {
		scoreScene()->right()->setParentItem(0);
		scoreScene()->left()->setParentItem(0);
	}		
}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

int TscoreStaff::noteToPos(const Tnote& note)	{
	int nPos = m_offset.octave * 7 + m_offset.note + upperLinePos() - 1 - (note.octave * 7 + (note.note - 1));
	if (isPianoStaff() && nPos > lowerLinePos() - 5)
		return nPos + 2;
	else
		return nPos;
}

		/** Calculation of note position works As folow:
		 * 1) expr: m_offset.octave * 7 + m_offset.note + upperLinePos() - 1 returns y position of note C in offset octave
		 * 2) (note.octave * 7 + (note.note - 1)) is number of note to be set.
		 * 3) Subtraction of them gives position of the note on staff with current clef and it is displayed 
		 * when this value is in staff scale. */
void TscoreStaff::setNote(int index, const Tnote& note) {
	if (index >= 0 && index < m_scoreNotes.size()) {
		Tnote prevNote = *getNote(index);
		if (note.note)
				m_scoreNotes[index]->setNote(noteToPos(note), (int)note.alter, note);
		else
				m_scoreNotes[index]->setNote(0, 0, note);
    if (note.note)
      setCurrentIndex(index);
		if (prevNote != note) // check it only when note was really changed
				checkNoteRange();
	}
}


Tnote* TscoreStaff::getNote(int index) {
	return m_scoreNotes[index]->note();
}


void TscoreStaff::insertNote(int index, const Tnote& note, bool disabled) {
	index = qBound(0, index, m_scoreNotes.size()); // 0 - adds at the begin, size() - adds at the end
	insert(index);
	setNote(index, note);
	m_scoreNotes[index]->setZValue(50);
//   m_scoreNotes[index]->enableAccidToKeyAnim(true);
	setNoteDisabled(index, disabled);
	if (number() > -1) {
		emit noteIsAdding(number(), index);
		if (maxNoteCount()) {
			if (count() > maxNoteCount()) {
					m_scoreNotes.last()->disconnect(SIGNAL(noteWasClicked(int)));
					m_scoreNotes.last()->disconnect(SIGNAL(noteWasSelected(int)));
					emit noteToMove(number(), m_scoreNotes.takeLast());
					checkNoteRange(); // find range again
			} else if (count() == maxNoteCount())
					emit noMoreSpace(number());
		}
	}
	updateIndex();
	updateNotesPos(index);
	if (number() == -1) {
			updateWidth();
			updateLines();
			updateSceneRect(); // Update only for single staff view
	}
}


void TscoreStaff::insertNote(int index, bool disabled) {
	insertNote(index, Tnote(0, 0, 0), disabled);
}


void TscoreStaff::addNote(Tnote& note, bool disabled) {
	insertNote(m_scoreNotes.size(), note, disabled);
}


void TscoreStaff::removeNote(int index) {
	if (index >= 0 && index < count()) {
		emit noteIsRemoving(number(), index);
		delete m_scoreNotes[index];
		m_scoreNotes.removeAt(index);
		if (maxNoteCount() > count())
				emit freeSpace(number(), 1);
		updateIndex();
		updateNotesPos(index);
		if (number() == -1)
				updateSceneRect();
	}
}


void TscoreStaff::addNotes(int index, QList<TscoreNote*>& nList) {
	if (index >= 0 && index <= count() && nList.size() <= maxNoteCount() - index)
	for (int i = index; i < nList.size() + index; i++) {
		TscoreNote *sn = nList[i - index];
		m_scoreNotes.insert(i, sn);
		connectNote(sn);
		sn->setParentItem(this);
		sn->setStaff(this);
	}
	updateNotesPos(index);
	updateIndex();
	checkNoteRange(false);
}


void TscoreStaff::addNote(int index, TscoreNote* freeNote) {
	m_scoreNotes.insert(index, freeNote);
	connectNote(freeNote);
	freeNote->setParentItem(this);
	freeNote->setStaff(this);
	updateNotesPos(index);
	updateIndex();
}


void TscoreStaff::takeNotes(QList<TscoreNote*>& nList, int from, int to) {
	if (from >= 0 && from < count() && to < count() && to >= from) {
		for (int i = from; i <= to; i++) { // 'from' is always the next item after takeAt() on current one
			m_scoreNotes[from]->disconnect(SIGNAL(noteWasClicked(int)));
			m_scoreNotes[from]->disconnect(SIGNAL(noteWasSelected(int)));
			m_scoreNotes[from]->setParentItem(0); // to avoid deleting with staff as its parent
			nList << m_scoreNotes.takeAt(from);
		}
		updateNotesPos();
		updateIndex();
	}
}


void TscoreStaff::updateSceneRect() {
// 	QRectF scRec = mapToScene(boundingRect()).boundingRect();
// 	scene()->setSceneRect(0.0, 0.0, scRec.width() + (isPianoStaff() ? 2.5 : 1.5), scRec.height());
}


void TscoreStaff::setNoteDisabled(int index, bool isDisabled) {
	if (index >=0 && index < m_scoreNotes.size())
		m_scoreNotes[index]->setReadOnly(isDisabled);
}


void TscoreStaff::setEnableKeySign(bool isEnabled) {
	if (isEnabled != (bool)m_keySignature) {
		if (isEnabled) {
			m_keySignature = new TscoreKeySignature(scoreScene(), this);
// 			m_keySignature->setPos(7.0, 0.0);
			m_keySignature->setPos(7.0, upperLinePos() - TscoreKeySignature::relatedLine);
			m_keySignature->setClef(m_clef->clef());
			m_keySignature->setZValue(30);
			connect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
			m_flyAccid = new QGraphicsSimpleTextItem;
			registryItem(m_flyAccid);
			m_flyAccid->setFont(TnooFont(5));
			m_flyAccid->setScale(scoreScene()->accidScale());
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
		} else {
        m_keySignature->setKeySignature(0);
        delete m_keySignature;
        m_keySignature = 0;
        m_accidAnim->deleteLater();
        m_accidAnim = 0;
        delete m_flyAccid;
        m_flyAccid = 0;
		}
		updateWidth();
		updateLines();
		updateNotesPos();
	}
}


void TscoreStaff::setScordature(Ttune& tune) {
	if (!hasScordature()) {
		m_scordature = new TscoreScordature(scoreScene(), this);
		m_scordature->setParentItem(this);
		m_scordature->setZValue(35); // above key signature
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
	updateNotesPos();
}


void TscoreStaff::removeScordatute() {
	delete m_scordature; 
	m_scordature = 0; 
	m_enableScord = false;
	updateWidth();
	updateNotesPos();
}


void TscoreStaff::setDisabled(bool disabled) {
	scoreClef()->setReadOnly(disabled);
	if (scoreKey()) {
		scoreKey()->setAcceptHoverEvents(!disabled); // stops displaying status tip
		scoreKey()->setReadOnly(disabled);
	}
	for (int i = 0; i < count(); i++)
		m_scoreNotes[i]->setReadOnly(disabled);
	if (disabled && count())
		m_scoreNotes[0]->hideWorkNote();
// 	setControlledNotes(!disabled);
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


void TscoreStaff::setPianoStaff(bool isPiano) {
	if (isPiano != m_isPianoStaff) {
		m_isPianoStaff = isPiano;
		if (isPiano) {
				m_upperLinePos = 14.0;
				m_lowerStaffPos = 28.0;
				m_height = 46.0;
		} else {
				m_upperLinePos = 16.0;
				m_lowerStaffPos = 0.0;
				m_height = 40.0;
		}
		prepareStaffLines();
		if (m_keySignature)
				m_keySignature->setPos(7.0, upperLinePos() - TscoreKeySignature::relatedLine);
		for (int i = 0; i < count(); i++) {
			noteSegment(i)->adjustSize();
			noteSegment(i)->setAmbitus(isPiano ? 40 : 34, 2); // TODO It may cause problems when any other class will invoke note ambitus
		}
		if (count())
			scoreScene()->adjustCursor(noteSegment(0));
		emit pianoStaffSwitched();
	}
}


int TscoreStaff::fixNotePos(int pianoPos) {
	if (isPianoStaff() && pianoPos > lowerLinePos() - 4)
		return pianoPos - 2; // piano staves gap
	else
		return pianoPos;
}


void TscoreStaff::setViewWidth(qreal viewW) {
	if (viewW != m_viewWidth) {
		m_viewWidth = viewW;
		int oldMax = m_maxNotesCount;
		if (viewW > 0.0)
			m_maxNotesCount = getMaxNotesNr(mapFromScene(viewW, 0.0).x());
		else
			m_maxNotesCount = 0;
		updateLines(); // calls updateWidth() as well
		updateNotesPos();
	}
}


void TscoreStaff::checkNoteRange(bool doEmit) {
	if (m_lockRangeCheck)
		return;
	qreal oldHi = m_hiNotePos, oldLo = m_loNotePos;
		findHighestNote();
		findLowestNote();
		if (doEmit && oldHi != m_hiNotePos)
			emit hiNoteChanged(number(), oldHi - m_hiNotePos);
		if (doEmit && oldLo != m_loNotePos)
			emit loNoteChanged(number(), m_loNotePos - oldLo);
		return;
}


void TscoreStaff::enableToAddNotes(bool alowAdding) {
	scoreScene()->left()->enableToAddNotes(alowAdding);
	scoreScene()->right()->enableToAddNotes(alowAdding);
}

//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreStaff::prepareStaffLines() {
	if (!m_lines[0]) // create main staff lines
			for (int i = 0; i < 5; i++) {
				m_lines[i] = new QGraphicsLineItem();
				registryItem(m_lines[i]);
				m_lines[i]->setPen(QPen(qApp->palette().text().color(), 0.18));
				m_lines[i]->setZValue(5);
			}
	if (isPianoStaff()) {
		if (!m_lowLines[0]) { // create lower staff lines
			for (int i = 0; i < 5; i++) {
				m_lowLines[i] = new QGraphicsLineItem();
				registryItem(m_lowLines[i]);
				m_lowLines[i]->setPen(QPen(qApp->palette().text().color(), 0.18));
				m_lowLines[i]->setZValue(5);
			}
			createBrace();
		}
	} else {
		if (m_lowLines[0]) { // delete lower staff lines
			for (int i = 0; i < 5; i++)
				delete m_lowLines[i];
			m_lowLines[0] = 0;
			delete m_brace;
		}
	}
	updateLines();
	updateNotesPos();
}


void TscoreStaff::insert(int index) {
	TscoreNote *newNote = new TscoreNote(scoreScene(), this, index);
	newNote->setZValue(50);
	connectNote(newNote);
	m_scoreNotes.insert(index, newNote);
}


void TscoreStaff::setEnableScordtature(bool enable) {
	if (enable != m_enableScord) {
		m_enableScord = enable;
		updateWidth();
		updateNotesPos();
	}
}


qreal TscoreStaff::notesOffset() {
	qreal off = 0.0;
	if (m_keySignature)
			off = KEY_WIDTH + 1.5;
	else if (m_enableScord)
			off = KEY_WIDTH / 2;
	return off;
}


//##########################################################################################################
//####################################### PUBLIC SLOTS     #################################################
//##########################################################################################################

void TscoreStaff::setCurrentIndex(int index) {
	if (m_selectableNotes) {
			if (currentIndex() != -1) { // unset the previous
				m_scoreNotes[currentIndex()]->setBackgroundColor(-1);
				m_scoreNotes[currentIndex()]->selectNote(false);
			}
			m_index = index;
			if (currentIndex() != -1) {
				m_scoreNotes[currentIndex()]->setBackgroundColor(qApp->palette().highlight().color());
				m_scoreNotes[currentIndex()]->selectNote(true);
			}
	}
}


void TscoreStaff::onClefChanged(Tclef clef) {
	setPianoStaff(clef.type() == Tclef::e_pianoStaff);
	switch(clef.type()) {
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
		case Tclef::e_pianoStaff:
      m_offset = TnoteOffset(3, 2); break;
		default: break;
  }
  m_lockRangeCheck = true;
  scoreClef()->setClef(clef);
  if (m_keySignature) {
			disconnect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
      m_keySignature->setClef(m_clef->clef());
			connect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
	}
	if (m_scoreNotes.size()) {
			for (int i = 0; i < m_scoreNotes.size(); i++) {
				if (m_scoreNotes[i]->notePos()) {
						setNote(i, *(m_scoreNotes[i]->note()));
				} 
			}
	}
	m_lockRangeCheck = false;
	checkNoteRange();
	emit clefChanged(scoreClef()->clef());
}


void TscoreStaff::noteChangedAccid(int accid) {
	if (scoreScene()->left())
		scoreScene()->left()->setAccidental(accid);
}

//##########################################################################################################
//####################################### PROTECTED SLOTS  #################################################
//##########################################################################################################

void TscoreStaff::onPianoStaffChanged(Tclef clef) {
	setPianoStaff(clef.type() == Tclef::e_pianoStaff);
	scoreClef()->setClef(clef);
}


void TscoreStaff::onKeyChanged() {
  for (int i = 0; i < m_scoreNotes.size(); i++) {
    if (m_scoreNotes[i]->notePos())
        m_scoreNotes[i]->moveNote(m_scoreNotes[i]->notePos());
  }
}


void TscoreStaff::onNoteClicked(int noteIndex) {
  int globalNr = notePosRelatedToClef(fixNotePos(m_scoreNotes[noteIndex]->notePos())
				+ m_scoreNotes[noteIndex]->ottava() * 7, m_offset);
	m_scoreNotes[noteIndex]->note()->note = (char)(56 + globalNr) % 7 + 1;
	m_scoreNotes[noteIndex]->note()->octave = (char)(56 + globalNr) / 7 - 8;
	m_scoreNotes[noteIndex]->note()->alter = (char)m_scoreNotes[noteIndex]->accidental();
	setCurrentIndex(noteIndex);
	emit noteChanged(noteIndex);
	checkNoteRange();
}


void TscoreStaff::onNoteSelected(int noteIndex) {
// 	if (selectableNotes() || controlledNotes()) { // no need to check, note does it
		setCurrentIndex(noteIndex);
		emit noteSelected(noteIndex);
// 	}
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

void TscoreStaff::updateIndex() {
	m_index = -1;
	for (int i = 0; i < m_scoreNotes.size(); i++) {
		m_scoreNotes[i]->changeIndex(i); // Update index of next notes in the list
		if (m_scoreNotes[i]->isSelected())
			m_index = i;
	}
}


void TscoreStaff::updateNotesPos(int startId) {
	qreal off = notesOffset();
	for (int i = startId; i < m_scoreNotes.size(); i++) // update positions of the notes
				m_scoreNotes[i]->setPos(7.0 + off + i * m_scoreNotes[0]->boundingRect().width(), 0);
}


void TscoreStaff::updateLines() {
	updateWidth();
	for (int l = 0; l < 5; l++) {
		m_lines[l]->setLine(1, upperLinePos() + l * 2, width() - 1.0, upperLinePos() + l * 2);
		if (isPianoStaff())
			m_lowLines[l]->setLine(1, lowerLinePos() + l * 2, width() - 1.0, lowerLinePos() + l * 2);
	}
}


void TscoreStaff::updateWidth() {
	qreal off = notesOffset();
	if (m_scoreNotes.size() < 1)
			m_width = 10.0 + off + 2.0;
	else
			m_width = 10.0 + off + m_scoreNotes.size() * m_scoreNotes[0]->boundingRect().width() + 2.0;
	if (m_viewWidth > 0.0)
			m_width = m_viewWidth;
}


void TscoreStaff::createBrace() {
	m_brace = new QGraphicsSimpleTextItem();
	registryItem(m_brace);
	QFont ff = QFont("nootka");
#if defined (Q_OS_MAC)
    ff.setPointSizeF(27.5);
#elif defined (Q_OS_WIN)
  ff.setPointSizeF(22);
#else
  ff.setPointSizeF(23.3);
#endif
  QFontMetrics fm(ff);
  ff.setPointSizeF(ff.pointSizeF() * (ff.pointSizeF() / fm.boundingRect(QChar(0xe16c)).height()));
  m_brace->setFont(ff);
	m_brace->setText(QString(QChar(0xe16c)));
#if defined (Q_OS_MAC)
  qreal distance = lowerLinePos() + 8.3 - upperLinePos();
  qreal fact = (distance + 1.3) / brace->boundingRect().height();
#elif defined (Q_OS_WIN)
//  qreal distance = lowerLinePos() + 7 - upperLinePos();
//  qreal fact = (distance + 1.8) / m_brace->boundingRect().height();
#endif
// 	m_brace->setScale(fact);
	m_brace->setBrush(qApp->palette().text().color());
// 	m_brace->setPos(-1.8, upperLinePos() + distance / 2 - (m_brace->boundingRect().height() * m_brace->scale()) / 2 + 0.4);
	m_brace->setPos(-1.9, upperLinePos() + (22.18 - m_brace->boundingRect().height()) / 2.0);
}


int TscoreStaff::getMaxNotesNr(qreal maxWidth) {
	maxWidth -= 2.0; // staff lines margins
	if (scoreClef())
		maxWidth -= CLEF_WIDTH;
	if (scoreKey())
		maxWidth -= KEY_WIDTH + 1.5;
	else if (hasScordature())
		maxWidth -= KEY_WIDTH / 2;
	return int(maxWidth / 7.0);
}


void TscoreStaff::findHighestNote() {
	m_hiNotePos = upperLinePos() - 4.0;
	for (int i = 0; i < m_scoreNotes.size(); i++)
		if (m_scoreNotes[i]->notePos()) // is visible
			m_hiNotePos = qMin(qreal(m_scoreNotes[i]->notePos() - 2), m_hiNotePos);
}


void TscoreStaff::findLowestNote() {
	if (hasScordature()) {
		m_loNotePos = height();
		return;
	}	
	m_loNotePos = (isPianoStaff() ? lowerLinePos(): upperLinePos()) + 13.0;
	for (int i = 0; i < m_scoreNotes.size(); i++)
			m_loNotePos = qMax(qreal(m_scoreNotes[i]->notePos() + 2), m_loNotePos);
}


void TscoreStaff::connectNote(TscoreNote* sn) {
	connect(sn, SIGNAL(noteWasClicked(int)), this, SLOT(onNoteClicked(int)));
	connect(sn, SIGNAL(noteWasSelected(int)), this, SLOT(onNoteSelected(int)));
}






