/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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
#include "help/tmainhelp.h"
#include "help/texamhelp.h"
#include "dialogs/tlevelselector.h"
#include <tglobals.h>
#include <tnootkaqml.h>
#include <taction.h>
#include <tpath.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <Android/tfiledialog.h>

#include <QtCore/qsettings.h>
#include <QtCore/qtimer.h>
#include <QtQml/qqmlengine.h>
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
  if (!TlevelSelector::checkLevel(*m_prevExerciseLevel).isEmpty() || m_prevExerciseLevel->instrument != GLOB->instrument().type())
    m_prevExerciseLevel->name.clear(); // Returned string means that the level doesn't match to current settings

  /** NOTE
   * Clazy tool blames emit call beneath that probably emit has no effect because it is made from constructor.
   * However clazy seems to be paranoid sometimes, cause it works fine.
   * To keep it quiet one may add 'no-incorrect-emit' to level 1 check when it is invoking.
   */
  emit lastExamFileChanged();
  emit recentModelChanged();
  emit openActChanged();
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


void TstartExamItem::continuePrevExam() {
  auto action = qobject_cast<Taction*>(sender());
  if (action)
    examToContSelected(action->property("path").toString());
}


void TstartExamItem::examFromFileDialog() {
#if defined (Q_OS_ANDROID)
  if (GLOB->E->examsDir.isEmpty())
    GLOB->E->examsDir = Tandroid::getExternalPath();
  QString fileName = TfileDialog::getOpenFileName(GLOB->E->examsDir, QStringLiteral("noo"));
#else
  QString fileName = TfileDialog::getOpenFileName(TexTrans::loadExamFileTxt(), GLOB->examParams->examsDir, TexTrans::examFilterTxt());
#endif
  if (!fileName.isEmpty())
    examToContSelected(fileName);
}


void TstartExamItem::examToContSelected(const QString& eFile) {
  if (!eFile.isEmpty()) {
    GLOB->examParams->examsDir = QFileInfo(eFile).absoluteDir().absolutePath();
    m_recentExams.prepend(eFile);
    m_selectedExamFile = eFile;
    emit continueExam(eFile);
  }
}
