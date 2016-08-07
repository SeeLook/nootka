/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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

#include "tmainscore.h"
#include "tcornerproxy.h"
#include "tscoreactions.h"
#include <score/tscorestaff.h>
#include <score/tscorenote.h>
#include <score/tscorekeysignature.h>
#include <score/tscoreclef.h>
#include <score/tscorescene.h>
#include <score/tnotecontrol.h>
#include <music/ttune.h>
#include <music/tmelody.h>
#include <tscoreparams.h>
#include <texamparams.h>
#include <graphics/tgraphicstexttip.h>
#include <animations/tstrikedoutitem.h>
#include <animations/tblinkingitem.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <widgets/tpushbutton.h>
#include <tinitcorelib.h>
#include <notename/tnotename.h>
#include <QtWidgets>


#define SENDER_TO_STAFF static_cast<TscoreStaff*>(sender())



TmainScore::TmainScore(QMainWindow* mw, QWidget* parent) :
	TmultiScore(mw, parent),
	m_questMark(0),
	m_questKey(0),
	m_strikeOut(0),
	m_bliking(0), m_keyBlinking(0),
	m_corrStyle(Tnote::defaultStyle),
	m_settBar(0),
	m_nameMenu(0),
  m_scoreIsPlayed(false),
  m_emitExpertNoteClicked(true)
{
	m_acts = new TscoreActions(this);
	
	scoreScene()->setNameColor(Tcore::gl()->S->nameColor);
	restoreNotesSettings();
	addStaff(staff());

  createActions();
// set preferred clef
	setClef(Tcore::gl()->S->clef);
	
// set note colors
// 	restoreNotesSettings();
	setScordature();
	setAnimationsEnabled(Tcore::gl()->useAnimations);
  enableAccidToKeyAnim(Tcore::gl()->useAnimations);
	setEnabledDblAccid(Tcore::gl()->S->doubleAccidentalsEnabled);
	setEnableKeySign(Tcore::gl()->S->keySignatureEnabled);
  setScoreScale(Tcore::gl()->S->scoreScale);
	if (staff()->scoreKey())
		staff()->scoreKey()->showKeyName(Tcore::gl()->S->showKeySignName);
	
	connect(scoreScene()->right(), SIGNAL(nameMenu(TscoreNote*)), SLOT(showNameMenu(TscoreNote*)));
//     setAmbitus(Tnote(Tcore::gl()->loString().chromatic()-1),
//                Tnote(Tcore::gl()->hiString().chromatic()+Tcore::gl()->GfretsNumber+1));

	createNoteName();
	isExamExecuting(false);
  setNote(0, Tnote()); // To display fake empty note properly
}


TmainScore::~TmainScore()
{}


//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void TmainScore::setEnableEnharmNotes(bool isEnabled) {
	if (!isEnabled) {
			clearNote(1);
			clearNote(2);
	} else {
			staff()->noteSegment(1)->setColor(Tcore::gl()->S->enharmNotesColor);
			staff()->noteSegment(2)->setColor(Tcore::gl()->S->enharmNotesColor);	
	}
}


