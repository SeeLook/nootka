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

#include "tmainscore.h"
#include "tcornerproxy.h"
#include "tscorekeys.h"
#include "tscoreactions.h"
#include <score/tscorestaff.h>
#include <score/tscorenote.h>
#include <score/tscorekeysignature.h>
#include <score/tscoreclef.h>
#include <score/tscorescene.h>
#include <score/tnotecontrol.h>
#include <music/ttune.h>
#include <music/tmelody.h>
#include <tglobals.h>
#include <tscoreparams.h>
#include <graphics/tgraphicstexttip.h>
#include <animations/tstrikedoutitem.h>
#include <animations/tblinkingitem.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <widgets/tpushbutton.h>
#include <notename/tnotename.h>
#include <QtWidgets>


#define SENDER_TO_STAFF static_cast<TscoreStaff*>(sender())

extern Tglobals *gl;


TmainScore::TmainScore(QMainWindow* mw, QWidget* parent) :
	TmultiScore(mw, parent),
	m_questMark(0),
	m_questKey(0),
	m_strikeOut(0),
	m_bliking(0), m_keyBlinking(0),
	m_corrStyle(Tnote::defaultStyle),
	m_nameMenu(0),
  m_scoreIsPlayed(false)
{
	m_acts = new TscoreActions(this, gl->path);
	
	scoreScene()->setNameColor(gl->S->nameColor);
	restoreNotesSettings();
	addStaff(staff());
// set preferred clef
	setClef(gl->S->clef);
	
	createActions();
	
// set note colors
// 	restoreNotesSettings();
	setScordature();
	setAnimationsEnabled(gl->useAnimations);
	setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
	setEnableKeySign(gl->S->keySignatureEnabled);
// 	if (gl->S->keySignatureEnabled)
// 				staff()->scoreKey()->showKeyName(true);
	
	connect(scoreScene()->right(), SIGNAL(nameMenu(TscoreNote*)), SLOT(showNameMenu(TscoreNote*)));
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));

	createNoteName();
	isExamExecuting(false);

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
	}
}


void TmainScore::acceptSettings() {
	bool refreshNoteNames = false; // This is CPU consuming operation - we try to avoid it
// Update note name style
	if  (Tnote::defaultStyle != gl->S->nameStyleInNoteName) {
		Tnote::defaultStyle = gl->S->nameStyleInNoteName;
		refreshNoteNames = true;
	}
	if ((gl->S->isSingleNoteMode && insertMode() != e_single) || (!gl->S->isSingleNoteMode && insertMode() == e_single))
		refreshNoteNames = true;
	if (gl->S->isSingleNoteMode) {
		setInsertMode(e_single);
		setEnableEnharmNotes(gl->S->showEnharmNotes);
	} else
		setInsertMode(e_multi);
// Double accidentals
	setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
	scoreScene()->left()->addAccidentals();
	setClef(Tclef(gl->S->clef));
// Enable/disable key signatures
	if ((bool)staff()->scoreKey() != gl->S->keySignatureEnabled) {
			setEnableKeySign(gl->S->keySignatureEnabled);
	}
	restoreNotesSettings();
// Note names on the score
	if (gl->S->nameColor != scoreScene()->nameColor()) {
			refreshNoteNames = true;
			m_acts->noteNames()->setThisColors(gl->S->nameColor, palette().highlightedText().color());
			scoreScene()->setNameColor(gl->S->nameColor);
	}
	if (gl->S->namesOnScore != m_acts->noteNames()->isChecked() || refreshNoteNames) {
		m_acts->noteNames()->setChecked(gl->S->namesOnScore);
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
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar)
			setScordature();
	else
			if (staff()->hasScordature())
				staff()->removeScordatute();
// 	if (!gl->S->doubleAccidentalsEnabled)
// 		clearNote(2);
	if (gl->S->keySignatureEnabled) // refreshKeySignNameStyle();
		if (staff()->scoreKey())
			staff()->scoreKey()->showKeyName(gl->S->showKeySignName);
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));
// 	restoreNotesSettings();
// refresh key signature, if any
	enableAccidToKeyAnim(false); // prevent accidental animation to empty note
	if (gl->S->keySignatureEnabled) {
		TkeySignature::setNameStyle(gl->S->nameStyleInKeySign, gl->S->majKeyNameSufix, gl->S->minKeyNameSufix);
		setKeySignature(keySignature());
	}
	enableAccidToKeyAnim(true);
