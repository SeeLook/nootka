/***************************************************************************
 *   Copyright (C) 2012-2020 by Tomasz Bojczuk                             *
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


#include "ttiphandler.h"
//#include "tnootkacertificate.h"
#include "texecutorsupply.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/tresulttext.h>
#include <exam/tlevel.h>
#include <tnootkaqml.h>
#include <instruments/tcommoninstrument.h>
#include <exam/textrans.h>
#include <taction.h>
#include <tcolor.h>
#include "tglobals.h"
#include "help/texamhelp.h"
#include "texamexecutor.h"
#include "tnameitem.h"
#include "tmainscoreobject.h"
#include <tsound.h>
#include <tpath.h>
#include <texamparams.h>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
  #include "mobile/tmobilemenu.h"
#endif
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>


#if defined (Q_OS_ANDROID)
//inline QString getTipText(const char* iconName, const char* barText) {
//  return pixToHtml(Tpath::img(iconName), qRound(Tmtr::fingerPixels() * 0.7)) + QLatin1String("<br>") +
//         QApplication::translate("TtoolBar", barText);
//}


QFont smalTipFont(QWidget* w) {
  int bSize = qBound(qRound(Tmtr::fingerPixels() * 1.1), Tmtr::longScreenSide() / 12, qRound(Tmtr::fingerPixels() * 1.6));
  QFont f = w->font();
  f.setPixelSize(qMin<int>(bSize / 5, w->fontMetrics().height()));
  return f;
}


/** Multiplexer of question tip scale factor to make it big enough on hi dpi tablet screens */
inline qreal multiScale() {
  return (static_cast<qreal>(Tmtr::shortScreenSide()) / Tmtr::fingerPixels()) / 5.0;
}

#endif

QString getTextHowAccid(Tnote::Ealter accid) {
  QString S = QString("<br><span style=\"color: %1\">").arg(GLOB->GselectedColor.name());
  if (accid)
    S += qApp->translate("TtipHandler", "Use %1").arg(QString::fromStdString(Tnote::alterSymbol(static_cast<int>(accid) + 2)));
  else
    S += qApp->translate("TtipHandler", " Don't use accidentals!");
  S +=  QLatin1String("</span>");
  return S;
}


QString onStringTxt(quint8 strNr) {
    return QLatin1String("<b>") + qApp->translate("TtipHandler", "on %1 string.").arg(QString("</b><span style=\"font-family: nootka;\">%1</span><b>").arg(strNr))
          + QLatin1String("</b>");
}


QString playOrSing(int instr) {
  if (static_cast<Tinstrument::Etype>(instr) == Tinstrument::NoInstrument)
    return qApp->translate("TtipHandler", "Play or sing");
  else
    return qApp->translate("TtipHandler", "Play");
}


QString getNiceNoteName(Tnote& note, Tnote::EnameStyle style) {
  return QString("<b><big>") + note.toRichText(style) + QLatin1String("</big></b>");
}


TtipHandler::TtipHandler(Texam* exam, QObject *parent) :
  QObject(parent),
  m_exam(exam),
  m_timerToConfirm(new QTimer(this)),
  m_timerOfWhatNext(new QTimer(this)),
  m_questTipPosType(e_bottomRight),
  m_iconSize(bigFont() * 1.2)
{
 connect(m_timerToConfirm, &QTimer::timeout, this, &TtipHandler::showConfirmTipSlot);
 connect(m_timerOfWhatNext, &QTimer::timeout, this, [=]{
   m_timerOfWhatNext->stop();
   emit wantWhatNextTip(m_whatNextText, qApp->palette().highlight().color(), getTipPosition(determineTipPos()));
  });
 qApp->installEventFilter(this);
//   int levelMessageDelay = 1;
//   if (TexecutorSupply::paramsChangedMessage())
//       levelMessageDelay = 7000;
//  QTimer::singleShot(levelMessageDelay, this, SLOT(levelStatusMessage()));
}

TtipHandler::~TtipHandler() {
  m_timerToConfirm->stop();
  m_timerOfWhatNext->stop();
#if defined (Q_OS_ANDROID)
  MOBILE_MENU->setExamMenuEntries(nullptr, nullptr, nullptr, nullptr);
#endif
}


void TtipHandler::changeExam(Texam* newExam) {
  m_exam = newExam;
}


//void TtipHandler::setStatusMessage(const QString& text, int duration) {
//#if defined (Q_OS_ANDROID)
//  tMessage->setMessage(text, duration);
//#else
//  STATUS->setMessage(text, duration);
//#endif
//}


//######################################################################
//##################################### TIPS ###########################
//######################################################################

int TtipHandler::bigFont() {
#if defined (Q_OS_ANDROID)
  return (NOO->fontSize() * 1.2);
#else
  return (NOO->fontSize() * 2);
#endif
}


