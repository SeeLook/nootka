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
#include <score/tscorestaff.h>
#include <score/tscorenote.h>
#include <score/tscorekeysignature.h>
#include <score/tscorecontrol.h>
#include <score/tscoreclef.h>
#include <score/tscorescene.h>
#include <score/tnotecontrol.h>
#include <music/ttune.h>
#include <tglobals.h>
#include <graphics/tgraphicstexttip.h>
#include <animations/tstrikedoutitem.h>
#include <animations/tblinkingitem.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <notename/tnotename.h>
#include <QtWidgets>


#define SENDER_TO_STAFF static_cast<TscoreStaff*>(sender())

extern Tglobals *gl;

QWidget *m_parent;

TmainScore::TmainScore(QWidget* parent) :
	TsimpleScore(1, parent),
	m_questMark(0),
	m_questKey(0),
	m_strikeOut(0),
	m_bliking(0), m_keyBlinking(0),
	m_corrStyle(Tnote::defaultStyle),
  m_inMode(e_record), 
  m_clickedOff(0), m_currentIndex(0),
  m_scale(1.0)
{
  m_parent = parent;
// 	score()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	score()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
// 	score()->setFrameShape(QFrame::Box);
	staff()->setZValue(11); // to be above next staves - TnoteControl requires it
	addStaff(staff());
// set preferred clef
	setClef(gl->Sclef);
	m_outZoomButt = new QPushButton(QIcon(gl->path + "/picts/zoom-out.png"), "", scoreController());
	m_inZoomBuut = new QPushButton(QIcon(gl->path + "/picts/zoom-in.png"), "", scoreController());
	static_cast<QVBoxLayout*>(scoreController()->layout())->insertStretch(0);
	static_cast<QVBoxLayout*>(scoreController()->layout())->insertWidget(0, m_inZoomBuut);
	static_cast<QVBoxLayout*>(scoreController()->layout())->addStretch();
	static_cast<QVBoxLayout*>(scoreController()->layout())->addWidget(m_outZoomButt);
	connect(m_outZoomButt, SIGNAL(clicked()), this, SLOT(zoomScoreSlot()));
	connect(m_inZoomBuut, SIGNAL(clicked()), this, SLOT(zoomScoreSlot()));
	
	m_noteName << 0 << 0;
// set note colors
	restoreNotesSettings();
	setScordature();
	setEnabledDblAccid(gl->doubleAccidentalsEnabled);
	setEnableKeySign(gl->SkeySignatureEnabled);
	if (m_staves.last()->scoreKey())
		connect(m_staves.last()->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedSlot()));
	
	connect(staff()->noteSegment(0)->right(), SIGNAL(nameMenu(TscoreNote*)), SLOT(showNameMenu(TscoreNote*)));
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));

	isExamExecuting(false);

	
// 	connect(this, SIGNAL(pianoStaffSwitched()), this, SLOT(onPianoSwitch()));
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
	setEnabledDblAccid(gl->doubleAccidentalsEnabled);
	setClef(Tclef(gl->Sclef));
	setEnableKeySign(gl->SkeySignatureEnabled);
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar)
			setScordature();
	else
			if (staff()->hasScordature())
				staff()->removeScordatute();
	if (!gl->doubleAccidentalsEnabled)
		clearNote(2);
	setEnableEnharmNotes(gl->showEnharmNotes);
	if (gl->SkeySignatureEnabled) // refreshKeySignNameStyle();
		if (staff()->scoreKey())
			staff()->scoreKey()->showKeyName(gl->SshowKeySignName);
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));
	restoreNotesSettings();
	enableAccidToKeyAnim(false); // prevent accidental animation to empty note
	if (gl->SkeySignatureEnabled)
		setKeySignature(keySignature());
	enableAccidToKeyAnim(true);
}


void TmainScore::setInsertMode(TmainScore::EinMode mode) {
	m_inMode = mode;
}