// 	if (gl->S->isSingleNoteMode) {
// 		setInsertMode(e_single);
// 		setEnableEnharmNotes(gl->S->showEnharmNotes);
// 	} else
// 		setInsertMode(e_multi);
	if (m_nameMenu) {
			m_nameMenu->setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
			m_nameMenu->setEnabledEnharmNotes(gl->S->showEnharmNotes);
			m_nameMenu->setNoteNamesOnButt(gl->S->nameStyleInNoteName);
			m_nameMenu->setStyle(gl->S->nameStyleInNoteName);
// 			m_nameMenu->setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
	}
}


void TmainScore::setNote(const Tnote& note) {
    TmultiScore::setNote(note);
		if (insertMode() == e_single && !isExam())
			m_nameMenu->setNoteName(note);
}


void TmainScore::setMelody(Tmelody* mel) {
	bool animEnabled = ainmationsEnabled();
	setAnimationsEnabled(false);
	for (int i = 0; i < mel->length(); ++i) {
		if (i > notesCount() - 1) {
			staves(i / staff()->maxNoteCount())->addNote(mel->notes()[i].p());
		} else {
			changeCurrentIndex(i);
			setNote(mel->notes()[i].p());
		}
	}
	setAnimationsEnabled(animEnabled);
	if (mel->length() < notesCount()) {
		for (int i = 0; i < notesCount() - mel->length(); ++i) {
			lastStaff()->removeNote(lastStaff()->count() - 1);
		}
	}
}


void TmainScore::getMelody(Tmelody* mel, const QString& title) {
	mel->setTitle(title);
	mel->setTempo(gl->S->tempo);
	mel->setKey(keySignature());
	for (int i = 0; i < notesCount(); ++i) {
		Tchunk n(getNote(i), Trhythm());
		mel->notes() << n;
	}
}


void TmainScore::setInsertMode(TmainScore::EinMode mode) {
	if (mode != insertMode()) {
		bool ignoreThat = false;
		if ((mode == e_record && insertMode() == e_multi) || (mode == e_multi && insertMode() == e_record))
			ignoreThat = true;
		TmultiScore::setInsertMode(mode);
		if (ignoreThat)
			return;
		if (mode == e_single) {
				m_nameMenu->enableArrows(false);
				m_currentNameSegment = staff()->noteSegment(0);
				enableCorners(false);
				m_nameMenu->show();
				if (gl->S->showEnharmNotes) {
					staff()->noteSegment(1)->setColor(gl->S->enharmNotesColor);
					staff()->noteSegment(2)->setColor(gl->S->enharmNotesColor);
				}
		} else {
				m_nameMenu->enableArrows(true);
				m_nameMenu->hide();
				enableCorners(true);
		}
	}
}


void TmainScore::noteWasClickedMain(int index) {
	TscoreStaff *st = SENDER_TO_STAFF;
	Tnote note = *(st->getNote(index));
	if (insertMode() == e_single)
		m_nameMenu->setNoteName(note);
	if (isExam() && m_selectReadOnly && st->noteSegment(index)->isReadOnly()) {
		selectNote(st->number() * st->maxNoteCount() + index);
		emit lockedNoteClicked(note);
	}
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
		connect(m_playTimer, SIGNAL(timeout()), this, SLOT(playSlot()));
		m_playTimer->start(60000 / gl->S->tempo);
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
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar) {
			performScordatureSet();
// 			resizeEvent(0);
	}
}


