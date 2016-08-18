/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "tstartexamdlg.h"
#include "texamparams.h"
#include <qtr.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <level/tlevelselector.h>
#include <widgets/troundedlabel.h>
#include <tpath.h>
#include <help/thelpdialogbase.h>
#include <help/tmainhelp.h>
#include <help/texamhelp.h>
#if defined (Q_OS_ANDROID)
  #include <touch/ttoucharea.h>
  #include <tfiledialog.h>
  #include <tmtr.h>
  #include <Android/tandroid.h>
#endif
#include <QtWidgets/QtWidgets>


QString TstartExamDlg::systemUserName() {
#if defined (Q_OS_ANDROID)
  return Tandroid::accountName();
#elif defined(Q_OS_WIN32)
  return qgetenv("USERNAME");
#else
  return qgetenv("USER");
#endif
}


TstartExamDlg::TstartExamDlg(const QString& nick, TexamParams* examParams, QWidget* parent) :
  QDialog(parent),
  m_Acction(e_none),
  m_examParams(examParams),
  m_selectedExamFile(QString())
{
#if defined (Q_OS_ANDROID)
  showMaximized();
  m_sideButtSize = Tmtr::fingerPixels() * 0.8;
#else
  setWindowTitle(tr("Start exercises or an exam"));
  m_sideButtSize = qMin<int>(fontMetrics().height() * 2.5, qApp->desktop()->height() / 16);
#endif

  const QString selectLevelText(tr("Select a level suitable for you<br>or create new one."));
  auto nameLab = new QLabel(tr("student name:"), this);
  m_nameEdit = new QLineEdit(nick, this);
#if !defined (Q_OS_ANDROID) // if undefined, keep it empty under Android, it will ask user
  if (nick.isEmpty())
    m_nameEdit->setText(systemUserName());
#endif
  m_nameEdit->setMaxLength(40);
    m_nameEdit->setStatusTip(tr("Enter your name or nick-name."));
//   if (m_nameEdit->text().isEmpty()) // when still there is no user name - put gray text of status tip
    m_nameEdit->setPlaceholderText(m_nameEdit->statusTip());

// 2nd layout with level selector/preview ===================================
  m_levelsView = new TlevelSelector(this);
  m_levelsView->disableNotSuitable();
#if defined (Q_OS_ANDROID)
  m_levelsView->setMinimumHeight(Tmtr::shortScreenSide() * 0.6);
#endif

  auto moreLab = new QLabel(TexTrans::moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);

  m_createLevelButt = new QPushButton(tr("Create new level"), this);
    m_createLevelButt->setStatusTip(tr("Dialog window for creating new level<br>will be opened."));
    m_createLevelButt->setIcon(QIcon(Tpath::img("levelCreator")));
    setIconSize(m_createLevelButt);
    m_createLevelButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
// ============================================================================
#if defined (Q_OS_ANDROID)
  m_selectLevelBut = new QPushButton(this); // button to switch between layouts
    m_selectLevelBut->setText(selectLevelText.left(selectLevelText.indexOf(QLatin1String("<br>"))));
    m_selectLevelBut->setIcon(QIcon(Tpath::img("nootka-level")));
    setIconSize(m_selectLevelBut);
  auto backButton = new QPushButton(qTR("QFileDialog", "Back"), this);
    backButton->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    setIconSize(backButton);
  m_mobExerButton = new QPushButton(QApplication::translate("levelSettings", "Start exercise"), this);
    m_mobExerButton->setIcon(QIcon(Tpath::img("practice")));
    setIconSize(m_mobExerButton);
    m_mobExerButton->setDisabled(true);
  m_mobExamButton = new QPushButton(QApplication::translate("TexamSummary", "Pass an exam"), this);
    m_mobExamButton->setIcon(QIcon(Tpath::img("exam")));
    setIconSize(m_mobExamButton);
    m_mobExamButton->setDisabled(true);
#endif

  m_exerciseButt = new QPushButton(tr("Start exercise on level:"));
    m_exerciseButt->setIcon(QIcon(Tpath::img("practice")));
    setIconSize(m_exerciseButt);
    m_exerciseButt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_exerLevelLab = new QLabel(this);
    m_exerLevelLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#if defined (Q_OS_ANDROID)
    m_exerLevelLab->setAlignment(Qt::AlignCenter);
#else
    m_exerLevelLab->setAlignment(Qt::AlignLeft);
#endif

  m_examMenu = new QMenu("open exam file", this);
  m_newExamButt = new QPushButton(tr("Pass new exam on level:"), this);
    m_newExamButt->setIcon(QIcon(Tpath::img("exam")));
    setIconSize(m_newExamButt);
    m_newExamButt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_examLevelLab = new QLabel(this);
    m_examLevelLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#if defined (Q_OS_ANDROID)
    m_examLevelLab->setAlignment(Qt::AlignCenter);
#else
    m_examLevelLab->setAlignment(Qt::AlignLeft);
#endif

  m_contExamButt = new QPushButton(tr("Continue exam"), this);
    m_contExamButt->setIcon(QIcon(Tpath::img("exam")));
    setIconSize(m_contExamButt);
    m_contExamButt->setStatusTip(tr("Click and select an exam to continue"));
    m_contExamButt->setMenu(m_examMenu);
    m_contExamButt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_lastExamButt = new QPushButton(QIcon(Tpath::img("exam")), tr("Latest exam"), this);
    setIconSize(m_lastExamButt);
//     m_lastExamButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  m_helpButt = new QPushButton(this);
    m_helpButt->setIcon(QIcon(Tpath::img("help")));
    m_helpButt->setStatusTip(qTR("QShortcut", "Help"));
    m_helpButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setIconSize(m_helpButt);
  m_cancelBut = new QPushButton(this);
    m_cancelBut->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)));
    m_cancelBut->setStatusTip(qTR("QPlatformTheme", "Close"));
    m_cancelBut->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setIconSize(m_cancelBut);

  m_hintLabel = new TroundedLabel(this);
  m_hintLabel->setFixedHeight(fontMetrics().height() * 4);
  m_hintLabel->setWordWrap(true);