void TmainScore::acceptSettings() {
	bool refreshNoteNames = false; // This is CPU consuming operation - we try to avoid it
// Update note name style
	if  (Tnote::defaultStyle != Tcore::gl()->S->nameStyleInNoteName) {
		Tnote::defaultStyle = Tcore::gl()->S->nameStyleInNoteName;
		refreshNoteNames = true;
	}
	if ((Tcore::gl()->S->isSingleNoteMode && insertMode() != e_single) || (!Tcore::gl()->S->isSingleNoteMode && insertMode() == e_single))
		refreshNoteNames = true;
	if (Tcore::gl()->S->isSingleNoteMode) {
		setInsertMode(e_single);
    setEnableEnharmNotes(Tcore::gl()->S->showEnharmNotes);
	} else
		setInsertMode(e_multi);
// Double accidentals
	setEnabledDblAccid(Tcore::gl()->S->doubleAccidentalsEnabled);
	scoreScene()->left()->addAccidentals();
	setClef(Tclef(Tcore::gl()->S->clef));
// Enable/disable key signatures
	if ((bool)staff()->scoreKey() != Tcore::gl()->S->keySignatureEnabled) {
		setEnableKeySign(Tcore::gl()->S->keySignatureEnabled);
	}
	if (staff()->scoreKey())
		staff()->scoreKey()->showKeyName(Tcore::gl()->S->showKeySignName);
	restoreNotesSettings();
// Note names on the score
	if (Tcore::gl()->S->nameColor != scoreScene()->nameColor()) {
			refreshNoteNames = true;
			scoreScene()->setNameColor(Tcore::gl()->S->nameColor);
	}
	if (Tcore::gl()->S->namesOnScore != m_acts->noteNames()->isChecked() || refreshNoteNames) {
		m_acts->noteNames()->setChecked(Tcore::gl()->S->namesOnScore);
		for (int st = 0; st < staffCount(); st++) {
			for (int no = 0; no < staves(st)->count(); no++) {
				if (m_acts->noteNames()->isChecked()) {
					if (refreshNoteNames) // remove name to pain it with new highlight
							staves(st)->noteSegment(no)->removeNoteName();
					staves(st)->noteSegment(no)->showNoteName();
				} else
					staves(st)->noteSegment(no)->removeNoteName();
			}
		}
	}
// scordature	
	if (Tcore::gl()->instrument == e_classicalGuitar || Tcore::gl()->instrument == e_electricGuitar)
			setScordature();
	else
			if (staff()->hasScordature())
				staff()->removeScordatute();
	if (Tcore::gl()->S->keySignatureEnabled) // refreshKeySignNameStyle();
		if (staff()->scoreKey())
			staff()->scoreKey()->showKeyName(Tcore::gl()->S->showKeySignName);
//     setAmbitus(Tnote(Tcore::gl()->loString().chromatic()-1),
//                Tnote(Tcore::gl()->hiString().chromatic()+Tcore::gl()->GfretsNumber+1));
// 	restoreNotesSettings();
// refresh key signature, if any
	enableAccidToKeyAnim(false); // prevent accidental animation to empty note
	if (Tcore::gl()->S->keySignatureEnabled) {
		TkeySignature::setNameStyle(Tcore::gl()->S->nameStyleInKeySign, Tcore::gl()->S->majKeyNameSufix, Tcore::gl()->S->minKeyNameSufix);
		setKeySignature(keySignature());
	}
	enableAccidToKeyAnim(true);
	if (m_nameMenu) {
			m_nameMenu->setEnabledDblAccid(Tcore::gl()->S->doubleAccidentalsEnabled);
			m_nameMenu->setEnabledEnharmNotes(Tcore::gl()->S->showEnharmNotes);
			m_nameMenu->setNoteNamesOnButt(Tcore::gl()->S->nameStyleInNoteName);
			m_nameMenu->setStyle(Tcore::gl()->S->nameStyleInNoteName);
// 			m_nameMenu->setAmbitus(Tcore::gl()->loString(), Tnote(Tcore::gl()->hiString().chromatic() + Tcore::gl()->GfretsNumber));
	}
	resizeEvent(0);
}


void TmainScore::setNote(const Tnote& note) {
	TmultiScore::setNote(note);
	if (insertMode() == e_single && !isExam()) {
    blockSignals(true);
    whenNoteWasChanged(0, note);
    blockSignals(false);
  }
}


void TmainScore::setMelody(Tmelody* mel) {
	bool animEnabled = ainmationsEnabled();
  bool accidAnimEnabled = isAccidToKeyAnimEnabled();
	setAnimationsEnabled(false);
  enableAccidToKeyAnim(false);
	setClef(Tclef(mel->clef()));
	if (staff()->scoreKey())
			setKeySignature(mel->key());
	for (int i = 0; i < mel->length(); ++i) {
		if (i > notesCount() - 1) {
			staves(i / staff()->maxNoteCount())->addNote(mel->note(i)->p(), isScoreDisabled());
		} else {
			changeCurrentIndex(i);
			setNote(mel->note(i)->p());
		}
	}
	selectNote(0);
	setAnimationsEnabled(animEnabled);
	if (mel->length() < notesCount()) { // remove automatically added empty note(s)
		for (int i = 0; i < notesCount() - mel->length(); ++i)
			lastStaff()->removeNote(lastStaff()->count() - 1);
	}
	enableAccidToKeyAnim(accidAnimEnabled);
}


void TmainScore::getMelody(Tmelody* mel, const QString& title) {
	mel->setTitle(title);
	mel->setTempo(Tcore::gl()->S->tempo);
	mel->setKey(keySignature());
	mel->setClef(clef().type());
	for (int i = 0; i < notesCount(); ++i) {
		Tchunk n(getNote(i), Trhythm(Trhythm::e_none));
    if (i == notesCount() - 1 && !getNote(i).isValid())
      continue; // skip last note when empty - it avoids adding temporary last note to exam melody
		mel->addNote(n);
	}
}


