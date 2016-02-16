/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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


#include "tcanvas.h"
#include "tquestiontip.h"
#include "tnootkacertificate.h"
#include "texecutorsupply.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/tresulttext.h>
#include <exam/tlevel.h>
#include <animations/tcombinedanim.h>
#include <texamparams.h>
#include <graphics/tnotepixmap.h>
#include <tinitcorelib.h>
#include <help/texamhelp.h>
#include <notename/tnotename.h>
#include <guitar/tfingerboard.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#include <widgets/tquestionaswdg.h>
#include <tpath.h>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
  #include <ttouchmessage.h>
#else
  #include <gui/tstatuslabel.h>
#endif
#include <score/tmainscore.h>
#include <gui/ttoolbar.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>



#if defined (Q_OS_ANDROID)
inline QString getTipText(const char* iconName, const char* barText) {
  return pixToHtml(Tpath::img(iconName), Tmtr::fingerPixels() * 0.7) + QLatin1String("<br>") +
         QApplication::translate("TtoolBar", barText);
}


QFont smalTipFont(QWidget* w) {
  int bSize = qBound<int>(Tmtr::fingerPixels() * 1.1, Tmtr::longScreenSide() / 12, Tmtr::fingerPixels() * 1.6);
  QFont f = w->font();
  f.setPixelSize(qMin<int>(bSize / 5, w->fontMetrics().height()));
  return f;
}
#endif


ThackedTouchTip::ThackedTouchTip(const QString& text, QColor bgColor) :
  TgraphicsTextTip(text, bgColor)
{
  setBaseColor(qApp->palette().text().color());
  setDefaultTextColor(qApp->palette().base().color());
}


Tcanvas::Tcanvas(QGraphicsView* view, Texam* exam) :
  QObject(view),
  m_view(view),
  m_scale(1),
  m_certifyTip(0),
  m_exam(exam),
  m_timerToConfirm(new QTimer(this)),
  m_flyEllipse(0),
  m_minimizedQuestion(false), m_melodyCorrectMessage(false)
{
  m_scene = m_view->scene();
	m_newSize = m_scene->sceneRect().size().toSize();
	m_prevSize = m_scene->sceneRect().size();
#if defined (Q_OS_ANDROID)
  m_iconSize = Tmtr::fingerPixels() * 0.7;
#else
    m_iconSize = m_view->fontMetrics().boundingRect("A").height() * 2;
#endif
  sizeChanged();
	connect(m_scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sizeChangedDelayed(QRectF)));
  connect(m_timerToConfirm, SIGNAL(timeout()), this, SLOT(showConfirmTip()));
	qApp->installEventFilter(this);
  int levelMessageDelay = 1;
  if (TexecutorSupply::paramsChangedMessage())
      levelMessageDelay = 7000;
  QTimer::singleShot(levelMessageDelay, this, SLOT(levelStatusMessage()));
}

Tcanvas::~Tcanvas()
{}


void Tcanvas::changeExam(Texam* newExam) {
  m_exam = newExam;
}


void Tcanvas::setStatusMessage(const QString& text, int duration) {
#if defined (Q_OS_ANDROID)
  tMessage->setMessage(text, duration);
#else
  STATUS->setMessage(text, duration);
#endif
}


//######################################################################
//##################################### TIPS ###########################
//######################################################################

int Tcanvas::bigFont() {
#if defined (Q_OS_ANDROID)
  return (m_view->fontMetrics().boundingRect("A").height() * 1.2);
#else
  return (m_view->fontMetrics().boundingRect("A").height() * 2);
#endif
}


QFont Tcanvas::tipFont(qreal factor) {
  QFont f = m_view->font();
  f.setPointSize(qRound((qreal)bigFont() * factor));
  return f;
}


