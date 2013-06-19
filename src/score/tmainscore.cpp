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


extern Tglobals *gl;


TmainScore::TmainScore(QWidget* parent) :
	TsimpleScore(3, parent),
	m_questMark(0),
	m_questKey(0)
{
// set prefered clef
	if (gl->Sclef == Tclef::e_pianoStaff)
			TsimpleScore::setPianoStaff(true);
	else
			staff()->scoreClef()->setClef(Tclef(gl->Sclef));
// set note colors
	restoreNotesSettings();
	
	setScordature();
	setEnabledDblAccid(gl->doubleAccidentalsEnabled);
	setEnableKeySign(gl->SkeySignatureEnabled);
	
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));

	isExamExecuting(false);

	
	connect(this, SIGNAL(pianoStaffSwitched()), this, SLOT(onPianoSwitch()));
}

TmainScore::~TmainScore()
{
}


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
	setScordature();
	setEnableKeySign(gl->SkeySignatureEnabled);
	if (!gl->doubleAccidentalsEnabled)
		clearNote(2);
	staff()->noteSegment(0)->setPointedColor(gl->SpointerColor);
  staff()->noteSegment(1)->setColor(gl->enharmNotesColor);
	staff()->noteSegment(2)->setColor(gl->enharmNotesColor);
	setEnableEnharmNotes(gl->showEnharmNotes);
	if (gl->SkeySignatureEnabled) // refreshKeySignNameStyle();
		if (staff()->scoreKey())
			staff()->scoreKey()->showKeyName(true);
//     setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()-1),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber+1));
}



void TmainScore::setScordature() {
	Ttune tmpTune = gl->Gtune();
	staff()->setScordature(tmpTune);
	resizeEvent(0);
}


void TmainScore::unLockScore() {
	setScoreDisabled(false);
	staff()->noteSegment(1)->setReadOnly(true);
	staff()->noteSegment(2)->setReadOnly(true);
//     if (m_questMark) {
//       setBGcolor(gl->mergeColors(gl->EanswerColor, palette().window().color()));
//       noteViews[0]->setStyleSheet(gl->getBGcolorText(gl->EanswerColor) + "border-radius: 10px;");
//     }
}


//####################################################################################################
//############################## METHODS RELATED TO EXAMS ############################################
//####################################################################################################

void TmainScore::isExamExecuting(bool isIt) {
	if (isIt) {
        disconnect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
        m_questMark = new QGraphicsSimpleTextItem();
        m_questMark->hide();
//         noteViews[2]->scene()->addItem(m_questMark);
//         QColor c = gl->EquestionColor;
//         c.setAlpha(220);
//         noteViews[1]->setColor(c);
//         m_questMark->setBrush(QBrush(c));
//         m_questMark->setText("?");
//         resizeQuestMark();
    }
    else {
        connect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(whenNoteWasChanged(int,Tnote)));
        disconnect(this, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(expertNoteChanged()));
        delete m_questMark;
        m_questMark = 0;
        delete m_questKey;
        m_questKey = 0;
    }
}


void TmainScore::clearScore() {
	clearNote(0);
	clearNote(1);
	staff()->noteSegment(1)->removeString(); // so far string number to remove occur only on this view
  // TODO also hide question mark when will be implemented
	if (staff()->scoreKey()) {
			setKeySignature(TkeySignature());
			staff()->scoreKey()->setBackgroundColor(-1);
			if (m_questKey) {
				delete m_questKey;
				m_questKey = 0;
			}
    }
    if (scoreControler())
			scoreControler()->setAccidental(0); // reset buttons with accidentals
//     m_questMark->hide();
		staff()->noteSegment(0)->setBackgroundColor(-1);
    setBGcolor(palette().base().color());
}


void TmainScore::askQuestion(Tnote note, char realStr) 
{

}

void TmainScore::askQuestion(Tnote note, TkeySignature key, char realStr)
{

}

void TmainScore::expertNoteChanged()
{

}

void TmainScore::forceAccidental(Tnote::Eacidentals accid)
{

}

void TmainScore::markAnswered(QColor blurColor)
{

}

void TmainScore::markQuestion(QColor blurColor)
{

}

void TmainScore::prepareKeyToAnswer(TkeySignature fakeKey, QString expectKeyName)
{

}

void TmainScore::setKeyViewBg(QColor C)
{

}

void TmainScore::setNoteViewBg(int id, QColor C)
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
	if (gl->Gtune() != Ttune::stdTune)
			setScordature();
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
}











