/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "tstartexamdlg.h"
#include "texamparams.h"
#include <exam/texam.h>
#include <exam/textrans.h>
#include <level/tlevelselector.h>
#include <widgets/troundedlabel.h>
#include <tpath.h>
#include <help/thelpdialogbase.h>
#include <help/tmainhelp.h>
#include <help/texamhelp.h>
#include <QtWidgets>
#include <stdlib.h> // for getenv()

QString TstartExamDlg::systemUserName() {
#if defined(Q_OS_WIN32)
        return QString::fromLocal8Bit(getenv("USERNAME"));
#else
        return QString::fromLocal8Bit(getenv("USER"));
#endif
}


QString statusTipText;
TstartExamDlg::TstartExamDlg(const QString& nick, TexamParams* examParams, QWidget* parent) :
  QDialog(parent),
  m_Acction(e_none),
  m_examParams(examParams),
  m_selectedExamFile("")
{
  setWindowTitle(tr("Start exercises or an exam"));

  QVBoxLayout *levLay = new QVBoxLayout;
  QHBoxLayout *nameLay = new QHBoxLayout;
  QLabel *nameLab = new QLabel(tr("student name:"), this);
  nameLay->addWidget(nameLab);
  m_nameEdit = new QLineEdit(nick, this);
  if (nick == "")
    m_nameEdit->setText(systemUserName());
  m_nameEdit->setMaxLength(30);
    m_nameEdit->setStatusTip(tr("Enter your name or nick-name."));
    nameLay->addWidget(m_nameEdit);
  levLay->addLayout(nameLay);
  m_levelsView = new TlevelSelector(this);
  m_levelsView->disableNotSuitable();
  levLay->addWidget(m_levelsView);
  QLabel *moreLab = new QLabel(TexTrans::moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);
    levLay->addWidget(moreLab, 0, Qt::AlignCenter);
  levelGr = new QGroupBox(this);
    levelGr->setStatusTip(tr("Select a level suitable for you<br>or create new one."));
    levelGr->setLayout(levLay);

  m_createLevelButt = new QPushButton(this);
    m_createLevelButt->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
    m_createLevelButt->setIcon(QIcon(Tpath::img("levelCreator")));
    setIconSize(m_createLevelButt);
  QLabel *newLevelLab = new QLabel(tr("Create new level").replace(" ", "<br>"), this);
    newLevelLab->setAlignment(Qt::AlignCenter);
    newLevelLab->setStatusTip(m_createLevelButt->statusTip());

  m_exerciseButt = new QPushButton(tr("exercises"));
    m_exerciseButt->setIcon(QIcon(Tpath::img("practice")));
    setIconSize(m_exerciseButt);

  m_examMenu = new QMenu("open exam file", this);
  m_newExamButt = new QPushButton(tr("pass new exam"), this);
    m_newExamButt->setIcon(QIcon(Tpath::img("exam")));
    setIconSize(m_newExamButt);
  m_contExamButt = new QPushButton(tr("continue exam"), this);
    m_contExamButt->setIcon(QIcon(Tpath::img("exam")));
    setIconSize(m_contExamButt);
    m_contExamButt->setStatusTip(tr("Click and select an exam to continue"));
    m_contExamButt->setMenu(m_examMenu);
  m_lastExamButt = new QPushButton(QIcon(Tpath::img("exam")), "", this);
    setIconSize(m_lastExamButt);
    m_lastExamButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  m_helpButt = new QPushButton(this);
    m_helpButt->setIcon(QIcon(Tpath::img("help")));
    m_helpButt->setStatusTip(tr("Help"));
    m_helpButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setIconSize(m_helpButt);
  m_cancelBut = new QPushButton(this);
    m_cancelBut->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)));
    m_cancelBut->setStatusTip(tr("Discard"));
    m_cancelBut->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setIconSize(m_cancelBut);

  m_hintLabel = new TroundedLabel(this);
  m_hintLabel->setFixedHeight(70);
  m_hintLabel->setWordWrap(true);


  QHBoxLayout *upperLay = new QHBoxLayout;
    upperLay->addWidget(levelGr);
    QVBoxLayout *helpLevelLay = new QVBoxLayout;
      helpLevelLay->addWidget(m_helpButt);
      helpLevelLay->addStretch(1);
      helpLevelLay->addWidget(m_createLevelButt);
      helpLevelLay->addWidget(newLevelLab);
      helpLevelLay->addStretch(1);
      helpLevelLay->addWidget(m_cancelBut);
    upperLay->addLayout(helpLevelLay);
  QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->addLayout(upperLay);
    QHBoxLayout *exerciseLay = new QHBoxLayout;
      exerciseLay->addSpacing(40);
      exerciseLay->addWidget(m_exerciseButt);
      exerciseLay->addSpacing(40);
    mainLay->addLayout(exerciseLay);
  QHBoxLayout *examLay = new QHBoxLayout;
    examLay->addWidget(m_newExamButt);
    examLay->addWidget(m_contExamButt);
    examLay->addWidget(m_lastExamButt);
  mainLay->addLayout(examLay);
  mainLay->addWidget(m_hintLabel);

  setLayout(mainLay);

  statusTipText = tr("To start exercising or to pass new exam put in your name and select a level. To continue the previous exam, select it from the list or load from file." );
  m_hintLabel->setStatusTip("<b>" + statusTipText + "</b>");

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) // I hate mess in Win registry
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
  QSettings sett;
