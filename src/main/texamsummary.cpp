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

#include "texamsummary.h"
#include "dialogs/tlevelpreviewitem.h"
#include "texamexecutor.h"
#include "texamview.h"
#include <exam/tattempt.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <exam/tlevel.h>
#include <exam/tqaunit.h>
#include <qtr.h>
#include <tpath.h>
#if defined(Q_OS_ANDROID)
#include <Android/tandroid.h>
#include <qtr.h>
#endif
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>

inline QStringList getDelegate(const QString &label, float value, float wTotal)
{
    return QStringList() << label + QLatin1String(": ") << QString(" <b>%1</b> (%2\%)").arg(value).arg(qRound(value * 100.0 / wTotal));
}

inline QStringList timeDelegate(const QString &label, const QString &time)
{
    return QStringList() << label + QLatin1String(": ") << QString(" <b>%1</b>").arg(time);
}

TexamSummary::TexamSummary(QQuickItem *parent)
    : QQuickItem(parent)
{
    m_exam = EXECUTOR->exam();

    m_answersLabel << tr("Number of questions:") << QString(" <b>%1</b>").arg(m_exam->count()) << TexTrans::corrAnswersNrTxt()
                   << QString("<b>%1</b>").arg(m_exam->count() - m_exam->mistakes() - m_exam->halfMistaken()) << TexTrans::mistakesNrTxt() + QLatin1String(":")
                   << QString("<b>%1</b>").arg(m_exam->mistakes()) << TexTrans::halfMistakenTxt() + QLatin1String(":")
                   << QString("<b>%1</b>").arg(m_exam->halfMistaken());

    m_answersModel << m_exam->count() - m_exam->mistakes() - m_exam->halfMistaken();
    m_answersModel << m_exam->halfMistaken();
    m_answersModel << m_exam->mistakes();

    m_timesModel << timeDelegate(TexTrans::totalTimetxt(), TexamView::formatedTotalTime(m_exam->totalTime() * 1000));
    m_timesModel << timeDelegate(tr("Time taken to answer"), TexamView::formatedTotalTime(m_exam->workTime() * 1000));
    m_timesModel << timeDelegate(TexTrans::averAnsverTimeTxt(), QString("%1 s").arg(static_cast<qreal>(m_exam->averageReactonTime()) / 10.0, 0, 'f', 1, '0'));

    if (m_exam->mistakes() || m_exam->halfMistaken()) {
        float wAccid = 0.0f, wKey = 0.0f, wNote = 0.0f, wOctave = 0.0f, wStyle = 0.0f, wPos = 0.0f, wString = 0.0f, wTotal;
        float wInto = 0.0f, wLittle = 0.0f, wPoor = 0.0f, wRtm = 0.0f;
        for (int i = 0; i < m_exam->count(); ++i) {
            if (!m_exam->question(i)->isCorrect()) {
                if (m_exam->question(i)->wrongAccid())
                    wAccid++;
                if (m_exam->question(i)->wrongKey())
                    wKey++;
                if (m_exam->question(i)->wrongNote())
                    wNote++;
                if (m_exam->question(i)->wrongOctave())
                    wOctave++;
                if (m_exam->question(i)->wrongStyle())
                    wStyle++;
                if (m_exam->question(i)->wrongPos())
                    wPos++;
                if (m_exam->question(i)->wrongString())
                    wString++;
                if (m_exam->question(i)->wrongIntonation())
                    wInto++;
                if (m_exam->question(i)->littleNotes())
                    wLittle++;
                if (m_exam->question(i)->poorEffect())
                    wPoor++;
                if (m_exam->question(i)->wrongRhythm())
                    wRtm++;
            }
        }

        wTotal = wAccid + wKey + wNote + wOctave + wStyle + wPos + wString + wInto + wLittle + wPoor + wRtm;
        if (wNote) {
            m_resultsModel << getDelegate(tr("Wrong notes"), wNote, wTotal);
            m_kindOfMistakes << static_cast<int>(wNote);
        }
        if (wAccid) {
            m_resultsModel << getDelegate(tr("Wrong accidentals"), wAccid, wTotal);
            m_kindOfMistakes << static_cast<int>(wAccid);
        }
        if (wKey) {
            m_resultsModel << getDelegate(tr("Wrong key signatures"), wKey, wTotal);
            m_kindOfMistakes << static_cast<int>(wKey);
        }
        if (wOctave) {
            m_resultsModel << getDelegate(tr("Wrong octaves"), wOctave, wTotal);
            m_kindOfMistakes << static_cast<int>(wOctave);
        }
        if (wStyle) {
            m_resultsModel << getDelegate(tr("Wrong note names"), wStyle, wTotal);
            m_kindOfMistakes << static_cast<int>(wStyle);
        }
        if (wPos) {
            m_resultsModel << getDelegate(tr("Wrong positions on guitar"), wPos, wTotal);
            m_kindOfMistakes << static_cast<int>(wPos);
        }
        if (wString) {
            m_resultsModel << getDelegate(tr("Wrong strings"), wString, wTotal);
            m_kindOfMistakes << static_cast<int>(wString);
        }
        if (wInto) {
            m_resultsModel << getDelegate(tr("Out of tune"), wInto, wTotal);
            m_kindOfMistakes << static_cast<int>(wInto);
        }
        if (wRtm) {
            m_resultsModel << getDelegate(qTR("AnswerText", "incorrect rhythm"), wRtm, wTotal);
            m_kindOfMistakes << static_cast<int>(wRtm);
        }
        if (wLittle) {
            m_resultsModel << getDelegate(qTR("AnswerText", "little valid notes"), wLittle, wTotal);
            m_kindOfMistakes << static_cast<int>(wLittle);
        }
        if (wPoor) {
            m_resultsModel << getDelegate(qTR("AnswerText", "poor effectiveness"), wPoor, wTotal);
            m_kindOfMistakes << static_cast<int>(wPoor);
        }
    }
    m_resultHeader = tr("Results:") + QLatin1String("<br>") + TexTrans::effectTxt() + QString(": <b>%1\%</b>").arg(qRound(m_exam->effectiveness()));
}

