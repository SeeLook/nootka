/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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


#include "texamsummary.h"
#include "texamexecutor.h"
#include "texamview.h"
#include "dialogs/tlevelpreviewitem.h"
#include <exam/texam.h>
#include <exam/tqaunit.h>
#include <exam/textrans.h>
#include <exam/tlevel.h>
#include <tpath.h>
#if defined (Q_OS_ANDROID)
  #include <Android/tandroid.h>
  #include <qtr.h>
#endif
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>


/**
 * returns 2 columns row of table
 */
QString row2(const QString& S1, const QString& S2) {
  return QString("<tr><td>%1: </td><td><b>%2</b></td></tr>").arg(S1, S2);
}


TexamSummary::TexamSummary(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_exam = EXECUTOR->exam();

/*
  TroundedLabel *questNrLab = new TroundedLabel("<center>" + tr("Number of questions:") + QString("%2  %1</big></b>").arg(exam->count()).arg(font20) +
                    QString("<br>%1: %2%3</big></b>").arg(TexTrans::corrAnswersNrTxt()).arg(font20).
                        arg(exam->count() - exam->mistakes() - exam->halfMistaken()) +
                    QString("<br>%1: %2%3</big></b>").arg(TexTrans::mistakesNrTxt()).arg(font20).arg(exam->mistakes()) +
                    QString("<br>%1: %2%3</big></b>").arg(TexTrans::halfMistakenTxt()).arg(font20).arg(exam->halfMistaken())
      ,this);
  QGroupBox *timeGr = new QGroupBox(tr("times:"), this);
  TroundedLabel *timeLab = new TroundedLabel("<table>" +
  row2(TexTrans::totalTimetxt(), TexamView::formatedTotalTime(exam->totalTime() * 1000)) +
  row2(tr("Time taken to answer"), TexamView::formatedTotalTime(exam->workTime() * 1000)) +
  row2(TexTrans::averAnsverTimeTxt(), QString("%1 s").
      arg((qreal)exam->averageReactonTime()/10.0, 0, 'f', 1, '0')) +
  "</table>", this);
  */
  QString effStr;
  if (m_exam->mistakes() || m_exam->halfMistaken()) {
    float wAccid = 0.0, wKey = 0.0, wNote = 0.0, wOctave = 0.0, wStyle = 0.0, wPos = 0.0, wString = 0.0, wTotal;
    float wInto = 0.0, wLittle = 0.0, wPoor = 0.0;
    for(int i = 0; i < m_exam->count(); ++i) {
      if (!m_exam->question(i)->isCorrect()) {
          if(m_exam->question(i)->wrongAccid())       wAccid++;
          if(m_exam->question(i)->wrongKey())         wKey++;
          if(m_exam->question(i)->wrongNote())        wNote++;
          if(m_exam->question(i)->wrongOctave())      wOctave++;
          if(m_exam->question(i)->wrongStyle())       wStyle++;
          if(m_exam->question(i)->wrongPos())         wPos++;
          if(m_exam->question(i)->wrongString())      wString++;
          if(m_exam->question(i)->wrongIntonation())  wInto++;
          if(m_exam->question(i)->littleNotes())      wLittle++;
          if(m_exam->question(i)->poorEffect())       wPoor++;
      }
    }
    effStr += QLatin1String("<tr><td colspan=\"2\">-------- ") + tr("Kinds of mistakes") + QLatin1String(": --------</td></tr>");
    wTotal = wAccid + wKey + wNote + wOctave + wStyle + wPos + wString + wInto + wLittle + wPoor;
    QString cp = QStringLiteral("%)"); // closing percent '%)'
    if (wNote) {
      effStr += row2(tr("Wrong notes"), QString("%1 (").arg(wNote) + QString::number(qRound(wNote * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wNote);
    }
    if (wAccid) {
      effStr += row2(tr("Wrong accidentals"), QString("%1 (").arg(wAccid) + QString::number(qRound(wAccid * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wAccid);
    }
    if (wKey) {
      effStr += row2(tr("Wrong key signatures"), QString("%1 (").arg(wKey) + QString::number(qRound(wKey * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wKey);
    }
    if (wOctave){
      effStr += row2(tr("Wrong octaves"), QString("%1 (").arg(wOctave) + QString::number(qRound(wOctave * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wOctave);
    }
    if (wStyle) {
      effStr += row2(tr("Wrong note names"), QString("%1 (").arg(wStyle)) + QString::number(qRound(wStyle * 100.0 / wTotal)) + cp;
      m_kindOfMistakes << static_cast<int>(wStyle);
    }
    if (wPos) {
      effStr += row2(tr("Wrong positions on guitar"), QString("%1 (").arg(wPos) + QString::number(qRound(wPos * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wPos);
    }
    if (wString) {
      effStr += row2(tr("Wrong strings"), QString("%1 (").arg(wString) + QString::number(qRound(wString * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wString);
    }
    if (wInto) {
      effStr += row2(tr("Out of tune"), QString("%1 (").arg(wInto) + QString::number(qRound(wInto * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wInto);
    }
    if (wLittle) {
      effStr += row2(QApplication::translate("AnswerText", "little valid notes", "the amount of correct notes in an answer is little"),
                     QString("%1 (").arg(wLittle) + QString::number(qRound(wLittle * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wLittle);
    }
    if (wPoor) {
      effStr += row2(QApplication::translate("AnswerText", "poor effectiveness"),
                     QString("%1 (").arg(wPoor) + QString::number(qRound(wPoor * 100.0 / wTotal)) + cp);
      m_kindOfMistakes << static_cast<int>(wPoor);
    }
  }
  m_results = tr("Results:") + QLatin1String("<table>") +
    row2(TexTrans::effectTxt(), QString::number(qRound(m_exam->effectiveness())) + QLatin1String("%")) + effStr + QLatin1String("</table>");
}


QString TexamSummary::title() const {
  if (m_exam) {
    if (m_exam->isExercise())
      return tr("Progress of exercises");
    else
      return tr("Exam results");
  }
  return QString();
}


void TexamSummary::setLevelPreview(TlevelPreviewItem* lp) {
  m_levelPreview = lp;
  m_levelPreview->setLevel(m_exam->level());
}


QString TexamSummary::student() const {
  return tr("student:") + QString("<font size=\"5\"><b>  %1</b></font>").arg(m_exam->userName());
}


bool TexamSummary::isExercise() const {
  return m_exam && m_exam->isExercise();
}


bool TexamSummary::continueExecutor() {
  m_accepted = true;
  if (m_exam->isExercise()) {
      EXECUTOR->continueExercise();
      return true;
  } else
      return EXECUTOR->continueInit();
}


void TexamSummary::exerciseToExam() {
  m_accepted = true;
  EXECUTOR->restoreExerciseAfterSummary();
  EXECUTOR->exerciseToExam();
}


void TexamSummary::closeSummary() {
  if (!m_accepted) {
    if (EXECUTOR->summaryReason() == TexamExecutor::SumFinishExer)
      QTimer::singleShot(10, [=]{ EXECUTOR->finishExerciseAfterSummary(); });
    else
      QTimer::singleShot(10, [=]{ EXECUTOR->closeExecutor(); });
  }
}


bool TexamSummary::discardedAtBegin() const {
  return EXECUTOR->summaryReason() == TexamExecutor::SumContExam && !EXECUTOR->isInitialized();
}


bool TexamSummary::enableContinue() const {
  return m_exam->isExercise() || !EXECUTOR->isInitialized();
}


int TexamSummary::buttColumsCount() const {
  return (!enableContinue() && isExercise()) || (enableContinue() && !isExercise()) ? 3 : 2;
}


#if defined (Q_OS_ANDROID)
void TexamSummary::sendExam() {
  QString space = QStringLiteral(" ");
  QString br = QStringLiteral("\n");
  QString message = qTR("TexamSummary", "student:") + space + m_exam->userName() + br;
  message += qTR("AnalyzeDialog", "level:") + space + m_exam->level()->name + br;
  message += tr("Number of questions:") + space + QString::number(m_exam->count()) + br;
  Tandroid::sendExam(tr("Send exam file"), message, m_exam->fileName());
}
#endif

