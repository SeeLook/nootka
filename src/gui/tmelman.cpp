/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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
#include "tmenu.h"
#include "score/tmainscore.h"
#include <exam/tqagroup.h>
#include <music/tmelody.h>
#include <tpath.h>
#include "exam/trandmelody.h"
#include <QtWidgets>

TmelMan::TmelMan(TmainScore* score) :
	QObject(score),
	m_score(score)
{	
	m_menu = new Tmenu(score);
	m_playMelAct = createAction(tr("Play"), SLOT(playMelodySlot()), QKeySequence(Qt::Key_Space),
							 QIcon(score->style()->standardIcon(QStyle::SP_MediaPlay)));
	m_recMelAct = createAction(tr("Record"), SLOT(recordMelodySlot()), QKeySequence("Ctrl+Space"),
							 QIcon(Tpath::img("record")));
	createAction(tr("Create"), SLOT(randomizeMelodySlot()), QKeySequence(), QIcon(Tpath::img("melody")));
	m_menu->addSeparator();
	QDialogButtonBox box;
	QPushButton *saveBut = box.addButton(QDialogButtonBox::Save);
	QPushButton *openBut = box.addButton(QDialogButtonBox::Open);
	createAction(openBut->text(), SLOT(loadMelodySlot()), QKeySequence::Open, 
							 QIcon(score->style()->standardIcon(QStyle::SP_DialogOpenButton)));
	createAction(saveBut->text(), SLOT(saveMelodySlot()), QKeySequence::Save, 
							 QIcon(score->style()->standardIcon(QStyle::SP_DialogSaveButton)));
	
	
	m_button = new QToolButton(score);
	m_button->setIcon(QIcon(Tpath::img("melody")));
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
		if (m_score->insertMode() == TmultiScore::e_record)
			m_button->setIcon(QIcon(Tpath::img("melody-rec")));
		else
			m_button->setIcon(QIcon(Tpath::img("melody")));
	} else {
		if (m_score->currentIndex() < 0) {
      if (m_score->notesCount() > 0)
        m_score->selectNote(0); // start playing from the first note
      else
        return;
    }
		m_recMelAct->setDisabled(true);
		m_playMelAct->setIcon(QIcon(m_score->style()->standardIcon(QStyle::SP_MediaStop)));
		m_score->playScore();
		m_button->setIcon(QIcon(Tpath::img("melody-play")));
	}
}


void TmelMan::recordMelodySlot() {
	if (m_score->insertMode() == TmainScore::e_multi) {
		m_recMelAct->setIcon(QIcon(m_score->style()->standardIcon(QStyle::SP_MediaStop)));
		m_score->setInsertMode(TmainScore::e_record);
		m_button->setIcon(QIcon(Tpath::img("melody-rec")));
	} else {
		m_recMelAct->setIcon(QIcon(Tpath::img("record")));
		m_score->setInsertMode(TmainScore::e_multi);
		if (!m_score->isScorePlayed())
			m_button->setIcon(QIcon(Tpath::img("melody")));
	}
}


void TmelMan::randomizeMelodySlot() {
	QList<TQAgroup> ql;
	int ambit = 25; //highestNote().chromatic() - lowestNote().chromatic();
	for (int i = 0; i < ambit; i++) {
		TQAgroup qa;
		qa.note = Tnote(1 + i);
		ql << qa;
	}
	Tmelody *mel = new Tmelody("", m_score->keySignature());
  mel->setClef(m_score->clef().type());
	getRandomMelody(ql, mel, 14, true, true);
	m_score->setMelody(mel);
	delete mel;
}


void TmelMan::loadMelodySlot() {
	QString melodyFile = QFileDialog::getOpenFileName(0, tr("Open melody file"), "",
                         tr("MusicXML file") + " (*.xml)", 0, QFileDialog::DontUseNativeDialog);
	if (!melodyFile.isEmpty()) {
		Tmelody *mel = new Tmelody();
		if (mel->grabFromMusicXml(melodyFile)) {
			m_score->setMelody(mel);
		}
		delete mel;
	}
}



void TmelMan::saveMelodySlot() {
	QString melodyFile = QFileDialog::getSaveFileName(0, tr("Save melody as:"), "",
                         tr("MusicXML file") + " (*.xml)", 0, QFileDialog::DontUseNativeDialog);
	if (!melodyFile.isEmpty()) {
		if (melodyFile.right(4) != ".xml")
        melodyFile += ".xml";
		Tmelody *mel = new Tmelody();
		m_score->getMelody(mel, "some melody");
		mel->saveToMusicXml(melodyFile);
		delete mel;
	}
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