void Tcanvas::resultTip(TQAunit* answer, int time) {
  clearConfirmTip();
  clearResultTip();
  clearTryAgainTip();

  bool autoNext = Tcore::gl()->E->autoNextQuest;
  if (Tcore::gl()->E->afterMistake == TexamParams::e_stop && !answer->isCorrect())
      autoNext = false; // when mistake and e_stop - the same like autoNext = false;
  if (autoNext) { // determine time of displaying
    if (answer->isCorrect() || Tcore::gl()->E->afterMistake == TexamParams::e_continue)
      time = 2500; // hard-coded
    else
      time = Tcore::gl()->E->mistakePreview; // user defined wait time
  }

  m_resultTip = new TgraphicsTextTip(wasAnswerOKtext(answer, TexecutorSupply::answerColor(answer->mistake()), bigFont()));
  m_scene->addItem(m_resultTip);
  m_resultTip->setZValue(100);
	if (answer->isNotSoBad())
		m_resultTip->setScale(m_scale);
	else
		m_resultTip->setScale(m_scale * 1.2);
  setResultPos();
  if (Tcore::gl()->E->showWrongPlayed && Tcore::gl()->E->showWrongPlayed && !answer->melody() &&
			answer->answerAsSound() && !answer->isCorrect() && SOUND->note().note)
					detectedNoteTip(SOUND->note()); // In exercise mode display detected note when it was incorrect
  if (time)
			QTimer::singleShot(time, this, SLOT(clearResultTip()));
}


QString Tcanvas::detectedText(const QString& txt) {
  return QString("<span style=\"color: %1;\"><big>").arg(Tcore::gl()->EquestionColor.name()) + txt + QLatin1String("</big></span>");
}


void Tcanvas::detectedNoteTip(const Tnote& note) {
  Tnote n = note;
  if (n.isValid())
    setStatusMessage(QLatin1String("<table valign=\"middle\" align=\"center\"><tr><td> ") +
        wrapPixToHtml(n, m_exam->level()->clef.type(),  TkeySignature(0), m_view->height() / 260.0) + QLatin1String(" ") +
        detectedText(tr("%1 was detected", "note name").arg(n.toRichText())) + QLatin1String("</td></tr></table>"), 5000);
}


void Tcanvas::tryAgainTip(int time) {
  m_tryAgainTip = new TgraphicsTextTip(QString("<span style=\"color: %1; font-size: %2px;\">")
      .arg(Tcore::gl()->EquestionColor.name()).arg(bigFont()) + tr("Try again!") + "</span>");
  m_scene->addItem(m_tryAgainTip);
  m_tryAgainTip->setZValue(100);
  m_tryAgainTip->setScale(m_scale);
  setTryAgainPos();
  QTimer::singleShot(time, this, SLOT(clearTryAgainTip()));
}


QString Tcanvas::startTipText() {
  return TexamHelp::toGetQuestTxt() + QLatin1String(":<br>") +
    TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"nextQuest\">") +
                                  pixToHtml(Tpath::img("nextQuest"), m_iconSize) + QLatin1String("</a>"))
    #if !defined (Q_OS_ANDROID)
      + QLatin1String(",<br>") + TexamHelp::pressSpaceKey() + QLatin1String(" ") + TexamHelp::orRightButtTxt()
    #endif
    ;
}


void Tcanvas::startTip() {
   m_startTip = new TgraphicsTextTip(QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75)) + startTipText() + ".<br>" +
     TexamHelp::toStopExamTxt("<a href=\"stopExam\"> " + pixToHtml(Tpath::img("stopExam"), m_iconSize) + "</a>") + 
     "</p>", m_view->palette().highlight().color());
   m_scene->addItem(m_startTip);
   m_startTip->setScale(m_scale);
   m_startTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
   connect(m_startTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
   setStartTipPos();
}


void Tcanvas::certificateTip() {
  delete m_questionTip;
  clearResultTip();
  clearWhatNextTip();
  if (!m_certifyTip) {
    m_certifyTip = new TnootkaCertificate(m_view, m_exam);
    connect(m_certifyTip, SIGNAL(userAction(QString)), this, SLOT(linkActivatedSlot(QString)));
  }
}