void TmainScore::setNote(Tnote note) {
	if (insertMode() != e_single) {
			TscoreStaff *thisStaff = m_staves[m_currentIndex / staff()->maxNoteCount()];
			int prevIndex = m_currentIndex;
			if (insertMode() == e_record) {
					m_currentIndex += m_clickedOff;
					if (m_currentIndex && m_currentIndex % staff()->maxNoteCount() == 0) {
						if (thisStaff->number() + 1 < m_staves.size())
							thisStaff = m_staves[thisStaff->number() + 1];
						else {
// 							staffHasNoSpace(m_staves.last()->number());
							qDebug() << "setNote() has no more staves";
							return;
						}
					}
					m_clickedOff = 1;
			}
			if (thisStaff->number() != prevIndex / staff()->maxNoteCount())
					m_staves[prevIndex / staff()->maxNoteCount()]->setCurrentIndex(-1); // reset previous selection
			checkAndAddNote(thisStaff);
			thisStaff->setNote(m_currentIndex % staff()->maxNoteCount(), note);
			if (m_staves.size() > 1)
					score()->centerOn(score()->mapFromScene(thisStaff->mapToScene(thisStaff->pos())));
	} else {
// 			if (staff()->currentIndex() != -1)
// 					TsimpleScore::setNote(staff()->currentIndex(), note);
// 			else
					qDebug() << "Single note mode not implemented";
	}
}


void TmainScore::noteWasClicked(int index) {
	TscoreStaff *st = SENDER_TO_STAFF;
	if (m_currentIndex / st->maxNoteCount() != st->number()) // reset previous selection
			m_staves[m_currentIndex / staff()->maxNoteCount()]->setCurrentIndex(-1);
	Tnote note = *(st->getNote(index));
	m_currentIndex = index + st->number() * st->maxNoteCount();
	m_clickedOff = 0;
	emit noteWasChanged(index, note);
  checkAndAddNote(st);
}


void TmainScore::onClefChanged(Tclef cl) {
	if (m_staves.size() > 1) {
			int staffNr = SENDER_TO_STAFF->number();
			for (int i = 0; i < m_staves.size(); i++)
				if (m_staves[i]->number() != staffNr) {
					m_staves[i]->disconnect(SIGNAL(clefChanged(Tclef)));
					m_staves[i]->onClefChanged(cl);
					connect(m_staves[i], SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
					if (i != m_currentIndex / staff()->maxNoteCount())
							m_staves[i]->setCurrentIndex(-1); // fix selection
				}
	}
	TsimpleScore::onClefChanged(cl);
}


void TmainScore::setScordature() {
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar) {
			performScordatureSet();
			resizeEvent(0);
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

TscoreControl* TmainScore::getFreeController() {
	layout()->removeWidget(scoreController());
	layoutHasControl = false;
	return scoreController();
}


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
	return qRound((qreal)hi / staff()->boundingRect().height()) * staff()->boundingRect().width() + 
			scoreController()->width() + 10;
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
    if (m_inMode == e_single && gl->showEnharmNotes) {
        TnotesList enharmList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        TnotesList::iterator it = enharmList.begin();
        ++it;
        if (it != enharmList.end())
            TsimpleScore::setNote(1, *(it));
        else
            clearNote(1);
        if (gl->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                TsimpleScore::setNote(2, *(it));
            else
                clearNote(2);
        }
    }
    emit noteChanged(index, note);
}


//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmainScore::onPianoSwitch() {
		restoreNotesSettings();
		if (*gl->Gtune() != Ttune::stdTune)
			setScordature();
}


void TmainScore::showNameMenu(TscoreNote* sn) {
	if (!m_nameMenu) {
			m_nameMenu = new TnoteName(parentWidget());
			m_nameMenu->setNoteName(*sn->note());
			m_currentNameSegment = sn;
			connect(m_nameMenu, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(menuChangedNote(Tnote)));
			QPoint mPos = score()->mapFromScene(sn->pos().x() + 11.0, 0.0);
			mPos.setY(10);
			mPos = score()->mapToGlobal(mPos);
			m_nameMenu->exec(mPos, score()->transform().m11());
			delete m_nameMenu;
	}
}


void TmainScore::menuChangedNote(Tnote n) {
	if (m_currentNameSegment) {
		m_currentNameSegment->staff()->setNote(m_currentNameSegment->index(), n);
		emit noteWasChanged(m_currentNameSegment->index(), n);
	}
}


/** Managing notes:
 */

void TmainScore::staffHasNoSpace(int staffNr) {
	addStaff();
	adjustStaffWidth(m_staves.last());
	m_staves.last()->checkNoteRange(false);
	m_staves.last()->setPos(staff()->pos().x(), 
													m_staves[staffNr]->y() + m_staves[staffNr]->loNotePos() - m_staves.last()->hiNotePos() + 4.0);
	updateSceneRect();
}


