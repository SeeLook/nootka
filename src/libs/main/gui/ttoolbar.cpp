/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include "tmelman.h"
#if !defined (Q_OS_ANDROID)
  #include <help/texamhelp.h>
  #include <tnootkalabel.h>
#endif
#include <music/tnote.h>
#include <tpath.h>
#include <tcolor.h>
#include <touch/ttouchproxy.h>
#include <score/tscoreactions.h>
#include <QtWidgets/QtWidgets>


TtoolBar* TtoolBar::m_instance = nullptr;


TtoolBar::TtoolBar(const QString& version, QMainWindow* mainWindow) :
	QToolBar(0),
	aboutAct(0),
	m_proxy(0)
{
  if (m_instance) {
    qDebug() << "TtoolBar instance already exists";
    return;
  }

  m_instance = this;
	settingsAct = new QAction(tr("Settings"), this);
	settingsAct->setStatusTip(tr("Application preferences"));
	settingsAct->setIcon(QIcon(Tpath::img("systemsettings")));

	analyseAct = new QAction(tr("Analyze", "could be Chart as well"), this);
	analyseAct->setIcon(QIcon(Tpath::img("charts")));
	analyseAct->setStatusTip(tr("Analysis of exam results"));

  aboutSimpleAct = new QAction(tr("About"), this);
  aboutSimpleAct->setStatusTip(tr("About Nootka"));
  aboutSimpleAct->setIcon(QIcon(Tpath::img("nootka")));
  QColor C(palette().text().color());
#if defined (Q_OS_WIN)
    C.setAlpha(50);
#else
    C.setAlpha(40);
#endif
  m_spacer = new QWidget(this);
  m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  addWidget(m_spacer);
  m_spacer->hide();
  C = Tcolor::merge(C, palette().window().color());
#if !defined(Q_OS_ANDROID)
  m_nootLabel = new TnootkaLabel(Tpath::img("logo"), this, C, version);
  aboutAct = addWidget(m_nootLabel);
  m_nootLabel->setStatusTip(tr("About Nootka"));
  m_nootLabel->setHoverColor(palette().highlight().color());
  connect(m_nootLabel, &TnootkaLabel::clicked, aboutAct, &QAction::trigger);
#endif

	levelCreatorAct = new QAction(this);
	startExamAct = new QAction(this);
	actionsAfterExam(); // set text and icon also for levelCreatorAct	

	addAction(settingsAct);
	addAction(levelCreatorAct);
	addAction(analyseAct);
	addAction(startExamAct);
	setMovable(false);
  if (TtouchProxy::touchEnabled())
    hide();
}