void TmainScore::setInsertMode(TmainScore::EinMode mode) {
	if (mode != insertMode()) {
		blockSignals(true);
		bool ignoreThat = false;
		if ((mode == e_record && insertMode() == e_multi) || (mode == e_multi && insertMode() == e_record))
			ignoreThat = true;
		TmultiScore::setInsertMode(mode);
		blockSignals(false);
		if (ignoreThat)
			return;
		if (mode == e_single) {
        m_delCorner->hideSpot();
        m_acts->mainAction()->setVisible(false);
				m_nameMenu->enableArrows(false);
				staff()->noteSegment(0)->removeNoteName();
				m_currentNameSegment = staff()->noteSegment(0);
				enableCorners(false);
				m_nameMenu->show();
        setEnableEnharmNotes(Tcore::gl()->S->showEnharmNotes);
		} else {
        m_acts->mainAction()->setVisible(true);
				m_nameMenu->enableArrows(true);
				m_nameMenu->hide();
				enableCorners(true);
				if (Tcore::gl()->S->namesOnScore)
					staff()->noteSegment(0)->showNoteName();
		}
	}
}


void TmainScore::noteWasClickedMain(int index) {
	TscoreStaff *st = SENDER_TO_STAFF;
	if (!isExam() && insertMode() == e_single)
		m_nameMenu->setNoteName(*(st->getNote(index)));
}


void TmainScore::playScore() {
	if (m_scoreIsPlayed) {
		m_scoreIsPlayed = false;
		if (m_playTimer) {
			m_playTimer->stop();
			delete m_playTimer;
		}
	} else {
		if (currentIndex() < 0)
			return;
		m_scoreIsPlayed = true;
		m_playTimer = new QTimer(this);
    m_playTimer->setTimerType(Qt::PreciseTimer);
		connect(m_playTimer, SIGNAL(timeout()), this, SLOT(playSlot()));
		m_playedIndex = currentIndex() - 1;
		playSlot();
	}
}


void TmainScore::onClefChanged(Tclef cl) {
	if (staff()->hasScordature())
			performScordatureSet();
	TmultiScore::onClefChanged(cl);
}


void TmainScore::setScordature() {
	if (Tcore::gl()->instrument == e_classicalGuitar || Tcore::gl()->instrument == e_electricGuitar) {
			performScordatureSet();
	}
}


void TmainScore::unLockScore() {
	setScoreDisabled(false);
	if (isExam() && insertMode() == e_single) {
		setNoteDisabled(1, true);
		setNoteDisabled(2, true);
	}
	if (isExam()) {
		setBGcolor(Tcolor::merge(Tcore::gl()->EanswerColor, mainWindow()->palette().window().color()));
		if (insertMode() == e_single)
			setNoteViewBg(0, Tcore::gl()->EanswerColor);
		else
			setReadOnlyReacting(false);
	}
  setClefDisabled(true); // setScoreDisabled() unlock it
	QPointF nPos = staff()->noteSegment(0)->mapFromScene(mapToScene(mapFromParent(mapFromGlobal(cursor().pos()))));
	if (nPos.x() > 0.0 && nPos.x() < 7.0) {
		staff()->noteSegment(0)->moveWorkNote(nPos);
	}
}


void TmainScore::setScoreDisabled(bool disabled) {
	TmultiScore::setScoreDisabled(disabled);
  if (insertMode() != e_single) {
    m_delCorner->setVisible(!disabled);
    m_acts->clearScore()->blockSignals(disabled);
    m_acts->clearScore()->setVisible(!disabled);
    m_acts->deleteCurrentNote()->setVisible(!disabled);
    m_acts->deleteCurrentNote()->blockSignals(disabled);
  }
}

/** When Key signature is enabled (exists) it is disabled/enabled by @p setScoreDisabled().
 * This method gives independent control on it. */
void TmainScore::lockKeySignature(bool lock) {
  if (staff()->scoreKey()) {
    for (int i = 0; i < staffCount(); ++i)
      staves(i)->scoreKey()->setReadOnly(lock);
  }
}


QRectF TmainScore::noteRect(int noteNr) {
	return QRectF(0, 0, staff()->noteSegment(noteNr)->mainNote()->rect().width() * transform().m11(), 
		staff()->noteSegment(noteNr)->mainNote()->rect().height() * transform().m11());
}


QPoint TmainScore::notePos(int noteNr) {
	QPointF nPos;
	if (staff()->noteSegment(noteNr)->mainNote()->isVisible())
		nPos = staff()->noteSegment(noteNr)->mainNote()->mapToScene(staff()->noteSegment(noteNr)->mainNote()->pos());
	QPoint vPos = mapFromScene(staff()->pos().x() + staff()->noteSegment(noteNr)->pos().x() + 
																				staff()->noteSegment(noteNr)->mainNote()->pos().x(), 
														 staff()->noteSegment(noteNr)->mainNote()->pos().y());
	return mapToGlobal(vPos);
}


void TmainScore::enableAccidToKeyAnim(bool enable) {
  scoreScene()->enableAccidsAnimation(enable);
}


bool TmainScore::isAccidToKeyAnimEnabled() {
  return scoreScene()->isAccidAnimated();
}