void TmainScore::staffHasFreeSpace(int staffNr, int notesFree) {
	qDebug() << "staffHasFreeSpace" << staffNr << notesFree;
	if (m_staves[staffNr] != m_staves.last()) { // is not the last staff
		QList<TscoreNote*> notes;
		m_staves[staffNr + 1]->takeNotes(notes, 0, notesFree - 1);
		m_staves[staffNr]->addNotes(m_staves[staffNr]->count(), notes);
		if (staffNr + 2 < m_staves.size()) {
			staffHasFreeSpace(staffNr + 1, notesFree);
		}
		if (!m_staves[staffNr + 1]->count()) {
			delete m_staves[staffNr + 1];
			m_staves.removeAt(staffNr + 1);
			qDebug() << "staff deleted" << staffNr + 1;
			updateSceneRect();
		}			
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
		qDebug() << "selected note moved forward";
		m_currentIndex++;
	}
	m_staves[staffNr]->noteSegment(noteToAdd)->showNoteName();
	m_staves[staffNr]->noteSegment(noteToAdd)->enableAccidToKeyAnim(true);
}

void TmainScore::noteRemovingSlot(int staffNr, int noteToDel) {
	if (staffNr * staff()->maxNoteCount() + noteToDel == m_currentIndex) {
		qDebug() << "current selected note will be removed";
		m_currentIndex = 0;
	} else if (staffNr * staff()->maxNoteCount() + noteToDel < m_currentIndex) {
		qDebug() << "selected note moved backward";
		m_currentIndex--;
	}
}


void TmainScore::staffHiNoteChanged(int staffNr, qreal hiNoteYoff) {
// 	if (staffNr > 1)
	qDebug() << "staffHiNoteChanged" << hiNoteYoff << m_staves[staffNr]->hiNotePos();
	for (int i = staffNr; i < m_staves.size(); i++) // move every staff with difference
			m_staves[i]->setY(m_staves[i]->y() + hiNoteYoff);
// 	scene()->setSceneRect(0.0, 0.0, scene()->sceneRect().width(), 
// 												scene()->sceneRect().height() + hiNoteYoff * score()->transform().m11());
// 	qDebug() << scene()->sceneRect();
	updateSceneRect();
}


void TmainScore::staffLoNoteChanged(int staffNr, qreal loNoteYoff) {
	qDebug() << "staffLoNoteChanged" << loNoteYoff << m_staves[staffNr]->loNotePos();
	if (m_staves.size() > 1 && staffNr < m_staves.size() - 1) { // more staves and not the last one
		for (int i = staffNr + 1; i < m_staves.size(); i++) // move every staff with difference
			m_staves[i]->setY(m_staves[i]->y() + loNoteYoff);
// 		scene()->setSceneRect(0.0, 0.0, scene()->sceneRect().width(), 
// 												scene()->sceneRect().height() + loNoteYoff * score()->transform().m11());
// 		qDebug() << scene()->sceneRect();
		updateSceneRect();
	}
}



void TmainScore::zoomScoreSlot() {
	qreal newScale = m_scale;
	if (sender() == m_outZoomButt) {
			newScale = qMin(m_scale + 0.25, 2.0);
	} else {
			newScale = qMax(m_scale - 0.25, 1.0);
	}
	if (newScale != m_scale) {
		m_scale = newScale;
		resizeEvent(0);
	}
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
// 	TsimpleScore::resizeEvent(event);
	int hh = height(), ww = width();
	if (event) {
		hh = event->size().height();
		ww = event->size().width();
	}
	if (ww < 500)
      return;
	QList<TscoreNote*> allNotes;
	for (int i = 0; i < m_staves.size(); i++) { // grab all TscoreNote
		m_staves[i]->setCurrentIndex(-1);
		m_staves[i]->takeNotes(allNotes, 0, m_staves[i]->count() - 1);
	}
	qreal staffOff = 0.0;
  if (staff()->isPianoStaff())
    staffOff = 1.1;
  qreal factor = (((qreal)hh / (staff()->height() + 4.0)) / score()->transform().m11()) / m_scale;
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
						delete m_staves.last();
						m_staves.removeLast();
						qDebug() << "staff deleted";
					}
			}
		}
// 		m_staves[i]->setPos(staffOff, 0.05 + i * (staff()->height() - 16.0));
		if (allNotes.size() > i * m_staves[i]->maxNoteCount()) {
				QList<TscoreNote*> stNotes = allNotes.mid(i * m_staves[i]->maxNoteCount(), m_staves[i]->maxNoteCount());
				m_staves[i]->addNotes(0, stNotes);
		}
		
		if (i == 0)
			m_staves[i]->setPos(staffOff, 0.05);
		else
			m_staves[i]->setPos(staffOff, m_staves[i - 1]->pos().y() + m_staves[i - 1]->loNotePos() - m_staves[i]->hiNotePos() + 4.0);
