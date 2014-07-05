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
#include <score/tscoreview.h>
#include <music/ttune.h>
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


TmainScore::TmainScore(QWidget* parent) :
	TsimpleScore(1, parent),
	m_questMark(0),
	m_questKey(0),
	m_strikeOut(0),
	m_bliking(0), m_keyBlinking(0),
	m_corrStyle(Tnote::defaultStyle),
  m_inMode(e_multi),
  m_clickedOff(0), m_currentIndex(-1),
  m_scoreIsPlayed(false),
  m_parent(parent),
	m_addNoteAnim(true)
{
	score()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	staff()->setZValue(11); // to be above next staves - TnoteControl requires it
	m_acts = new TscoreActions(this, gl->path);
	
	TscoreNote::setNameColor(gl->S->nameColor);
	restoreNotesSettings();
	addStaff(staff());
// set preferred clef
	setClef(gl->S->clef);
	
	createActions();
  layout()->setContentsMargins(2, 2, 2, 2);
	
	m_noteName << 0 << 0;
// set note colors
// 	restoreNotesSettings();
	setScordature();
	setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
	setEnableKeySign(gl->S->keySignatureEnabled);
	if (m_staves.last()->scoreKey())
		connect(m_staves.last()->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedSlot()));
	
	connect(staff()->noteSegment(0)->right(), SIGNAL(nameMenu(TscoreNote*)), SLOT(showNameMenu(TscoreNote*)));
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));

	isExamExecuting(false);

}


TmainScore::~TmainScore()
{}


//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void TmainScore::setEnableEnharmNotes(bool isEnabled) {
// 	if (!isEnabled) {
// 		clearNote(1);
// 		clearNote(2);
// 	}
}