QString TtipHandler::startTipText() {
  return TexamHelp::toGetQuestTxt() + QLatin1String(":<br>") +
    TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"nextQuest\">") +
                                  NOO->pixToHtml(QLatin1String("nextQuest"), m_iconSize) + QLatin1String("</a>"))
//    #if !defined (Q_OS_ANDROID)
//      + QLatin1String(",<br>") + TexamHelp::pressSpaceKey() + QLatin1String(" ") + TexamHelp::orRightButtTxt()
//    #endif
    ;
}


void TtipHandler::showStartTip() {
#if defined (Q_OS_ANDROID)
  QTimer::singleShot(500, this, [=]{
    m_startAct = new Taction(QGuiApplication::translate("Texam", "Let's start!"), QStringLiteral("nextQuest"), this);
    m_startAct->setBgColor(qApp->palette().highlight().color());
    connect(m_startAct, &Taction::triggered, EXECUTOR->nextQuestAct(), &Taction::trigger);
    MOBILE_MENU->setForceText(true);
    MOBILE_MENU->setFlyActions(EXECUTOR->stopExamAct(), nullptr,
                               m_startAct, nullptr, nullptr);
    MOBILE_MENU->setExamMenuEntries(m_startAct, EXECUTOR->stopExamAct(), nullptr, nullptr);
    m_startAct->shake();
    EXECUTOR->stopExamAct()->shake();
  });
//  QString tipText = QString("<table><tr><td style=\"font-size: %1px; vertical-align: middle;\">").arg(bigFont())
//      + QGuiApplication::translate("Texam", "Let's start")
//      + QLatin1String("  <a href=\"nextQuest\">")
//      + NOO->pixToHtml(QLatin1String("nextQuest"), m_iconSize) + QLatin1String("</a>")
//      + QLatin1String("</td></tr></table>");
#else
  QString tipText = QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75))
      + startTipText() + QLatin1String(".<br>")
      + TexamHelp::toStopExamTxt(QLatin1String("<a href=\"stopExam\"> ")
                                 + NOO->pixToHtml(QLatin1String("stopExam"), m_iconSize)
                                 + QLatin1String("</a>"))
      + QLatin1String("</p>");
  QTimer::singleShot(200, this, [=]{
    emit wantStartTip(tipText, qApp->palette().highlight().color(),
                      QPointF(EXECUTOR->width() / 2.0, EXECUTOR->height() / 2.0));
  });
#endif
}


void TtipHandler::showConfirmTip(int time) {
#if defined (Q_OS_ANDROID)
  Q_UNUSED(time);
#else
  deleteConfirmTip();
  m_timerToConfirm->start(time + 1); // add 1 to show it immediately when time = 0
#endif
  EXECUTOR->checkQuestAct()->shake();
}


void TtipHandler::showConfirmTipSlot() {
  m_timerToConfirm->stop();
  const QString br_ = QStringLiteral("<br>- ");
  const QString a = QStringLiteral("</a>");
  QString tipText = QLatin1String("<p style=\"text-align: center;\">") + tr("To check the answer confirm it:") + br_ +
    TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"checkAnswer\">") + NOO->pixToHtml(QLatin1String("check"), m_iconSize) + a) + br_ +
    TexamHelp::pressEnterKey() + br_ + TexamHelp::orRightButtTxt() + QLatin1String("<br>") +
    tr("Check in exam help %1 how to do it automatically").arg(QStringLiteral("<a href=\"examHelp\">") +
    NOO->pixToHtml(QLatin1String("help"), m_iconSize) + a) + QLatin1String("</p>");
  emit wantConfirmTip(tipText, GLOB->EanswerColor, QPointF(EXECUTOR->width() * 0.75, EXECUTOR->height() * 0.1));
}


void TtipHandler::showResultTip(TQAunit* answer, int time) {
  deleteResultTip();
  deleteConfirmTip();

 bool autoNext = GLOB->E->autoNextQuest;
 if (GLOB->E->afterMistake == TexamParams::e_stop && !answer->isCorrect())
     autoNext = false; // when mistake and e_stop - the same like autoNext = false;
 if (autoNext) { // determine time of displaying
   if (answer->isCorrect() || GLOB->E->afterMistake == TexamParams::e_continue)
     time = 2500; // hard-coded
   else
     time = GLOB->E->mistakePreview; // user defined wait time
 }

  emit wantResultTip(wasAnswerOKtext(answer), TexecutorSupply::answerColor(answer->mistake()));

//  if (answer->isNotSoBad())
//    m_resultTip->setScale(m_scale);
//  else
//    m_resultTip->setScale(m_scale * 1.2);
//  setResultPos();
//  if (GLOB->E->showWrongPlayed && GLOB->E->showWrongPlayed && !answer->melody() &&
//      answer->answerAsSound() && !answer->isCorrect() && SOUND->note().note)
//          detectedNoteTip(SOUND->note()); // In exercise mode display detected note when it was incorrect
  if (time)
    QTimer::singleShot(time, [=]{ deleteResultTip(); });
}