int TmainScore::widthToHeight(int hi) {
	return qRound((qreal)hi / height()) * width();
}


//####################################################################################################
//############################## METHODS RELATED TO EXAMS ############################################
//####################################################################################################


void TmainScore::isExamExecuting(bool isIt) {
	if (isIt) {
			disconnect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
			disconnect(m_nameMenu, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(menuChangedNote(Tnote)));
			connect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
			setNoteNameEnabled(false);
			setScoreDisabled(true);
			setClefDisabled(true);
			m_correctNoteNr = -1;
			m_questMark = new QGraphicsSimpleTextItem();
			m_questMark->hide();
			scoreScene()->addItem(m_questMark);
      m_questMark->setZValue(4);
      if (insertMode() == e_single) {
        staff()->noteSegment(1)->setColor(qApp->palette().text().color()); // it can have color of enharmonic notes
        staff()->noteSegment(2)->setColor(qApp->palette().text().color());
      }
	} else {
			connect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
			connect(m_nameMenu, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(menuChangedNote(Tnote)));
			disconnect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
			if (m_questMark) {
				delete m_questMark;
				m_questMark = 0;
			}
			delete m_questKey;
			m_questKey = 0;
			setClefDisabled(false);
			setNoteNameEnabled(true);
      enableAccidToKeyAnim(Tcore::gl()->useAnimations);
	}
	m_acts->setForExam(isIt);
}


void TmainScore::clearScore() {
	bool enableAnim = isAccidToKeyAnimEnabled();
	enableAccidToKeyAnim(false); // prevent animations to empty score
	if (insertMode() == e_single) {
		for (int i = 0; i < 3; ++i) {
			clearNote(i);
			deleteNoteName(i);
			staff()->noteSegment(i)->removeNoteName();
		}
		staff()->noteSegment(1)->removeString(); // so far string number to remove occurs only on this view
		staff()->noteSegment(0)->hideWorkNote();
	} else {
			m_emitExpertNoteClicked = false; // don't emit noteClicked() in expert exam mode
			deleteNotes();
			setNote(Tnote());
			m_emitExpertNoteClicked = true; // better single bool than blockSignal()
			selectNote(-1);
			staff()->noteSegment(0)->markNote(-1);
			staff()->noteSegment(0)->removeNoteName();
      staff()->noteSegment(0)->removeString();
	}
// 	for (int st = 0; st < staffCount(); st++) {
// 		for (int no = 0; no < staves(st)->count(); no++)
// 				staves(st)->noteSegment(no)->removeNoteName();
// 	}
	if (staff()->scoreKey()) {
			setKeySignature(TkeySignature());
			if (m_questKey) {
				delete m_questKey;
				m_questKey = 0;
			}
    }
	scoreScene()->setCurrentAccid(0);
	for(int i = 0; i < m_bgRects.size(); i++)
		delete m_bgRects[i];
	m_bgRects.clear();
	if (m_questMark)
		m_questMark->hide();
	setBGcolor(mainWindow()->palette().base().color());
	enableAccidToKeyAnim(enableAnim);
}


void TmainScore::askQuestion(Tnote note, char realStr) {
	setQuestionMarkPos();
	setBGcolor(Tcolor::merge(Tcore::gl()->EquestionColor, mainWindow()->palette().window().color()));
	m_questMark->show();
  TsimpleScore::setNote(1, note);
  staff()->noteSegment(1)->setColor(qApp->palette().text().color()); // Otherwise note is strange light
	if (realStr) 
		setStringNumber(1, realStr);
}


void TmainScore::askQuestion(Tnote note, TkeySignature key, char realStr) {
	setKeySignature(key);
	askQuestion(note, realStr);
}


void TmainScore::askQuestion(Tmelody* mel) {
	setBGcolor(Tcolor::merge(Tcore::gl()->EquestionColor, mainWindow()->palette().window().color()));
	setQuestionMarkPos();
	m_questMark->show();
	setMelody(mel);
	setScoreDisabled(true);
}


void TmainScore::expertNoteChanged() {
	if (m_emitExpertNoteClicked)
		emit noteClicked();
}


void TmainScore::forceAccidental(Tnote::Ealter accid) {
	scoreScene()->setCurrentAccid(accid);
}


void TmainScore::markAnswered(QColor blurColor, int noteNr) {
	if (noteNr < notesCount()) {
		TscoreNote *sn;
		if (insertMode() == e_single)
			sn = staff()->noteSegment(noteNr);
		else
			sn = noteFromId(noteNr);
		sn->setEmptyLinesVisible(true);
		if (blurColor == -1)
			sn->markNote(-1);
		else
			sn->markNote(QColor(blurColor.lighter().name()));
	}
}


