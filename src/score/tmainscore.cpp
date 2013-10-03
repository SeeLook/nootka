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

#include "tmainscore.h"
#include "tscorestaff.h"
#include "tscorenote.h"
#include "tscorekeysignature.h"
#include "tscorecontrol.h"
#include "tscoreclef.h"
#include "ttune.h"
#include "tglobals.h"
#include <tgraphicstexttip.h>
#include <tgraphicsstrikeitem.h>
#include <QPen>
#include <QLayout>
#include <QTimer>


extern Tglobals *gl;


TmainScore::TmainScore(QWidget* parent) :
	TsimpleScore(3, parent),
	m_questMark(0),
	m_questKey(0),
	m_strikeOut(0)
{
// set prefered clef
	if (gl->Sclef == Tclef::e_pianoStaff)
			TsimpleScore::setPianoStaff(true);
	else
			setClef(gl->Sclef);
// set note colors
	restoreNotesSettings();
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar)
			setScordature();
	setEnabledDblAccid(gl->doubleAccidentalsEnabled);
	setEnableKeySign(gl->SkeySignatureEnabled);
	
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));

	isExamExecuting(false);

	
	connect(this, SIGNAL(pianoStaffSwitched()), this, SLOT(onPianoSwitch()));
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
	setEnableKeySign(gl->SkeySignatureEnabled);
	setClef(Tclef(gl->Sclef));
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar)
			setScordature();
	else
			if (staff()->hasScordature()) {
				staff()->removeScordatute();
				if (staff()->lower())
					staff()->lower()->removeScordatute();
			}
	if (!gl->doubleAccidentalsEnabled)
		clearNote(2);
	setEnableEnharmNotes(gl->showEnharmNotes);
	if (gl->SkeySignatureEnabled) // refreshKeySignNameStyle();
		if (staff()->scoreKey())
			staff()->scoreKey()->showKeyName(gl->SshowKeySignName);
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));
	restoreNotesSettings();
	if (gl->SkeySignatureEnabled)
		setKeySignature(keySignature());
}



void TmainScore::setScordature() {
	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar) {
			Ttune tmpTune(*gl->Gtune());
			staff()->setScordature(tmpTune);
			resizeEvent(0);
	}
}


void TmainScore::unLockScore() {
	setScoreDisabled(false);
	setNoteDisabled(1, true);
	setNoteDisabled(2, true);
    if (m_questMark) { // question mark exists only when exam is performing
      setBGcolor(gl->mergeColors(gl->EanswerColor, palette().window().color()));
			setNoteViewBg(0, gl->EanswerColor);
    }
  setClefDisabled(true);
}


TscoreControl* TmainScore::getFreeController() {
	layout()->removeWidget(scoreController());
	layoutHasControl = false;
	return scoreController();
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
			m_questMark->setFont(QFont("nootka", 10));
		#else
			m_questMark->setFont(QFont("nootka", 8));
		#endif
			m_questMark->setParentItem(staff()->noteSegment(2));
			QColor c = gl->EquestionColor;
			c.setAlpha(220);
			staff()->noteSegment(1)->setColor(c);
			if (staff()->lower()) {
				staff()->lower()->noteSegment(1)->setColor(c);
			}
			m_questMark->setBrush(QBrush(c));
			m_questMark->setText("?");
			m_questMark->setPos(0, (staff()->boundingRect().height() - m_questMark->boundingRect().height()) / 2 );
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
    }
}


void TmainScore::clearScore() {
	clearNote(0);
	clearNote(1);
	clearNote(2);
	staff()->noteSegment(1)->removeString(); // so far string number to remove occurs only on this view
	staff()->noteSegment(0)->hideWorkNote();
	if (staff()->lower()) {
		staff()->lower()->noteSegment(1)->removeString();
		staff()->lower()->noteSegment(0)->hideWorkNote();
	}
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
}


void TmainScore::askQuestion(Tnote note, char realStr) {
		setNote(1, note);
    setBGcolor(gl->mergeColors(gl->EquestionColor, palette().window().color()));
    m_questMark->show();
    if (realStr) 
			setStringNumber(1, realStr);
}


void TmainScore::askQuestion(Tnote note, TkeySignature key, char realStr) {
	setKeySignature(key);
	askQuestion(note, realStr);
}

void TmainScore::expertNoteChanged() {
		emit noteClicked();
}


void TmainScore::forceAccidental(Tnote::Eacidentals accid) {
		if (scoreController())
			scoreController()->setAccidental(accid);
}


void TmainScore::markAnswered(QColor blurColor) {
		staff()->noteSegment(0)->markNote(blurColor);
		if (staff()->lower())
				staff()->lower()->noteSegment(0)->markNote(blurColor);
}


