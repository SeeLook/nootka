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
#include <QDebug>
#include <QGraphicsView>
#include <QHBoxLayout>

TsimpleScore::TsimpleScore(QWidget* parent) :
  QWidget(parent),
  m_lowerStaff(0)
{
//   setGeometry(parent->geometry());
  QHBoxLayout *lay = new QHBoxLayout;
  m_score = new QGraphicsView(this);
  lay->addWidget(m_score);
  
  
  m_score->setMouseTracking(true);
  m_score->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  
  m_scene = new TscoreScene(m_score);
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
  m_score->setScene(m_scene);
  
  m_staff = new TscoreStaff(m_scene, 3, TscoreStaff::e_normal);
	m_staff->noteSegment(2)->setReadOnly(true);
  
  m_scoreControl = new TscoreControl(this);
  lay->addWidget(m_scoreControl);
  setLayout(lay);

	m_staff->setScoreControler(m_scoreControl);
	connect(m_staff, SIGNAL(pianoStaffSwitch(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
  
  
}

TsimpleScore::~TsimpleScore()
{}

qreal m_pianoFactor = 1.0;
void TsimpleScore::setPianoStaff(bool isPiano) {
	if (isPiano != isPianoStaff()) {
		bool keyEnabled = (bool)m_staff->scoreKey();
		if (isPiano) { // There was a singe staff and we add one staff more
				delete m_staff;
				m_staff = new TscoreStaff(m_scene, 3, TscoreStaff::e_upper);
				m_staff->setScoreControler(m_scoreControl);
// 				m_staff->setPos(0, 0);
				m_lowerStaff = new TscoreStaff(m_scene, 3, TscoreStaff::e_lower);
				m_lowerStaff->setPos(0, m_staff->boundingRect().height());
				m_lowerStaff->setScoreControler(m_scoreControl);
				if (keyEnabled) {
						m_lowerStaff->setEnableKeySign(true);
						connectToKeyChangedSlot(m_lowerStaff);
				}
				connect(m_lowerStaff, SIGNAL(pianoStaffSwitch(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
				m_pianoFactor = 0.95;
				resizeEvent(0);
		} else {
				delete m_lowerStaff;
				m_lowerStaff = 0;
				delete m_staff;
				m_staff = new TscoreStaff(m_scene, 3, TscoreStaff::e_normal);
				m_staff->setScoreControler(m_scoreControl);
				m_pianoFactor = 1.0;
				resizeEvent(0);
// 				m_staff->setPos(0, 0);
		}
		if (keyEnabled) {
				m_staff->setEnableKeySign(true);
				m_staff->scoreKey()->showKeyName(true);
				if (isPianoStaff())
						connectToKeyChangedSlot(m_staff);
		}
		connect(m_staff, SIGNAL(pianoStaffSwitch(Tclef)), this, SLOT(switchToPianoStaff(Tclef)));
	}
}


void TsimpleScore::setEnableKeySign(bool isEnabled) {
	if (isEnabled != (bool)m_staff->scoreKey()) {
		m_staff->setEnableKeySign(isEnabled);
		if (isEnabled) {
				m_staff->scoreKey()->showKeyName(true);
				if (m_lowerStaff) // only for piano staves
					connectToKeyChangedSlot(m_staff);
		}
		if (m_lowerStaff) {
			m_lowerStaff->setEnableKeySign(isEnabled);
			if (isEnabled) {
					m_lowerStaff->scoreKey()->showKeyName(false);
					connectToKeyChangedSlot(m_lowerStaff);
			}
		}
	}
}



//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################


int TsimpleScore::heightForWidth(int w ) const {
  return w * 8;
}


void TsimpleScore::resizeEvent(QResizeEvent* event) {
  qreal factor = ((qreal)height() / 40.0) / m_score->transform().m11();
//   factor = factor / 3;
  m_score->scale(factor * m_pianoFactor, factor * m_pianoFactor);
}


void TsimpleScore::keyChangedInPianoStaff() {
	if (sender() == m_staff->scoreKey()) {
		connectToKeyChangedSlot(m_lowerStaff, false);
		m_lowerStaff->scoreKey()->setKeySignature(m_staff->scoreKey()->keySignature());
		connectToKeyChangedSlot(m_lowerStaff);
	}	else if ( sender() == m_lowerStaff->scoreKey()) {
				connectToKeyChangedSlot(m_staff, false);
				m_staff->scoreKey()->setKeySignature(m_lowerStaff->scoreKey()->keySignature());
				connectToKeyChangedSlot(m_staff);
	}
}


void TsimpleScore::switchToPianoStaff(Tclef clef) {
	if (isPianoStaff() && clef.type() != Tclef::e_pianoStaff) {
		setPianoStaff(false);
		m_staff->scoreClef()->setClef(clef);
	}
	if (!isPianoStaff() && clef.type() == Tclef::e_pianoStaff)
		setPianoStaff(true);
}


//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

void TsimpleScore::connectToKeyChangedSlot(TscoreStaff* staff, bool join) {
	if (join)
		connect(staff->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedInPianoStaff()));
	else
		disconnect(staff->scoreKey(), SIGNAL(keySignatureChanged()), this, SLOT(keyChangedInPianoStaff()));
}