void TmainScore::markQuestion(QColor blurColor, int noteNr) {
	markAnswered(blurColor, noteNr);
}


void TmainScore::prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName) {
	setKeySignature(fakeKey);
	m_questKey = new QGraphicsTextItem();
	m_questKey->setParentItem(staff()->scoreKey()); // we are sure that key exist - exam checked that
	m_questKey->setHtml(QString("<span style=\"color: %1;\"><span style=\"font-family: nootka;\">?</span><br>").
				arg(Tcore::gl()->EquestionColor.name()) + expectKeyName + "</span>");
	TgraphicsTextTip::alignCenter(m_questKey);
	TscoreKeySignature::setKeyNameScale(m_questKey);
	m_questKey->setPos(
				(staff()->scoreKey()->boundingRect().width() - m_questKey->boundingRect().width() * m_questKey->scale()) / 2 - 2.5,
				 - 3.0 - m_questKey->boundingRect().height() * m_questKey->scale());
	setKeyViewBg(Tcore::gl()->EanswerColor);
}


void TmainScore::setKeyViewBg(QColor C) {
	if (staff()->scoreKey()) {
			createBgRect(C, staff()->scoreKey()->boundingRect().width() + 6.0, 
							QPointF(staff()->scoreKey()->pos().x() - 6.0, 0.0));
	}
}


void TmainScore::setNoteViewBg(int id, QColor C) {
		for (int i = 0; i < m_bgRects.size(); i++) { // check for the same rectangles
			if (staff()->noteSegment(id)->pos() == m_bgRects[i]->pos()) // the same position
				if (m_bgRects[i]->brush().color() == C) // the same color
					return; // means exist - ignore 
		}
		createBgRect(C, staff()->noteSegment(id)->boundingRect().width(), staff()->noteSegment(id)->pos());
}


void TmainScore::correctNote(Tnote& goodNote, const QColor& color, int noteNr) {
	if (noteNr >= notesCount()) {
		qDebug() << "Correction of not existing note" << noteNr;
		return;
	}
	if (m_correctNoteNr != -1) {
		qDebug() << "Correction in progress";
		return;
	}
	TscoreNote *corrN = noteFromId(noteNr);
	m_goodNote = goodNote;
	if (corrN->mainNote()->isVisible())
			m_strikeOut = new TstrikedOutItem(staff()->noteSegment(noteNr)->mainNote());
	else {
		m_strikeOut = new TstrikedOutItem(QRectF(0.0, 0.0, corrN->boundingRect().width() - 3.0, 8.0), corrN);
		m_strikeOut->setPos((corrN->boundingRect().width() - m_strikeOut->boundingRect().width()) / 2, 
														(corrN->boundingRect().height() - m_strikeOut->boundingRect().height()) / 2.0);
	}
	QPen pp(QColor(color.lighter().name()), 0.5);
	m_strikeOut->setPen(pp);
	m_correctNoteNr = noteNr;
	connect(m_strikeOut, SIGNAL(strikedFInished()), this, SLOT(strikeBlinkingFinished()));
	m_strikeOut->startBlinking();
}


/** As long as correctAccidental() is used in single mode only 
 * it is sufficient to set m_correctNoteNr = 0 here
 * but number of note will be necessary if melody will want it */
void TmainScore::correctAccidental(Tnote& goodNote) {
	m_correctNoteNr = 0;
	m_goodNote = goodNote;
	QPen pp(QColor(Tcore::gl()->EnotBadColor.name()), 0.5);
	if (getNote(0).alter != m_goodNote.alter) {
			m_bliking = new TblinkingItem(staff()->noteSegment(0)->mainAccid());
	} else {
				m_bliking = new TblinkingItem(staff()->noteSegment(0));
				staff()->noteSegment(0)->mainNote()->setBrush(QBrush(pp.color()));
	}
	staff()->noteSegment(0)->mainAccid()->setBrush(QBrush(pp.color()));
	m_bliking->startBlinking(3);
	connect(m_bliking, SIGNAL(finished()), this, SLOT(strikeBlinkingFinished()));
}


void TmainScore::correctKeySignature(TkeySignature newKey) {
	if (staff()->scoreKey())
			m_keyBlinking = new TblinkingItem(staff()->scoreKey());
	else
			return;
	m_goodKey = newKey;
	connect(m_keyBlinking, SIGNAL(finished()), this, SLOT(keyBlinkingFinished()));
	m_keyBlinking->startBlinking(2);
}


void TmainScore::showNames(Tnote::EnameStyle st) {
	Tnote::EnameStyle tmpStyle = Tnote::defaultStyle;
	Tnote::defaultStyle = st;
	for (int st = 0; st < staffCount(); st++) {
		for (int no = 0; no < staves(st)->count(); no++)
			staves(st)->noteSegment(no)->showNoteName(staves(st)->noteSegment(no)->mainNote()->pen().color());
	}
	Tnote::defaultStyle = tmpStyle;
}


