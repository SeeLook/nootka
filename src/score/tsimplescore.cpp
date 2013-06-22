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


#include "tsimplescore.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include "tscorecontrol.h"
#include "tscorenote.h"
#include "tscorekeysignature.h"
#include "tscoreclef.h"
#include "tscorepianostaff.h"
#include "tscoreview.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>

#include <QDebug>

TsimpleScore::TsimpleScore(int notesNumber, QWidget* parent, bool controler) :
  QWidget(parent),
	m_isPianoStaff(false),
	m_notesNr(notesNumber),
	m_scoreControl(0),
	m_pianoFactor(1.0)
{
  QHBoxLayout *lay = new QHBoxLayout;
  m_score = new TscoreView(this);
  lay->addWidget(m_score);
   
  m_score->setMouseTracking(true);
  m_score->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	m_score->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_score->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_score->setFrameShape(QFrame::NoFrame);
  
  m_scene = new TscoreScene(m_score);
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
  m_score->setScene(m_scene);
  
  m_staff = new TscoreStaff(m_scene, m_notesNr, TscoreStaff::e_normal);
	connect(m_staff, SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
	connect(m_staff, SIGNAL(pianoStaffSwitched(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
  
	if (controler) {
			m_scoreControl = new TscoreControl(this);
// 			lay->addSpacing(4);
			lay->addWidget(m_scoreControl);
	}
  setLayout(lay);

	m_staff->setScoreControler(m_scoreControl);
	
	setBGcolor(palette().base().color());
	setEnabledDblAccid(false);
	resizeEvent(0);
  
}

TsimpleScore::~TsimpleScore() {}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

Tnote TsimpleScore::getNote(int index) {
	if (index >= 0 && index < m_notesNr)
		return *(m_staff->getNote(index));
	else
		return Tnote();
}


void TsimpleScore::setNote(int index, Tnote note) {
		m_staff->setNote(index, note);
}


void TsimpleScore::clearNote(int index) {
	m_staff->noteSegment(index)->markNote(-1);
//   m_staff->noteSegment(index)->hideNote();
  setNote(index, Tnote(0, 0, 0));
	if (m_staff->lower()) {
		m_staff->lower()->noteSegment(index)->markNote(-1);
	}
}


void TsimpleScore::setStringNumber(int index, int realNr) {
	if (index >= 0 && index < m_notesNr)
		m_staff->noteSegment(index)->setString(realNr);
}


void TsimpleScore::clearStringNumber(int index) {
	if (index >= 0 && index < m_notesNr)
			m_staff->noteSegment(index)->removeString();
}


void TsimpleScore::setClef(Tclef clef) {
	if (m_staff->scoreClef()) {
		m_staff->scoreClef()->setClef(clef);
    m_staff->onClefChanged();
  }
}


Tclef TsimpleScore::clef() {
	if (isPianoStaff())
		return Tclef(Tclef::e_pianoStaff);
	else if (m_staff->scoreClef())
					return m_staff->scoreClef()->clef();
			else 
					return Tclef(Tclef::e_none);
}


void TsimpleScore::setClefDisabled(bool isDisabled) {
	if (m_staff->scoreClef()) {
		if (isDisabled) {
			m_staff->scoreClef()->setReadOnly(true);
			m_staff->scoreClef()->setIsClickable(false);
		} else {
			m_staff->scoreClef()->setReadOnly(false);
			m_staff->scoreClef()->setIsClickable(true);
		}
	}
}



void TsimpleScore::setKeySignature(TkeySignature keySign) {
	if (m_staff->scoreKey())
		m_staff->scoreKey()->setKeySignature(keySign.value());
}


TkeySignature TsimpleScore::keySignature() {
	TkeySignature key(0);
	if (m_staff->scoreKey())
		key = TkeySignature(m_staff->scoreKey()->keySignature());
	return key;
}


void TsimpleScore::setEnableKeySign(bool isEnabled) {
	if (isEnabled != (bool)m_staff->scoreKey()) {
		m_staff->setEnableKeySign(isEnabled);
		if (isEnabled)
				m_staff->scoreKey()->showKeyName(true);
		resizeEvent(0);
	}
}


void TsimpleScore::setEnabledDblAccid(bool isEnabled) {
	if (m_scoreControl)
		m_scoreControl->enableDoubleAccidentals(isEnabled);
	m_scene->setDoubleAccidsEnabled(isEnabled);
}


void TsimpleScore::setPianoStaff(bool isPiano) {
	if (isPiano != isPianoStaff()) {
		bool keyEnabled = (bool)m_staff->scoreKey();
		char key = 0;
		if (keyEnabled)
			key = m_staff->scoreKey()->keySignature();
		if (isPiano) {
				m_isPianoStaff = true;
				delete m_staff;
				m_staff = new TscorePianoStaff(m_scene, m_notesNr);
				m_staff->setScoreControler(m_scoreControl);
				m_pianoFactor = 0.80;
		} else {
				m_isPianoStaff = false;
				delete m_staff;
				m_staff = new TscoreStaff(m_scene, m_notesNr, TscoreStaff::e_normal);
				m_staff->setScoreControler(m_scoreControl);
				m_pianoFactor = 1.0;
		}
		if (keyEnabled) {
				m_staff->setEnableKeySign(true);
        m_staff->scoreKey()->showKeyName(true);
				m_staff->scoreKey()->setKeySignature(key);
		}
		connect(m_staff, SIGNAL(pianoStaffSwitched(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
		connect(m_staff, SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
		resizeEvent(0);
	}
}


void TsimpleScore::setNoteDisabled(int index, bool isDisabled) {
	m_staff->noteSegment(index)->setReadOnly(isDisabled);
}



void TsimpleScore::setScoreDisabled(bool disabled) {
// 	m_score->setMouseTracking(!disabled);
	m_staff->noteSegment(0)->hideWorkNote();
	if (m_scoreControl)
		m_scoreControl->setDisabled(disabled);
	m_staff->setDisabled(disabled);
}


//##########################################################################################################
//########################################## PUBLIC SLOTS ##################################################
//##########################################################################################################

void TsimpleScore::noteWasClicked(int index) {
	Tnote note = *(m_staff->getNote(index));
	emit noteHasChanged(index, note);
}




//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################


void TsimpleScore::resizeEvent(QResizeEvent* event) {
  qreal factor = (((qreal)height() / 40.0) / m_score->transform().m11()) * m_pianoFactor;
  m_score->scale(factor, factor);
	m_scene->setSceneRect(0, 0, m_staff->boundingRect().width() * m_score->transform().m11(), 
												m_staff->boundingRect().height() * m_score->transform().m11()	);
	m_score->setMaximumSize(m_scene->sceneRect().width(), m_scene->sceneRect().height());
  qreal staffOff = 0.0;
  if (isPianoStaff())
    staffOff = m_score->transform().m11() * 5;
	m_staff->setPos(m_score->mapToScene(staffOff, 0));
	int xOff = 0;
		if (m_scoreControl)
			xOff = m_scoreControl->width() + 10; // 15 is space between m_scoreControl and m_score - looks good
	setMaximumWidth(m_scene->sceneRect().width() + xOff);
}


void TsimpleScore::switchToPianoStaff(Tclef clef) {
// staff will be deleted so let's store its notes
	QList<Tnote> tmpList;
	for (int i = 0; i < m_notesNr; i++)
		tmpList << *(m_staff->getNote(i));
// Key signature is restored in setPianoStaff method
	if (isPianoStaff() && clef.type() != Tclef::e_pianoStaff) {
		setPianoStaff(false);
		m_staff->scoreClef()->setClef(clef);
		m_staff->onClefChanged(); // refresh note offset for selected staff
	}
	if (!isPianoStaff() && clef.type() == Tclef::e_pianoStaff)
		setPianoStaff(true);
	// restore notes
	for (int i = 0; i < m_notesNr; i++)
		if(tmpList[i].note)
				setNote(i, tmpList[i]);
	emit pianoStaffSwitched();
}


void TsimpleScore::statusTipChanged(QString status) {
	QStatusTipEvent *tipEvent = new QStatusTipEvent(status);
	qApp->postEvent(parent(), tipEvent);
}


void TsimpleScore::setBGcolor(QColor bgColor) {
	m_score->setStyleSheet(QString("border: 1px solid palette(Text); border-radius: 10px; background-color: rgba(%1, %2, %3, 220)")
		.arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()) );
}


//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################