void TtipHandler::detectedNoteTip(const Tnote& note) {
  auto noteText = QStringLiteral("<big><b>") + (note.isValid() ? note.toRichText() : QStringLiteral("Rest"));
  if (m_exam->level()->useRhythms() && note.rtm.isValid())
    noteText += QLatin1String(" (") + note.rtm.string() + QLatin1String(")");
  NOO->showTimeMessage(tr("%1 was detected", "note name").arg(noteText + QLatin1String("</b></big>")), 5000, QQuickItem::Top);
}


void TtipHandler::shouldBeNoteTip(const Tnote& note) {
  auto noteText = QStringLiteral("<big><b>") + (note.isValid() ? note.toRichText() : QStringLiteral("Rest"));
  if (m_exam->level()->useRhythms() && note.rtm.isValid())
    noteText += QLatin1String(" (") + note.rtm.string() + QLatin1String(")");
  NOO->showTimeMessage(tr("It should be %1", "note name follows").arg(noteText + QLatin1String("</b></big>")), 5000, QQuickItem::Top);
}


void TtipHandler::showTryAgainTip(int time) {
  deleteWhatNextTip();
  deleteTryAgainTip();

  emit wantTryAgainTip();
  QTimer::singleShot(time, [=]{ deleteTryAgainTip(); });
}


void TtipHandler::certificateTip() {
  if (m_certifyTip)
    return;

  deleteQuestionTip();
  deleteResultTip();
  deleteWhatNextTip();
  emit wantCertificate();
}


void TtipHandler::deleteCertTip() {
  if (m_certifyTip) {
    m_certifyTip->deleteLater();
    m_certifyTip = nullptr;
  }
}


void TtipHandler::showWhatNextTip(bool isCorrect, bool toCorrection) {
#if defined (Q_OS_ANDROID)
  Taction* prevAct = nullptr;
  if (!isCorrect)
    prevAct = m_exam->melodies() ? EXECUTOR->newAtemptAct() : EXECUTOR->repeatQuestAct();
  MOBILE_MENU->setFlyActions(EXECUTOR->nextQuestAct(),
                             toCorrection ? EXECUTOR->correctAct() : nullptr,
                             EXECUTOR->stopExamAct(), nullptr,
                             prevAct);
  MOBILE_MENU->setExamMenuEntries(EXECUTOR->nextQuestAct(), prevAct, toCorrection ? EXECUTOR->correctAct() : nullptr,
                                  EXECUTOR->stopExamAct());
  if (toCorrection && EXECUTOR->correctAct())
    EXECUTOR->correctAct()->shake();
  if (prevAct)
    prevAct->shake();
  QTimer::singleShot(500, [=]{ EXECUTOR->nextQuestAct()->shake(); });
#else
  if (isCorrect)
    deleteQuestionTip();
  deleteWhatNextTip();
  m_whatNextText = QLatin1String("<p style=\"text-align: center; font-size: large;\">") + startTipText();
  const QString br = QStringLiteral("<br>");
  const QString space = QStringLiteral(" ");
  const QString a = QStringLiteral("</a>");
  if (!isCorrect) {
    QString t = tr("To correct an answer");
    QString href = QStringLiteral("<a href=\"prevQuest\">");
    if (m_exam->melodies()) {
      t = tr("To try this melody again");
      href = QStringLiteral("<a href=\"newAttempt\">");
    }
    m_whatNextText += br + t + space +
        TexamHelp::clickSomeButtonTxt(href + NOO->pixToHtml(QLatin1String("prevQuest"), m_iconSize) + a)
        + space + TexamHelp::orPressBackSpace();
  }
  if (toCorrection) {
    QString t = tr("To see corrected answer");
    if (m_exam->curQ()->melody())
        t = tr("To see some hints");
    m_whatNextText += br + t + space +
      TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"correct\">") + NOO->pixToHtml(QLatin1String("correct"), m_iconSize) + a)
      + br + TexamHelp::orPressEnterKey();
    if (EXECUTOR->correctAct())
      EXECUTOR->correctAct()->shake();
  }
  m_whatNextText += br + TexamHelp::toStopExamTxt(QLatin1String("<a href=\"stopExam\">")
               + NOO->pixToHtml(QLatin1String("stopExam"), m_iconSize) + a) + QLatin1String("</p>");
  m_timerOfWhatNext->start(1000);
//   QTimer::singleShot(1000, [=]{ emit wantWhatNextTip(m_whatNextText, qApp->palette().highlight().color(), getTipPosition(determineTipPos())); });
  if (!isCorrect) {
    if (m_exam->melodies())
      EXECUTOR->newAtemptAct()->shake();
    else
      EXECUTOR->repeatQuestAct()->shake();
  }
  QTimer::singleShot(500, [=]{ EXECUTOR->nextQuestAct()->shake(); });
