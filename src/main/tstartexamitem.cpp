/***************************************************************************
 *   Copyright (C) 2011-2019 by Tomasz Bojczuk                             *
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


#include "tstartexamitem.h"
#include "texamparams.h"
#include "help/thelpdialogbase.h"
#include "help/tmainhelp.h"
#include "help/texamhelp.h"
#include "dialogs/tlevelselector.h"
#include <tglobals.h>
#include <taction.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <Android/tfiledialog.h>

#include <QtCore/qsettings.h>
#include <QtCore/qtimer.h>
#include <QtQml/qqmlengine.h>
#include <QtWidgets/qmessagebox.h>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
  #include <Android/tandroid.h>
#endif


TstartExamItem::TstartExamItem(QQuickItem* parent) :
  QQuickItem(parent)
{

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) // I hate mess in Win registry
  QSettings sett(QSettings::IniFormat, QSettings::UserScope, QLatin1String("Nootka"), QLatin1String("Nootka"));
#else
  QSettings sett;
#endif
  auto loadExamAct = new Taction(TexTrans::loadExamFileTxt(), QStringLiteral("nootka-exam"), this);
  connect(loadExamAct, &Taction::triggered, this, &TstartExamItem::examFromFileDialog);
  QQmlEngine e;
  QQmlComponent c(&e, this);
  loadExamAct->createQmlShortcut(&c, "StandardKey.Open");
  m_recentModel << loadExamAct;
//   m_examMenu->addAction(tr("recent opened exams:"));

  const QString recentExams(QStringLiteral("recentExams"));
  m_recentExams = sett.value(recentExams).toStringList();
  for (int i = 0; i < m_recentExams.size() && i < RECENT_EXAMS_LIMIT; i++) {
      QFileInfo fi(m_recentExams[i]);
      if (fi.exists()) {
          auto recentAct = new Taction(fi.fileName(), QString(), this);
          recentAct->setProperty("path", m_recentExams[i]);
          connect(recentAct, &Taction::triggered, this, &TstartExamItem::continuePrevExam);
          m_recentModel << recentAct;
      } else
          m_recentExams.removeAt(i);
  }
  if (m_recentExams.size()) {
      sett.setValue(recentExams, m_recentExams);
  }

  m_prevExerciseLevel = new Tlevel;

  QString exerciseFile = QDir::toNativeSeparators(QFileInfo(sett.fileName()).absolutePath() + QLatin1String("/exercise2.noo"));
  m_prevExerciseLevel->name.clear(); // empty means - no previous level
  if (QFileInfo::exists(exerciseFile)) {
      Texam exam(m_prevExerciseLevel, QString());
      Texam::EerrorType err = exam.loadFromFile(exerciseFile);
      if (err != Texam::e_file_OK && err != Texam::e_file_corrupted) {
          qDebug() << "[TstartExamItem] exercise file was corrupted... and deleted...";
          QFile::remove(exerciseFile);
      }
  }
  if (!TlevelSelector::checkLevel(*m_prevExerciseLevel).isEmpty())
      m_prevExerciseLevel->name.clear(); // Returned string means that the level doesn't match to current settings

  /** NOTE
   * Clazy tool blames emit call beneath that probably emit has no effect because it is made from constructor.
   * However clazy seems to be paranoid sometimes, cause it works fine.
   * To keep it quiet one may add 'no-incorrect-emit' to level 1 check when it is invoking.
   */
  emit lastExamFileChanged();
  emit recentModelChanged();
  emit openActChanged();

  if (GLOB->E->showVeryBeginHelp)
    QTimer::singleShot(100, [=]{ getHelpDialog(); });
}


QString TstartExamItem::lastExamFile() const {
  return m_recentExams.isEmpty() ? QString() : m_recentExams.first();
}


QString TstartExamItem::prevLevelName() const {
  return m_prevExerciseLevel->name;
}


void TstartExamItem::continueTheLast() {
  if (m_recentExams.size()) {
      examToContSelected(m_recentExams[0]);
  }
}