void TmainScore::unLockScore() {
	setScoreDisabled(false);
	if (isExam() && insertMode() == e_single) {
		setNoteDisabled(1, true);
		setNoteDisabled(2, true);
	}
	if (isExam()) {
		setBGcolor(Tcolor::merge(gl->EanswerColor, mainWindow()->palette().window().color()));
		if (insertMode() == e_single)
			setNoteViewBg(0, gl->EanswerColor);
	}
//   setClefDisabled(true);
	QPointF nPos = staff()->noteSegment(0)->mapFromScene(mapToScene(mapFromParent(mapFromGlobal(cursor().pos()))));
	if (nPos.x() > 0.0 && nPos.x() < 7.0) {
		staff()->noteSegment(0)->moveWorkNote(nPos);
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
	staff()->noteSegment(0)->enableAccidToKeyAnim(enable);
}


bool TmainScore::isAccidToKeyAnimEnabled() {
	return staff()->noteSegment(0)->accidToKeyAnim();
}


int TmainScore::widthToHeight(int hi) {
	return qRound((qreal)hi / sizeHint().height()) * sizeHint().width();
}


//####################################################################################################
//############################## METHODS RELATED TO EXAMS ############################################
//####################################################################################################


void TmainScore::isExamExecuting(bool isIt) {
	if (isIt) {
			if (insertMode() == e_single)
				resizeSlot();
			enableCorners(false);
			disconnect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
			disconnect(m_nameMenu, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(menuChangedNote(Tnote)));
			connect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
			m_questMark = new QGraphicsSimpleTextItem();
			m_questMark->hide();
		#if defined(Q_OS_MACX)
			m_questMark->setFont(TnooFont(10));
		#else
			m_questMark->setFont(TnooFont(8));
		#endif
			scoreScene()->addItem(m_questMark);
			QColor c = gl->EquestionColor;
// 			if (insertMode() == e_single) {
// 				c.setAlpha(255);
// 				staff()->noteSegment(1)->setColor(c);
// 			}
			c.setAlpha(80);
			m_questMark->setBrush(QBrush(c));
			m_questMark->setText("?");
			m_questMark->setScale(((sizeHint().height() / transform().m11()) / m_questMark->boundingRect().height()));
			m_questMark->setPos(((sizeHint().width() / transform().m11()) - m_questMark->boundingRect().width() * m_questMark->scale()) / 2, 
													((sizeHint().height() / transform().m11()) - m_questMark->boundingRect().height() * m_questMark->scale()) / 2 );
			m_questMark->setZValue(4);
			setScoreDisabled(true);
			setClefDisabled(true);
    } else {
        connect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
				connect(m_nameMenu, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(menuChangedNote(Tnote)));
        disconnect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
        delete m_questMark;
        m_questMark = 0;
        delete m_questKey;
        m_questKey = 0;
				setClefDisabled(false);
    }
}


void TmainScore::clearScore() {
	bool enableAnim = isAccidToKeyAnimEnabled();
	enableAccidToKeyAnim(false); // prevent animations to empty score
	if (insertMode() == e_single) {
		for (int i = 0; i < 3; ++i) {
			clearNote(i);
			deleteNoteName(i);
		}
		staff()->noteSegment(1)->removeString(); // so far string number to remove occurs only on this view
		staff()->noteSegment(0)->hideWorkNote();
	} else {
			deleteNotes();
			selectNote(-1);
			staff()->noteSegment(0)->markNote(-1);
	}
	for (int st = 0; st < staffCount(); st++) {
		for (int no = 0; no < staves(st)->count(); no++)
				staves(st)->noteSegment(no)->removeNoteName();
	}
	m_showNameInCorrection = false;
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
	m_questMark->hide();
	setBGcolor(mainWindow()->palette().base().color());
	enableAccidToKeyAnim(enableAnim);
}


void TmainScore::askQuestion(Tnote note, char realStr) {
	TsimpleScore::setNote(1, note);
	setBGcolor(Tcolor::merge(gl->EquestionColor, mainWindow()->palette().window().color()));
	m_questMark->show();
	if (realStr) 
		setStringNumber(1, realStr);
}


void TmainScore::askQuestion(Tnote note, TkeySignature key, char realStr) {
	setKeySignature(key);
	askQuestion(note, realStr);
}


void TmainScore::askQuestion(Tmelody* mel) {
	if (staff()->scoreKey())
		setKeySignature(mel->key());
	setBGcolor(Tcolor::merge(gl->EquestionColor, mainWindow()->palette().window().color()));
// 	m_questMark->show();
	setMelody(mel);
	setScoreDisabled(true);
}



void TmainScore::expertNoteChanged() {
	emit noteClicked();
}


void TmainScore::forceAccidental(Tnote::Eacidentals accid) {
	scoreScene()->setCurrentAccid(accid);
}


void TmainScore::selectReadOnly(bool doIt) {
	m_selectReadOnly = doIt;
}


void TmainScore::markAnswered(QColor blurColor, int noteNr) {
	if (noteNr < notesCount())
// 		staff()->noteSegment(noteNr)->markNote(QColor(blurColor.lighter().name()));
		staves(noteNr / staff()->maxNoteCount())->noteSegment(noteNr % staff()->maxNoteCount())->markNote(QColor(blurColor.lighter().name()));
	else
		qDebug() << "TmainScore: Try to mark a note that not exists!";
}


void TmainScore::markQuestion(QColor blurColor, int noteNr) {
// 		staff()->noteSegment(noteNr)->markNote(QColor(blurColor.lighter().name()));
	markAnswered(blurColor, noteNr);
}


void TmainScore::prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName) {
	setKeySignature(fakeKey);
	m_questKey = new QGraphicsTextItem();
	m_questKey->setParentItem(staff()->scoreKey()); // we are sure that key exist - exam checked that
	m_questKey->setHtml(QString("<span style=\"color: %1;\"><span style=\"font-family: nootka;\">?</span><br>").
				arg(gl->EquestionColor.name()) + expectKeyName + "</span>");
	TgraphicsTextTip::alignCenter(m_questKey);
	TscoreKeySignature::setKeyNameScale(m_questKey);
	m_questKey->setPos(
				(staff()->scoreKey()->boundingRect().width() - m_questKey->boundingRect().width() * m_questKey->scale()) / 2 - 2.5,
				 - 3.0 - m_questKey->boundingRect().height() * m_questKey->scale());
	setKeyViewBg(gl->EanswerColor);
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


void TmainScore::correctNote(Tnote& goodNote, const QColor& color) {
		m_goodNote = goodNote;
		if (staff()->noteSegment(0)->mainNote()->isVisible())
				m_strikeOut = new TstrikedOutItem(staff()->noteSegment(0)->mainNote());
		else {
			m_strikeOut = new TstrikedOutItem(QRectF(0.0, 0.0, 
																staff()->noteSegment(0)->boundingRect().width() - 3.0, 8.0), staff()->noteSegment(0));
			m_strikeOut->setPos((staff()->noteSegment(0)->boundingRect().width() - m_strikeOut->boundingRect().width()) / 2, 
														(staff()->noteSegment(0)->boundingRect().height() - m_strikeOut->boundingRect().height()) / 2.0);
		}
		QPen pp(QColor(color.name()), 0.5);
		m_strikeOut->setPen(pp);
		connect(m_strikeOut, SIGNAL(strikedFInished()), this, SLOT(strikeBlinkingFinished()));
		m_strikeOut->startBlinking();
}


void TmainScore::correctAccidental(Tnote& goodNote) {
		m_goodNote = goodNote;
		QPen pp(QColor(gl->EnotBadColor.name()), 0.5);
		if (getNote(0).acidental != m_goodNote.acidental) {
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
		for (int no = 0; no < staves(st)->count(); no++) {
			scoreScene()->setNameColor(staves(st)->noteSegment(no)->mainNote()->pen().color());
			staves(st)->noteSegment(no)->showNoteName();
		}
	}
	Tnote::defaultStyle = tmpStyle;
}


void TmainScore::deleteNoteName(int id) {
	if (id < notesCount()) {
		if (insertMode() == e_single)
			staff()->noteSegment(id)->removeNoteName();
		else
			staves(id / staff()->maxNoteCount())->noteSegment(id % staff()->maxNoteCount())->removeNoteName();
	}
}

//####################################################################################################
//########################################## PUBLIC SLOTS ############################################
//####################################################################################################

void TmainScore::whenNoteWasChanged(int index, Tnote note) {
	//We are sure that index is 0, cause others are disabled :-)
    if (insertMode() == e_single && gl->S->showEnharmNotes) {
        TnotesList enharmList = note.getTheSameNotes(gl->S->doubleAccidentalsEnabled);
        TnotesList::iterator it = enharmList.begin();
        ++it;
        if (it != enharmList.end())
            TsimpleScore::setNote(1, *(it));
        else
            clearNote(1);
        if (gl->S->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                TsimpleScore::setNote(2, *(it));
            else
                clearNote(2);
        }
        m_nameMenu->setNoteName(enharmList);
    }
    emit noteChanged(index, note);
}

//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmainScore::showNameMenu(TscoreNote* sn) {
	m_nameMenu->setNoteName(*sn->note());
	m_currentNameSegment = sn;
	changeCurrentIndex(sn->staff()->number() * staff()->maxNoteCount() + sn->index());
	QPoint mPos = mapFromScene(sn->pos().x() + 8.0, 0.0);
// 	mPos.setY(30);
// 	mPos = mapToGlobal(mPos);
	mPos.setX(mPos.x() + mainWindow()->pos().x());
	mPos.setY(pos().y() + 50 + mainWindow()->pos().y());
	resetClickedOff();
	m_nameClickCounter = 0;
	m_nameMenu->exec(mPos, transform().m11());
}