#endif
}


void TtipHandler::playMelodyAgainMessage() {
  NOO->setMessageColor(GLOB->EanswerColor);
  NOO->showTimeMessage(tr("Select any note to play it again."), 3000, QQuickItem::Top);
}


void TtipHandler::showQuestionTip() {
  if (NOO->messageTimerActive())
    NOO->showTimeMessage(QString(), 10);
  auto question = m_exam->curQ();

#if defined (Q_OS_ANDROID)
  MOBILE_MENU->setFlyActions(EXECUTOR->playAgainAct(), EXECUTOR->tuningForkAct(),
                             EXECUTOR->checkQuestAct(),
                             question->melody() && question->answerOnScore() ? MAIN_SCORE->scoreMenuAct() : nullptr,
                             nullptr);
  MOBILE_MENU->setExamMenuEntries(EXECUTOR->playAgainAct(), EXECUTOR->tuningForkAct(),
                                  EXECUTOR->checkQuestAct(), EXECUTOR->stopExamAct());
#endif

  deleteWhatNextTip();
  if (!GLOB->E->autoNextQuest)
    deleteResultTip();
  deleteQuestionTip();

  auto br = QStringLiteral("<br>");
  auto sp = QStringLiteral(" ");
  QString questText;
  QString attemptText;
  auto questNr = m_exam->count();
  auto level = m_exam->level();
  if (question->attemptsCount() > 1)
    attemptText = QLatin1String(" <small><i>") + TexTrans::attemptTxt() + QString(" %1").arg(question->attemptsCount()) + "</i></small>";
  questText += QString("<center><b><u>&nbsp;%1.&nbsp;</u></b>").arg(questNr) + attemptText + br;
  QString apendix;
  QString noteStr;
  switch (question->questionAs) {
    case TQAtype::e_onScore: {
      if (question->answerOnScore()) {
          if (question->qa.note.alter() != question->qa_2.note.alter())
              questText += tr("Change enharmonically and show on the staff");
          else
              questText += tr("Given note show on the staff");
          if (level->useKeySign && level->manualKey)
              apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
          questText += getTextHowAccid(static_cast<Tnote::Ealter>(question->qa_2.note.alter()));
      } else if (question->answerAsName()) {
          questText += tr("Give name of");
      } else if (question->answerOnInstr()) {
          if (Tinstrument(level->instrument).isGuitar())
            questText += tr("Show on the guitar");
          else
            questText += tr("Point on the instrument");
      } else if (question->answerAsSound()) {
          if (question->melody())
            questText += tr("Play or sing a melody.");
          else
            questText += playOrSing(int(level->instrument));
      }
      if (question->answerOnInstr() || question->answerAsSound()) {
        if (Tinstrument(level->instrument).isGuitar() && !level->canBeMelody() && level->showStrNr && !level->onlyLowPos) {
          apendix = br + sp + onStringTxt(question->qa.pos().str());
        }
      }
      if (!question->melody()) {
        questText += "<br> . . . <br>"; // TODO: add score into tip
//           if (level->useKeySign && level->manualKey && question->answerAsNote()) // hide key signature
//                 m_questText += br + wrapPixToHtml(question->qa.note, true, TkeySignature(0), sc);
//           else
//                 m_questText += br + wrapPixToHtml(question->qa.note, true, question->key, sc);
      }
      if (!apendix.isEmpty())
        questText += apendix;
      break;
    }

    case TQAtype::e_asName:
      noteStr = sp + sp + getNiceNoteName(question->qa.note, question->styleOfQuestion());
      if (question->answerOnScore()) {
          questText += tr("Show on the staff") + noteStr;
          if (level->useKeySign && level->manualKey)
            questText += tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
      } else if (question->answerAsName()) {
          noteStr = sp + sp + getNiceNoteName(question->qa.note, question->styleOfQuestion());
          if (question->qa.note.alter() != question->qa_2.note.alter()) {
              questText += tr("Change enharmonically and give name of");
              questText += noteStr + getTextHowAccid((Tnote::Ealter)question->qa_2.note.alter());
          } else
              questText += tr("Use another style to give name of") + noteStr;
      } else if (question->answerOnInstr()) {
          if (Tinstrument(level->instrument).isGuitar())
            questText += tr("Show on the guitar") + noteStr;
          else
            questText += tr("Point on the instrument") + noteStr;
      } else if (question->answerAsSound()) {
          questText += playOrSing(int(level->instrument)) + noteStr;
      }
      if (question->answerOnInstr() || question->answerAsSound()) {
          if (level->instrument != Tinstrument::NoInstrument && level->showStrNr && !level->onlyLowPos)
            questText += br + sp + onStringTxt(question->qa.pos().str());
      }
    break;

    case TQAtype::e_onInstr:
      if (question->answerOnScore()) {
          questText += tr("Show on the staff note played on");
          if (level->useKeySign && level->manualKey)
            apendix = tr("<b>in %1 key.</b>", "in key signature").arg(question->key.getName());
      } else if (question->answerAsName()) {
          questText += tr("Give name of");
      } else if (question->answerOnInstr()) {
          questText += tr("Show sound from position:", "... and string + fret numbers folowing");
          apendix = br + sp + onStringTxt(question->qa_2.pos().str());
      } else if (question->answerAsSound()) {
          questText += playOrSing(int(level->instrument));
      }
      if (question->qa.pos().isValid())
        questText += QString("<br><span style=\"font-size: xx-large; %1\">&nbsp;").arg(Tcolor::bgTag(NOO->alpha(GLOB->EquestionColor, 40))) +
                  question->qa.pos().toHtml() + QLatin1String(" </span>");
      if (!apendix.isEmpty())
          questText += br + apendix;
      if (question->answerOnScore() || question->answerAsName()) {
        if (level->forceAccids)
              questText += getTextHowAccid((Tnote::Ealter)question->qa.note.alter());
      }
    break;

    case TQAtype::e_asSound:
      if (question->answerOnScore()) {
          if (question->melody()) {
                  questText += TexTrans::writeDescTxt();
              if (level->useKeySign && level->manualKey && level->onlyCurrKey)
                  questText += br + tr("Guess a key signature");
          } else {
                  questText += tr("Listened sound show on the staff");
              if (level->useKeySign && level->manualKey)
                  questText += tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
              if (level->forceAccids)
                  questText += getTextHowAccid((Tnote::Ealter)question->qa.note.alter());
          }
      } else if (question->answerAsName()) {
          questText += tr("Give name of listened sound");
          if (level->forceAccids)
              questText += getTextHowAccid((Tnote::Ealter)question->qa.note.alter());
      } else if (question->answerOnInstr()) {
            questText += tr("Listened sound show on the instrument");
            if (level->showStrNr)
              questText += br + sp + onStringTxt(question->qa.pos().str());
      } else if (question->answerAsSound()) {
              questText += tr("Play or sing listened sound");
      }
    break;
  }
  questText += QLatin1String("</center>");
  m_questTipPosType = determineTipPos();
  QPointF p(m_factorPosOfQT[m_questTipPosType].x() * EXECUTOR->width(), m_factorPosOfQT[m_questTipPosType].y() * EXECUTOR->height());
  if (m_factorPosOfQT[m_questTipPosType].isNull())
    p = getTipPosition(m_questTipPosType);
  emit wantQuestionTip(questText, p);
//  m_questionTip->setMinimized(m_minimizedQuestion);
}