#endif
  QAction *loadExamAct = new QAction(TexTrans::loadExamFileTxt(), this);
    loadExamAct->setIcon(QIcon(Tpath::img("nootka-exam")));
    loadExamAct->setShortcut(QKeySequence::Open);
    connect(loadExamAct, SIGNAL(triggered(bool)), this, SLOT(examFromFileDialog()));
  m_examMenu->addAction(loadExamAct);
  m_examMenu->addSeparator();
  m_examMenu->addAction(tr("recent opened exams:"));

  m_recentExams = sett.value("recentExams").toStringList();
  for (int i = 0; i < m_recentExams.size(); i++) {
      QFileInfo fi(m_recentExams[i]);
      if (fi.exists()) {
          QAction *recentAct = m_examMenu->addAction(fi.fileName());
          recentAct->setData(m_recentExams[i]);
          connect(recentAct, SIGNAL(triggered(bool)), this, SLOT(continuePrevExam()));
      }
      else
          m_recentExams.removeAt(i);
  }
  if (m_recentExams.size()) {
      sett.setValue("recentExams", m_recentExams);
  }

  updateButtonStatusText("");
  QString exerciseFile = QDir::toNativeSeparators(QFileInfo(sett.fileName()).absolutePath() + "/exercise.noo");
  m_prevExerciseLevel.name = ""; // empty means - no previous level
  if (QFileInfo(exerciseFile).exists()) {
      Texam exam(&m_prevExerciseLevel, "");
      Texam::EerrorType err = exam.loadFromFile(exerciseFile);
      if (err != Texam::e_file_OK && err != Texam::e_file_corrupted) {
          qDebug() << "exercise file was corrupted... and deleted...";
          QFile::remove(exerciseFile);
      }
  }
  if (TlevelSelector::checkLevel(m_prevExerciseLevel) != "")
      m_prevExerciseLevel.name = ""; // Returned string means that the level doesn't match to current settings
  if (m_prevExerciseLevel.name != "")
      m_exerciseButt->setStatusTip(tr("Start exercise on level:") + "<br><b>" + m_prevExerciseLevel.name + "</b>");

  if (m_recentExams.size())
      m_lastExamButt->setStatusTip(tr("Continue the latest exam:") + "<br><b>" + m_recentExams.at(0) + "</b>");
  else
      m_lastExamButt->hide();

  connect(m_levelsView, SIGNAL(levelToLoad()), this, SLOT(levelToLoad()));
  connect(m_cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
  connect(m_createLevelButt, SIGNAL(clicked()), this, SLOT(createLevel()));
  connect(m_newExamButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
  connect(m_contExamButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
  connect(m_lastExamButt, SIGNAL(clicked()), this, SLOT(continueTheLast()));
  connect(m_exerciseButt, SIGNAL(clicked()), this, SLOT(startAccepted()));
  connect(m_levelsView, SIGNAL(levelChanged(Tlevel)), this, SLOT(levelWasSelected(Tlevel)));
  connect(m_helpButt,  SIGNAL(clicked()), this, SLOT(helpSelected()));

  if (m_examParams->showVeryBeginHelp)
    QTimer::singleShot(10, this, SLOT(helpSelected()));
}


TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt, Tlevel &lev) {
    exec();
    if (result() == QDialog::Accepted) {
        if (m_Acction == e_newExam || m_Acction == e_runExercise) {
            txt = m_nameEdit->text();
						if (m_prevExerciseLevel.name == "")
								lev = m_levelsView->getSelectedLevel();
						else
								lev = m_prevExerciseLevel;
            return m_Acction;
        } else if (m_Acction == e_contExam) {
            if (m_selectedExamFile != "") {
                txt = m_selectedExamFile;
                return e_contExam;
            } else
								return e_none;
					}
    } else if (m_Acction == e_levelCreator)
						return e_levelCreator;
		return e_none;
}


void TstartExamDlg::levelToLoad() {
    m_levelsView->loadFromFile();
}


bool TstartExamDlg::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        m_hintLabel->setText("<center>" + se->tip() + "</center>");
    }
    return QDialog::event(event);
}