void TmainScore::acceptSettings() {
	bool refreshNoteNames = false; // This is CPU consuming operation - we try to avoid it
// Update note name style
	if  (Tnote::defaultStyle != gl->S->nameStyleInNoteName) {
		Tnote::defaultStyle = gl->S->nameStyleInNoteName;
		refreshNoteNames = true;
	}
// Double accidentals
	setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
	staff()->noteSegment(0)->left()->addAccidentals();
	setClef(Tclef(gl->S->clef));
// Enable/disable key signatures
	if ((bool)staff()->scoreKey() != gl->S->keySignatureEnabled) {
			setEnableKeySign(gl->S->keySignatureEnabled);
	}
	restoreNotesSettings();
// Note names on the score
	if (gl->S->nameColor != TscoreNote::nameColor()) {
			refreshNoteNames = true;
			m_acts->noteNames()->setThisColors(gl->S->nameColor, palette().highlightedText().color());
			TscoreNote::setNameColor(gl->S->nameColor);
	}
	if (gl->S->namesOnScore != m_acts->noteNames()->isChecked() || refreshNoteNames) {
		m_acts->noteNames()->setChecked(gl->S->namesOnScore);
		for (int st = 0; st < m_staves.size(); st++) {
			for (int no = 0; no < m_staves[st]->count(); no++) {
				if (m_acts->noteNames()->isChecked()) {
					if (refreshNoteNames) // remove name to pain it with new highlight
							m_staves[st]->noteSegment(no)->removeNoteName();
					m_staves[st]->noteSegment(no)->showNoteName();
				} else
					m_staves[st]->noteSegment(no)->removeNoteName();
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
// enharmonic alternatives  
// 	setEnableEnharmNotes(gl->S->showEnharmNotes);
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
	if (m_nameMenu) {
			m_nameMenu->setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
			m_nameMenu->setEnabledEnharmNotes(gl->S->showEnharmNotes);
			m_nameMenu->setNoteNamesOnButt(gl->S->nameStyleInNoteName);
			m_nameMenu->setStyle(gl->S->nameStyleInNoteName);
// 			m_nameMenu->setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
	}
}


void TmainScore::setInsertMode(TmainScore::EinMode mode) {
	m_inMode = mode;
}


TscoreStaff* TmainScore::currentStaff() {
	return m_staves[m_currentIndex / staff()->maxNoteCount()];
}


inline int TmainScore::notesCount() {
	return (m_staves.size() - 1) * staff()->maxNoteCount() + m_staves.last()->count();
}



void TmainScore::setNote(Tnote note) {
	if (insertMode() != e_single) {
			if (m_currentIndex == -1)
				changeCurrentIndex(0);
			TscoreStaff *thisStaff = currentStaff();
			if (insertMode() == e_record) {
					changeCurrentIndex(m_currentIndex + m_clickedOff);
					thisStaff = currentStaff();
					m_clickedOff = 1;
			}
			thisStaff->setNote(m_currentIndex % staff()->maxNoteCount(), note);
			if (m_staves.size() > 1)
					score()->ensureVisible(thisStaff, 0, 0);
	} else {
// 			if (staff()->currentIndex() != -1)
// 					TsimpleScore::setNote(staff()->currentIndex(), note);
// 			else
					qDebug() << "Single note mode not implemented";
	}
}


void TmainScore::noteWasClicked(int index) {
	TscoreStaff *st = SENDER_TO_STAFF;
	Tnote note = *(st->getNote(index));
	changeCurrentIndex(index + st->number() * st->maxNoteCount());
	m_clickedOff = 0;
	emit noteWasChanged(index, note);
	st->noteSegment(index)->update();
  checkAndAddNote(st, index);
}


void TmainScore::noteWasSelected(int index) {
	m_clickedOff = 0;
	TscoreStaff *st = SENDER_TO_STAFF;
	changeCurrentIndex(index + st->number() * st->maxNoteCount());
	emit noteWasChanged(index, *st->getNote(index));
}


void TmainScore::playScore() {
	if (m_scoreIsPlayed) {
		m_scoreIsPlayed = false;
		if (m_playTimer) {
			m_playTimer->stop();
			delete m_playTimer;
		}
	} else {
		if (m_currentIndex < 0)
			return;
		m_scoreIsPlayed = true;
		m_playTimer = new QTimer(this);
		connect(m_playTimer, SIGNAL(timeout()), this, SLOT(playSlot()));
		m_playTimer->start(60000 / gl->S->tempo);
		m_playedIndex = m_currentIndex - 1;
		playSlot();
	}
}


void TmainScore::onClefChanged(Tclef cl) {
	if (staff()->hasScordature())
			performScordatureSet();
	if (m_staves.size() > 1) {
			int staffNr = SENDER_TO_STAFF->number();
			for (int i = 0; i < m_staves.size(); i++)
				if (m_staves[i]->number() != staffNr) {
					m_staves[i]->disconnect(SIGNAL(clefChanged(Tclef)));
					m_staves[i]->onClefChanged(cl);
					connect(m_staves[i], SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
				}
	}
	TsimpleScore::onClefChanged(cl);
}


void TmainScore::setScordature() {
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar) {
			performScordatureSet();
// 			resizeEvent(0);
	}
}


void TmainScore::keyChangedSlot() {
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


/*
void TmainScore::unLockScore() {
	setScoreDisabled(false);
	setNoteDisabled(1, true);
	setNoteDisabled(2, true);
    if (m_questMark) { // question mark exists only when exam is performing
      setBGcolor(Tcolor::merge(gl->EanswerColor, palette().window().color()));
			setNoteViewBg(0, gl->EanswerColor);
    }
  setClefDisabled(true);
	QPointF nPos = staff()->noteSegment(0)->mapFromScene(score()->mapToScene(score()->mapFromParent(mapFromGlobal(cursor().pos()))));
	if (nPos.x() > 0.0 && nPos.x() < 7.0) {
		staff()->noteSegment(0)->moveWorkNote(nPos);
	}
}
*/

QRectF TmainScore::noteRect(int noteNr) {
		return QRectF(0, 0, staff()->noteSegment(noteNr)->mainNote()->rect().width() * score()->transform().m11(), 
			staff()->noteSegment(noteNr)->mainNote()->rect().height() * score()->transform().m11());
}


QPoint TmainScore::notePos(int noteNr) {
	QPointF nPos;
	if (staff()->noteSegment(noteNr)->mainNote()->isVisible())
		nPos = staff()->noteSegment(noteNr)->mainNote()->mapToScene(staff()->noteSegment(noteNr)->mainNote()->pos());
	QPoint vPos = score()->mapFromScene(staff()->pos().x() + staff()->noteSegment(noteNr)->pos().x() + staff()->noteSegment(noteNr)->mainNote()->pos().x(), staff()->noteSegment(noteNr)->mainNote()->pos().y());
	return mapToParent(score()->mapToParent(vPos));
}


void TmainScore::enableAccidToKeyAnim(bool enable) {
	staff()->noteSegment(0)->enableAccidToKeyAnim(enable);
}


bool TmainScore::isAccidToKeyAnimEnabled() {
	return staff()->noteSegment(0)->accidToKeyAnim();
}


int TmainScore::widthToHeight(int hi) {
	return qRound((qreal)hi / staff()->boundingRect().height()) * staff()->boundingRect().width();
}


//####################################################################################################
//############################## METHODS RELATED TO EXAMS ############################################
//####################################################################################################


void TmainScore::isExamExecuting(bool isIt) {
	if (isIt) {
			disconnect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
			connect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
			m_questMark = new QGraphicsSimpleTextItem();
			m_questMark->hide();
		#if defined(Q_OS_MACX)
			m_questMark->setFont(TnooFont(10));
		#else
			m_questMark->setFont(TnooFont(8));
		#endif
			m_questMark->setParentItem(staff()->noteSegment(2));
			QColor c = gl->EquestionColor;
			c.setAlpha(220);
			staff()->noteSegment(1)->setColor(c);
			m_questMark->setBrush(QBrush(c));
			m_questMark->setText("?");
			m_questMark->setScale(12.0 / m_questMark->boundingRect().width()); // 7.0 is not scaled segment width (12.0 is a bit bigger)
			m_questMark->setPos(0, 
													(staff()->boundingRect().height() - m_questMark->boundingRect().height() * m_questMark->scale()) / 2 );
			setScoreDisabled(true);
			setClefDisabled(true);
    } else {
        connect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        disconnect(this, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
        delete m_questMark;
        m_questMark = 0;
        delete m_questKey;
        m_questKey = 0;
				setClefDisabled(false);
				for (int i = 0; i < 2; i++)
					deleteNoteName(i);
    }
}

/*
void TmainScore::clearScore() {
	bool enableAnim = isAccidToKeyAnimEnabled();
	enableAccidToKeyAnim(false); // prevent animations to empty score
	clearNote(0);
	clearNote(1);
	clearNote(2);
	for (int i = 0; i < 2; i++)
			deleteNoteName(i);
	m_showNameInCorrection = false;
	staff()->noteSegment(1)->removeString(); // so far string number to remove occurs only on this view
	staff()->noteSegment(0)->hideWorkNote();
	if (staff()->scoreKey()) {
			setKeySignature(TkeySignature());
			if (m_questKey) {
				delete m_questKey;
				m_questKey = 0;
			}
    }
	if (scoreController())
		scoreController()->setAccidental(0); // reset buttons with accidentals
	for(int i = 0; i < m_bgRects.size(); i++)
		delete m_bgRects[i];
	m_bgRects.clear();
	m_questMark->hide();
	setBGcolor(palette().base().color());
	enableAccidToKeyAnim(enableAnim);
}


void TmainScore::askQuestion(Tnote note, char realStr) {
		TsimpleScore::setNote(1, note);
    setBGcolor(Tcolor::merge(gl->EquestionColor, palette().window().color()));
    m_questMark->show();
    if (realStr) 
			setStringNumber(1, realStr);
}


void TmainScore::askQuestion(Tnote note, TkeySignature key, char realStr) {
	setKeySignature(key);
	askQuestion(note, realStr);
}
*/

void TmainScore::expertNoteChanged() {
		emit noteClicked();
}

/*
void TmainScore::forceAccidental(Tnote::Eacidentals accid) {
		if (scoreController())
			scoreController()->setAccidental(accid);
}


void TmainScore::markAnswered(QColor blurColor) {
		staff()->noteSegment(0)->markNote(QColor(blurColor.lighter().name()));
}


void TmainScore::markQuestion(QColor blurColor) {
		staff()->noteSegment(1)->markNote(QColor(blurColor.lighter().name()));
}


void TmainScore::prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName) {
		setKeySignature(fakeKey);
		m_questKey = new QGraphicsTextItem();
		m_questKey->setParentItem(staff()->scoreKey()); // we are sure that key exist - exam checked that
		m_questKey->setHtml(QString("<span style=\"color: %1;\"><span style=\"font-family: nootka;\">?</span><br>").
					arg(gl->EquestionColor.name()) + expectKeyName + "</span>");
		TgraphicsTextTip::alignCenter(m_questKey);
		TscoreKeySignature::setKeyNameScale(m_questKey);
		m_questKey->setPos((staff()->scoreKey()->boundingRect().width() - m_questKey->boundingRect().width() * m_questKey->scale()) / 2 - 2.5,
						staff()->upperLinePos() - 3 - m_questKey->boundingRect().height() * m_questKey->scale());
		setKeyViewBg(gl->EanswerColor);
}


void TmainScore::setKeyViewBg(QColor C) {
	if (staff()->scoreKey()) {
			createBgRect(C, staff()->scoreKey()->boundingRect().width() + 6.0, 
									 QPointF(staff()->scoreKey()->pos().x() - 6.0, staff()->scoreKey()->pos().y()));
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



void TmainScore::showNames(Tnote::EnameStyle st, bool forAll) {
	int max = 1;
	if (forAll)
			max = 2;
	m_showNameInCorrection = false;
	m_corrStyle = st;
	for (int i = 0; i < max; i++) {
		if (getNote(i).note) {
			m_noteName[i] = new TgraphicsTextTip(getNote(i).toRichText(st));
			m_noteName[i]->setZValue(30);
			m_noteName[i]->setDropShadow(m_noteName[i], QColor(staff()->noteSegment(i)->mainNote()->pen().color().name()));
			m_noteName[i]->setDefaultTextColor(palette().text().color());
			m_noteName[i]->setParentItem(staff()->noteSegment(i));
// 			m_noteName[i]->setScale((score()->transform().m11()) / m_noteName[i]->boundingRect().height());
			m_noteName[i]->setScale(8.0 / m_noteName[i]->boundingRect().height());
			m_noteName[i]->setPos((7.0 - m_noteName[i]->boundingRect().width() * m_noteName[i]->scale()) / 2,
					staff()->noteSegment(i)->notePos() > staff()->upperLinePos() ? 
								staff()->noteSegment(i)->notePos() - m_noteName[i]->boundingRect().height() * m_noteName[i]->scale() : // above note
								staff()->noteSegment(i)->notePos() + staff()->noteSegment(i)->mainNote()->boundingRect().height()); // below note
			staff()->noteSegment(i)->removeString(); // String number is not needed here and could collide with name
		}
	}
	if (m_noteName[0])
			m_showNameInCorrection = true;
}
*/

void TmainScore::deleteNoteName(int id) {
	if (id < 0 || id > 1) // so far only two instances exist
		return;
	if (m_noteName.at(id)) { 
		delete m_noteName.at(id);
		m_noteName[id] = 0; 
	}
}

//####################################################################################################
//########################################## PUBLIC SLOTS ############################################
//####################################################################################################

void TmainScore::whenNoteWasChanged(int index, Tnote note) {
	//We are sure that index is 0, cause others are disabled :-)
    if (m_inMode == e_single && gl->S->showEnharmNotes) {
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
    }
    emit noteChanged(index, note);
}


void TmainScore::removeCurrentNote() {
	if (m_currentIndex > 0 || (m_currentIndex == 0 && staff()->count() > 1))
		currentStaff()->removeNote(m_currentIndex % staff()->maxNoteCount());
	else if (m_currentIndex == 0) { // just clear first note when only one
		m_clickedOff = 0;
		setNote(Tnote());
		emit noteWasChanged(0, Tnote());
	}
}

//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmainScore::showNameMenu(TscoreNote* sn) {
	if (!m_nameMenu) {
			m_nameMenu = new TnoteName(parentWidget());
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
	}
	m_nameMenu->setNoteName(*sn->note());
	m_currentNameSegment = sn;
	changeCurrentIndex(sn->staff()->number() * staff()->maxNoteCount() + sn->index());
	QPoint mPos = score()->mapFromScene(sn->pos().x() + 8.0, 0.0);
	mPos.setY(10);
	mPos = score()->mapToGlobal(mPos);
	m_clickedOff = 0;
	m_nameClickCounter = 0;
	m_nameMenu->exec(mPos, score()->transform().m11());
}


void TmainScore::menuChangedNote(Tnote n) {
	if (m_currentNameSegment) {
		if (m_nameClickCounter == 0) // add next note when user changes note through name menu
				checkAndAddNote(m_currentNameSegment->staff(), m_currentNameSegment->index());
		m_currentNameSegment->staff()->setNote(m_currentNameSegment->index(), n);
		m_currentNameSegment->update(); // Menu above covers focus
		emit noteWasChanged(m_currentNameSegment->index(), n);
		m_nameClickCounter++;
	}
}


void TmainScore::extraAccidsSlot() {
	m_acts->extraAccids()->setChecked(!m_acts->extraAccids()->isChecked());
	for (int st = 0; st < m_staves.size(); st++) {
		m_staves[st]->setExtraAccids(m_acts->extraAccids()->isChecked());
		for (int no = 0; no < m_staves[st]->count(); no++) {
			if (m_staves[st]->getNote(no)->acidental == -1 || m_staves[st]->getNote(no)->acidental == 1)
				m_staves[st]->setNote(no, *m_staves[st]->getNote(no));
		}
	}
}


void TmainScore::showNamesSlot() {
	m_acts->noteNames()->setChecked(!m_acts->noteNames()->isChecked());
	for (int st = 0; st < m_staves.size(); st++) {
		for (int no = 0; no < m_staves[st]->count(); no++) {
			if (m_acts->noteNames()->isChecked())
				m_staves[st]->noteSegment(no)->showNoteName();
			else
				m_staves[st]->noteSegment(no)->removeNoteName();
		}
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

void TmainScore::staffHasNoSpace(int staffNr) {
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


void TmainScore::staffHasFreeSpace(int staffNr, int notesFree) {
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


void TmainScore::noteGetsFree(int staffNr, TscoreNote* freeNote) {
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


void TmainScore::noteAddingSlot(int staffNr, int noteToAdd) {
	if (staffNr * staff()->maxNoteCount() + noteToAdd < m_currentIndex) {
// 		qDebug() << "selected note moved forward";
		m_currentIndex++;
	}
	if (gl->S->namesOnScore)
			m_staves[staffNr]->noteSegment(noteToAdd)->showNoteName();
	m_staves[staffNr]->noteSegment(noteToAdd)->enableAccidToKeyAnim(true);
	if (m_addNoteAnim)
		m_staves[staffNr]->noteSegment(noteToAdd)->popUpAnim(300);
	m_addNoteAnim = true;
}


void TmainScore::noteRemovingSlot(int staffNr, int noteToDel) {
	if (staffNr * staff()->maxNoteCount() + noteToDel == m_currentIndex) {
		qDebug() << "current selected note will be removed";
		changeCurrentIndex(-1);
	} else if (staffNr * staff()->maxNoteCount() + noteToDel < m_currentIndex) {
		qDebug() << "selected note moved backward";
		m_currentIndex--;
	}
}


void TmainScore::staffHiNoteChanged(int staffNr, qreal hiNoteYoff) {
// 	qDebug() << "staffHiNoteChanged" << hiNoteYoff << m_staves[staffNr]->hiNotePos();
	for (int i = staffNr; i < m_staves.size(); i++) // move every staff with difference
			m_staves[i]->setY(m_staves[i]->y() + hiNoteYoff);
	updateSceneRect();
}


void TmainScore::staffLoNoteChanged(int staffNr, qreal loNoteYoff) {
	if (m_staves.size() > 1 && staffNr < m_staves.size() - 1) { // more staves and not the last one
// 		qDebug() << "staffLoNoteChanged" << loNoteYoff << m_staves[staffNr]->loNotePos();
		for (int i = staffNr + 1; i < m_staves.size(); i++) // move every staff with difference
			m_staves[i]->setY(m_staves[i]->y() + loNoteYoff);
		updateSceneRect();
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
		resizeEvent(0);
	}
}


void TmainScore::deleteNotes() {
	if (staff()->count() <= 1)
			return; // nothing to delete
	m_currentIndex = 0;
	m_clickedOff = 0;
	while (m_staves.size() > 1) {
		deleteLastStaff();
	}
	if (staff()->count() > 1) {
		QList<TscoreNote*> notesToDel;
		staff()->takeNotes(notesToDel, 1, staff()->count() - 1);
		for (int i = 0; i <notesToDel.size(); i++)
			delete notesToDel[i];
	}
// 	staff()->noteSegment(0)->setNote(0, 0, Tnote());
	setNote(Tnote());
	updateSceneRect();
	emit noteWasChanged(0, Tnote());
}


void TmainScore::moveSelectedNote(TmainScore::EmoveNote nDir) {
	int prevIndex = m_currentIndex;
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
			if (m_currentIndex > 0)
					changeCurrentIndex(m_currentIndex - 1);
			break;
		}
		case e_nextNote: {
			if (m_currentIndex < notesCount() - 1 || (insertMode() == e_record && m_currentIndex == notesCount() - 1)) {
					changeCurrentIndex(m_currentIndex + 1); // record mode adds new note at the end`
			}
			break;
		}
		case e_nextStaff: {
			if (currentStaff() != m_staves.last())
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
	if (prevIndex != m_currentIndex) {
			emit noteWasChanged(m_currentIndex % staff()->maxNoteCount(), 
												*currentStaff()->getNote(m_currentIndex % staff()->maxNoteCount()));
			m_clickedOff = 0;
	}
}


void TmainScore::playSlot() {
	m_playedIndex++;
	if (m_playedIndex < notesCount()) {
		// by emitting that signal note is played and shown on the guitar
			emit noteWasChanged(m_playedIndex % staff()->maxNoteCount(), 
												*currentStaff()->getNote(m_playedIndex% staff()->maxNoteCount()));
			changeCurrentIndex(m_playedIndex);
	} else
			emit playbackFinished();
}


/*
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
	if (m_showNameInCorrection)
			showNames(m_corrStyle);
}
*/

void TmainScore::resizeEvent(QResizeEvent* event) {
	int hh = height(), ww = width();
	if (event) {
		hh = event->size().height();
		ww = event->size().width();
	}
	if (ww < 300)
      return;
	hh = score()->rect().height();
	setBarsIconSize();
	QList<TscoreNote*> allNotes;
	for (int i = 0; i < m_staves.size(); i++) { // grab all TscoreNote
		m_staves[i]->takeNotes(allNotes, 0, m_staves[i]->count() - 1);
	}
	qreal staffOff = 0.0;
  if (staff()->isPianoStaff())
    staffOff = 1.1;
  qreal factor = (((qreal)hh / (staff()->height() + 2.0)) / score()->transform().m11()) / gl->S->scoreScale;
  score()->scale(factor, factor);
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
	TcornerProxy *settCorner = new TcornerProxy(scoreScene(), m_settBar, Qt::BottomRightCorner);
	
	m_clearBar = new QToolBar();
	m_clearBar->addAction(m_acts->clearScore());
	TcornerProxy *delCorner = new TcornerProxy(scoreScene(), m_clearBar, Qt::BottomLeftCorner);
	delCorner->setSpotColor(Qt::red);
	
	m_rhythmBar = new QToolBar();
	QLabel *rl = new QLabel("Rhythms<br>not implemented yet", m_rhythmBar);
	m_rhythmBar->addWidget(rl);
	TcornerProxy *rhythmCorner = new TcornerProxy(scoreScene(), m_rhythmBar, Qt::TopLeftCorner);
	rhythmCorner->setSpotColor(Qt::yellow);
	
	m_keys = new TscoreKeys(this);
	m_acts->assignKeys(m_keys);
}


void TmainScore::restoreNotesSettings() {
// 		if (gl->S->enharmNotesColor == -1)
// 					gl->S->enharmNotesColor = palette().highlight().color();
// 	TscoreNote::setNameColor(gl->S->nameColor);
	staff()->noteSegment(0)->right()->adjustSize();
	if (gl->S->pointerColor == -1) {
				gl->S->pointerColor = Tcolor::invert(palette().highlight().color());
				gl->S->pointerColor.setAlpha(200);
	}
	if (staff()->count())
			staff()->noteSegment(0)->setPointedColor(gl->S->pointerColor);
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


void TmainScore::updateSceneRect() {
	qreal sh;
	if (m_staves.size() == 1)
		sh = (staff()->height() + 0.1) * gl->S->scoreScale;
	else
		sh = m_staves.last()->pos().y() + m_staves.last()->height();
	QRectF scRec = staff()->mapToScene(QRectF(0.0, 0.0, 
								staff()->width() + (staff()->isPianoStaff() ? 1.1 : 0.0),	sh)).boundingRect();
	scoreScene()->setSceneRect(0.0, 0.0, scRec.width(), scRec.height());
	qDebug() << "updateSceneRect" << scoreScene()->sceneRect() << m_staves.size();
}

/**  In record mode  add new 'empty' note segment at the end off the staff when index is on its last note 
 * but ignore last possible note on the staff - new staff is already created with a new single note */
void TmainScore::checkAndAddNote(TscoreStaff* sendStaff, int noteIndex) {
  if (insertMode() == e_record && noteIndex == sendStaff->count() - 1 && noteIndex != sendStaff->maxNoteCount() - 1) {
      Tnote nn(0, 0, 0);
			m_addNoteAnim = false; // do not show adding note animation when note is added here
			sendStaff->addNote(nn);
			if (gl->S->namesOnScore)
				sendStaff->noteSegment(sendStaff->count() - 1)->showNoteName();
  }
}


void TmainScore::adjustStaffWidth(TscoreStaff* st) {
	int scrollOff = score()->verticalScrollBar()->isVisible() ? score()->verticalScrollBar()->width() : 0;
	st->setViewWidth((score()->width() - 25 - scrollOff) / score()->transform().m11());
// 	st->setViewWidth((score()->rect().width() - scrollOff) / score()->transform().m11());
}


void TmainScore::changeCurrentIndex(int newIndex) {
	if (newIndex != m_currentIndex) {
			int prevIndex = m_currentIndex;
			if (m_currentIndex >= 0) { // deselect previous note
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->selectNote(false);
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->setBackgroundColor(-1);
			}
			m_currentIndex = newIndex;
			if (m_currentIndex / staff()->maxNoteCount() == m_staves.size()) // add new staff with single note
					staffHasNoSpace(m_currentIndex / staff()->maxNoteCount() - 1);
			else if (m_currentIndex % staff()->maxNoteCount() == currentStaff()->count())
					checkAndAddNote(currentStaff(), m_currentIndex % staff()->maxNoteCount() - 1);
			else if (m_currentIndex / staff()->maxNoteCount() > m_staves.size() ||
							m_currentIndex % staff()->maxNoteCount() > currentStaff()->count()) {
									qDebug() << "Something wrong with current index" << m_currentIndex; 
									return;
			}
			if (m_currentIndex >= 0) { // select a new note
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->setBackgroundColor(palette().highlight().color());
				currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount())->selectNote(true);
			}
			if (prevIndex / staff()->maxNoteCount() != m_currentIndex / staff()->maxNoteCount())
				score()->ensureVisible(currentStaff(), 0, 0);
	}
}


void TmainScore::setBarsIconSize() {
#if defined (Q_OS_ANDROID)
	QSize ss(parentWidget()->height() / 10, parentWidget()->height() / 10);
#else
	QSize ss(parentWidget()->height() / 20, parentWidget()->height() / 20);
#endif
	m_settBar->setIconSize(ss);
	m_clearBar->setIconSize(ss);
	m_settBar->adjustSize();
	m_clearBar->adjustSize();
}


void TmainScore::moveName(TmainScore::EmoveNote moveDir) {
	int oldIndex = m_currentIndex;
	moveSelectedNote(moveDir);
	if (oldIndex != m_currentIndex) {
		showNameMenu(currentStaff()->noteSegment(m_currentIndex % staff()->maxNoteCount()));
	}
}



void TmainScore::addStaff(TscoreStaff* st) {
	if (st == 0) { // create new staff at the end of a list
			m_staves << new TscoreStaff(scoreScene(), 1);
			m_staves.last()->onClefChanged(m_staves.first()->scoreClef()->clef());
			m_staves.last()->setEnableKeySign(gl->S->keySignatureEnabled);
			if (m_staves.last()->scoreKey())
				m_staves.last()->scoreKey()->setKeySignature(m_staves.first()->scoreKey()->keySignature());
			connect(m_staves.last(), SIGNAL(hiNoteChanged(int,qreal)), this, SLOT(staffHiNoteChanged(int,qreal))); // ignore for first
	} else { // staff of TsimpleScore is added this way
			st->enableToAddNotes(true);
			st->disconnect(SIGNAL(noteChanged(int)));
			st->disconnect(SIGNAL(clefChanged(Tclef)));
			m_staves << st;
	}
	if (gl->S->namesOnScore)
			m_staves.last()->noteSegment(0)->showNoteName();
	m_staves.last()->setStafNumber(m_staves.size() - 1);
	m_staves.last()->setExtraAccids(m_acts->extraAccids()->isChecked());
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
	qDebug() << "staff Added";
}


void TmainScore::deleteLastStaff() {
	delete m_staves.last();
	m_staves.removeLast();
	qDebug() << "staff deleted";
}