QString TexamSummary::title() const
{
    if (m_exam) {
        if (m_exam->isExercise())
            return tr("Progress of exercises");
        else
            return tr("Exam results");
    }
    return QString();
}

void TexamSummary::setLevelPreview(TlevelPreviewItem *lp)
{
    m_levelPreview = lp;
    m_levelPreview->setLevel(m_exam->level());
}

QString TexamSummary::student() const
{
    return tr("student:") + QString("<font size=\"5\"><b>  %1</b></font>").arg(m_exam->userName());
}

bool TexamSummary::isExercise() const
{
    return m_exam && m_exam->isExercise();
}

void TexamSummary::continueExecutor()
{
    m_accepted = true;
    if (m_exam->isExercise())
        EXECUTOR->continueExercise();
    else
        EXECUTOR->continueInit();
}

bool TexamSummary::hasQuestions() const
{
    return m_exam && m_exam->count() > 0;
}

void TexamSummary::exerciseToExam()
{
    m_accepted = true;
    EXECUTOR->restoreExerciseAfterSummary();
    EXECUTOR->exerciseToExam();
}

void TexamSummary::closeSummary()
{
    if (!m_accepted) {
        if (EXECUTOR->summaryReason() == TexamExecutor::SumFinishExer)
            QTimer::singleShot(10, [=] {
                EXECUTOR->finishExerciseAfterSummary();
            });
        else
            QTimer::singleShot(10, [=] {
                EXECUTOR->closeExecutor();
            });
    }
}

bool TexamSummary::discardedAtBegin() const
{
    return EXECUTOR->summaryReason() == TexamExecutor::SumContExam && !EXECUTOR->isInitialized();
}

bool TexamSummary::enableContinue() const
{
    return m_exam->isExercise() || !EXECUTOR->isInitialized();
}

int TexamSummary::buttColumsCount() const
{
    return (!enableContinue() && isExercise()) || (enableContinue() && !isExercise()) ? 3 : 2;
}

#if defined(Q_OS_ANDROID)
void TexamSummary::sendExam()
{
    QString space = QStringLiteral(" ");
    QString br = QStringLiteral("\n");
    QString message = qTR("TexamSummary", "student:") + space + m_exam->userName() + br;
    message += qTR("AnalyzeDialog", "level:") + space + m_exam->level()->name + br;
    message += tr("Number of questions:") + space + QString::number(m_exam->count()) + br;
    // Tandroid::sendExam(tr("Send exam file"), message, m_exam->fileName());
}
#endif