void TmainScore::menuChangedNote(Tnote n) {
	if (m_currentNameSegment) {
		if (m_nameClickCounter == 0) // add next note when user changes note through name menu
				checkAndAddNote(m_currentNameSegment->staff(), m_currentNameSegment->index());
		m_currentNameSegment->staff()->setNote(m_currentNameSegment->index(), n);
		m_currentNameSegment->update(); // Menu above covers focus
		emit noteWasChanged(m_currentNameSegment->index(), n);
		m_nameClickCounter++;
		if (insertMode() == e_single && gl->S->showEnharmNotes && !isExam()) {
			staff()->setNote(1, m_nameMenu->getNoteName(1));
			staff()->setNote(2, m_nameMenu->getNoteName(2));
		}
	}
}


void TmainScore::extraAccidsSlot() {
	m_acts->extraAccids()->setChecked(!m_acts->extraAccids()->isChecked());
	for (int st = 0; st < staffCount(); st++) {
		staves(st)->setExtraAccids(m_acts->extraAccids()->isChecked());
		for (int no = 0; no < staves(st)->count(); no++) {
			if (staves(st)->getNote(no)->acidental == -1 || staves(st)->getNote(no)->acidental == 1)
				staves(st)->setNote(no, *staves(st)->getNote(no));
		}
	}
}