void TmainScore::deleteNoteName(int id) {
	if (insertMode() == e_single ) {
		if (id < staff()->count())
			staff()->noteSegment(id)->removeNoteName();
	} else if (id < notesCount())
			noteFromId(id)->removeNoteName();
}

//####################################################################################################
//########################################## PUBLIC SLOTS ############################################
//####################################################################################################

void TmainScore::whenNoteWasChanged(int index, Tnote note) {
	//We are sure that index is 0, cause others are disabled :-)
    if (insertMode() == e_single && Tcore::gl()->S->showEnharmNotes) {
      if (note.isValid()) {
        TnotesList enharmList = note.getTheSameNotes(Tcore::gl()->S->doubleAccidentalsEnabled);
        TnotesList::iterator it = enharmList.begin();
        ++it;
        if (it != enharmList.end())
            TsimpleScore::setNote(1, *(it));
        else
            clearNote(1);
        if (Tcore::gl()->S->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                TsimpleScore::setNote(2, *(it));
            else
                clearNote(2);
        }
        m_nameMenu->setNoteName(enharmList);
      } else
          m_nameMenu->setNoteName(note);
    }
    emit noteChanged(index, note);
}

//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmainScore::showNameMenu(TscoreNote* sn) {
  sn->staff()->applyAutoAddedNote();
	m_nameMenu->setNoteName(*sn->note());
	m_currentNameSegment = sn;
	changeCurrentIndex(sn->staff()->number() * staff()->maxNoteCount() + sn->index());
	QPointF notePos = sn->staff()->mapToScene(sn->pos());
	QPoint mPos = mapFromScene(notePos.x() + 8.0, notePos.y() + 6.0);
	mPos.setX(x() + mPos.x());
	mPos.setY(y() + mPos.y());
	resetClickedOff();
	m_nameMenu->exec(mPos, transform().m11());
}


void TmainScore::menuChangedNote(Tnote n) {
	if (m_currentNameSegment) {
		m_currentNameSegment->staff()->setNote(m_currentNameSegment->index(), n);
		m_currentNameSegment->update(); // Menu above covers focus
		emit noteWasChanged(m_currentNameSegment->index(), n);
		if (insertMode() == e_single && Tcore::gl()->S->showEnharmNotes && !isExam()) {
			staff()->setNote(1, m_nameMenu->getNoteName(1));
			staff()->setNote(2, m_nameMenu->getNoteName(2));
		}
	}
}


void TmainScore::extraAccidsSlot() {
	for (int st = 0; st < staffCount(); st++) {
		staves(st)->setExtraAccids(m_acts->extraAccids()->isChecked());
		for (int no = 0; no < staves(st)->count(); no++) {
			if (staves(st)->getNote(no)->alter == -1 || staves(st)->getNote(no)->alter == 1)
				staves(st)->setNote(no, *staves(st)->getNote(no));
		}
	}
}


void TmainScore::showNamesSlot() {
	for (int st = 0; st < staffCount(); st++) {
		for (int no = 0; no < staves(st)->count(); no++) {
			if (m_acts->noteNames()->isChecked())
				staves(st)->noteSegment(no)->showNoteName();
			else
				staves(st)->noteSegment(no)->removeNoteName();
		}
	}
	Tcore::gl()->S->namesOnScore = m_acts->noteNames()->isChecked();
}


void TmainScore::zoomScoreSlot() {
	qreal newScale = Tcore::gl()->S->scoreScale;
	if (sender() == m_acts->zoomOut()) {
			newScale = qMin(Tcore::gl()->S->scoreScale + 0.25, 2.0);
	} else {
			newScale = qMax(Tcore::gl()->S->scoreScale - 0.25, 1.0);
	}
	if (newScale != Tcore::gl()->S->scoreScale) {
		Tcore::gl()->S->scoreScale = newScale;
		setScoreScale(newScale);
    if (m_questMark) {
      m_questMark->setPos(0, 0); // reset position to enable positioning again
      setQuestionMarkPos();
    }
	}
}