//void TtipHandler::outOfTuneTip(float pitchDiff) {
//  if (m_outTuneTip)
//    return;
//  QString tuneText;
//  bool tooLow = true;
//  if (pitchDiff < 0)
//    tuneText = tr("too low");
//  else {
//    tuneText = tr("too high");
//    tooLow = false;
//  }
//  m_outTuneTip = new TgraphicsTextTip(QString("<span style=\"color: %1; font-size: %2px;\">")
//      .arg(GLOB->EanswerColor.name()).arg(bigFont()) + tuneText + "</span>");
//  m_scene->addItem(m_outTuneTip);
//  m_outTuneTip->setZValue(100);
//  m_outTuneTip->setScale(m_scale);
//  m_outTuneTip->setData(0, QVariant::fromValue<bool>(tooLow));
//  setOutTunePos();
//}


void TtipHandler::melodyCorrectMessage() {
  NOO->setMessageColor(GLOB->EanswerColor);
  NOO->showTimeMessage(tr("Click incorrect notes to see<br>and to listen to them corrected."), 7000, QQuickItem::Top);
}


///** @p prevTime param is to call clearing method after this time. */
//void TtipHandler::correctToGuitar(TQAtype::Etype &question, int prevTime, TfingerPos& goodPos) {
//  if (m_correctAnim)
//    return;
//  m_goodPos = goodPos;
//  m_flyEllipse = new QGraphicsEllipseItem;
//  m_flyEllipse->setPen(Qt::NoPen);
//  m_flyEllipse->setBrush(QBrush(QColor(GLOB->EquestionColor.name())));
//  m_scene->addItem(m_flyEllipse);
//  if (question == TQAtype::e_onScore) {
//      m_flyEllipse->setRect(SCORE->noteRect(1)); // 1 - answer note segment
//      m_flyEllipse->setPos(m_view->mapToScene(SCORE->notePos(1)));
//  } else if (question == TQAtype::e_asName) {
//      m_flyEllipse->setRect(QRectF(0, 0, NOTENAME->labelRect().height() * 2, NOTENAME->labelRect().height()));
//      m_flyEllipse->setPos(m_view->mapToScene(NOTENAME->mapToParent(NOTENAME->labelRect().topLeft())));
//  } else if (question == TQAtype::e_asSound) {
//      m_flyEllipse->setRect(QRectF(0, 0, SOUND->pitchView()->height() * 2, SOUND->pitchView()->height()));
//      m_flyEllipse->setPos(m_view->mapToScene((SOUND->pitchView()->geometry().topLeft() +
//        QPoint(SOUND->pitchView()->width() / 2 , 0))));
//  }