void Tcanvas::whatNextTip(bool isCorrect, bool toCorrection) {
	delete m_questionTip;
  clearWhatNextTip();
#if defined (Q_OS_ANDROID)
  m_nextTip = new ThackedTouchTip(getTipText("nextQuest", "Next"), m_view->palette().highlight().color());
  m_scene->addItem(m_nextTip);
  m_nextTip->setFont(smalTipFont(m_view));
  connect(m_nextTip, &ThackedTouchTip::clicked, [=] {
      QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("nextQuest")); });
  });
  int maxTipWidth = m_view->fontMetrics().boundingRect(QApplication::translate("TtoolBar", "Next")).width();
  if (!isCorrect) {
    m_prevTip = new ThackedTouchTip(getTipText("prevQuest", m_exam->melodies() ? "Try again" : "Repeat"),
                                     m_view->palette().highlight().color());
    m_scene->addItem(m_prevTip);
    m_prevTip->setFont(smalTipFont(m_view));
    maxTipWidth = qMax<int>(maxTipWidth,
            m_view->fontMetrics().boundingRect(QApplication::translate("TtoolBar", m_exam->melodies() ? "Try again" : "Repeat")).width()) * 1.2;
    m_prevTip->setTextWidth(maxTipWidth);
    QString tipLink;
    if (m_exam->melodies())
      connect(m_prevTip, &ThackedTouchTip::clicked, [=] {
          QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("newAttempt")); });
      });
    else
      connect(m_prevTip, &ThackedTouchTip::clicked, [=] {
          QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("prevQuest")); });
      });
  }
  if (toCorrection) {
    m_correctTip = new ThackedTouchTip(getTipText("correct", "Correct"), Tcore::gl()->EanswerColor);
    m_scene->addItem(m_correctTip);
    m_correctTip->setFont(smalTipFont(m_view));
    connect(m_correctTip, &ThackedTouchTip::clicked, [=] {
      QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("correct")); });
    });
    maxTipWidth = qMax<int>(maxTipWidth,
                            m_view->fontMetrics().boundingRect(QApplication::translate("TtoolBar", "Correct")).width()) * 1.2;
    m_correctTip->setTextWidth(maxTipWidth); // keep the same width if both tips are displayed
  }
  m_nextTip->setTextWidth(maxTipWidth);
#else
	QString whatNextText = startTipText();
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
				TexamHelp::clickSomeButtonTxt(href + pixToHtml(Tpath::img("prevQuest"), m_iconSize) +	a)
				+ space + TexamHelp::orPressBackSpace();
	}
	if (toCorrection) {
		QString t = tr("To see corrected answer");
		if (m_exam->curQ()->melody())
				t = tr("To see some hints");
		whatNextText += br + t + space +
			TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"correct\">") + pixToHtml(Tpath::img("correct"), m_iconSize) + a)
      + br + TexamHelp::orPressEnterKey();
	}
	whatNextText += br + TexamHelp::toStopExamTxt(QLatin1String("<a href=\"stopExam\">") + pixToHtml(Tpath::img("stopExam"), m_iconSize) + a);
  m_whatTip = new TgraphicsTextTip(whatNextText, m_view->palette().highlight().color());
// 	if (m_guitarFree) // tip is wide there, otherwise text is word-wrapped and is narrowest but higher
// 			m_whatTip->setTextWidth(m_maxTipWidth);
  m_scene->addItem(m_whatTip);
  m_whatTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
  m_whatTip->setTipMovable(true);
  connect(m_whatTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
  connect(m_whatTip, SIGNAL(moved()), this, SLOT(tipMoved()));
#endif
  setWhatNextPos();
}


void Tcanvas::confirmTip(int time) {
#if defined (Q_OS_ANDROID)
  showConfirmTip();
#else
  m_timerToConfirm->start(time + 1); // add 1 to show it immediately when time = 0
#endif
}