void TmainScore::moveSelectedNote(TmainScore::EmoveNote nDir) {
	int prevIndex = currentIndex();
	if (nDir == e_doNotMove) { // determine action by sender which invoked this slot
			if (sender() == m_acts->firstNote())
				nDir = e_first;
			else if (sender() == m_acts->lastNote())
				nDir = e_last;
			else if (sender() == m_acts->staffUp())
				nDir = e_prevStaff;
			else if (sender() == m_acts->staffDown())
				nDir = e_nextStaff;
			else if (sender() == m_acts->nextNote())
				nDir = e_nextNote;
			else if (sender() == m_acts->prevNote())
				nDir = e_prevNote;
	}
	switch(nDir) {
		case e_first:
			changeCurrentIndex(0); break;
		case e_last:
			changeCurrentIndex(notesCount() - 1);
			break;
		case e_prevNote: {
			if (currentIndex() > 0)
					changeCurrentIndex(currentIndex() - 1);
			break;
		}
		case e_nextNote: {
			if (currentIndex() < notesCount() - 1)
				changeCurrentIndex(currentIndex() + 1);
			else if (insertMode() == e_record && currentIndex() == notesCount() - 1) { // record mode:
				checkAndAddNote(currentStaff(), currentIndex()); // first to add new note at the end
				changeCurrentIndex(currentIndex() + 1); // then change current index
			}
			break;
		}
		case e_nextStaff: {
			if (currentStaff() != lastStaff())
				changeCurrentIndex((currentStaff()->number() + 1) * staff()->maxNoteCount() );
			break;
		}
		case e_prevStaff: {
			if (currentStaff() != staff())
				changeCurrentIndex((currentStaff()->number() - 1) * staff()->maxNoteCount());
			break;
		}
		default: 
			return;
	}
	if (prevIndex != currentIndex()) {
			emit noteWasChanged(currentIndex() % staff()->maxNoteCount(), 
												*currentStaff()->getNote(currentIndex() % staff()->maxNoteCount()));
      if (readOnlyReacting())
        emit lockedNoteClicked(currentIndex());
			resetClickedOff();
	}
}


void TmainScore::playSlot() {
	m_playedIndex++;
	if (m_playedIndex < notesCount()) {
			changeCurrentIndex(m_playedIndex);
		// by emitting that signal note is played and shown on the guitar
			emit noteWasChanged(m_playedIndex % staff()->maxNoteCount(), 
												*currentStaff()->getNote(m_playedIndex % staff()->maxNoteCount()));
      if (!m_playTimer->isActive()) // timer is started here to skip out stream initialization delay
        m_playTimer->start(60000 / Tcore::gl()->S->tempo);
	} else
			emit playbackFinished();
}


void TmainScore::strikeBlinkingFinished() {
	if (m_strikeOut) {
		m_strikeOut->deleteLater();
		m_strikeOut = 0;
	}
  delete m_bliking;
  m_bliking = 0;
	if (m_correctNoteNr < 0) {
		qDebug() << "TmainScore::strikeBlinkingFinished has wrong note number. Fix it!";
		return;
	}
	deleteNoteName(m_correctNoteNr);
	TscoreNote *sn = noteFromId(m_correctNoteNr);
	sn->setColor(qApp->palette().text().color());
	sn->enableNoteAnim(true, 300);
	sn->markNote(-1);
	bool animEnabled = isAccidToKeyAnimEnabled();
	enableAccidToKeyAnim(false); // prevent animations - it looks ugly with correction animations
	staves(m_correctNoteNr / staff()->maxNoteCount())->setNote(m_correctNoteNr % staff()->maxNoteCount(), m_goodNote);
	enableAccidToKeyAnim(animEnabled);
	QTimer::singleShot(320, this, SLOT(finishCorrection()));	
}


void TmainScore::keyBlinkingFinished() {
	if (m_goodKey.value() != keySignature().value()) { // finished 1st time
			bool animEnabled = isAccidToKeyAnimEnabled();
			enableAccidToKeyAnim(false); // prevent animations - it looks ugly with correction animations
			setKeySignature(m_goodKey); // set proper key
			enableAccidToKeyAnim(animEnabled);
			if (m_questKey) // desired key name make green and replace ? for !
				m_questKey->setHtml(m_questKey->toHtml().replace("?", "!").replace(Tcore::gl()->EquestionColor.name(), Tcore::gl()->EanswerColor.name()));
			m_keyBlinking->startBlinking(3); // and blink again
	} else { // finished 2nd time
			delete m_keyBlinking;
      m_keyBlinking = 0;
      if (!m_strikeOut && !m_bliking) // no other animations that will emit correctingFinished()
        QTimer::singleShot(100, this, SLOT(finishCorrection())); // it might be invoked even immediately
	}
}


void TmainScore::finishCorrection() {
  if (m_correctNoteNr > -1) {
    noteFromId(m_correctNoteNr)->enableNoteAnim(false);
    noteFromId(m_correctNoteNr)->markNote(QColor(Tcore::gl()->EanswerColor.lighter().name()));
    m_correctNoteNr = -1;
  }
  emit correctingFinished();
}