void TmainScore::showNamesSlot() {
	m_acts->noteNames()->setChecked(!m_acts->noteNames()->isChecked());
	for (int st = 0; st < staffCount(); st++) {
		for (int no = 0; no < staves(st)->count(); no++) {
			if (m_acts->noteNames()->isChecked())
				staves(st)->noteSegment(no)->showNoteName();
			else
				staves(st)->noteSegment(no)->removeNoteName();
		}
	}		
}


void TmainScore::zoomScoreSlot() {
	qreal newScale = gl->S->scoreScale;
	if (sender() == m_acts->zoomOut()) {
			newScale = qMin(gl->S->scoreScale + 0.25, 2.0);
	} else {
			newScale = qMax(gl->S->scoreScale - 0.25, 1.0);
	}
	if (newScale != gl->S->scoreScale) {
		gl->S->scoreScale = newScale;
		setScoreScale(newScale);
	}
}


void TmainScore::moveSelectedNote(TmainScore::EmoveNote nDir) {
	int prevIndex = currentIndex();
	if (nDir == e_doNotMove) { // determine action by sender which invoked this slot
			if (sender() == m_acts->firstNote() || sender() == m_keys->firstNote())
				nDir = e_first;
			else if (sender() == m_acts->lastNote() || sender() == m_keys->lastNote())
				nDir = e_last;
			else if (sender() == m_acts->staffUp() || sender() == m_keys->staffUp())
				nDir = e_prevStaff;
			else if (sender() == m_acts->staffDown() || sender() == m_keys->staffDown())
				nDir = e_nextStaff;
			else if (sender() == m_keys->nextNote())
				nDir = e_nextNote;
			else if (sender() == m_keys->prevNote())
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
			if (currentIndex() < notesCount() - 1 || (insertMode() == e_record && currentIndex() == notesCount() - 1)) {
					changeCurrentIndex(currentIndex() + 1); // record mode adds new note at the end`
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
	} else
			emit playbackFinished();
}


void TmainScore::strikeBlinkingFinished() {
	if (m_strikeOut) {
		m_strikeOut->deleteLater();
		m_strikeOut = 0;
	}
  delete m_bliking;
	deleteNoteName(0);
	staff()->noteSegment(0)->setColor(palette().text().color());
	staff()->noteSegment(0)->enableNoteAnim(true, 300);
	staff()->noteSegment(0)->markNote(-1);
	bool animEnabled = isAccidToKeyAnimEnabled();
	enableAccidToKeyAnim(false); // prevent animations - it looks ugly with correction animations
	TsimpleScore::setNote(0, m_goodNote);
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
				m_questKey->setHtml(m_questKey->toHtml().replace("?", "!").replace(gl->EquestionColor.name(), gl->EanswerColor.name()));
			m_keyBlinking->startBlinking(3); // and blink again
	} else { // finished 2nd time
			delete m_keyBlinking;
	}
}


