/***************************************************************************
 *   Copyright (C) 2012-2018 by Tomasz Bojczuk                             *
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
#include <tcolor.h>
#include "tglobals.h"
#include "texamhelp.h"
#include "texamexecutor.h"
#include "tnameitem.h"
#include <tsound.h>
#include <tpath.h>
#include <texamparams.h>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
#else
//  #include <gui/tstatuslabel.h>
#endif
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>


#if defined (Q_OS_ANDROID)
inline QString getTipText(const char* iconName, const char* barText) {
  return pixToHtml(Tpath::img(iconName), qRound(Tmtr::fingerPixels() * 0.7)) + QLatin1String("<br>") +
         QApplication::translate("TtoolBar", barText);
}


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
  if (accid) S += qApp->translate("TtipHandler", "Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
  else S += qApp->translate("TtipHandler", " Don't use accidentals!");
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
  return QString("<b><span style=\"%1\">&nbsp;").arg(Tcolor::bgTag(NOO->alpha(GLOB->EquestionColor, 40))) + note.toRichText(style) + QLatin1String(" </span></b>");
}



TtipHandler::TtipHandler(Texam* exam, QObject *parent) :
  QObject(parent),
//  m_certifyTip(nullptr),
  m_exam(exam),
  m_timerToConfirm(new QTimer(this)),
//  m_minimizedQuestion(false), m_melodyCorrectMessage(false),
  m_questTipPosType(e_bottomRight),
  m_iconSize(bigFont() * 1.2)
{
 connect(m_timerToConfirm, &QTimer::timeout, this, &TtipHandler::showConfirmTip);
//  qApp->installEventFilter(this);
//   int levelMessageDelay = 1;
//   if (TexecutorSupply::paramsChangedMessage())
//       levelMessageDelay = 7000;
//  QTimer::singleShot(levelMessageDelay, this, SLOT(levelStatusMessage()));
  m_prevWidth = EXECUTOR->width();
  connect(EXECUTOR, &TexamExecutor::widthChanged, [=]{
    qreal sc = EXECUTOR->width() / m_prevWidth;
    for (int t = 0; t < TIP_POS_NUM; ++t) {
      if (!m_posOfQuestTips[t].isNull())
        m_posOfQuestTips[t] = QPointF(m_posOfQuestTips[t].x() * sc, m_posOfQuestTips[t].y() * sc);
    }
    m_prevWidth = EXECUTOR->width();
  });
}

TtipHandler::~TtipHandler()
{}


void TtipHandler::changeExam(Texam* newExam) {
  m_exam = newExam;
}


void TtipHandler::setTipPos(const QPointF& p) {
  m_posOfQuestTips[static_cast<int>(m_questTipPosType)] = p;
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


void TtipHandler::resultTip(TQAunit* answer, int time) {
  m_timerToConfirm->stop();
  clearCanvas();

 bool autoNext = GLOB->E->autoNextQuest;
 if (GLOB->E->afterMistake == TexamParams::e_stop && !answer->isCorrect())
     autoNext = false; // when mistake and e_stop - the same like autoNext = false;
 if (autoNext) { // determine time of displaying
   if (answer->isCorrect() || GLOB->E->afterMistake == TexamParams::e_continue)
     time = 2500; // hard-coded
   else
     time = GLOB->E->mistakePreview; // user defined wait time
 }

  emit showResultTip(wasAnswerOKtext(answer), TexecutorSupply::answerColor(answer->mistake()));
 
//  if (answer->isNotSoBad())
//    m_resultTip->setScale(m_scale);
//  else
//    m_resultTip->setScale(m_scale * 1.2);
//  setResultPos();
//  if (GLOB->E->showWrongPlayed && GLOB->E->showWrongPlayed && !answer->melody() &&
//      answer->answerAsSound() && !answer->isCorrect() && SOUND->note().note)
//          detectedNoteTip(SOUND->note()); // In exercise mode display detected note when it was incorrect
  if (time)
    QTimer::singleShot(time, [=]{ emit destroyResultTip(); });
}


//QString TtipHandler::detectedText(const QString& txt) {
//  return QString("<span style=\"color: %1;\"><big>").arg(GLOB->EquestionColor.name()) + txt + QLatin1String("</big></span>");
//}


//void TtipHandler::detectedNoteTip(const Tnote& note) {
//  Tnote n = note;
//  if (n.isValid())
//    setStatusMessage(QLatin1String("<table valign=\"middle\" align=\"center\"><tr><td> ") +
//        wrapPixToHtml(n, m_exam->level()->clef.type(),  TkeySignature(0), m_view->height() / 260.0) + QLatin1String(" ") +
//        detectedText(tr("%1 was detected", "note name").arg(n.toRichText())) + QLatin1String("</td></tr></table>"), 5000);
//}


//void TtipHandler::tryAgainTip(int time) {
//  m_tryAgainTip = new TgraphicsTextTip(QString("<span style=\"color: %1; font-size: %2px;\">")
//      .arg(GLOB->EquestionColor.name()).arg(bigFont()) + tr("Try again!") + "</span>");
//  m_scene->addItem(m_tryAgainTip);
//  m_tryAgainTip->setZValue(100);
//  m_tryAgainTip->setScale(m_scale);
//  setTryAgainPos();
//  QTimer::singleShot(time, this, SLOT(clearTryAgainTip()));
//}


QString TtipHandler::startTipText() {
  return TexamHelp::toGetQuestTxt() + QLatin1String(":<br>") +
    TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"nextQuest\">") +
                                  NOO->pixToHtml(QLatin1String("nextQuest"), m_iconSize) + QLatin1String("</a>"))
    #if !defined (Q_OS_ANDROID)
      + QLatin1String(",<br>") + TexamHelp::pressSpaceKey() + QLatin1String(" ") + TexamHelp::orRightButtTxt()
    #endif
    ;
}


void TtipHandler::startTip() {
  QString tipText = QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75))
      + startTipText() + QLatin1String(".<br>")
      + TexamHelp::toStopExamTxt(QLatin1String("<a href=\"stopExam\"> ")
                                 + NOO->pixToHtml(QLatin1String("stopExam"), m_iconSize)
                                 + QLatin1String("</a>"))
      + QLatin1String("</p>");
  QTimer::singleShot(200, [=]{
    emit showStartTip(tipText, qApp->palette().highlight().color(), QPointF(EXECUTOR->width() / 2.0, EXECUTOR->height() / 2.0));
  });
}


//void TtipHandler::certificateTip() {
//  if (m_certifyTip)
//    return;

//  delete m_questionTip;
//  clearResultTip();
//  clearWhatNextTip();
//  if (!m_certifyTip) {
//    m_certifyTip = new TnootkaCertificate(m_view, m_exam);
//    connect(m_certifyTip, SIGNAL(userAction(QString)), this, SLOT(linkActivatedSlot(QString)));
//  }
//}


void TtipHandler::whatNextTip(bool isCorrect, bool toCorrection) {
//  delete m_questionTip;
//  clearWhatNextTip();
//#if defined (Q_OS_ANDROID)
//  m_nextTip = new ThackedTouchTip(getTipText("nextQuest", "Next"), m_view->palette().highlight().color());
//  m_scene->addItem(m_nextTip);
//  m_nextTip->setFont(smalTipFont(m_view));
//  connect(m_nextTip, &ThackedTouchTip::clicked, [=] {
//      QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("nextQuest")); });
//  });
//  int maxTipWidth = qRound(m_view->fontMetrics().boundingRect(QApplication::translate("TtoolBar", "Next")).width() * 1.5);
//  if (!isCorrect) {
//    m_prevTip = new ThackedTouchTip(getTipText("prevQuest", m_exam->melodies() ? "Try again" : "Repeat"),
//                                     m_view->palette().highlight().color());
//    m_scene->addItem(m_prevTip);
//    m_prevTip->setFont(smalTipFont(m_view));
//    maxTipWidth = qMax(maxTipWidth,
//           qRound(m_view->fontMetrics().boundingRect(QApplication::translate("TtoolBar", m_exam->melodies() ? "Try again" : "Repeat")).width() * 1.5));
//    m_prevTip->setTextWidth(maxTipWidth);
//    if (m_exam->melodies())
//      connect(m_prevTip, &ThackedTouchTip::clicked, [=] {
//          QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("newAttempt")); });
//      });
//    else
//      connect(m_prevTip, &ThackedTouchTip::clicked, [=] {
//          QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("prevQuest")); });
//      });
//  }
//  if (toCorrection) {
//    m_correctTip = new ThackedTouchTip(getTipText("correct", "Correct"), GLOB->EanswerColor);
//    m_scene->addItem(m_correctTip);
//    m_correctTip->setFont(smalTipFont(m_view));
//    connect(m_correctTip, &ThackedTouchTip::clicked, [=] {
//      QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("correct")); });
//    });
//    maxTipWidth = qMax(maxTipWidth,
//                           qRound(m_view->fontMetrics().boundingRect(QApplication::translate("TtoolBar", "Correct")).width() * 1.5));
//    m_correctTip->setTextWidth(maxTipWidth); // keep the same width if both tips are displayed
//  }
//  m_nextTip->setTextWidth(maxTipWidth);
//#else
  QString whatNextText = QLatin1String("<p style=\"text-align: center; font-size: large;\">") + startTipText();
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
    whatNextText += br + t + space +
        TexamHelp::clickSomeButtonTxt(href + NOO->pixToHtml(QLatin1String("prevQuest"), m_iconSize) + a)
        + space + TexamHelp::orPressBackSpace();
  }
  if (toCorrection) {
    QString t = tr("To see corrected answer");
    if (m_exam->curQ()->melody())
        t = tr("To see some hints");
    whatNextText += br + t + space +
      TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"correct\">") + NOO->pixToHtml(QLatin1String("correct"), m_iconSize) + a)
      + br + TexamHelp::orPressEnterKey();
  }
  whatNextText += br + TexamHelp::toStopExamTxt(QLatin1String("<a href=\"stopExam\">")
               + NOO->pixToHtml(QLatin1String("stopExam"), m_iconSize) + a) + QLatin1String("</p>");
  QTimer::singleShot(1000, [=]{
      emit showStartTip(whatNextText, qApp->palette().highlight().color(), getTipPosition(determineTipPos()));
  });
//#endif
}


void TtipHandler::confirmTip(int time) {
  if (!m_confirmTipOn) {
#if defined (Q_OS_ANDROID)
    showConfirmTip();
#else
    m_timerToConfirm->start(time + 1); // add 1 to show it immediately when time = 0
#endif
    m_confirmTipOn = true;
  }
}


void TtipHandler::showConfirmTip() {
  m_timerToConfirm->stop();
  const QString br_ = QStringLiteral("<br>- ");
  const QString a = QStringLiteral("</a>");
  QString tipText = QLatin1String("<p style=\"text-align: center; font-size: large;\">") + tr("To check the answer confirm it:") + br_ +
    TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"checkAnswer\">") + NOO->pixToHtml(QLatin1String("check"), m_iconSize) + a) + br_ +
    TexamHelp::pressEnterKey() + br_ + TexamHelp::orRightButtTxt() + QLatin1String("<br>") +
    tr("Check in exam help %1 how to do it automatically").arg(QStringLiteral("<a href=\"examHelp\">") +
    NOO->pixToHtml(QLatin1String("help"), m_iconSize) + a) + QLatin1String("</p>");
  emit showStartTip(tipText, GLOB->EanswerColor, QPointF(EXECUTOR->width() * 0.75, EXECUTOR->height() * 0.1));
}


//void TtipHandler::playMelodyAgainMessage() {
//#if defined (Q_OS_ANDROID)
//  tMessage->setMessage(detectedText(tr("Select any note to play it again.")), 3000);
//#else
//  STATUS->setMessage(detectedText(tr("Select any note to play it again.")), 3000);
//#endif
//}


void TtipHandler::questionTip() {
  emit destroyResultTip();
  QString br = QStringLiteral("<br>");
  QString sp = QStringLiteral(" ");
  QString questText;
  QString attemptText;
  auto question = m_exam->curQ();
  auto questNr = m_exam->count();
  auto level = m_exam->level();
  if (question->attemptsCount() > 1)
    attemptText = QLatin1String(" <small><i>") + TexTrans::attemptTxt() + QString(" %1").arg(question->attemptsCount()) + "</i></small>";
  questText += QString("<center><b><u>&nbsp;%1.&nbsp;</u></b>").arg(questNr) + attemptText + br;
  QString apendix;
  QString noteStr;
  switch (question->questionAs) {
    case TQAtype::e_asNote: {
      if (question->answerAsNote()) {
          if (question->qa.note.alter() != question->qa_2.note.alter())
              questText += tr("Change enharmonically and show on the staff");
          else
              questText += tr("Given note show on the staff");
          if (level->useKeySign && level->manualKey)
              apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
                questText += getTextHowAccid((Tnote::Ealter)question->qa_2.note.alter());
      } else if (question->answerAsName()) {
          questText += tr("Give name of");
      } else if (question->answerAsFret()) {
            questText += tr("Show on the guitar");
      } else if (question->answerAsSound()) {
                if (question->melody())
                        questText += tr("Play or sing a melody.");
                else
                        questText += playOrSing(int(level->instrument));
      }
      if (question->answerAsFret() || question->answerAsSound()) {
        if (level->instrument != Tinstrument::NoInstrument && !level->canBeMelody() && level->showStrNr && !level->onlyLowPos) {
          apendix = br + sp + onStringTxt(question->qa.pos.str());
        }
      }
      if (!question->melody()) {
        questText += "<br> . . . <br>";
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
      noteStr = br + getNiceNoteName(question->qa.note, question->styleOfQuestion());
      if (question->answerAsNote()) {
          questText += tr("Show on the staff") + noteStr;
          if (level->useKeySign && level->manualKey)
            questText += tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
      } else if (question->answerAsName()) {
          noteStr = br + getNiceNoteName(question->qa.note, question->styleOfQuestion());
          if (question->qa.note.alter() != question->qa_2.note.alter()) {
              questText += tr("Change enharmonically and give name of");
              questText += noteStr + getTextHowAccid((Tnote::Ealter)question->qa_2.note.alter());
          } else
              questText += tr("Use another style to give name of") + noteStr;
      } else if (question->answerAsFret()) {
          questText += tr("Show on the guitar") + noteStr;
      } else if (question->answerAsSound()) {
          questText += playOrSing(int(level->instrument)) + noteStr;
      }
      if (question->answerAsFret() || question->answerAsSound()) {
          if (level->instrument != Tinstrument::NoInstrument && level->showStrNr && !level->onlyLowPos)
                      questText += br + sp + onStringTxt(question->qa.pos.str());
      }
    break;

    case TQAtype::e_asFretPos:
      if (question->answerAsNote()) {
                questText += tr("Show on the staff note played on");
          if (level->useKeySign && level->manualKey) {
            apendix = tr("<b>in %1 key.</b>", "in key signature").arg(question->key.getName());
          }
      } else if (question->answerAsName()) {
          questText += tr("Give name of");
      } else if (question->answerAsFret()) {
            questText += tr("Show sound from position:", "... and string + fret numbers folowing");
            apendix = br + sp + onStringTxt(question->qa_2.pos.str());
      } else if (question->answerAsSound()) {
                  questText += playOrSing(int(level->instrument));
      }
      questText += QString("<br><span style=\"font-size: xx-large; %1\">&nbsp;").arg(Tcolor::bgTag(NOO->alpha(GLOB->EquestionColor, 40))) +
                  question->qa.pos.toHtml() + QLatin1String(" </span>");
      if (!apendix.isEmpty())
          questText += br + apendix;
      if (question->answerAsNote() || question->answerAsName())
        if (level->forceAccids)
              questText += getTextHowAccid((Tnote::Ealter)question->qa.note.alter());
    break;

    case TQAtype::e_asSound:
      if (question->answerAsNote()) {
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
      } else if (question->answerAsFret()) {
            questText += tr("Listened sound show on the guitar");
            if (level->showStrNr)
              questText += br + sp + onStringTxt(question->qa.pos.str());
      } else if (question->answerAsSound()) {
              questText += tr("Play or sing listened sound");
      }
    break;
  }
  questText += QLatin1String("</center>");
  m_questTipPosType = determineTipPos();
  emit showQuestionTip(questText, m_posOfQuestTips[m_questTipPosType].isNull() ? getTipPosition(m_questTipPosType) : m_posOfQuestTips[m_questTipPosType]);
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


//void TtipHandler::melodyCorrectMessage() {
//  if (m_melodyCorrectMessage)
//    return;

//  m_melodyCorrectMessage = true;
//  QString message = QString("<span style=\"color: %1;\"><big>").arg(GLOB->EanswerColor.name()) +
//                    tr("Click incorrect notes to see<br>and to listen to them corrected.") + QLatin1String("</big></span>");
//#if defined (Q_OS_ANDROID)
//  tMessage->setMessage(message, 10000); // temporary message on a tip
//#else
//  STATUS->setBackground(-1);
//  setStatusMessage(message); // permanent message on status label
//#endif
//}


///** @p prevTime param is to call clearing method after this time. */
//void TtipHandler::correctToGuitar(TQAtype::Etype &question, int prevTime, TfingerPos& goodPos) {
//  if (m_correctAnim)
//    return;
//  m_goodPos = goodPos;
//  m_flyEllipse = new QGraphicsEllipseItem;
//  m_flyEllipse->setPen(Qt::NoPen);
//  m_flyEllipse->setBrush(QBrush(QColor(GLOB->EquestionColor.name())));
//  m_scene->addItem(m_flyEllipse);
//  if (question == TQAtype::e_asNote) {
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


