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

#include "ttoolbar.h"
#include <help/texamhelp.h>
#include <music/tnote.h>
#include <QAction>
#include <QMainWindow>



TtoolBar::TtoolBar(const QString& path, QMainWindow* mainWindow) :
	QToolBar(0),
	m_path(path + "picts/")
{
	settingsAct = new QAction(tr("Settings"), this);
	settingsAct->setStatusTip(tr("Application preferences"));
	settingsAct->setIcon(QIcon(m_path + "systemsettings.png"));

	analyseAct = new QAction(tr("Analyze", "could be Chart as well"), this);
	analyseAct->setIcon(QIcon(m_path + "charts.png"));
	analyseAct->setStatusTip(tr("Analysis of exam results"));

	aboutAct = new QAction(tr("About"), this);
	aboutAct->setStatusTip(tr("About Nootka"));
	aboutAct->setIcon(QIcon(m_path + "about.png"));
	
	levelCreatorAct = new QAction(this);
	startExamAct = new QAction(this);
	actionsAfterExam(); // set text and icon also for levelCreatorAct	

	addAction(settingsAct);
#if !defined (Q_OS_ANDROID)
	addAction(levelCreatorAct);
	addAction(analyseAct);
#endif
//     nootBar->addAction(aboutAct);
	addAction(startExamAct);
	setMovable(false);

}


void TtoolBar::actionsAfterExam() {
	levelCreatorAct->setText(tr("Level"));
	levelCreatorAct->setStatusTip(tr("Levels creator"));
	levelCreatorAct->setIcon(QIcon(m_path + "levelCreator.png"));

	startExamAct->setText(tr("Start!"));
	startExamAct->setStatusTip(tr("Start exercises or an exam"));
	startExamAct->setIcon(QIcon(m_path + "startExam.png"));
	
	aboutAct->setVisible(true);
	analyseAct->setVisible(true);
	startExamAct->setDisabled(false);
	
	if (nextQuestAct) {
		delete nextQuestAct;
		delete prevQuestAct;
		delete checkAct;
	}
	if (repeatSndAct)
		delete repeatSndAct;
	if (correctAct)
		delete correctAct;
	if (tuneForkAct)
		delete tuneForkAct;
	
}


void TtoolBar::actionsToExam() {
	aboutAct->setVisible(false);
	analyseAct->setVisible(false);
	levelCreatorAct->setIcon(QIcon(m_path + "help.png"));
	levelCreatorAct->setText(tr("Help"));
	levelCreatorAct->setStatusTip(levelCreatorAct->text());
	startExamAct->setIcon(QIcon(m_path + "stopExam.png"));
	startExamAct->setText(tr("Stop"));
	
	if (!nextQuestAct) {
		nextQuestAct = new QAction(tr("Next", "like a next question"), this);
		nextQuestAct->setStatusTip(tr("next question\n(space %1)").arg(TexamHelp::orRightButtTxt()));
		nextQuestAct->setIcon(QIcon(m_path + "nextQuest.png"));
		nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
		addAction(nextQuestAct);

		prevQuestAct = new QAction(tr("Repeat", "like a repeat question"), this);
		prevQuestAct->setStatusTip(tr("repeat previous question (backspace)"));
		prevQuestAct->setIcon(QIcon(m_path + "prevQuest.png"));
		prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));

		checkAct = new QAction(tr("Check", "like a check answer"), this);
		checkAct->setStatusTip(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()));
		checkAct->setIcon(QIcon(m_path + "check.png"));
		checkAct->setShortcut(QKeySequence(Qt::Key_Return));
	}
}


void TtoolBar::createRepeatSoundAction() {
	if (!repeatSndAct) {
		repeatSndAct = new QAction(tr("Play"), this);
		repeatSndAct->setStatusTip(tr("play sound again") + "<br>(" + 
										TexamHelp::pressSpaceKey().replace("<b>", " ").replace("</b>", ")"));
		repeatSndAct->setShortcut(QKeySequence(Qt::Key_Space));
		repeatSndAct->setIcon(QIcon(m_path + "repeatSound.png"));
	}
}


void TtoolBar::createCorrectAction() {
	if (!correctAct) {
		correctAct = new QAction(tr("Correct", "like a correct answer with mistake"), this);
		correctAct->setStatusTip(tr("correct answer\n(enter)"));
		correctAct->setIcon(QIcon(m_path + "correct.png"));
		correctAct->setShortcut(QKeySequence(Qt::Key_Return));
	}
}


void TtoolBar::createTuneForkAction() {
	if (!tuneForkAct) {
		tuneForkAct = new QAction(Tnote(6, 1, 0).toText(), this);
		tuneForkAct->setStatusTip(tr("Play <i>middle a</i> like a tune fork"));
		tuneForkAct->setIcon(QIcon(m_path + "fork.png"));
	}
}


void TtoolBar::setForQuestion(bool repeatSound, bool tuneFork) {
	removeAction(nextQuestAct);
	removeAction(prevQuestAct);
	if (repeatSound && repeatSndAct)
		addAction(repeatSndAct);
	if (tuneFork && tuneForkAct)
		addAction(tuneForkAct);
	addAction(checkAct);
}


void TtoolBar::setAfterAnswer() {
	removeAction(checkAct);
	if (repeatSndAct)
		removeAction(repeatSndAct);
	if (tuneForkAct)
		removeAction(tuneForkAct);
}