TtoolBar::~TtoolBar()
{
  m_instance = nullptr;
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TtoolBar::addAction(QAction* a) {
#if defined(Q_OS_ANDROID)
  a->setVisible(true); // No tool bar under mobile, just ensure that action is visible
#else
	insertAction(actions()[actions().count() - 2], a);
#endif
}


void TtoolBar::removeAction(QAction* a) {
#if defined(Q_OS_ANDROID)
  a->setVisible(false);
#else
  QToolBar::removeAction(a);
#endif
}



void TtoolBar::addMelodyButton(TmelMan* melBut) {
	m_melButton = melBut;
	insertAction(startExamAct, m_melButton->melodyAction());
}


void TtoolBar::setMelodyButtonVisible(bool vis) {
	m_melButton->melodyAction()->setVisible(vis);
	m_melButton->button()->menu()->setDisabled(!vis);
#if defined (Q_OS_ANDROID)
  playMelody()->setVisible(vis); // This way Android menu skips those actions when not visible
  recordMelody()->setVisible(vis);
  generateMelody()->setVisible(vis);
  if (vis) {
      flyActions()->insert(0, recordMelody()); // 'pitch detection' is first so insert other actions before
      flyActions()->insert(0, playMelody());
  } else {
      flyActions()->removeOne(playMelody());
      flyActions()->removeOne(recordMelody());
  }
#endif
}


void TtoolBar::addScoreActions(TscoreActions* scoreBut) {
  m_scoreActs = scoreBut;
  insertAction(startExamAct, m_scoreActs->mainAction());
}


QAction* TtoolBar::scoreZoomIn() {
  return m_scoreActs->zoomIn();
}


QAction* TtoolBar::scoreZoomOut() {
  return m_scoreActs->zoomOut();
}


QAction* TtoolBar::scoreExtraAccids() {
  return m_scoreActs->extraAccids();
}


QAction* TtoolBar::scoreShowNames() {
  return m_scoreActs->noteNames();
}


QAction* TtoolBar::scoreDeleteAll() {
  return m_scoreActs->clearScore();
}


QAction* TtoolBar::playMelody() {
  return m_melButton->playAction();
}


QAction* TtoolBar::generateMelody() {
  return m_melButton->generateAction();
}


QAction* TtoolBar::recordMelody() {
  return m_melButton->recordAction();
}



void TtoolBar::actionsAfterExam() {
	levelCreatorAct->setText(tr("Level"));
	levelCreatorAct->setStatusTip(tr("Levels creator"));
	levelCreatorAct->setIcon(QIcon(Tpath::img("levelCreator")));

	startExamAct->setText(tr("Lessons"));
	startExamAct->setStatusTip(tr("Start exercises or an exam"));
	startExamAct->setIcon(QIcon(Tpath::img("startExam")));
	
	settingsAct->setIcon(QIcon(Tpath::img("systemsettings")));
	settingsAct->setStatusTip(tr("Application preferences"));
	
  if (aboutAct)
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
	if (attemptAct)
		delete attemptAct;
}


void TtoolBar::actionsToExam() {
  if (aboutAct)
    aboutAct->setVisible(false);
	analyseAct->setVisible(false);
	levelCreatorAct->setIcon(QIcon(Tpath::img("help")));
	levelCreatorAct->setText(tr("Help"));
	levelCreatorAct->setStatusTip(levelCreatorAct->text());
	startExamAct->setIcon(QIcon(Tpath::img("stopExam")));
	startExamAct->setText(tr("Stop"));
	settingsAct->setIcon(QIcon(Tpath::img("exam-settings")));
	settingsAct->setStatusTip(tr("Exercise or exam preferences"));
	
	if (!nextQuestAct) {
		nextQuestAct = new QAction(tr("Next", "like a next question"), this);
#if !defined (Q_OS_ANDROID)
		nextQuestAct->setStatusTip(tr("next question\n(space %1)").arg(TexamHelp::orRightButtTxt()).replace(QLatin1String("\n"), QLatin1String("<br>")));
#endif
		nextQuestAct->setIcon(QIcon(Tpath::img("nextQuest")));
		nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
    nextQuestAct->setData(true);
		addAction(nextQuestAct);

		prevQuestAct = new QAction(tr("Repeat", "like a repeat question"), this);
		prevQuestAct->setStatusTip(tr("repeat previous question (backspace)").replace(QLatin1String("("), QLatin1String("<br>(")));
		prevQuestAct->setIcon(QIcon(Tpath::img("prevQuest")));
		prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));

		checkAct = new QAction(tr("Check", "like a check answer"), this);
#if !defined (Q_OS_ANDROID)
		checkAct->setStatusTip(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()).replace(QLatin1String("\n"), QLatin1String("<br>")));
#endif
		checkAct->setIcon(QIcon(Tpath::img("check")));
		checkAct->setShortcut(QKeySequence(Qt::Key_Return));
	}
#if defined (Q_OS_ANDROID)
	prevQuestAct->setVisible(false);
  checkAct->setVisible(false);
#endif
}


void TtoolBar::createRepeatSoundAction() { // TODO add bool for melodies/single to adjust status tip
	if (!repeatSndAct) {
		repeatSndAct = new QAction(tr("Play"), this);
#if defined (Q_OS_ANDROID)
    repeatSndAct->setVisible(false);
#else
		repeatSndAct->setStatusTip(tr("play sound again") + QStringLiteral("<br>(") +
          TexamHelp::pressSpaceKey().replace(QStringLiteral("<b>"), QStringLiteral(" ")).replace(QStringLiteral("</b>"), QStringLiteral(")")));
#endif
		repeatSndAct->setShortcut(QKeySequence(Qt::Key_Space));
		repeatSndAct->setIcon(QIcon(Tpath::img("playMelody")));
	}
}