void TmainScore::finishCorrection() {
	staff()->noteSegment(0)->enableNoteAnim(false);
	staff()->noteSegment(0)->markNote(QColor(gl->EanswerColor.name()));
// 	if (m_showNameInCorrection)
// 			showNames(m_corrStyle);
}


void TmainScore::resizeEvent(QResizeEvent* event) {
	TmultiScore::resizeEvent(event);
	if (width() < 300)
      return;
	if (insertMode() == e_single) {
		if (m_nameMenu->size().width() + sizeHint().width() > mainWindow()->width()) {
			if (m_nameMenu->buttonsDirection() == QBoxLayout::LeftToRight || m_nameMenu->buttonsDirection() == QBoxLayout::RightToLeft) {
				qDebug() << "name is too big. Changing direction ";
				m_nameMenu->setDirection(QBoxLayout::BottomToTop);
			}
		} else {
			if (m_nameMenu->buttonsDirection() == QBoxLayout::BottomToTop || m_nameMenu->buttonsDirection() == QBoxLayout::TopToBottom) {
				if (m_nameMenu->widthForHorizontal() + size().width() < mainWindow()->width()) {
					qDebug() << "There is enough space for horizontal name. Changing";
					m_nameMenu->setDirection(QBoxLayout::LeftToRight);
				}
			}
		}
// 		setFixedWidth(mapFromScene(scoreScene()->sceneRect()).boundingRect().width() + 1);
	}
	setBarsIconSize();
	performScordatureSet(); // To keep scordature size up to date with score size
}


void TmainScore::performScordatureSet() {
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar) {
			Ttune tmpTune(*gl->Gtune());
			staff()->setScordature(tmpTune);
	}
}



//####################################################################################################
//########################################## PRIVATE #################################################
//####################################################################################################