//  m_correctAnim = new TcombinedAnim(m_flyEllipse, this);
//  m_correctAnim->setDuration(600);
//  connect(m_correctAnim, SIGNAL(finished()), this, SLOT(correctAnimFinished()));
//  QPointF destP = m_view->mapToScene(GUITAR->mapToParent(GUITAR->mapFromScene(GUITAR->fretToPos(goodPos))));
//  if (!GLOB->GisRightHanded) { // fix destination position point for left-handed guitars
//    if (goodPos.fret())
//      destP.setX(destP.x() - GUITAR->fingerRect().width());
//    else
//      destP.setX(m_view->width() - destP.x());
//  }
//  m_correctAnim->setMoving(m_flyEllipse->pos(), destP);
//  m_correctAnim->moving()->setEasingCurveType(QEasingCurve::InOutBack);
//  if (goodPos.fret() != 0) {
//      m_correctAnim->setScaling(GUITAR->fingerRect().width() / m_flyEllipse->rect().width(), 2.0);
//      m_correctAnim->scaling()->setEasingCurveType(QEasingCurve::OutQuint);
//  }
//  m_correctAnim->setColoring(QColor(GLOB->EanswerColor.name()));
//  if (goodPos.fret() == 0) {
//    QPointF p1(m_view->mapToScene(GUITAR->mapToParent(
//              GUITAR->mapFromScene(GUITAR->stringLine(goodPos.str()).p1()))));
//    QPointF p2(m_view->mapToScene(GUITAR->mapToParent(
//              GUITAR->mapFromScene(GUITAR->stringLine(goodPos.str()).p2()))));
//    m_correctAnim->setMorphing(QLineF(p1, p2), GUITAR->stringWidth(goodPos.str() - 1));
//  }
//  m_correctAnim->startAnimations();
//  QTimer::singleShot(prevTime, this, SLOT(clearCorrection()));
//}


//void TtipHandler::levelStatusMessage() {
//#if !defined (Q_OS_ANDROID)
//  QString message;
//  if (m_exam->isExercise())
//      message = tr("You are exercising on level");
//  else
//      message = tr("Exam started on level");
//  message.append(QLatin1String(":<br><b>") + m_exam->level()->name + QLatin1String("</b>"));
//  STATUS->setBackground(-1); // reset background
//  setStatusMessage(message);
//#endif
//}


void TtipHandler::clearTips(bool resTipAlso) {
  deleteStartTip();
  deleteQuestionTip();
  deleteConfirmTip();
  if (resTipAlso)
    deleteResultTip();
  deleteTryAgainTip();
  deleteWhatNextTip();
//  clearMelodyCorrectMessage();
//#if defined (Q_OS_ANDROID)
//  delete m_nextTip;
//  delete m_prevTip;
//  delete m_correctTip;
//#endif
}




//void TtipHandler::clearCertificate() {
//  if (m_certifyTip) {
//    m_certifyTip->deleteLater();
//    m_certifyTip = nullptr;
//  }
//}


//void TtipHandler::clearCorrection() {
//  if (m_correctAnim) {
//    m_correctAnim->deleteLater();
//    m_correctAnim = 0;
//  }
//  if (m_flyEllipse) {
//    delete m_flyEllipse;
//    m_flyEllipse = 0;
//  }
//  emit correctingFinished();
//}



//void TtipHandler::clearMelodyCorrectMessage() {
//  if (m_melodyCorrectMessage) {
//    m_melodyCorrectMessage = false;
//    levelStatusMessage();
//  }
//}


////#################################################################################################
////###################              PROTECTED           ############################################
////#################################################################################################

//void TtipHandler::correctAnimFinished() {
////   clearCorrection();
//  m_flyEllipse->hide();
//  GUITAR->setFinger(m_goodPos);
//  GUITAR->markAnswer(QColor(GLOB->EanswerColor.name()));
//  m_view->update();
//}