#if !defined (Q_OS_ANDROID)
  m_cancelBut->setFixedHeight(m_hintLabel->height());
#endif

// Main layout
  auto mainLay = new QVBoxLayout;
    auto topLay = new QHBoxLayout;
      topLay->addWidget(m_helpButt);
      topLay->addWidget(nameLab);
      topLay->addWidget(m_nameEdit);
#if defined (Q_OS_ANDROID)
      topLay->addWidget(m_cancelBut);
#endif
  mainLay->addLayout(topLay);
#if defined (Q_OS_ANDROID)
  mainLay->addStretch(1);
  mainLay->addWidget(m_selectLevelBut);
  mainLay->addStretch(1);
#endif

// -------- level selector and preview (under mobile it is another widget on the stack)
  auto levelsLay = new QVBoxLayout;
#if defined (Q_OS_ANDROID)
  levelsLay->addStretch();
    auto addButtLay = new QHBoxLayout;
      addButtLay->addWidget(backButton);
      addButtLay->addWidget(m_mobExerButton);
      addButtLay->addWidget(m_mobExamButton);
  levelsLay->addLayout(addButtLay);
#endif
  levelsLay->addWidget(m_levelsView);
  levelsLay->addWidget(m_createLevelButt);
  levelsLay->addWidget(moreLab, 0, Qt::AlignCenter);
  levelsLay->addStretch();
  auto levelsBox = new QWidget(this);
    levelsLay->setContentsMargins(0, 0, 0, 0);
    levelsBox->setLayout(levelsLay);
#if !defined (Q_OS_ANDROID)
    levelsBox->setStatusTip(tr("Select a level suitable for you<br>or create new one."));
    mainLay->addWidget(levelsBox);
#endif
// --------
#if defined (Q_OS_ANDROID) // Mobile has button then its label vertically
  mainLay->addWidget(m_exerciseButt);
  mainLay->addWidget(m_exerLevelLab);
  mainLay->addStretch(1);
  mainLay->addWidget(m_newExamButt);
  mainLay->addWidget(m_examLevelLab);
  mainLay->addStretch(1);
#else // Desktop has | Button | Label | in two rows
  auto buttonsLay = new QGridLayout;
    buttonsLay->addWidget(m_exerciseButt, 0, 0);
    buttonsLay->addWidget(m_exerLevelLab, 0, 1);
    buttonsLay->addWidget(m_newExamButt, 1, 0);
    buttonsLay->addWidget(m_examLevelLab, 1, 1);
    mainLay->addLayout(buttonsLay);