void TtoolBar::createCorrectAction() {
	if (!correctAct) {
		correctAct = new QAction(tr("Correct", "like a correct answer with mistake"), this);
		correctAct->setStatusTip(tr("correct answer\n(enter)").replace(QLatin1String("\n"), QLatin1String("<br>")));
		correctAct->setIcon(QIcon(Tpath::img("correct")));
		correctAct->setShortcut(QKeySequence(Qt::Key_Return));
#if defined (Q_OS_ANDROID)
    correctAct->setVisible(false);
#endif
	}
}


void TtoolBar::createTuneForkAction() {
	if (!tuneForkAct) {
		tuneForkAct = new QAction(Tnote(6, 1, 0).toText(), this);
		tuneForkAct->setStatusTip(tr("Play <i>middle a</i> like a tuning fork.\n(Press key 'a')").replace(QLatin1String("\n"), QLatin1String("<br>")));
		tuneForkAct->setIcon(QIcon(Tpath::img("fork")));
		tuneForkAct->setShortcut(QKeySequence(Qt::Key_A));
#if defined (Q_OS_ANDROID)
    tuneForkAct->setVisible(false);
#endif
	}
}


void TtoolBar::createAttemptAction() {
	if (!attemptAct) {
		attemptAct = new QAction(tr("Try again"), this);
		attemptAct->setStatusTip(tr("Try this melody once again. (backspace)").replace(QLatin1String("("), QLatin1String("<br>(")));
		attemptAct->setIcon(QIcon(Tpath::img("prevQuest")));
		attemptAct->setShortcut(QKeySequence(Qt::Key_Backspace));
#if defined (Q_OS_ANDROID)
    attemptAct->setVisible(false);
#endif
	}
}


void TtoolBar::setForQuestion(bool repeatSound, bool tuneFork) {
	removeAction(nextQuestAct);
	removeAction(prevQuestAct);
	if (repeatSound && repeatSndAct) {
		addAction(repeatSndAct);
#if defined (Q_OS_ANDROID)
    flyActions()->append(repeatSndAct);
#endif
  }
	if (tuneFork && tuneForkAct) {
		addAction(tuneForkAct);
#if defined (Q_OS_ANDROID)
    flyActions()->append(tuneForkAct);
#endif
  }
	addAction(checkAct);
}


void TtoolBar::setAfterAnswer() {
	removeAction(checkAct);
	if (repeatSndAct) {
		removeAction(repeatSndAct);
#if defined (Q_OS_ANDROID)
    flyActions()->removeOne(repeatSndAct);
#endif
  }
	if (tuneForkAct) {
		removeAction(tuneForkAct);
#if defined (Q_OS_ANDROID)
    flyActions()->removeOne(tuneForkAct);
#endif
  }
}


void TtoolBar::setBarIconStyle(Qt::ToolButtonStyle iconStyle, int iconS) {
	if (iconStyle != toolButtonStyle()) {
    setToolButtonStyle(iconStyle);
		m_melButton->button()->setToolButtonStyle(iconStyle);
    m_scoreActs->button()->setToolButtonStyle(iconStyle);
	}
	int tmpSize = iconS;
	if (toolButtonStyle() == Qt::ToolButtonIconOnly)
		iconS *= 1.4; // increase icons size when no text under
	if (toolButtonStyle() != Qt::ToolButtonTextOnly) {
		if (iconS != iconSize().width()) {
			setIconSize(QSize(iconS, iconS));
        m_melButton->button()->setIconSize(QSize(iconS, iconS));
        m_scoreActs->button()->setIconSize(QSize(iconS, iconS));
		}
	}
#if !defined(Q_OS_ANDROID)
  m_nootLabel->setMaximumHeight(tmpSize * 1.5);
#endif
	adjustSize();
	if (m_proxy)
		m_proxy->adjustSize();
}


void TtoolBar::setProxy(QGraphicsProxyWidget* proxy) {
	m_proxy = proxy;
  if (!TtouchProxy::touchEnabled()) {
    if (m_proxy)
      m_spacer->show();
    else
      m_spacer->hide();
    update();
  }
}


void TtoolBar::resizeEvent(QResizeEvent* event) {
  Q_UNUSED(event)
	if (!TtouchProxy::touchEnabled() && m_proxy) {
		adjustSize();
		m_proxy->resize(size());
	}
}