void Tcanvas::showConfirmTip() {
  m_timerToConfirm->stop();
  if (!m_confirmTip) {
#if defined (Q_OS_ANDROID)
    m_confirmTip = new ThackedTouchTip(getTipText("check", "Check"), Tcore::gl()->EanswerColor);
    m_scene->addItem(m_confirmTip);
    m_confirmTip->setFont(smalTipFont(m_view));
    connect(m_confirmTip, &ThackedTouchTip::clicked, [=] {
      QTimer::singleShot(10, [=] { linkActivatedSlot(QLatin1String("checkAnswer")); });
    });
#else
    const QString br_ = QStringLiteral("<br>- ");
    const QString a = QStringLiteral("</a>");
    m_confirmTip = new TgraphicsTextTip(tr("To check the answer confirm it:") + br_ +
      TexamHelp::clickSomeButtonTxt(QLatin1String("<a href=\"checkAnswer\">") + pixToHtml(Tpath::img("check"), m_iconSize) + a) + br_ +
      TexamHelp::pressEnterKey() + br_ + TexamHelp::orRightButtTxt() + QLatin1String("<br>") +
      tr("Check in exam help %1 how to do it automatically").arg(QStringLiteral("<a href=\"examHelp\">") +
      pixToHtml(Tpath::img("help"), m_iconSize) + a), Tcore::gl()->EanswerColor
    );
    m_confirmTip->setScale(m_scale * 1.2);
    m_scene->addItem(m_confirmTip);
    m_confirmTip->setTipMovable(true);
    m_confirmTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    connect(m_confirmTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
    connect(m_confirmTip, SIGNAL(moved()), this, SLOT(tipMoved()));
#endif
    setConfirmPos();
  }
}


void Tcanvas::playMelodyAgainMessage() {
#if defined (Q_OS_ANDROID)
  tMessage->setMessage(detectedText(tr("Select any note to play it again.")), 3000);
#else
  STATUS->setMessage(detectedText(tr("Select any note to play it again.")), 3000);
#endif
}


void Tcanvas::questionTip() {
	delete m_startTip;
	delete m_outTuneTip;
  clearWhatNextTip();
  clearMelodyCorrectMessage();
	createQuestionTip();
	m_guitarFree = m_questionTip->freeGuitar() && GUITAR->isVisible();
	m_nameFree = m_questionTip->freeName() && SCORE->insertMode() == TmultiScore::e_single;
	m_scoreFree = m_questionTip->freeScore();
  if (!m_guitarFree && !m_nameFree && !m_scoreFree) // workaround when only score is visible
    m_scoreFree = true;
	m_tipPos = e_scoreOver; // score is visible always
	if (m_nameFree && SCORE->insertMode() == TmultiScore::e_single)
		m_tipPos = e_nameOver;
	else if (m_scoreFree)
		m_tipPos = e_scoreOver;
	else if (m_guitarFree && GUITAR->isVisible())
		m_tipPos = e_guitarOver;
	m_questionTip->setMinimized(m_minimizedQuestion);
	setQuestionPos();
}


void Tcanvas::addTip(TgraphicsTextTip* tip) {
  m_scene->addItem(tip);
}


void Tcanvas::outOfTuneTip(float pitchDiff) {
	if (m_outTuneTip)
		return;
	QString tuneText;
	bool tooLow = true;
	if (pitchDiff < 0)
		tuneText = tr("too low");
	else {
		tuneText = tr("too high");
		tooLow = false;
	}
	m_outTuneTip = new TgraphicsTextTip(QString("<span style=\"color: %1; font-size: %2px;\">")
      .arg(Tcore::gl()->EanswerColor.name()).arg(bigFont()) + tuneText + "</span>");
  m_scene->addItem(m_outTuneTip);
  m_outTuneTip->setZValue(100);
  m_outTuneTip->setScale(m_scale);
	m_outTuneTip->setData(0, QVariant::fromValue<bool>(tooLow));
  setOutTunePos();
}


void Tcanvas::melodyCorrectMessage() {
	if (m_melodyCorrectMessage)
		return;

	m_melodyCorrectMessage = true;
  QString message = QString("<span style=\"color: %1;\"><big>").arg(Tcore::gl()->EanswerColor.name()) +
                    tr("Click incorrect notes to see<br>and to listen to them corrected.") + QLatin1String("</big></span>");
#if defined (Q_OS_ANDROID)
  tMessage->setMessage(message, 10000); // temporary message on a tip
#else
  STATUS->setBackground(-1);
  setStatusMessage(message); // permanent message on status label
#endif
}


/** @p prevTime param is to call clearing method after this time. */
void Tcanvas::correctToGuitar(TQAtype::Etype &question, int prevTime, TfingerPos& goodPos) {
	if (m_correctAnim)
		return;
	m_goodPos = goodPos;
	m_flyEllipse = new QGraphicsEllipseItem;
	m_flyEllipse->setPen(Qt::NoPen);
	m_flyEllipse->setBrush(QBrush(QColor(Tcore::gl()->EquestionColor.name())));
	m_scene->addItem(m_flyEllipse);
	if (question == TQAtype::e_asNote) {
			m_flyEllipse->setRect(SCORE->noteRect(1)); // 1 - answer note segment
			m_flyEllipse->setPos(m_view->mapToScene(SCORE->notePos(1)));
	} else if (question == TQAtype::e_asName) {
			m_flyEllipse->setRect(QRectF(0, 0, NOTENAME->labelRect().height() * 2, NOTENAME->labelRect().height()));
			m_flyEllipse->setPos(m_view->mapToScene(NOTENAME->mapToParent(NOTENAME->labelRect().topLeft())));
	} else if (question == TQAtype::e_asSound) {
			m_flyEllipse->setRect(QRectF(0, 0, SOUND->pitchView()->height() * 2, SOUND->pitchView()->height()));
			m_flyEllipse->setPos(m_view->mapToScene((SOUND->pitchView()->geometry().topLeft() +
				QPoint(SOUND->pitchView()->width() / 2 , 0))));
	}

	m_correctAnim = new TcombinedAnim(m_flyEllipse, this);
	m_correctAnim->setDuration(600);
	connect(m_correctAnim, SIGNAL(finished()), this, SLOT(correctAnimFinished()));
	QPointF destP = m_view->mapToScene(GUITAR->mapToParent(GUITAR->mapFromScene(GUITAR->fretToPos(goodPos))));
	if (!Tcore::gl()->GisRightHanded) { // fix destination position point for left-handed guitars
		if (goodPos.fret())
			destP.setX(destP.x() - GUITAR->fingerRect().width());
		else
			destP.setX(m_view->width() - destP.x());
	}
	m_correctAnim->setMoving(m_flyEllipse->pos(), destP);
	m_correctAnim->moving()->setEasingCurveType(QEasingCurve::InOutBack);
	if (goodPos.fret() != 0) {
			m_correctAnim->setScaling(GUITAR->fingerRect().width() / m_flyEllipse->rect().width(), 2.0);
			m_correctAnim->scaling()->setEasingCurveType(QEasingCurve::OutQuint);
	}
	m_correctAnim->setColoring(QColor(Tcore::gl()->EanswerColor.name()));
	if (goodPos.fret() == 0) {
		QPointF p1(m_view->mapToScene(GUITAR->mapToParent(
							GUITAR->mapFromScene(GUITAR->stringLine(goodPos.str()).p1()))));
		QPointF p2(m_view->mapToScene(GUITAR->mapToParent(
							GUITAR->mapFromScene(GUITAR->stringLine(goodPos.str()).p2()))));
		m_correctAnim->setMorphing(QLineF(p1, p2), GUITAR->stringWidth(goodPos.str() - 1));
	}
	m_correctAnim->startAnimations();
	QTimer::singleShot(prevTime, this, SLOT(clearCorrection()));
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void Tcanvas::levelStatusMessage() {
#if !defined (Q_OS_ANDROID)
  QString message;
  if (m_exam->isExercise())
      message = tr("You are exercising on level");
  else
      message = tr("Exam started on level");
  message.append(QLatin1String(":<br><b>") + m_exam->level()->name + QLatin1String("</b>"));
  STATUS->setBackground(-1); // reset background
  setStatusMessage(message);
#endif
}


void Tcanvas::clearCanvas() {
  clearConfirmTip();
  clearResultTip();
  if (m_whatTip) {
    delete m_whatTip;
  }
	delete m_startTip;
	delete m_questionTip;
	delete m_certifyTip;
  delete m_outTuneTip;
  clearMelodyCorrectMessage();
#if defined (Q_OS_ANDROID)
  delete m_nextTip;
  delete m_prevTip;
  delete m_correctTip;
#endif
}


void Tcanvas::clearResultTip() { delete m_resultTip; }


void Tcanvas::clearTryAgainTip() { delete m_tryAgainTip; }


void Tcanvas::clearConfirmTip() {
    m_timerToConfirm->stop();
		delete m_confirmTip;
}


void Tcanvas::clearCertificate() {
	if (m_certifyTip) {
		m_certifyTip->deleteLater();
		m_certifyTip = 0;
	}
}


void Tcanvas::clearCorrection() {
	if (m_correctAnim) {
		m_correctAnim->deleteLater();
		m_correctAnim = 0;
	}
	if (m_flyEllipse) {
		delete m_flyEllipse;
		m_flyEllipse = 0;
	}
	emit correctingFinished();
}


void Tcanvas::clearWhatNextTip() {
#if defined (Q_OS_ANDROID)
  delete m_nextTip;
  delete m_prevTip;
  delete m_correctTip;
#else
  delete m_whatTip;
#endif
}


void Tcanvas::clearMelodyCorrectMessage() {
  if (m_melodyCorrectMessage) {
    m_melodyCorrectMessage = false;
    levelStatusMessage();
  }
}


void Tcanvas::markAnswer(TQAtype::Etype qType, TQAtype::Etype aType) {
}


const QRect& Tcanvas::getRect(TQAtype::Etype kindOf) {
  switch (kindOf) {
    case TQAtype::e_asNote:
      return SCORE->geometry();
    case TQAtype::e_asName:
      return NOTENAME->geometry();
    case TQAtype::e_asFretPos:
          return GUITAR->geometry();
    case TQAtype::e_asSound:
      return SOUND->pitchView()->geometry();
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void Tcanvas::sizeChangedDelayed(const QRectF& newRect) {
	QSizeF factor(newRect.width() / m_prevSize.width(), newRect.height() / m_prevSize.height());
	for (int i = 0; i < 3; ++i) {
		if (!m_posOfQuestTips[i].isNull())
			m_posOfQuestTips[i] = QPointF(m_posOfQuestTips[i].x() * factor.width(), m_posOfQuestTips[i].y() * factor.height());
		if (!m_posOfWhatTips[i].isNull())
			m_posOfWhatTips[i] = QPointF(m_posOfWhatTips[i].x() * factor.width(), m_posOfWhatTips[i].y() * factor.height());
	}
	if (!m_posOfConfirm.isNull())
			m_posOfConfirm = QPointF(m_posOfConfirm.x() * factor.width(), m_posOfConfirm.y() * factor.height());
	m_prevSize = newRect.size();
	m_newSize = newRect.size().toSize();
	QTimer::singleShot(2, this, SLOT(sizeChanged()));
}


void Tcanvas::sizeChanged() {
	updateRelatedPoint();
  int hi;
  if (m_scene->height())
    hi = m_scene->height();
  else
    hi = 580;
  m_scale = m_scale * ((double)m_newSize.height() / hi);
	m_maxTipWidth = m_view->width() / 3;
  if (m_resultTip) {
			if (m_exam->curQ()->isNotSoBad())
				m_resultTip->setScale(m_scale);
			else
				m_resultTip->setScale(m_scale * 1.2);
      setResultPos();
  }
  if (m_tryAgainTip) {
    m_tryAgainTip->setScale(m_scale);
    setTryAgainPos();
  }
  if (m_whatTip) {
      m_whatTip->setScale(m_scale);
      setWhatNextPos();
  }
  if (m_startTip) {
    m_startTip->setScale(m_scale);
    setStartTipPos();
  }
  if (m_questionTip) {
    createQuestionTip();
		setQuestionPos();
  }
  if (m_confirmTip) {
    clearConfirmTip(); // To re-create confirm tip works better than re-scaling
    showConfirmTip();
//     m_confirmTip->setScale(m_scale * 1.2);
//     setConfirmPos();
  }
  if (m_certifyTip) {
    clearCertificate();
		QTimer::singleShot(50, this, SLOT(certificateTip()));
  }
  if (m_outTuneTip) {
		m_outTuneTip->setScale(m_scale);
		setOutTunePos();
  }
#if defined (Q_OS_ANDROID)
  if (m_nextTip) {
    setWhatNextPos();
  }
#endif
}


void Tcanvas::linkActivatedSlot(const QString& link) {
	emit buttonClicked(link);
#if defined (Q_OS_ANDROID)
  if (link == QLatin1String("correct"))
    delete m_correctTip;
#endif
	if (m_certifyTip)
		clearCertificate();
}


void Tcanvas::correctAnimFinished() {
// 	clearCorrection();
	m_flyEllipse->hide();
	GUITAR->setFinger(m_goodPos);
	GUITAR->markAnswer(QColor(Tcore::gl()->EanswerColor.name()));
	m_view->update();
}


bool m_menuKeyPressed = false;
bool Tcanvas::eventFilter(QObject* obj, QEvent* event) {
#if defined (Q_OS_ANDROID)
  if (event->type() == QEvent::KeyPress) // Press any physical device key with single tap to get cert preview
      m_menuKeyPressed = true;
  else if (event->type() == QEvent::KeyRelease)
      m_menuKeyPressed = false;
#endif
	if (event->type() == QEvent::MouseButtonPress) {
#if defined (Q_OS_ANDROID)
      if (m_menuKeyPressed)
#else
      QMouseEvent *me = static_cast<QMouseEvent*>(event);
			if (me->button() == Qt::MiddleButton && me->modifiers() | Qt::ShiftModifier &&  me->modifiers() | Qt::AltModifier)
#endif
      {
					if (m_exam)
						emit certificateMagicKeys();
			}
  }
	return QObject::eventFilter(obj, event);
}

//##################################################################################################
//#################################### PRIVATE #####################################################
//##################################################################################################

int Tcanvas::getMaxTipHeight() {
	if (m_nameFree || m_scoreFree)
		return SCORE->height() * 0.6;
	else
		return GUITAR->height() * 1.1;
}


void Tcanvas::setPosOfTip(TgraphicsTextTip* tip) {
	QRect geoRect;
	if (m_nameFree) { // middle of the noteName
			geoRect = NOTENAME->geometry();
	} else if (m_scoreFree) {// on the score at its center
			geoRect = SCORE->geometry();
			fixWidthOverScore(tip);
      if (SCORE->insertMode() != TmultiScore::e_single && !GUITAR->isVisible()) // only score - place it bottom right
        geoRect = QRect(m_view->width() - tip->realW(), m_view->height() - tip->realH(), tip->realW(), tip->realH());
	} else { // middle of the guitar
			geoRect = GUITAR->geometry();
      if (m_exam && !m_exam->melodies()) // in single mode put a tip on the right guitar side, below note name
        geoRect.setRect(NOTENAME->x(), GUITAR->y(), NOTENAME->width(), GUITAR->height());
  }
	tip->setPos(qMin(geoRect.x() + (geoRect.width() - tip->realW()) / 2, m_view->width() - tip->realW() - 5.0),
							qMin(geoRect.y() + (geoRect.height() - tip->realH()) / 2, m_view->height() - tip->realH() - 5.0));
	// qMin guards a tip position in scene boundaries 
}


void Tcanvas::setResultPos() {
	m_resultTip->setPos(m_scene->width() * 0.52 + (m_scene->width() * 0.48 - m_resultTip->realW()) / 2, m_scene->height() * 0.05);
}


void Tcanvas::setTryAgainPos() {
	QPointF tl(m_scene->width() * 0.6, m_scene->height() * 0.10); // top left of tip area
	if (m_resultTip) // place it below result tip
		tl.setY(m_resultTip->pos().y() + m_resultTip->realH());
	m_tryAgainTip->setPos(tl.x() + (m_scene->width() * 0.4 - m_scale * m_tryAgainTip->boundingRect().width()) / 2, tl.y());
}


void Tcanvas::setWhatNextPos() {
#if defined (Q_OS_ANDROID)
  qreal sc = (m_view->height() / 8.0) / m_nextTip->realH();
  if (sc > 1.0)
    m_nextTip->setScale(sc);
  qreal hh = m_view->height() * 0.75; // place tips above guitar, even it is hidden or doesn't exist
  m_nextTip->setPos(m_view->width() - m_nextTip->realW() - 4, hh - m_nextTip->realH());
  if (m_prevTip) {
    if (sc > 1.0)
      m_prevTip->setScale(sc);
    m_prevTip->setPos(4, hh - m_prevTip->realH());
  }
  if (m_correctTip) {
    if (sc > 1.0)
      m_correctTip->setScale(sc);
    m_correctTip->setPos(m_view->width() - m_correctTip->realW() - 4, m_nextTip->y() - m_correctTip->realH() - 8);
  }
#else
	int maxTipHeight = getMaxTipHeight();
  if (!m_nameFree && m_whatTip->realH() != maxTipHeight)
			m_whatTip->setScale((qreal)maxTipHeight / m_whatTip->realH());
	if (m_tipPos == e_nameOver) {
		if (m_whatTip->realW() != m_view->width() * 0.45)
			m_whatTip->setScale((m_view->width() * 0.45) / m_whatTip->realW());
		if (m_whatTip->realH() > SCORE->height())
			m_whatTip->setScale((qreal)(SCORE->height()) / m_whatTip->realH());
	} else
			fixWidthOverScore(m_whatTip);
	if (m_posOfWhatTips[(int)m_tipPos].isNull()) // calculate tip position only when user doesn't change it
		setPosOfTip(m_whatTip);
	else
		m_whatTip->setFixPos(m_posOfWhatTips[(int)m_tipPos]);
#endif
}


void Tcanvas::setStartTipPos() {
// in the middle of a window
  m_startTip->setPos((m_scene->width() - m_startTip->realW()) / 2, (m_scene->height() - m_startTip->realH()) / 2);  
}


void Tcanvas::setConfirmPos() { // right top corner
#if defined (Q_OS_ANDROID)
  qreal sc = (m_view->height() / 8.0) / m_confirmTip->realH();
  if (sc > 1.0)
    m_confirmTip->setScale(sc);
  m_confirmTip->setPos(m_view->width() - m_confirmTip->realW() - 4, 4); // 4 is more-less tip shadow size
#else
   m_confirmTip->setPos(m_view->width() - m_confirmTip->realW() - 20, 20);
#endif
}


void Tcanvas::createQuestionTip() {
	delete m_questionTip;
  m_questionTip = new TquestionTip(m_exam, m_scale * 1.2);
	m_questionTip->setTextWidth(m_maxTipWidth);
  m_scene->addItem(m_questionTip);
  connect(m_questionTip, SIGNAL(moved()), this, SLOT(tipMoved()));
  connect(m_questionTip, SIGNAL(minimizeChanged()), this, SLOT(tipStateChanged()));
}


void Tcanvas::setQuestionPos() {
	int maxTipHeight = getMaxTipHeight() * 1.1;
	qreal fineScale;
	if (m_questionTip->boundingRect().height() > maxTipHeight) { // check is scaling needed
			fineScale = (qreal)maxTipHeight / m_questionTip->boundingRect().height();
			qreal scaleStep = 0.0;
			while (m_questionTip->realH() > maxTipHeight) {
					delete m_questionTip;
					m_questionTip = new TquestionTip(m_exam, fineScale - scaleStep);
					m_questionTip->setTextWidth(m_maxTipWidth);
					m_scene->addItem(m_questionTip);
					scaleStep += 0.1;
			}
	}
	if (m_posOfQuestTips[(int)m_tipPos].isNull()) // calculate tip position only when user doesn't change it
			setPosOfTip(m_questionTip);
	else {
		fixWidthOverScore(m_questionTip);
		m_questionTip->setFixPos(m_posOfQuestTips[(int)m_tipPos]);
	}
	m_questionTip->show();
}


void Tcanvas::setOutTunePos() {
	int startX = SOUND->pitchView()->geometry().x();
	if (m_outTuneTip->realW() > SOUND->pitchView()->geometry().width() / 2)
			m_outTuneTip->setScale(m_outTuneTip->realW() / (SOUND->pitchView()->geometry().width() / 2));
	if (!m_outTuneTip->data(0).toBool())
		startX += SOUND->pitchView()->geometry().width() / 2;
	m_outTuneTip->setPos(startX + (SOUND->pitchView()->geometry().width() / 2 - m_outTuneTip->realW()) / 2,
												SOUND->pitchView()->y() - m_outTuneTip->realH());
}


void Tcanvas::updateRelatedPoint() {
	m_relPoint.setX(SCORE->geometry().x() + (NOTENAME->geometry().x() - SCORE->geometry().x()) / 2);
	m_relPoint.setY(SCORE->geometry().y());
}


void Tcanvas::fixWidthOverScore ( TgraphicsTextTip* tip ) {
	if (m_tipPos == e_scoreOver && tip->realW() > SCORE->width())
    tip->setScale((qMax((qreal)SCORE->width() * 0.9, m_view->width() / 3.0) / (tip->boundingRect().width())));
}



void Tcanvas::tipMoved() {
	if (sender() == m_questionTip)
		m_posOfQuestTips[(int)m_tipPos] = m_questionTip->pos();
	else if (sender() == m_whatTip)
		m_posOfWhatTips[(int)m_tipPos] = m_whatTip->pos();
	else if (sender() == m_confirmTip)
		m_posOfConfirm = m_confirmTip->pos();
}


void Tcanvas::tipStateChanged() {
	if (sender() == m_questionTip)
		m_minimizedQuestion = m_questionTip->isMinimized();
}