// 		qDebug() << i << "staff position" << staffYPos << m_staves[i]->minHight();
	}
	updateSceneRect();
	m_staves[m_currentIndex / staff()->maxNoteCount()]->setCurrentIndex(m_currentIndex % staff()->maxNoteCount());
	
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

void TmainScore::restoreNotesSettings() {
// 		if (gl->enharmNotesColor == -1)
// 					gl->enharmNotesColor = palette().highlight().color();
	if (gl->SpointerColor == -1) {
				gl->SpointerColor = Tcolor::invert(palette().highlight().color());
				gl->SpointerColor.setAlpha(200);
	}
// 	if (staff()->count())
// 			staff()->noteSegment(0)->setPointedColor(gl->SpointerColor);
	for (int i = 0; i < staff()->count(); i++)
			staff()->noteSegment(0)->enableAccidToKeyAnim(true);
// 		staff()->noteSegment(1)->setReadOnly(true);
// 		staff()->noteSegment(1)->setColor(gl->enharmNotesColor);
// 		staff()->noteSegment(2)->setReadOnly(true);
// 		staff()->noteSegment(2)->setColor(gl->enharmNotesColor);
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
		sh = (staff()->height() + 0.1) * m_scale;
	else
		sh = m_staves.last()->pos().y() + m_staves.last()->height() /*- m_staves.last()->hiNotePos() + 4.0*/;
	QRectF scRec = staff()->mapToScene(QRectF(0.0, 0.0, 
								staff()->width() + (staff()->isPianoStaff() ? 1.1 : 0.0),	sh)).boundingRect();
// 	QRectF scRec = staff()->mapToScene(QRectF(0.0, 0.0, staff()->width() + (staff()->isPianoStaff() ? 1.0 : 0.0),
// 																16.0 + (staff()->height() - 16.0) * qMax(m_scale, (qreal)m_staves.size()))).boundingRect();
	scene()->setSceneRect(0.0, 0.0, scRec.width(), scRec.height());
	qDebug() << "updateSceneRect" << scene()->sceneRect() << m_staves.size();
}


void TmainScore::checkAndAddNote(TscoreStaff* sendStaff) {
  if (insertMode() != e_single && sendStaff->currentIndex() == sendStaff->count() - 1) {
      Tnote nn(0, 0, 0);
			if (sendStaff->count() - 1 < sendStaff->maxNoteCount()) { // add note to staff invoking this
					sendStaff->addNote(nn);
// 					sendStaff->noteSegment(sendStaff->count() - 1)->enableAccidToKeyAnim(true);
// 					sendStaff->noteSegment(sendStaff->count() - 1)->showNoteName();
			} // when it is last possible note on the staff - staff has already sent noMoreSpace() signal
  }
}


void TmainScore::adjustStaffWidth(TscoreStaff* st) {
	int scrollOff = score()->verticalScrollBar()->isVisible() ? score()->verticalScrollBar()->width() : 0;
	st->setViewWidth((score()->width() - 20 - scrollOff) / score()->transform().m11());
}


void TmainScore::addStaff(TscoreStaff* st) {
	if (st == 0) { // create new staff at the end of a list
			m_staves << new TscoreStaff(scene(), 1);
			m_staves.last()->setScoreControler(scoreController());
			m_staves.last()->onClefChanged(m_staves.first()->scoreClef()->clef());
			m_staves.last()->setEnableKeySign(gl->SkeySignatureEnabled);
			if (m_staves.last()->scoreKey())
				m_staves.last()->scoreKey()->setKeySignature(m_staves.first()->scoreKey()->keySignature());
			connect(m_staves.last(), SIGNAL(hiNoteChanged(int,qreal)), this, SLOT(staffHiNoteChanged(int,qreal))); // ignore for first
	} else { // staff of TmainScore is added this way
			st->disconnect(SIGNAL(noteChanged(int)));
			st->disconnect(SIGNAL(clefChanged(Tclef)));
			m_staves << st;
	}
	m_staves.last()->noteSegment(0)->showNoteName();
	m_staves.last()->setStafNumber(m_staves.size() - 1);
	m_staves.last()->setSelectableNotes(true);
	connect(m_staves.last(), SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
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








