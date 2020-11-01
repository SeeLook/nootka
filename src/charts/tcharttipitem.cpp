/***************************************************************************
 *   Copyright (C) 2018-2020 by Tomasz Bojczuk                             *
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

#include "tcharttipitem.h"
#include "tgroupedqaunit.h"
#include <tcolor.h>
#include <tnootkaqml.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/tresulttext.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include <score/tscoreobject.h>
#include <score/tstaffitem.h>
#include <score/tnoteitem.h>
#include <tglobals.h>
#include <qtr.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>


QColor answerColor(quint32 mistake) {
  if (mistake == static_cast<quint32>(TQAunit::e_correct))
    return GLOB->correctColor();
  if (!(mistake & TQAunit::e_wrongPos) && !(mistake & TQAunit::e_wrongNote) && !(mistake & TQAunit::e_veryPoor))
    return GLOB->notBadColor();
  else
    return GLOB->wrongColor();
}


TchartTipItem::TchartTipItem(QQuickItem* parent) :
  QQuickItem(parent)
{
}


TchartTipItem::~TchartTipItem()
{
}


int TchartTipItem::number() const {
  return m_question ? m_question->nr() : 0;
}


QColor TchartTipItem::color() const {
  return m_question ? m_question->color() : QColor(0, 0, 0, 0);
}


QPointF TchartTipItem::pos() const {
  return m_question ? m_question->cursorPos() : QPointF();
}


QString TchartTipItem::qaText() const {
  if (m_question) {
    if (m_question->qaUnit()->melody()) {
        QString qaT = qTR("QuestionsBox", "question") + QLatin1String(": <b>");
        if (m_question->qaUnit()->questionOnScore() && m_question->qaUnit()->answerAsSound())
          qaT += TexTrans::playMelodyTxt();
        else if (m_question->qaUnit()->questionAsSound() && m_question->qaUnit()->answerOnScore())
          qaT += TexTrans::writeMelodyTxt();
        else
          return QString();
        return qaT + QLatin1String("</b>");
    } else {
        return qTR("QuestionsBox", "question") + QLatin1String(": <b>") + NOO->qaTypeText(m_question->qaUnit()->questionAs)
                + QLatin1String("</b><br>") + qTR("QuestionsBox", "answer") + QLatin1String(": <b>")
                + NOO->qaTypeText(m_question->qaUnit()->answerAs);
    }
  }
  return QString();
}


bool TchartTipItem::isMelody() const {
  return m_exam && m_exam->melodies();
}


bool TchartTipItem::hasSecondScore() const {
  return m_exam && !m_exam->melodies() && m_exam->level()->answerIsNote();
}


bool TchartTipItem::leftScoreVisible() const {
  return m_question && (m_question->qaUnit()->melody()
                        || (m_question->qaUnit()->questionOnScore()
                        || (m_question->qaUnit()->questionAsSound() && m_question->qaUnit()->answerAsSound())));
}


bool TchartTipItem::rightScoreVisible() const {
  return m_question && !m_exam->melodies() && m_question->qaUnit()->answerOnScore();
}


QString TchartTipItem::questionText() const {
  if (m_question && (!m_question->qaUnit()->questionOnScore() || !m_question->qaUnit()->questionAsSound())) {
    if (m_question->qaUnit()->questionAsName())
      return QLatin1String("<span style=\"font-size: xx-large;\">") + m_question->qaUnit()->qa.note.toRichText(m_question->qaUnit()->styleOfQuestion())
             + QLatin1String("</span>");
    else if (m_question->qaUnit()->questionOnInstr())
      return m_question->qaUnit()->qa.pos().toHtml();
  }
  return QString();
}


QString TchartTipItem::answerText() const {
  if (m_question && !m_question->qaUnit()->answerOnScore()) {
    TQAgroup& qaGroup = m_question->qaUnit()->questionAs == m_question->qaUnit()->answerAs ? m_question->qaUnit()->qa_2 : m_question->qaUnit()->qa;
    if (m_question->qaUnit()->answerAsName())
      return QLatin1String("<span style=\"font-size: xx-large;\">") + qaGroup.note.toRichText(m_question->qaUnit()->styleOfAnswer())
             + QLatin1String("</span>");
    else if (m_question->qaUnit()->answerOnInstr())
      return qaGroup.pos().toHtml();
    else if (m_question->qaUnit()->answerAsSound())
      return QString(" <span style=\"font-size: %1px; font-family: Nootka\">n</span>").arg(NOO->fontSize() * 3);
  }
  return QString();
}


qreal TchartTipItem::yScoreLeftOff() const {
  return m_leftScore && m_exam->melodies() ? m_leftScore->firstStaff()->hiNotePos() * -m_leftScore->firstStaff()->scale() : 0.0;
}


qreal TchartTipItem::yScoreRightOff() const {
  return m_secondScore && m_exam->melodies() ? m_secondScore->firstStaff()->hiNotePos() * -m_secondScore->firstStaff()->scale() : 0.0;
}


qreal TchartTipItem::leftScoreHeight() const {
  return m_leftScore ? (m_leftScore->firstStaff()->minHeight() - 2.0) * m_leftScore->firstStaff()->scale() : 0.0;
}


qreal TchartTipItem::rightScoreHeight() const {
  return m_secondScore ? (m_secondScore->firstStaff()->minHeight() - 2.0) * m_secondScore->firstStaff()->scale() : 0.0;
}


int TchartTipItem::attempts() const {
  return m_question ? m_question->qaUnit()->attemptsCount() : 0;
}


void TchartTipItem::setLeftScore(TscoreObject* ls) {
  if (ls != m_leftScore) {
    m_leftScore = ls;
  }
}


void TchartTipItem::setSecondScore(TscoreObject* ss) {
  if (ss != m_secondScore) {
    m_secondScore = ss;
  }
}


QString TchartTipItem::resultText() const {
  if (m_question && m_question->qaUnit()) {
    return wasAnswerOKtext(m_question->qaUnit()).replace(QLatin1String("<br>"), QLatin1String(" "));
  }
  return QString();
}


QString TchartTipItem::timeText() const {
  if (m_question) {
    QString txt;
    if (m_question->qaUnit()->melody()) {
      txt += tr("Melody was played <b>%n</b> times", "", m_question->qaUnit()->totalPlayBacks()) + QLatin1String("<br>");
      if (!m_question->qaUnit()->isWrong())
        txt += TexTrans::effectTxt() + QString(": <b>%1%</b>, ").arg(m_question->qaUnit()->effectiveness(), 0, 'f', 1, '0');
    }
    txt += TexTrans::reactTimeTxt() + QString("<b>  %1</b>").arg(Texam::formatReactTime(m_question->qaUnit()->time, true));
    return txt;
  }
  return QString();
}


QString TchartTipItem::tipText() const {
  return m_question ? m_question->tipText : QString();
}


void TchartTipItem::setQuestion(TtipInfo* q) {
  if (m_question != q) {
    bool emitShow = m_question == nullptr || q == nullptr;
    m_question = q;
    if (emitShow)
      emit showChanged();
    if (m_question) {
      emit questionUpdated();
      if (m_question->kind == TtipInfo::e_point) {
          emit questionTipUpdated();
          if (m_leftScore) {
            if (m_question->qaUnit()->melody()) {
                int transposition = 0;
                auto tempKey = m_question->qaUnit()->melody()->key();
                if (m_question->qaUnit()->melody()->key() != m_question->qaUnit()->key) {
                  transposition = m_question->qaUnit()->melody()->key().difference(m_question->qaUnit()->key);
                  m_question->qaUnit()->melody()->setKey(m_question->qaUnit()->key);
                }
                m_leftScore->setMelody(m_question->qaUnit()->melody(), true, 7, transposition);
                m_question->qaUnit()->melody()->setKey(tempKey);
            } else {
                if (m_question->qaUnit()->questionOnScore() || (m_question->qaUnit()->questionAsSound() && m_question->qaUnit()->answerAsSound())) {
                  if (m_leftScore->notesCount())
                    m_leftScore->setNote(0, m_question->qaUnit()->qa.note);
                  else
                    m_leftScore->addNote(m_question->qaUnit()->qa.note);
                  if (m_exam->level()->useKeySign)
                    m_leftScore->setKeySignature(m_question->qaUnit()->key.value());
                }
                bool qaTheSameType = m_question->qaUnit()->questionAs == m_question->qaUnit()->answerAs;
                if (m_question->qaUnit()->answerOnScore()) {
                  if (m_secondScore->notesCount())
                    m_secondScore->setNote(0, qaTheSameType ? m_question->qaUnit()->qa_2.note : m_question->qaUnit()->qa.note);
                  else
                    m_secondScore->addNote(qaTheSameType ? m_question->qaUnit()->qa_2.note : m_question->qaUnit()->qa.note);
                  if (m_exam->level()->useKeySign)
                    m_secondScore->setKeySignature(m_question->qaUnit()->key.value());
                }
            }
            emit questionWasSet();
          }
      } else if (m_question->kind == TtipInfo::e_line) {
          emit lineTipUpdated();
      }
      if (m_kindOfTip != m_question->kind) {
        m_kindOfTip = m_question->kind;
        emit tipTypeChanged();
      }
    }
  }
}


void TchartTipItem::setExam(Texam* e) {
  m_exam = e;
  m_leftScore->clearScore();
  emit examChanged();
  if (!m_exam->melodies()) {
    m_leftScore->setClefType(m_exam->level()->clef.type());
    m_leftScore->setMeter(Tmeter::NoMeter);
    m_leftScore->setKeySignatureEnabled(m_exam->level()->useKeySign);
  }
  if (hasSecondScore()) {
    m_secondScore->setMeter(Tmeter::NoMeter);
    m_secondScore->setClefType(m_exam->level()->clef.type());
    m_secondScore->setKeySignatureEnabled(m_exam->level()->useKeySign);
  }
}


int TchartTipItem::tipType() const {
  return m_kindOfTip;
}


void TchartTipItem::setAttemptNr(int attNr) {
  auto lastUnit = m_question ? m_question->qaUnit() : nullptr;
  if (lastUnit && lastUnit->attemptsCount()) {
    for (int n = 0; n < m_leftScore->notesCount(); ++n) {
      if (attNr > 0)
        m_leftScore->note(n)->markNoteHead(answerColor(lastUnit->attempt(attNr - 1)->mistakes[n]));
      else
        m_leftScore->note(n)->markNoteHead(Qt::transparent);
    }
  }
}


QString TchartTipItem::attemptDetails(int attNr) const {
  auto lastUnit = m_question ? m_question->qaUnit() : nullptr;
  if (attNr > 0 && lastUnit && lastUnit->attemptsCount() && lastUnit->attempt(attNr - 1)) {
    static const QString coma = QStringLiteral(", ");
    return QString("<b>%1: </b>").arg((attNr)) + qApp->translate("ChartTip", "played", "a melody was played (and number follows)")
          + QString(" <b>%1</b>").arg(lastUnit->attempt(attNr - 1)->playedCount()) + coma + TexTrans::effectTxt().toLower()
          + QString(": <b>%1%</b>").arg(lastUnit->attempt(attNr - 1)->effectiveness(), 0, 'f', 1, '0') + coma
          + qApp->translate("ChartTip", "time") + ": " + QString("<b>  %1</b>").arg(Texam::formatReactTime(lastUnit->attempt(attNr - 1)->totalTime(), true));
  }
  return QString();
}


QString TchartTipItem::attemptResult(int attNr) const {
  if (m_question && attNr > 0) {
    auto lastUnit = m_question->qaUnit();
    QColor attemptColor = GLOB->correctColor();
    if (lastUnit->attempt(attNr - 1) && lastUnit->attempt(attNr - 1)->summary()) { // something was wrong
      if (lastUnit->attempt(attNr - 1)->summary() & TQAunit::e_wrongNote)
        attemptColor = GLOB->wrongColor();
      else
        attemptColor = GLOB->notBadColor();
    }
    return QString("<font color=\"%1\">").arg(attemptColor.name())
    + wasAnswerOKtext(lastUnit, attNr).replace(QLatin1String("<br>"), QLatin1String(" "))
    + QLatin1String("</font>");
  }
  return QString();
}