#endif
  auto examsLay = new QHBoxLayout;
    examsLay->addWidget(m_contExamButt);
    examsLay->addWidget(m_lastExamButt);
  mainLay->addLayout(examsLay);

#if defined (Q_OS_ANDROID)
  m_hintLabel->hide();
  mainLay->addStretch(2);
  auto mainArea = new TtouchArea(this);
    mainArea->setLayout(mainLay);
  auto mobileLay = new QStackedLayout;
    mobileLay->addWidget(mainArea);
    mobileLay->addWidget(levelsBox);
  setLayout(mobileLay);
#else
  auto bottomLay = new QHBoxLayout;
    bottomLay->addWidget(m_hintLabel);
    bottomLay->addWidget(m_cancelBut);
  mainLay->addLayout(bottomLay);
  setLayout(mainLay);
#endif

#if !defined (Q_OS_ANDROID)
  m_hintLabel->setStatusTip("<b>" + tr("To start exercising or to pass new exam put in your name and select a level. To continue the previous exam, select it from the list or load from file." ) + "</b>");
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) // I hate mess in Win registry
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
  QSettings sett;
#endif
  QAction *loadExamAct = new QAction(TexTrans::loadExamFileTxt(), this);
    loadExamAct->setIcon(QIcon(Tpath::img("nootka-exam")));
#if !defined (Q_OS_ANDROID)
    loadExamAct->setShortcut(QKeySequence::Open);
#endif
    connect(loadExamAct, SIGNAL(triggered(bool)), this, SLOT(examFromFileDialog()));
  m_examMenu->addAction(loadExamAct);
  m_examMenu->addSeparator();
  m_examMenu->addAction(tr("recent opened exams:"));

  const QString recentExams(QStringLiteral("recentExams"));
  m_recentExams = sett.value(recentExams).toStringList();
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
      sett.setValue(recentExams, m_recentExams);
  }

  QString exerciseFile = QDir::toNativeSeparators(QFileInfo(sett.fileName()).absolutePath() + "/exercise.noo");
  m_prevExerciseLevel.name.clear(); // empty means - no previous level
  if (QFileInfo(exerciseFile).exists()) {
      Texam exam(&m_prevExerciseLevel, QString());
      Texam::EerrorType err = exam.loadFromFile(exerciseFile);
      if (err != Texam::e_file_OK && err != Texam::e_file_corrupted) {
          qDebug() << "exercise file was corrupted... and deleted...";
          QFile::remove(exerciseFile);
      }
  }
  if (!TlevelSelector::checkLevel(m_prevExerciseLevel).isEmpty())
      m_prevExerciseLevel.name.clear(); // Returned string means that the level doesn't match to current settings

  if (m_recentExams.size())
      m_lastExamButt->setStatusTip(tr("Continue the latest exam:") + QLatin1String("<br><b>") + m_recentExams.at(0) + QLatin1String("</b>"));
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
#if defined (Q_OS_ANDROID)
  connect(m_selectLevelBut, &QPushButton::clicked, [=] { mobileLay->setCurrentIndex(1); });
  connect(backButton, &QPushButton::clicked, [=] { mobileLay->setCurrentIndex(0); });
  connect(m_mobExamButton, &QPushButton::clicked, this, &TstartExamDlg::startAccepted);
  connect(m_mobExerButton, &QPushButton::clicked, this, &TstartExamDlg::startAccepted);
#endif

  QTimer::singleShot(10, [=] {
      updateButtonStatusText(QString());
      if (!m_prevExerciseLevel.name.isEmpty()) {
          m_exerciseButt->setStatusTip(tr("Start exercise on level:") + QLatin1String("<br><b>") +
                                    m_prevExerciseLevel.name + QLatin1String("</b>"));
          m_exerLevelLab->setText(QLatin1String("<b>") + m_prevExerciseLevel.name + QLatin1String("</b>"));
      }
  });

  if (m_examParams->showVeryBeginHelp)
    QTimer::singleShot(100, this, SLOT(helpSelected()));
}


TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt, Tlevel &lev) {
    exec();
    if (result() == QDialog::Accepted) {
        if (m_Acction == e_newExam || m_Acction == e_runExercise) {
            txt = m_nameEdit->text();
						if (m_prevExerciseLevel.name.isEmpty())
								lev = m_levelsView->getSelectedLevel();
						else
								lev = m_prevExerciseLevel;
            return m_Acction;
        } else if (m_Acction == e_contExam) {
            if (!m_selectedExamFile.isEmpty()) {
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
	if (l.name.isEmpty()) { // nothing selected
			QMessageBox::warning(this, QStringLiteral("Nootka"), tr("No level was selected!"));
			return false;
	}
	return true;
}


void TstartExamDlg::startAccepted() {
    if (sender() == m_newExamButt || sender() == m_mobExamButton) { // new exam on selected level
        if (!isAnyLevelSelected())
            return;
				if (m_nameEdit->text().isEmpty()) {
						giveUserNameMessage();
						return;
				}
				m_Acction = e_newExam;
				accept();
    } else if (sender() == m_exerciseButt || sender() == m_mobExerButton) { // exercise on selected level or previous one
				if (m_prevExerciseLevel.name.isEmpty() && !isAnyLevelSelected())
					return;
				if (m_nameEdit->text().isEmpty()) {
						giveUserNameMessage();
						return;
				}
				m_Acction = e_runExercise;
				accept();
    } else { // exam to continue
				if (!m_selectedExamFile.isEmpty())    {
					m_Acction = e_contExam;
					accept();
			}
    }
}


void TstartExamDlg::giveUserNameMessage() {
#if defined (Q_OS_ANDROID)
  QString name = QInputDialog::getText(this, tr("Give a user name!"), tr("Enter your name or nick-name."));
  if (name.isEmpty())
    name = QStringLiteral("Android");
  m_nameEdit->setText(name);
#else
  QMessageBox::warning(this, QString(), tr("Give a user name!"));
#endif
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
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getOpenFileName(this, m_examParams->examsDir, QStringLiteral("noo"));
#else
  QString fileName = QFileDialog::getOpenFileName(this, TexTrans::loadExamFileTxt(), m_examParams->examsDir, TexTrans::examFilterTxt());
#endif
  if (!fileName.isEmpty())
      examToContSelected(fileName);
}


void TstartExamDlg::examToContSelected(const QString& eFile) {
  if (!eFile.isEmpty()) {
      m_examParams->examsDir = QFileInfo(eFile).absoluteDir().absolutePath();
      m_recentExams.prepend(eFile);
      m_selectedExamFile = eFile;
      m_Acction = e_contExam;
      accept();
  }
}


void TstartExamDlg::updateButtonStatusText(const QString& levelName) {
  QString statusText = levelName.isEmpty() ? tr("No level was selected!") : levelName;
  statusText.prepend(QLatin1String("<br><b>"));
  statusText.append(QLatin1String("</b>"));
  m_newExamButt->setStatusTip(tr("Pass new exam on level:") + statusText);
  m_exerciseButt->setStatusTip(tr("Start exercise on level:") + statusText);
  m_exerLevelLab->setText(statusText);
  m_examLevelLab->setText(statusText);
}


void TstartExamDlg::levelWasSelected(Tlevel level) {
	m_prevExerciseLevel.name.clear(); // Reset the name - now level is taken from selection 
	updateButtonStatusText(level.name);
#if defined (Q_OS_ANDROID)
  m_mobExerButton->setDisabled(false);
  m_mobExamButton->setDisabled(false);
#endif
}


void TstartExamDlg::helpSelected() {
  ThelpDialogBase *help = new ThelpDialogBase(this);
#if defined (Q_OS_ANDROID)
  help->showMaximized();
#else
  help->setFixedSize(width(), height() * 0.8);
#endif
  QLatin1String br("<br>");
  QString ht = QLatin1String("<center><h2>") + help->pix("practice", 64) + QLatin1String(" ") + tr("To exercise or to pass an exam?") +
  QLatin1String(" ") + help->pix("exam", 64) + QLatin1String("</h2>") + TmainHelp::youWillLearnText() + br + br +
  QLatin1String("</center><table><tr><td style=\"padding: 10px;\" align=\"center\">") +
  TmainHelp::duringExercisingText() + br + TexamHelp::exerciseFeaturesText() +
  QLatin1String("</td></tr><tr><td style=\"padding: 10px;\" align=\"center\">") +
  TmainHelp::duringExamsText() + br + TexamHelp::examFeaturesText() + QLatin1String("</td></tr></table>") +
  help->onlineDocP(QStringLiteral("start-exam"));

  help->helpText()->setHtml(ht);
  help->showCheckBox(&m_examParams->showVeryBeginHelp);
// 		qDebug() << help->helpText()->toHtml();
  help->exec();
  delete help;
}