void TstartExamDlg::continueTheLast() {
		if (m_recentExams.size()) {
				examToContSelected(m_recentExams[0]);
		}
}


bool TstartExamDlg::isAnyLevelSelected() {
	Tlevel l = m_levelsView->getSelectedLevel();
	if (l.name == "") { // nothing selected
			QMessageBox::warning(this, "Nootka", tr("No level was selected!"));
			return false;
	}
	return true;
}


void TstartExamDlg::startAccepted() {
    if (sender() == m_newExamButt) { // new exam on selected level
        if (!isAnyLevelSelected())
            return;
				if (m_nameEdit->text() == "") {
						QMessageBox::warning(this, "", tr("Give a user name!"));
						return;
				}
				m_Acction = e_newExam;
				accept();
    } else if (sender() == m_exerciseButt) { // exercise on selected level or previous one
				if (m_prevExerciseLevel.name == "" && !isAnyLevelSelected())
					return;
				if (m_nameEdit->text() == "") {
						QMessageBox::warning(this, "", tr("Give a user name!"));
						return;
				}
				m_Acction = e_runExercise;
				accept();
    } else { // exam to continue
				if (m_selectedExamFile != "")    {
					m_Acction = e_contExam;
					accept();
			}
    }
}


TstartExamDlg::Eactions TstartExamDlg::createLevel() {
  m_Acction = e_levelCreator;
  close();
  return e_levelCreator;
}


void TstartExamDlg::continuePrevExam() {
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
			examToContSelected(action->data().toString());
}


void TstartExamDlg::examFromFileDialog() {
  QString fileName = QFileDialog::getOpenFileName(this, TexTrans::loadExamFileTxt(), m_examParams->examsDir, TexTrans::examFilterTxt());
  if (fileName != "") {
      examToContSelected(fileName);
  }
}


void TstartExamDlg::examToContSelected(QString eFile) {
  if (eFile != "") {
      m_examParams->examsDir = QFileInfo(eFile).absoluteDir().absolutePath();
      m_recentExams.prepend(eFile);
      m_selectedExamFile = eFile;
      m_Acction = e_contExam;
      accept();
  }
}


void TstartExamDlg::updateButtonStatusText(const QString& levelName) {
	QString statusText;
	if (levelName == "") {
			statusText += tr("No level was selected!");
	}	else {
			statusText += levelName;
	}
	statusText.prepend("<br><b>");
	statusText.append("</b>");
	m_newExamButt->setStatusTip(tr("Pass new exam on level:") + statusText);
	m_exerciseButt->setStatusTip(tr("Start exercise on level:") + statusText);
}


void TstartExamDlg::levelWasSelected(Tlevel level) {
	m_prevExerciseLevel.name = ""; // Reset the name - now level is taken from selection 
	updateButtonStatusText(level.name);		
}


void TstartExamDlg::helpSelected() {
  ThelpDialogBase *help = new ThelpDialogBase(this);
  help->setFixedSize(width(), height() * 0.8);
  QString ht = "<center><h2>" + help->pix("practice", 64) + " " + tr("To exercise or to pass an exam?") + " " +
  help->pix("exam", 64) + "</h2>" +
  TmainHelp::youWillLearnText() + "<br><br>" +
  "</center><table><tr><td style=\"padding: 10px;\" align=\"center\">" +
  TmainHelp::duringExercisingText() + "<br>" + TexamHelp::exerciseFeaturesText() +
  "</td></tr><tr><td style=\"padding: 10px;\" align=\"center\">" +
  TmainHelp::duringExamsText() + "<br>" + TexamHelp::examFeaturesText() + "</td></tr></table>" +
  help->onlineDocP("start-exam");

  help->helpText()->setHtml(ht);
  help->showCheckBox(&m_examParams->showVeryBeginHelp);
// 		qDebug() << help->helpText()->toHtml();
  help->exec();
  delete help;
}


void TstartExamDlg::setIconSize(QPushButton* button) {
	button->setIconSize(QSize(48, 48));
}