bool TtipHandler::eventFilter(QObject* obj, QEvent* event) {
#if defined (Q_OS_ANDROID)
////   if (event->type() == QEvent::KeyPress) {
////     auto ke = static_cast<QKeyEvent*>(event);
////     if (ke->key() == static_cast<int>(Qt::Key_VolumeUp)) {
////       if (m_exam && !m_certifyTip)
////           emit certificateMagicKeys();
////     }
////   }
#else
  if (event->type() == QEvent::MouseButtonPress) {
    auto me = static_cast<QMouseEvent*>(event);
    if (me) {
      if (me->button() == Qt::MiddleButton && me->modifiers() & Qt::ShiftModifier &&  me->modifiers() & Qt::AltModifier) {
        if (m_exam && !m_certifyTip)
          emit certificateMagicKeys();
      }
    }
  }
#endif
  return QObject::eventFilter(obj, event);
}

////##################################################################################################
////#################################### PRIVATE #####################################################
////##################################################################################################

void TtipHandler::deleteStartTip() {
#if defined (Q_OS_ANDROID)
  if (m_startAct) {
    MOBILE_MENU->setForceText(false);
    delete m_startAct;
    m_startAct = nullptr;
  }
#else
  if (m_startTip) {
    m_startTip->deleteLater();
    m_startTip = nullptr;
  }
#endif
}


void TtipHandler::deleteTryAgainTip() {
  if (m_tryAgainTip) {
    m_tryAgainTip->deleteLater();
    m_tryAgainTip = nullptr;
  }
}


void TtipHandler::deleteQuestionTip() {
  if (m_questionTip) {
    m_factorPosOfQT[m_questTipPosType].setX((m_questionTip->x() + m_questionTip->width() / 2.0) / EXECUTOR->width());
    m_factorPosOfQT[m_questTipPosType].setY((m_questionTip->y() + m_questionTip->height() / 2.0) / EXECUTOR->height());
    m_questionTip->deleteLater();
    m_questionTip = nullptr;
  }
}


void TtipHandler::deleteConfirmTip() {
  m_timerToConfirm->stop();
  if (m_confirmTip) {
    m_confirmTip->deleteLater();
    m_confirmTip = nullptr;
  }
}


void TtipHandler::deleteResultTip() {
  if (m_resultTip) {
    m_resultTip->deleteLater();
    m_resultTip = nullptr;
  }
}


void TtipHandler::deleteWhatNextTip() {
  m_timerOfWhatNext->stop();
  if (m_whatNextTip) {
    m_whatNextTip->deleteLater();
    m_whatNextTip = nullptr;
  }
}


QPointF TtipHandler::getTipPosition(TtipHandler::EtipPos tp) {
  qreal offY = NOO->isAndroid() ? 0.0 : EXECUTOR->height() / 14.0; // controls coordinates are shifted by tool bar height
  if (tp == e_nameOver && NOTENAME)
      return QPointF(NOTENAME->x() + NOTENAME->width() / 2.0, NOTENAME->y() + NOTENAME->height() / 2.0 + offY);
  else if (tp == e_scoreOver)
      return QPointF( NOO->mainScore()->x() + NOO->mainScore()->width() / 2.0, NOO->mainScore()->y() + NOO->mainScore()->height() / 2.0 + offY);
  else if (tp == e_instrumentOver && !GLOB->instrument().isSax()) {
      return QPointF(NOO->instrument()->x() + NOO->instrument()->width() / 2.0,
                   NOO->mainScore()->y() + NOO->mainScore()->height() + NOO->instrument()->height() / 2.0 + offY);
  } else
      return QPointF(EXECUTOR->width() * 0.75, EXECUTOR->height() * 0.7);
}


//void TtipHandler::setTryAgainPos() {
//  QPointF tl(m_scene->width() * 0.6, m_scene->height() * 0.10); // top left of tip area
//  if (m_resultTip) // place it below result tip
//    tl.setY(m_resultTip->pos().y() + m_resultTip->realH());
//  m_tryAgainTip->setPos(tl.x() + (m_scene->width() * 0.4 - m_scale * m_tryAgainTip->boundingRect().width()) / 2, tl.y());
//}


//void TtipHandler::setWhatNextPos() {
//#if defined (Q_OS_ANDROID)
//  qreal sc = (m_view->height() / 8.0) / m_nextTip->realH();
//  if (sc > 1.0)
//    m_nextTip->setScale(sc);
//  qreal hh = m_view->height() * 0.75; // place tips above guitar, even it is hidden or doesn't exist
//  m_nextTip->setPos(m_view->width() - m_nextTip->realW() - 4, hh - m_nextTip->realH());
//  if (m_prevTip) {
//    if (sc > 1.0)
//      m_prevTip->setScale(sc);
//    m_prevTip->setPos(4, hh - m_prevTip->realH());
//  }
//  if (m_correctTip) {
//    if (sc > 1.0)
//      m_correctTip->setScale(sc);
//    m_correctTip->setPos(m_view->width() - m_correctTip->realW() - 4, m_nextTip->y() - m_correctTip->realH() - 8);
//  }
//#else
//  int maxTipHeight = getMaxTipHeight();
//  if (m_tipPos != e_nameOver && m_whatTip->realH() != maxTipHeight)
//    m_whatTip->setScale(maxTipHeight / m_whatTip->realH());

