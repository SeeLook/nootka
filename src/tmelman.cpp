/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tmelman.h"
#include "score/tmainscore.h"
#include <exam/tqaunit.h>
#include <music/tmelody.h>
#include "exam/trandmelody.h"
#include <QtWidgets>

TmelMan::TmelMan(TmainScore* score, const QString& path) :
	QObject(score),
	m_score(score),
	m_path(path)
{	
	m_menu = new QMenu(score);
	m_playMelAct = createAction(tr("Play"), SLOT(playMelodySlot()), QKeySequence(Qt::Key_Space),
							 QIcon(score->style()->standardIcon(QStyle::SP_MediaPlay)));
	m_recMelAct = createAction(tr("Record"), SLOT(recordMelodySlot()), QKeySequence("Alt+Space"),
							 QIcon(m_path + "picts/record.png"));
	createAction(tr("Create"), SLOT(randomizeMelodySlot()));
	
	
	m_button = new QToolButton(score);
	m_button->setIcon(QIcon(path + "picts/melody.png"));
	m_button->setText(tr("Melody"));
	m_button->setStatusTip(tr("Open, save, generate and play a melody."));
	m_button->setMenu(m_menu);
	m_button->setPopupMode(QToolButton::InstantPopup);
	m_button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_melAct = new QWidgetAction(score);
	m_melAct->setDefaultWidget(m_button);
	connect(m_score, SIGNAL(playbackFinished()), this, SLOT(playMelodySlot()));
}
//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmelMan::playMelodySlot() {
	if (m_score->isScorePlayed()) {
		m_score->playScore(); // It will be stopped
		m_recMelAct->setDisabled(false);
		m_playMelAct->setIcon(QIcon(m_score->style()->standardIcon(QStyle::SP_MediaPlay)));
	} else {
		if (m_score->currentIndex() < 0)
			return;
		m_recMelAct->setDisabled(true);
		m_playMelAct->setIcon(QIcon(m_score->style()->standardIcon(QStyle::SP_MediaStop)));
		m_score->playScore();
	}
}


void TmelMan::recordMelodySlot() {
	if (m_score->insertMode() == TmainScore::e_multi) {
		m_recMelAct->setIcon(QIcon(m_score->style()->standardIcon(QStyle::SP_MediaStop)));
		m_score->setInsertMode(TmainScore::e_record);
	} else {
		m_recMelAct->setIcon(QIcon(m_path + "picts/record.png"));
		m_score->setInsertMode(TmainScore::e_multi);
	}
}


void TmelMan::randomizeMelodySlot() {
	QList<TQAunit::TQAgroup> ql;
	int ambit = 24; //highestNote().getChromaticNrOfNote() - lowestNote().getChromaticNrOfNote();
	for (int i = 0; i < ambit; i++) {
		TQAunit::TQAgroup qa;
		qa.note = Tnote(1 + i);
		ql << qa;
	}
	Tmelody *mel = new Tmelody("", m_score->keySignature());
	getRandomMelody(ql, mel, 14, false, true);
	m_score->setMelody(mel);
	delete mel;
}



//####################################################################################################
//########################################## PRIVATE #################################################
//####################################################################################################
QAction* TmelMan::createAction(const QString& t, const char* slot, const QKeySequence& k, const QIcon& i) {
	QAction *a = new QAction(this);
	if (!i.isNull())
		a->setIcon(i);
	if (!k.isEmpty())
		a->setShortcut(k);
	a->setText(t);
	connect(a, SIGNAL(triggered()), this, slot);
	m_menu->addAction(a);
	return a;
}