void TmainScore::createActions() {		
	m_settBar = new QToolBar();
	m_acts->noteNames()->setThisColors(gl->S->nameColor, palette().highlightedText().color());
	m_acts->noteNames()->setChecked(gl->S->namesOnScore);
	m_settBar->addWidget(m_acts->noteNames());
	m_settBar->addWidget(m_acts->extraAccids());
	
	
	m_settBar->addAction(m_acts->zoomOut());
	m_settBar->addAction(m_acts->zoomIn());
	m_settBar->addAction(m_acts->firstNote());
#if !defined (Q_OS_ANDROID)
	m_settBar->addAction(m_acts->staffUp());
	m_settBar->addAction(m_acts->staffDown());
#endif
	m_settBar->addAction(m_acts->lastNote());	
	m_settCorner = new TcornerProxy(scoreScene(), m_settBar, Qt::BottomRightCorner);
	
	m_clearBar = new QToolBar();
	m_clearBar->addAction(m_acts->clearScore());
	m_delCorner = new TcornerProxy(scoreScene(), m_clearBar, Qt::BottomLeftCorner);
	   m_delCorner->setSpotColor(Qt::red);
	
	m_rhythmBar = new QToolBar();
	QLabel *rl = new QLabel("Rhythms<br>not implemented yet", m_rhythmBar);
	m_rhythmBar->addWidget(rl);
	m_rhythmCorner = new TcornerProxy(scoreScene(), m_rhythmBar, Qt::TopLeftCorner);
	   m_rhythmCorner->setSpotColor(Qt::yellow);
	
	m_keys = new TscoreKeys(this);
	m_acts->assignKeys(m_keys);
}


void TmainScore::restoreNotesSettings() {
// 		if (gl->S->enharmNotesColor == -1)
// 					gl->S->enharmNotesColor = palette().highlight().color();
// 	TscoreNote::setNameColor(gl->S->nameColor);
	scoreScene()->right()->adjustSize();
	if (gl->S->pointerColor == -1) {
				gl->S->pointerColor = Tcolor::invert(palette().highlight().color());
				gl->S->pointerColor.setAlpha(200);
	}
	scoreScene()->setPointedColor(gl->S->pointerColor);
// 	for (int i = 0; i < staff()->count(); i++)
// 			staff()->noteSegment(0)->enableAccidToKeyAnim(true);
// 		staff()->noteSegment(1)->setReadOnly(true);
// 		staff()->noteSegment(1)->setColor(gl->S->enharmNotesColor);
// 		staff()->noteSegment(2)->setReadOnly(true);
// 		staff()->noteSegment(2)->setColor(gl->S->enharmNotesColor);
// 		staff()->noteSegment(0)->enableAccidToKeyAnim(true);
		
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
	QSize ss(mainWindow()->height() / 20, mainWindow()->height() / 20);
#endif
	m_settBar->setIconSize(ss);
	m_clearBar->setIconSize(ss);
	m_settBar->adjustSize();
	m_clearBar->adjustSize();
}


void TmainScore::createNoteName() {
	if (!m_nameMenu) {
			m_nameMenu = new TnoteName(mainWindow());
// #if defined (Q_OS_ANDROID)
// 			m_nameMenu->resize(fontMetrics().boundingRect("A").height() * 0.8);
// #else
// 			m_nameMenu->resize(fontMetrics().boundingRect("A").height());
// 			m_nameMenu->resize((qApp->desktop()->availableGeometry().height() / 20));
// #endif
			connect(m_nameMenu, SIGNAL(nextNote()), this, SLOT(moveNameForward()));
			connect(m_nameMenu, SIGNAL(prevNote()), this, SLOT(moveNameBack()));
			connect(m_nameMenu, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(menuChangedNote(Tnote)));
      connect(m_nameMenu, SIGNAL(statusTipRequired(QString)), this, SLOT(statusTipChanged(QString)));
			m_nameMenu->hide();
	}
}


void TmainScore::enableCorners(bool enable) {
	if (enable) {
		if (!isExam()) {
			m_settCorner->show();
			m_rhythmCorner->show();
		}
			m_delCorner->show();
	} else {
			m_settCorner->hide();
			m_rhythmCorner->hide();
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
	if (gl->S->namesOnScore)
			lastStaff()->noteSegment(0)->showNoteName();
	lastStaff()->setExtraAccids(m_acts->extraAccids()->isChecked());
	qDebug() << "staff Added";
}










