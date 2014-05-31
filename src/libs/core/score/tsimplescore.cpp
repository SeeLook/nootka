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
#include <score/tscorenote.h>
#include <score/tscorekeysignature.h>
#include <score/tscoreclef.h>
#include "tscoreview.h"
#include "tnotecontrol.h"
#include <music/tinstrument.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>
#include <QLayout>
#include <QScrollBar>
#include <QGraphicsView>

#include <QDebug>

TsimpleScore::TsimpleScore(int notesNumber, QWidget* parent) :
  QWidget(parent),
  m_bgGlyph(0),
	m_notesNr(notesNumber),
	layoutHasControl(false),
	m_prevBGglyph(-1)
{
  QHBoxLayout *lay = new QHBoxLayout;
  m_score = new TscoreView(this);
   
#if !defined (Q_OS_ANDROID)
  m_score->setMouseTracking(true);
#endif
  m_score->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	m_score->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_score->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_score->setFrameShape(QFrame::NoFrame);
  
  m_scene = new TscoreScene(m_score);
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
  m_score->setScene(m_scene);
  
  m_staff = new TscoreStaff(m_scene, m_notesNr);
	m_staff->enableToAddNotes(false);
	m_clefType = m_staff->scoreClef()->clef().type();
	connect(m_staff, SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
	connect(m_staff, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
  
	lay->addWidget(m_score);
  setLayout(lay);
	
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
		return *(staff()->getNote(index));
	else
		return Tnote();
}


void TsimpleScore::setNote(int index, Tnote note) {
		staff()->setNote(index, note);
		if (staff()->noteSegment(index)->pos().x() * m_score->transform().m11() > m_score->width() * 0.75)
				m_score->centerOn(staff()->noteSegment(index)->mapToScene(staff()->noteSegment(index)->pos()));
}


void TsimpleScore::clearNote(int index) {
	staff()->noteSegment(index)->markNote(-1);
	setNote(index, Tnote(0, 0, 0));
}


void TsimpleScore::setStringNumber(int index, int realNr) {
	if (index >= 0 && index < m_notesNr)
		staff()->noteSegment(index)->setString(realNr);
}


void TsimpleScore::clearStringNumber(int index) {
	if (index >= 0 && index < m_notesNr)
			staff()->noteSegment(index)->removeString();
}


void TsimpleScore::setClef(Tclef clef) {
	if (this->clef().type() != clef.type()) {
		staff()->onClefChanged(clef);
		m_clefType = clef.type();
	}
}


Tclef TsimpleScore::clef() {
	if (isPianoStaff())
			return Tclef(Tclef::e_pianoStaff);
	else if (staff()->scoreClef())
			return staff()->scoreClef()->clef();
	else
			return Tclef(Tclef::e_none);
}


void TsimpleScore::setClefDisabled(bool isDisabled) {
	if (staff()->scoreClef()) {
		if (isDisabled)
				staff()->scoreClef()->setReadOnly(true);
		else
			staff()->scoreClef()->setReadOnly(false);
	}
}


void TsimpleScore::setKeySignature(TkeySignature keySign) {
	if (staff()->scoreKey())
		staff()->scoreKey()->setKeySignature(keySign.value());
}


TkeySignature TsimpleScore::keySignature() {
	TkeySignature key(0);
	if (staff()->scoreKey())
		key = TkeySignature(staff()->scoreKey()->keySignature());
	return key;
}


void TsimpleScore::setEnableKeySign(bool isEnabled) {
	if (isEnabled != (bool)staff()->scoreKey()) {
		staff()->setEnableKeySign(isEnabled);
		if (isEnabled)
				staff()->scoreKey()->showKeyName(true);
		resizeEvent(0);
	}
}


void TsimpleScore::setEnabledDblAccid(bool isEnabled) {
	m_scene->setDoubleAccidsEnabled(isEnabled);
	if (staff()->noteSegment(0) && staff()->noteSegment(0)->left())
		staff()->noteSegment(0)->left()->addAccidentals();
}


void TsimpleScore::setNoteDisabled(int index, bool isDisabled) {
	staff()->noteSegment(index)->setReadOnly(isDisabled);
}


bool TsimpleScore::isNoteDisabled(int index) {
	return staff()->noteSegment(index)->isReadOnly();
}


void TsimpleScore::setScoreDisabled(bool disabled) {
	staff()->setDisabled(disabled);
  setAttribute(Qt::WA_TransparentForMouseEvents, disabled);
}


void TsimpleScore::setNoteNameEnabled(bool nameEnabled) {
	staff()->noteSegment(0)->right()->enableNoteName(nameEnabled);
}


void TsimpleScore::setAmbitus(int index, Tnote lo, Tnote hi) {
	if (index >= 0 && index < m_notesNr)
			staff()->noteSegment(index)->setAmbitus((staff()->noteToPos(lo)) + 1, (staff()->noteToPos(hi)) + 1);
}


void TsimpleScore::setAmbitus(Tnote lo, Tnote hi) {
		for (int i = 0; i < m_notesNr; i++)
			setAmbitus(i, lo, hi);
}

/** !!!!All values are hard coded */
Tnote TsimpleScore::lowestNote() {
	if (staff()->isPianoStaff())
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
	if (staff()->isPianoStaff())
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
	m_bgGlyph->setParentItem(staff());
	m_bgGlyph->setFont(TnooFont());
	QColor bgColor = palette().highlight().color();
	bgColor.setAlpha(75);
	m_bgGlyph->setBrush(bgColor);
	qreal factor = (staff()->height() / m_bgGlyph->boundingRect().height());
	m_bgGlyph->setScale(factor);
	m_bgGlyph->setPos(/*(staff()->width() - m_bgGlyph->boundingRect().width() * factor) / 2*/ 12.0, 
									(staff()->height() - m_bgGlyph->boundingRect().height() * factor) / 2);
	m_bgGlyph->setZValue(1);
	
}


void TsimpleScore::noteWasClicked(int index) {
	Tnote note = *(staff()->getNote(index));
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
	qreal staffOff = 1.0;
  if (staff()->isPianoStaff())
    staffOff = 2.0;
  qreal factor = ((qreal)hh / (staff()->height() + 4.0)) / m_score->transform().m11();
// 	qreal factor = ((qreal)hh / (staff()->height() + 2.0)) * m_pianoFactor;
// 	qreal factor = (qreal)m_score->frameRect().height() / (m_scene->sceneRect().height() * m_score->transform().m11());
  m_score->scale(factor, factor);
	staff()->setExternalWidth((score()->width()) / score()->transform().m11() - (1.0 + staffOff));
	if (m_score->horizontalScrollBar()->isVisible()) {
		m_score->horizontalScrollBar()->setValue(scrollV);
	}
	staff()->setPos(staffOff, 0.05);
	staff()->updateSceneRect();
}


bool TsimpleScore::isPianoStaff() {
	return staff()->isPianoStaff(); 
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
	if ((m_clefType == Tclef::e_pianoStaff && clef.type() != Tclef::e_pianoStaff) ||
			(m_clefType != Tclef::e_pianoStaff && clef.type() == Tclef::e_pianoStaff)	)
					resizeEvent(0);
	m_clefType = clef.type();
}