void TmainScore::markQuestion(QColor blurColor) {
		staff()->noteSegment(1)->markNote(blurColor);
		if (staff()->lower())
				staff()->lower()->noteSegment(1)->markNote(blurColor);
}


void TmainScore::prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName) {
		setKeySignature(fakeKey);
		m_questKey = new QGraphicsTextItem();
		m_questKey->setParentItem(staff()->scoreKey()); // we are sure that key exist - exam checked that
		m_questKey->setHtml(QString("<span style=\"color: %1;\"><span style=\"font-family: nootka;\">?</span><br>").
					arg(gl->EquestionColor.name()) + expectKeyName + "</span>");
		TgraphicsTextTip::alignCenter(m_questKey);
		qreal sc = (staff()->scoreKey()->boundingRect().width() + 5.0) / m_questKey->boundingRect().width();
		m_questKey->setScale(sc);
		m_questKey->setPos(-6.0, staff()->upperLinePos() - 3 - m_questKey->boundingRect().height() * sc);
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


void TmainScore::correctNote(Tnote& goodNote, const QColor &color) {
		m_goodNote = goodNote;
		m_strikeOut = new TgraphicsStrikeItem(staff()->noteSegment(0)->mainNote());
		QPen pp(QColor(color.name()), 0.5);
		m_strikeOut->setPen(pp);
		connect(m_strikeOut, SIGNAL(blinkingFinished()), this, SLOT(strikeBlinkingFinished()));
		m_strikeOut->startBlinking();
}


void TmainScore::correctKeySignature(TkeySignature newKey)
{

}

//####################################################################################################
//########################################## PUBLIC SLOTS ############################################
//####################################################################################################

void TmainScore::whenNoteWasChanged(int index, Tnote note) {
	//We are sure that index is 0, cause others are disabled :-)
    if (gl->showEnharmNotes) {
        TnotesList enharmList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        TnotesList::iterator it = enharmList.begin();
        ++it;
        if (it != enharmList.end())
            setNote(1, *(it));
        else
            clearNote(1);
        if (gl->doubleAccidentalsEnabled) {
            ++it;
            if (it != enharmList.end())
                setNote(2, *(it));
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
// 	if (gl->instrument == e_classicalGuitar || gl->instrument == e_electricGuitar)
		if (*gl->Gtune() != Ttune::stdTune)
			setScordature();
}


void TmainScore::strikeBlinkingFinished() {
	m_strikeOut->deleteLater();
	m_strikeOut = 0;
	staff()->noteSegment(0)->enableAnimation(true, 300);
	staff()->noteSegment(0)->markNote(-1);
	if (staff()->lower()) {
			staff()->lower()->noteSegment(0)->markNote(-1);
			staff()->lower()->noteSegment(0)->enableAnimation(true, 300);
	}
	setNote(0, m_goodNote);
	QTimer::singleShot(320, this, SLOT(finishCorrection()));	
}


void TmainScore::finishCorrection() {
	staff()->noteSegment(0)->enableAnimation(false);
	staff()->noteSegment(0)->markNote(QColor(gl->EanswerColor.name()));
	if (staff()->lower()) {
			staff()->noteSegment(0)->enableAnimation(false);
			staff()->lower()->noteSegment(0)->markNote(QColor(gl->EanswerColor.name()));
	}
}


//####################################################################################################
//########################################## PRIVATE #################################################
//####################################################################################################

void TmainScore::restoreNotesSettings() {
		if (gl->enharmNotesColor == -1)
					gl->enharmNotesColor = palette().highlight().color();
		if (gl->SpointerColor == -1) {
					gl->SpointerColor = gl->invertColor(palette().highlight().color());
					gl->SpointerColor.setAlpha(200);
		}
		staff()->noteSegment(0)->setPointedColor(gl->SpointerColor);
		staff()->noteSegment(1)->setReadOnly(true);
		staff()->noteSegment(1)->setColor(gl->enharmNotesColor);
		staff()->noteSegment(2)->setReadOnly(true);
		staff()->noteSegment(2)->setColor(gl->enharmNotesColor);
// 		staff()->noteSegment(0)->enableAnimation(true);
		if (staff()->lower()) {
				staff()->lower()->noteSegment(0)->setPointedColor(gl->SpointerColor);
				staff()->lower()->noteSegment(1)->setReadOnly(true);
				staff()->lower()->noteSegment(1)->setColor(gl->enharmNotesColor);
				staff()->lower()->noteSegment(2)->setReadOnly(true);
				staff()->lower()->noteSegment(2)->setColor(gl->enharmNotesColor);
// 				staff()->lower()->noteSegment(0)->enableAnimation(true);
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










