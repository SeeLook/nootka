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


#include "tsimplescore.h"
#include <score/tscorescene.h>
#include <score/tscorestaff.h>
#include <score/tscorecontrol.h>
#include <score/tscorenote.h>
#include <score/tscorekeysignature.h>
#include <score/tscoreclef.h>
#include <score/tscorepianostaff.h>
#include <score/tscoreview.h>
#include <music/tinstrument.h>
#include <tcolor.h>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>
#include <QLayout>
#include <QScrollBar>

#include <QDebug>

TsimpleScore::TsimpleScore(int notesNumber, QWidget* parent, bool controler) :
  QWidget(parent),
	m_isPianoStaff(false),
	m_notesNr(notesNumber),
	m_scoreControl(0),
	m_pianoFactor(1.0),
	m_bgGlyph(0),
	layoutHasControl(false)
{
  QHBoxLayout *lay = new QHBoxLayout;
  m_score = new TscoreView(this);
   
//   m_score->setMouseTracking(true);
//   m_score->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
// 	m_score->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//   m_score->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//   m_score->setFrameShape(QFrame::NoFrame);
  
//   m_scene = new TscoreScene(m_score);
	m_scene = m_score->scoreScene();
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
//   m_score->setScene(m_scene);
  
  m_staff = new TscoreStaff(m_scene, m_notesNr, TscoreStaff::e_normal);
	m_score->setStaff(m_staff);
	connect(m_staff, SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
	connect(m_staff, SIGNAL(pianoStaffSwitched(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
	connect(m_staff, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
  
	lay->addWidget(m_score);
	if (controler) {
			m_scoreControl = new TscoreControl(this);
			lay->addWidget(m_scoreControl, 0, Qt::AlignRight);
			layoutHasControl = true;
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
	if (m_staff->lower()) {
		m_staff->lower()->noteSegment(index)->markNote(-1);
	}
	setNote(index, Tnote(0, 0, 0));
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
	if (this->clef().type() != clef.type()) {
		if (clef.type() == Tclef::e_pianoStaff)
			setPianoStaff(true);
		else {
			if (isPianoStaff())
				setPianoStaff(false);
			m_staff->scoreClef()->setClef(clef);
		}
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
			if (m_staff->lower()) {
					m_staff->lower()->scoreClef()->setReadOnly(true);
			}
		} else {
			m_staff->scoreClef()->setReadOnly(false);
			if (m_staff->lower()) {
					m_staff->lower()->scoreClef()->setReadOnly(false);
			}
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

int m_prevBGglyph = -1;
void TsimpleScore::setPianoStaff(bool isPiano) {
	if (isPiano != isPianoStaff()) {
		bool keyEnabled = (bool)m_staff->scoreKey();
		char key = 0;
		bool disNotes[m_notesNr];
		for (int i = 0; i < m_notesNr; i++)
			disNotes[i] = isNoteDisabled(i);
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
		if (m_bgGlyph) {
			m_bgGlyph = 0; // it was deleted with staff
			addBGglyph(m_prevBGglyph);
		}
		for (int i = 0; i < m_notesNr; i++)
			setNoteDisabled(i, disNotes[i]);
		connect(m_staff, SIGNAL(pianoStaffSwitched(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
		connect(m_staff, SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
		connect(m_staff, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
// 		updateGeometry();
		resizeEvent(0);
	}
}


void TsimpleScore::setNoteDisabled(int index, bool isDisabled) {
	m_staff->noteSegment(index)->setReadOnly(isDisabled);
	if (m_staff->lower())
			m_staff->lower()->noteSegment(index)->setReadOnly(isDisabled);
}


bool TsimpleScore::isNoteDisabled(int index) {
	return m_staff->noteSegment(index)->isReadOnly();
}


void TsimpleScore::setScoreDisabled(bool disabled) {
	if (m_scoreControl)
		m_scoreControl->setDisabled(disabled);
	m_staff->setDisabled(disabled);
  setAttribute(Qt::WA_TransparentForMouseEvents, disabled);
}


void TsimpleScore::setAmbitus(int index, Tnote lo, Tnote hi) {
	if (index >= 0 && index < m_notesNr) {
		if (staff()->lower()) {
				staff()->noteSegment(index)->setAmbitus(staff()->height() - 2, staff()->noteToPos(hi) + 1);
				staff()->lower()->noteSegment(index)->setAmbitus(staff()->lower()->noteToPos(lo) + 1, 2);
		} else
				staff()->noteSegment(index)->setAmbitus(staff()->noteToPos(lo) + 1, staff()->noteToPos(hi) + 1);
	}
}


void TsimpleScore::setAmbitus(Tnote lo, Tnote hi) {
		for (int i = 0; i < m_notesNr; i++)
			setAmbitus(i, lo, hi);
}

/** !!!!All values are hard coded */
Tnote TsimpleScore::lowestNote() {
	if (staff()->lower()) // piano staff for sure
			return Tnote(4, -2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G)
			return Tnote(6, -1);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G_8down)
			return Tnote(6, -2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F)
			return Tnote(6, -2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F_8down)
			return Tnote(1, -3);
	if (staff()->scoreClef()->clef().type() == Tclef::e_alto_C)
			return Tnote(7, -2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_tenor_C)
			return Tnote(5, -2);
	qDebug() << "lowestNote() riches end of method without result";
	return Tnote(6, -2); // It should never happened
}


Tnote TsimpleScore::highestNote() {
	if (staff()->lower()) // piano staff for sure
		return Tnote(1, 4);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G)
		return Tnote(4, 4);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G_8down)
		return Tnote(4, 3);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F)
		return Tnote(6, 2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F_8down)
		return Tnote(6, 1);
	if (staff()->scoreClef()->clef().type() == Tclef::e_alto_C)
		return Tnote(5, 3);
	if (staff()->scoreClef()->clef().type() == Tclef::e_tenor_C)
		return Tnote(3, 3);
	qDebug() << "highestNote() riches end of method without result";
	return Tnote(4, 4);
}


void TsimpleScore::addBGglyph(int instr) {
	if (instr < 0 || instr > 3)
			return;
	m_prevBGglyph = instr;
	if (m_bgGlyph)
		delete m_bgGlyph;
	m_bgGlyph = new QGraphicsSimpleTextItem(instrumentToGlyph(Einstrument(instr)));
	m_bgGlyph->setFont(QFont("nootka", 20, QFont::Normal));
	QColor bgColor = palette().highlight().color();
	bgColor.setAlpha(75);
	m_bgGlyph->setBrush(bgColor);
	m_bgGlyph->setParentItem(m_staff);
	qreal factor = (m_staff->boundingRect().height() / m_bgGlyph->boundingRect().height());
	m_bgGlyph->setScale(factor);
	m_bgGlyph->setPos((m_staff->boundingRect().width() - m_bgGlyph->boundingRect().width() * factor) / 2, 
									(m_staff->boundingRect().height() - m_bgGlyph->boundingRect().height() * factor) / 2);
	m_bgGlyph->setZValue(1);
	
}

/*
int TsimpleScore::heightForWidth(int w) const {
	int xOff = 0;
	if (m_scoreControl && layoutHasControl)
			xOff = m_scoreControl->width() + 10;
	if (w < xOff)
			return -1;
	qreal styleOff = 0.0; // some styles quirks - it steals some space
  if (style()->objectName() == "bespin" || style()->objectName() == "windowsvista" || style()->objectName() == "plastique")
			styleOff = 1.0;
	qDebug() << "preferred H:" << ((w - xOff) / (m_staff->boundingRect().width() + styleOff)) * m_staff->boundingRect().height() <<
			"for W:" << w;
	return ((w - xOff) / (m_staff->boundingRect().width() + styleOff)) * m_staff->boundingRect().height();
}


QSize TsimpleScore::sizeHint() const {
// 	int xOff = 0;
// 	if (m_scoreControl && layoutHasControl)
// 			xOff = m_scoreControl->width() + 10; // 10 is space between m_scoreControl and m_score - looks good
// 	return QSize(m_scene->sceneRect().width() + xOff, heightForWidth(m_scene->sceneRect().width() + xOff));
//   return QSize(m_scene->sceneRect().width() + xOff, m_scene->sceneRect().height());
	return QSize(-1, -1);
}
*/

//##########################################################################################################
//########################################## PUBLIC SLOTS ##################################################
//##########################################################################################################

void TsimpleScore::noteWasClicked(int index) {
	Tnote note = *(m_staff->getNote(index));
	emit noteWasChanged(index, note);
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TsimpleScore::resizeEvent(QResizeEvent* event) {
	int hh = height(), ww = width();
	if (event) {
		hh = event->size().height();
		ww = event->size().width();
	}
	int scrollV;
	if (m_score->horizontalScrollBar()->isVisible()) {
		hh -= m_score->horizontalScrollBar()->height();
		scrollV = m_score->horizontalScrollBar()->value();
	}
	qreal styleOff = 1.0; // some styles quirks - it steals some space
  if (style()->objectName() == "oxygen" || style()->objectName() == "oxygen transparent" || style()->objectName() == "qtcurve")
			styleOff = 0.0;
  qreal factor = (((qreal)hh / 42.0) / m_score->transform().m11()) * m_pianoFactor;
// 	qreal factor = (qreal)m_score->frameRect().height() / (m_scene->sceneRect().height() * m_score->transform().m11());
  m_score->scale(factor, factor);
	staff()->setExternalWidth((score()->width()) / score()->transform().m11() - 2.0);
// 	qDebug() << m_scene->sceneRect() << m_scene->itemsBoundingRect();
// 	m_score->setSceneRect(0, 0, (m_staff->boundingRect().width() + styleOff) * factor, 
// 												m_staff->boundingRect().height() * factor	);
	m_scene->setSceneRect(0.0, -factor, m_scene->itemsBoundingRect().width(), m_scene->itemsBoundingRect().height());
// 	m_scene->setSceneRect(0, 0, (m_staff->boundingRect().width() + styleOff) * factor, 
// 												m_staff->boundingRect().height() * factor);
// 	qDebug() << m_scene->sceneRect() << m_scene->itemsBoundingRect();
// 	m_score->setMaximumSize(m_scene->sceneRect().width(), m_scene->sceneRect().height() / m_pianoFactor);
//   m_score->setMinimumSize(m_scene->sceneRect().width(), m_scene->sceneRect().height());
	if (m_score->horizontalScrollBar()->isVisible()) {
		m_score->horizontalScrollBar()->setValue(scrollV);
	}
  qreal staffOff = 1.0;
  if (isPianoStaff())
    staffOff = 2.0;
// 	m_staff->setPos(m_score->mapToScene(staffOff, 0));
	m_staff->setPos(staffOff, 0.0);
// 	int xOff = 0;
// 	if (m_scoreControl && layoutHasControl)
// 			xOff = m_scoreControl->width() + 10; // 10 is space between m_scoreControl and m_score - looks good
// 	setFixedWidth(m_scene->sceneRect().width() + xOff);
//   setMinimumWidth(m_scene->sceneRect().width() + xOff);
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
	resizeEvent(0);
}


void TsimpleScore::statusTipChanged(QString status) {
	QStatusTipEvent *tipEvent = new QStatusTipEvent(status);
	qApp->postEvent(parent(), tipEvent);
	emit statusTip(status);
}


void TsimpleScore::setBGcolor(QColor bgColor) {
	bgColor.setAlpha(220);
	m_score->setStyleSheet(QString("border: 1px solid palette(Text); border-radius: 10px; %1").arg(Tcolor::bgTag(bgColor)));
}


void TsimpleScore::onClefChanged(Tclef clef) {
	if (isPianoStaff())
		emit clefChanged(Tclef(Tclef::e_pianoStaff));
	else
		emit clefChanged(staff()->scoreClef()->clef());
}



//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################