//  if (m_tipPos == e_nameOver) {
//      if (m_whatTip->realW() != m_view->width() * 0.45)
//        m_whatTip->setScale((m_view->width() * 0.45) / m_whatTip->realW());
//      if (m_whatTip->realH() > SCORE->height())
//        m_whatTip->setScale(SCORE->height() / m_whatTip->realH());
//  } else
//        fixWidthOverScore(m_whatTip);
//  if (m_posOfWhatTips[static_cast<int>(m_tipPos)].isNull()) // calculate tip position only when user doesn't change it
//    setPosOfTip(m_whatTip);
//  else
//    m_whatTip->setFixPos(m_posOfWhatTips[static_cast<int>(m_tipPos)]);
//#endif
//}


//void TtipHandler::setConfirmPos() { // right top corner
//#if defined (Q_OS_ANDROID)
//  qreal sc = (m_view->height() / 8.0) / m_confirmTip->realH();
//  if (sc > 1.0)
//    m_confirmTip->setScale(sc);
//  m_confirmTip->setPos(m_view->width() - m_confirmTip->realW() - 4, 4); // 4 is more-less tip shadow size
//#else
//   m_confirmTip->setPos(m_view->width() - m_confirmTip->realW() - 20, 20);
//#endif
//}


//void TtipHandler::createQuestionTip() {
//  delete m_questionTip;
//  qreal scaleFactor = 1.2;
//#if defined (Q_OS_ANDROID) // HACK: to keep question big enough on tablet big screens
//  scaleFactor = 1.2 * multiScale();
//#endif
//  m_questionTip = new TquestionTip(m_exam, m_scale * scaleFactor);
//  m_questionTip->setTextWidth(m_maxTipWidth);
//  m_scene->addItem(m_questionTip);
//  connect(m_questionTip, SIGNAL(moved()), this, SLOT(tipMoved()));
//  connect(m_questionTip, SIGNAL(minimizeChanged()), this, SLOT(tipStateChanged()));
//}


//void TtipHandler::setOutTunePos() {
//  int startX = SOUND->pitchView()->geometry().x();
//  if (m_outTuneTip->realW() > SOUND->pitchView()->geometry().width() / 2)
//      m_outTuneTip->setScale(m_outTuneTip->realW() / (SOUND->pitchView()->geometry().width() / 2));
//  if (!m_outTuneTip->data(0).toBool())
//    startX += SOUND->pitchView()->geometry().width() / 2;
//  m_outTuneTip->setPos(startX + (SOUND->pitchView()->geometry().width() / 2 - m_outTuneTip->realW()) / 2,
//                        SOUND->pitchView()->y() - m_outTuneTip->realH());
//}


//void TtipHandler::tipStateChanged() {
//  if (sender() == m_questionTip)
//    m_minimizedQuestion = m_questionTip->isMinimized();
//}


/**
* For details, see table in tip_positions.html file
*/
TtipHandler::EtipPos TtipHandler::determineTipPos() {
 EtipPos tipPos = e_bottomRight;
 switch (m_exam->curQ()->questionAs) {
   /** Question is note on the score, so place a tip over name if not used or over guitar if visible but if not - in bottom-right corner. */
   case TQAtype::e_onScore : {
     if (GLOB->isSingleNote()) {
         if (m_exam->curQ()->answerAs == TQAtype::e_asName) {
             tipPos = NOO->instrument() ? e_instrumentOver : e_bottomRight;
         } else
             tipPos = e_nameOver;
     } else { // melody, so answer is sound (only supported case)
         tipPos = NOO->instrument() ? e_instrumentOver : e_bottomRight;
     }
     break;
   }
   /** Question is note name, so place a tip over score if not used, or over guitar. */
   case TQAtype::e_asName : { // single note mode only
     if (m_exam->curQ()->answerAs == TQAtype::e_onScore)
       tipPos = NOO->instrument() ? e_instrumentOver : e_bottomRight;
     else
       tipPos = e_scoreOver;
     break;
   }
   case TQAtype::e_onInstr: // single note mode only
      tipPos = m_exam->curQ()->answerAs == TQAtype::e_onScore ? e_nameOver : e_scoreOver;
      break;
   case TQAtype::e_asSound : {
     if (GLOB->isSingleNote()) {
         if (m_exam->curQ()->answerAs == TQAtype::e_onScore)
          tipPos = m_exam->curQ()->answerAs == TQAtype::e_onScore ? e_nameOver : e_scoreOver;
     } else
         tipPos = NOO->instrument() ? e_instrumentOver : e_bottomRight;
     break;
   }
  }
  return tipPos;
}