void TstartExamItem::giveUserNameMessage() {
// #if defined (Q_OS_ANDROID)
//   QString name = QInputDialog::getText(this, tr("Give an user name!"), tr("Enter your name or nick-name."));
//   if (name.isEmpty())
//     name = QStringLiteral("Android");
//   m_nameEdit->setText(name);
// #else
  QMessageBox::warning(nullptr, QString(), QApplication::translate("StartExam", "Give an user name!"));
// #endif
}


void TstartExamItem::continuePrevExam() {
  auto action = qobject_cast<Taction*>(sender());
  if (action)
    examToContSelected(action->property("path").toString());
}


void TstartExamItem::examFromFileDialog() {
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getOpenFileName(GLOB->E->examsDir, QStringLiteral("noo"));
#else
  QString fileName = TfileDialog::getOpenFileName(TexTrans::loadExamFileTxt(), GLOB->E->examsDir, TexTrans::examFilterTxt());
#endif
  if (!fileName.isEmpty())
    examToContSelected(fileName);
}


void TstartExamItem::examToContSelected(const QString& eFile) {
  if (!eFile.isEmpty()) {
    GLOB->E->examsDir = QFileInfo(eFile).absoluteDir().absolutePath();
    m_recentExams.prepend(eFile);
    m_selectedExamFile = eFile;
    emit continueExam(eFile);
  }
}



void TstartExamItem::levelWasSelected(const Tlevel& level) {
//   m_prevExerciseLevel.name.clear(); // Reset the name - now level is taken from selection 
//   updateButtonStatusText(level.name);
// #if defined (Q_OS_ANDROID)
//   m_mobExerButton->setDisabled(false);
//   m_mobExamButton->setDisabled(false);
// #endif
}


QString TstartExamItem::exerOrExamHelpTxt() {
  QLatin1String br("<br>");
  return QLatin1String("<center><h2>") + ThelpDialogBase::pix("practice", 64) + QLatin1String(" ") +
  QApplication::translate("TstartExamDlg", "To exercise or to pass an exam?") +
  QLatin1String(" ") + ThelpDialogBase::pix("exam", 64) + QLatin1String("</h2>") + TmainHelp::youWillLearnText() + br + br +
  QLatin1String("</center><hr><table><tr><td style=\"padding: 10px;\" align=\"center\">") +
  TmainHelp::duringExercisingText() + br + TexamHelp::exerciseFeaturesText() +
  QLatin1String("</td></tr><tr><td style=\"padding: 10px;\" align=\"center\">") +
  TmainHelp::duringExamsText() + br + TexamHelp::examFeaturesText() + QLatin1String("</td></tr></table>") +
  ThelpDialogBase::onlineDocP(QStringLiteral("start-exam"));
}


void TstartExamItem::getHelpDialog() {
  auto help = new ThelpDialogBase(nullptr);
#if defined (Q_OS_ANDROID)
  help->showMaximized();
#else
  help->setFixedSize(width() * 0.8, height() * 0.8);
#endif
//   QLatin1String br("<br>");
//   QString ht = QLatin1String("<center><h2>") + help->pix("practice", 64) + QLatin1String(" ") +
//   QApplication::translate("TstartExamDlg", "To exercise or to pass an exam?") +
//   QLatin1String(" ") + help->pix("exam", 64) + QLatin1String("</h2>") + TmainHelp::youWillLearnText() + br + br +
//   QLatin1String("</center><hr><table><tr><td style=\"padding: 10px;\" align=\"center\">") +
//   TmainHelp::duringExercisingText() + br + TexamHelp::exerciseFeaturesText() +
//   QLatin1String("</td></tr><tr><td style=\"padding: 10px;\" align=\"center\">") +
//   TmainHelp::duringExamsText() + br + TexamHelp::examFeaturesText() + QLatin1String("</td></tr></table>") +
//   help->onlineDocP(QStringLiteral("start-exam"));

  help->helpText()->setHtml(exerOrExamHelpTxt());
  help->showCheckBox(&GLOB->E->showVeryBeginHelp);
//     qDebug() << help->helpText()->toHtml();
  help->exec();
  delete help;
}