void TtipHandler::clearCanvas() {
  m_confirmTipOn = false;
  emit destroyTips();
//  clearConfirmTip();
//  clearResultTip();
//  if (m_whatTip) {
//    delete m_whatTip;
//  }
//  delete m_startTip;
//  delete m_questionTip;
//  clearCertificate();
//  delete m_outTuneTip;
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


//bool TtipHandler::eventFilter(QObject* obj, QEvent* event) {
//#if defined (Q_OS_ANDROID)
////   if (event->type() == QEvent::KeyPress) {
////     auto ke = static_cast<QKeyEvent*>(event);
////     if (ke->key() == static_cast<int>(Qt::Key_VolumeUp)) {
////       if (m_exam && !m_certifyTip)
////           emit certificateMagicKeys();
////     }
////   }
//#else
//  if (event->type() == QEvent::MouseButtonPress) {
//    auto *me = static_cast<QMouseEvent*>(event);
//    if (me->button() == Qt::MiddleButton && me->modifiers() | Qt::ShiftModifier &&  me->modifiers() | Qt::AltModifier) {
//        if (m_exam && !m_certifyTip)
//          emit certificateMagicKeys();
//    }
//  }
//#endif
//  return QObject::eventFilter(obj, event);
//}

////##################################################################################################
////#################################### PRIVATE #####################################################
////##################################################################################################

QPointF TtipHandler::getTipPosition(TtipHandler::EtipPos tp) {
  qreal offY = NOO->isAndroid() ? 0.0 : EXECUTOR->height() / 14; // controls coordinates are sifted by tool bar height
  if (tp == e_nameOver && NOTENAME)
    return QPointF(NOTENAME->x() + NOTENAME->width() / 2.0, NOTENAME->y() + NOTENAME->height() / 2.0 + offY);
  else if (tp == e_scoreOver)
    return QPointF(NOO->mainScore()->x() + NOO->mainScore()->width() / 2.0, NOO->mainScore()->y() + NOO->mainScore()->height() / 2.0 + offY);
  else if (tp == e_instrumentOver && !GLOB->instrument().isSax())
    return QPointF(NOO->instrument()->x() + NOO->instrument()->width() / 2.0,
                   NOO->mainScore()->y() + NOO->mainScore()->height() + NOO->instrument()->height() / 2.0 + offY);
  else
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
 EtipPos tipPos;
 switch (m_exam->curQ()->questionAs) {
   /** Question is note on the score, so place a tip over name if not used or over guitar if visible but if not - in bottom-right corner. */
   case TQAtype::e_asNote : {
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
     if (m_exam->curQ()->answerAs == TQAtype::e_asNote)
       tipPos = NOO->instrument() ? e_instrumentOver : e_bottomRight;
     else
       tipPos = e_scoreOver;
     break;
   }
   case TQAtype::e_asFretPos: // single note mode only
      tipPos = m_exam->curQ()->answerAs == TQAtype::e_asNote ? e_nameOver : e_scoreOver;
      break;
   case TQAtype::e_asSound : {
     if (GLOB->isSingleNote()) {
         if (m_exam->curQ()->answerAs == TQAtype::e_asNote)
          tipPos = m_exam->curQ()->answerAs == TQAtype::e_asNote ? e_nameOver : e_scoreOver;
     } else
         tipPos = NOO->instrument() ? e_instrumentOver : e_bottomRight;
     break;
   }
  }
  return tipPos;
}