void TmainScore::resizeEvent(QResizeEvent* event) {
	TmultiScore::resizeEvent(event);
	if (width() < 300 || height() < 200)
      return;
	setBarsIconSize();
	performScordatureSet(); // To keep scordature size up to date with score size
}


void TmainScore::performScordatureSet() {
	if (Tcore::gl()->instrument == e_classicalGuitar || Tcore::gl()->instrument == e_electricGuitar) {
			Ttune tmpTune(*Tcore::gl()->Gtune());
			staff()->setScordature(tmpTune);
	}
}


//####################################################################################################
//########################################## PRIVATE #################################################
//####################################################################################################

void TmainScore::createActions() {
  m_acts->noteNames()->setChecked(Tcore::gl()->S->namesOnScore);
	m_clearBar = new QToolBar();
	m_clearBar->addAction(m_acts->clearScore());
	m_clearBar->setMovable(false);
	m_delCorner = new TcornerProxy(scoreScene(), m_clearBar, Qt::TopRightCorner);
	m_delCorner->setSpotColor(Qt::red);
}


void TmainScore::restoreNotesSettings() {
  if (Tcore::gl()->S->enharmNotesColor == -1)
      Tcore::gl()->S->enharmNotesColor = QColor(0, 162, 162); // turquoise	scoreScene()->right()->adjustSize();
	if (Tcore::gl()->S->pointerColor == -1) {
				Tcore::gl()->S->pointerColor = Tcolor::invert(palette().highlight().color());
				Tcore::gl()->S->pointerColor.setAlpha(200);
	}
	scoreScene()->setPointedColor(Tcore::gl()->S->pointerColor);
}


void TmainScore::setQuestionMarkPos() {
	if (m_questMark && m_questMark->pos().isNull()) {
// 		#if defined(Q_OS_MACX)
// 			m_questMark->setFont(TnooFont(10));
// 		#else
		m_questMark->setFont(TnooFont(8));
// 		#endif
		QColor c = Tcore::gl()->EquestionColor;
		c.setAlpha(30);
		m_questMark->setBrush(QBrush(c));
		m_questMark->setText("?");
		m_questMark->setScale(scene()->height() / m_questMark->boundingRect().height());
		m_questMark->setPos(staff()->pos().x() + (staff()->width() - m_questMark->boundingRect().width() * m_questMark->scale()) / 2, 
												(scene()->height() - m_questMark->boundingRect().height() * m_questMark->scale()) / 2 );
	}
}


void TmainScore::createBgRect(QColor c, qreal width, QPointF pos) {
  QGraphicsRectItem* bgRect = new QGraphicsRectItem;
  bgRect->setParentItem(staff());
  bgRect->setRect(0, 0, width, staff()->boundingRect().height());
  bgRect->setPos(pos);
  bgRect->setZValue(1);
  bgRect->setPen(QPen(Qt::NoPen));
  bgRect->setBrush(c);
  m_bgRects << bgRect;
}


void TmainScore::setBarsIconSize() {
#if defined (Q_OS_ANDROID)
	QSize ss(mainWindow()->height() / 10, mainWindow()->height() / 10);
#else
	QSize ss(mainWindow()->height() / 15, mainWindow()->height() / 15);
#endif
	m_clearBar->setIconSize(ss);
	m_clearBar->adjustSize();
}


void TmainScore::createNoteName() {
	if (!m_nameMenu) {
    m_nameMenu = new TnoteName(mainWindow());
    connect(m_nameMenu, &TnoteName::nextNote, this, &TmainScore::moveNameForward);
    connect(m_nameMenu, &TnoteName::prevNote, this, &TmainScore::moveNameBack);
    connect(m_nameMenu, &TnoteName::statusTipRequired, this, &TmainScore::statusTipChanged);
    m_nameMenu->setEnabledDblAccid(Tcore::gl()->S->doubleAccidentalsEnabled);
    m_nameMenu->hide();
	}
}


void TmainScore::enableCorners(bool enable) {
	if (enable) {
			m_delCorner->show();
	} else {
			m_delCorner->hide();
	}
}


void TmainScore::moveName(TmainScore::EmoveNote moveDir) {
	int oldIndex = currentIndex();
	moveSelectedNote(moveDir);
	if (oldIndex != currentIndex()) {
		showNameMenu(currentStaff()->noteSegment(currentIndex() % staff()->maxNoteCount()));
	}
}


void TmainScore::addStaff(TscoreStaff* st) {
	TmultiScore::addStaff(st);
	connect(lastStaff(), SIGNAL(noteChanged(int)), this, SLOT(noteWasClickedMain(int)));
	if (Tcore::gl()->S->namesOnScore)
			lastStaff()->noteSegment(0)->showNoteName();
	lastStaff()->setExtraAccids(m_acts->extraAccids()->isChecked());
// 	qDebug() << "staff Added";
}








