/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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


#include "tcanvas.h"
#include "tquestiontip.h"
#include "tnootkacertificate.h"
#include "texecutorsupply.h"
#include "mainwindow.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/tresulttext.h>
#include <animations/tcombinedanim.h>
#include <texamparams.h>
#include <graphics/tgraphicstexttip.h>
#include <graphics/tnotepixmap.h>
#include <tglobals.h>
#include <help/texamhelp.h>
#include <notename/tnotename.h>
#include <guitar/tfingerboard.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#include <widgets/tquestionaswdg.h>
#include <tpath.h>
#include <score/tmainscore.h>
#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>


extern Tglobals *gl;


Tcanvas::Tcanvas(QGraphicsView* view, Texam* exam, MainWindow* parent) :
  QObject(parent->centralWidget()),
  m_view(view),
  m_window(parent),
  m_certifyTip(0),
  m_exam(exam),
  m_scale(1),
  m_flyEllipse(0),
  m_timerToConfirm(new QTimer(this)),
  m_minimizedQuestion(false), m_melodyCorrectMessage(false)
{
  m_scene = m_view->scene();
	m_newSize = m_scene->sceneRect().size().toSize();
	m_prevSize = m_scene->sceneRect().size();
  m_iconSize = m_view->fontMetrics().boundingRect("A").height() * 2;
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


//######################################################################
//##################################### TIPS ###########################
//######################################################################

int Tcanvas::bigFont() {
  return (m_view->fontMetrics().boundingRect("A").height() * 2);
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

  bool autoNext = gl->E->autoNextQuest;
  if (gl->E->afterMistake == TexamParams::e_stop && !answer->isCorrect())
      autoNext = false; // when mistake and e_stop - the same like autoNext = false;
  if (autoNext) { // determine time of displaying
    if (answer->isCorrect() || gl->E->afterMistake == TexamParams::e_continue)
      time = 2500; // hard-coded
    else
      time = gl->E->mistakePreview; // user defined wait time
  }
    
  m_resultTip = new TgraphicsTextTip(wasAnswerOKtext(answer, TexecutorSupply::answerColor(answer->mistake()), bigFont()));
  m_scene->addItem(m_resultTip);
  m_resultTip->setZValue(100);
	if (answer->isNotSoBad())
		m_resultTip->setScale(m_scale);
	else
		m_resultTip->setScale(m_scale * 1.2);
  setResultPos();
  if (gl->E->showWrongPlayed && gl->E->showWrongPlayed && !answer->melody() &&
			answer->answerAsSound() && !answer->isCorrect() && m_window->sound->note().note)
					detectedNoteTip(m_window->sound->note()); // In exercise mode display detected note when it was incorrect
  if (time)
			QTimer::singleShot(time, this, SLOT(clearResultTip()));
}


QString Tcanvas::detectedText(const QString& txt) {
  return QString("<span style=\"color: %1;\"><big>").arg(gl->EquestionColor.name()) + txt + "</big></span>";
}


void Tcanvas::detectedNoteTip(const Tnote& note) {
  Tnote n = note;
  if (n.isValid())
    m_window->setStatusMessage("<table valign=\"middle\" align=\"center\"><tr><td> " +
        wrapPixToHtml(n, m_exam->level()->clef.type(),	TkeySignature(0), m_window->centralWidget()->height() / 260.0) + " " +
      detectedText(tr("%1 was detected", "note name").arg(n.toRichText())) + "</td></tr></table>", 5000);
}


void Tcanvas::tryAgainTip(int time) {
  m_tryAgainTip = new TgraphicsTextTip(QString("<span style=\"color: %1; font-size: %2px;\">")
      .arg(gl->EquestionColor.name()).arg(bigFont()) + tr("Try again!") + "</span>");
  m_scene->addItem(m_tryAgainTip);
  m_tryAgainTip->setZValue(100);
  m_tryAgainTip->setScale(m_scale);
  setTryAgainPos();
  QTimer::singleShot(time, this, SLOT(clearTryAgainTip()));
}


QString Tcanvas::startTipText() {
  return TexamHelp::toGetQuestTxt() + ":<br>" + 
    TexamHelp::clickSomeButtonTxt("<a href=\"nextQuest\">" + pixToHtml(Tpath::img("nextQuest"), m_iconSize) + "</a>") + 
    ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt();
}


void Tcanvas::startTip() {
   m_startTip = new TgraphicsTextTip(QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75)) + startTipText() + ".<br>" +
     TexamHelp::toStopExamTxt("<a href=\"stopExam\"> " + pixToHtml(Tpath::img("stopExam"), m_iconSize) + "</a>") + 
     "</p>", m_window->palette().highlight().color());
   m_scene->addItem(m_startTip);
   m_startTip->setScale(m_scale);
   m_startTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
   connect(m_startTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
   setStartTipPos();
}


void Tcanvas::certificateTip() {
  if (m_certifyTip)
    return;

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
	delete m_whatTip;
	QString whatNextText = startTipText();;
	if (!isCorrect) {
		QString t = tr("To correct an answer");
		QString href = "<a href=\"prevQuest\">";
		if (m_exam->melodies()) {
			t = tr("To try this melody again");
			href = "<a href=\"newAttempt\">";
		}
		whatNextText += "<br>" + t + " " + 
				TexamHelp::clickSomeButtonTxt(href + pixToHtml(Tpath::img("prevQuest"), m_iconSize) +	"</a>") +
				" " + TexamHelp::orPressBackSpace();
	}
	if (toCorrection) {
		QString t = tr("To see corrected answer");
		if (m_exam->curQ()->melody())
				t = tr("To see some hints");
		whatNextText += "<br>" + t + " " + 
			TexamHelp::clickSomeButtonTxt("<a href=\"correct\">" + pixToHtml(Tpath::img("correct"), m_iconSize) + "</a>") + "<br>" +
			TexamHelp::orPressEnterKey();
	}
	whatNextText += "<br>" + TexamHelp::toStopExamTxt("<a href=\"stopExam\">" + pixToHtml(Tpath::img("stopExam"), m_iconSize) + "</a>");		
  m_whatTip = new TgraphicsTextTip(whatNextText, m_window->palette().highlight().color());
// 	if (m_guitarFree) // tip is wide there, otherwise text is word-wrapped and is narrowest but higher
// 			m_whatTip->setTextWidth(m_maxTipWidth);
  m_scene->addItem(m_whatTip);
  m_whatTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
  m_whatTip->setTipMovable(true);
  connect(m_whatTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
  connect(m_whatTip, SIGNAL(moved()), this, SLOT(tipMoved()));
  setWhatNextPos();
}


void Tcanvas::confirmTip(int time) {
  m_timerToConfirm->start(time + 1); // add 1 to show it immediately when time = 0
}


void Tcanvas::showConfirmTip() {
  m_timerToConfirm->stop();
  if (!m_confirmTip) {
    m_confirmTip = new TgraphicsTextTip(tr("To check the answer confirm it:") + "<br>- " +
      TexamHelp::clickSomeButtonTxt("<a href=\"checkAnswer\">" + pixToHtml(Tpath::img("check"), m_iconSize) + "</a>") +
      "<br>- " + TexamHelp::pressEnterKey() + "<br>- " + TexamHelp::orRightButtTxt() + "<br>" +
      tr("Check in exam help %1 how to do it automatically").arg("<a href=\"examHelp\">" +
      pixToHtml(Tpath::img("help"), m_iconSize) + "</a>"), gl->EanswerColor);
    m_confirmTip->setScale(m_scale * 1.2);
    m_scene->addItem(m_confirmTip);
    m_confirmTip->setTipMovable(true);
    m_confirmTip->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    connect(m_confirmTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
    connect(m_confirmTip, SIGNAL(moved()), this, SLOT(tipMoved()));
    setConfirmPos();
  }
}


void Tcanvas::playMelodyAgainMessage() {
  m_window->setStatusMessage(detectedText(tr("Select any note to play it again.")), 3000);
}


void Tcanvas::questionTip() {
	delete m_startTip;  
  delete m_whatTip;
	delete m_outTuneTip;
  clearMelodyCorrectMessage();
	createQuestionTip();
	m_guitarFree = m_questionTip->freeGuitar() && m_window->guitar->isVisible();
	m_nameFree = m_questionTip->freeName() && m_window->score->insertMode() == TmultiScore::e_single;
	m_scoreFree = m_questionTip->freeScore();
  if (!m_guitarFree && !m_nameFree && !m_scoreFree) // workaround when only score is visible
    m_scoreFree = true;
	m_tipPos = e_scoreOver; // score is visible always
	if (m_nameFree && m_window->score->insertMode() == TmultiScore::e_single)
		m_tipPos = e_nameOver;
	else if (m_scoreFree)
		m_tipPos = e_scoreOver;
	else if (m_guitarFree && m_window->guitar->isVisible())
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
      .arg(gl->EanswerColor.name()).arg(bigFont()) + tuneText + "</span>");
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
  m_window->setMessageBg (-1);
  m_window->setStatusMessage(QString("<span style=\"color: %1;\"><big>").arg(gl->EanswerColor.name()) + 
										tr("Click incorrect notes to see<br>and to listen to them corrected.") + "</big></span>");
}


/** @p prevTime param is to call clearing method after this time. */
void Tcanvas::correctToGuitar(TQAtype::Etype &question, int prevTime, TfingerPos& goodPos) {
	if (m_correctAnim)
		return;
	m_goodPos = goodPos;
	m_flyEllipse = new QGraphicsEllipseItem;
	m_flyEllipse->setPen(Qt::NoPen);
	m_flyEllipse->setBrush(QBrush(QColor(gl->EquestionColor.name())));
	m_scene->addItem(m_flyEllipse);
	if (question == TQAtype::e_asNote) {
			m_flyEllipse->setRect(m_window->score->noteRect(1)); // 1 - answer note segment	
			m_flyEllipse->setPos(m_view->mapToScene(m_window->score->notePos(1)));
	} else if (question == TQAtype::e_asName) {
			m_flyEllipse->setRect(QRectF(0, 0, m_window->noteName->labelRect().height() * 2, m_window->noteName->labelRect().height()));
			m_flyEllipse->setPos(m_view->mapToScene(m_window->noteName->mapToParent(m_window->noteName->labelRect().topLeft())));
	} else if (question == TQAtype::e_asSound) {
			m_flyEllipse->setRect(QRectF(0, 0, m_window->pitchView->height() * 2, m_window->pitchView->height()));
			m_flyEllipse->setPos(m_view->mapToScene((m_window->pitchView->geometry().topLeft() + 
				QPoint(m_window->pitchView->width() / 2 , 0))));
	}
	
	m_correctAnim = new TcombinedAnim(m_flyEllipse, this);
	m_correctAnim->setDuration(600);
	connect(m_correctAnim, SIGNAL(finished()), this, SLOT(correctAnimFinished()));
	QPointF destP = m_view->mapToScene(m_window->guitar->mapToParent(m_window->guitar->mapFromScene(m_window->guitar->fretToPos(goodPos))));
	if (!gl->GisRightHanded) { // fix destination position point for left-handed guitars
		if (goodPos.fret())
			destP.setX(destP.x() - m_window->guitar->fingerRect().width());
		else
			destP.setX(m_view->width() - destP.x());
	}
	m_correctAnim->setMoving(m_flyEllipse->pos(), destP);
	m_correctAnim->moving()->setEasingCurveType(QEasingCurve::InOutBack);
	if (goodPos.fret() != 0) {
			m_correctAnim->setScaling(m_window->guitar->fingerRect().width() / m_flyEllipse->rect().width(), 2.0);
			m_correctAnim->scaling()->setEasingCurveType(QEasingCurve::OutQuint);
	}
	m_correctAnim->setColoring(QColor(gl->EanswerColor.name()));
	if (goodPos.fret() == 0) {
		QPointF p1(m_view->mapToScene(m_window->guitar->mapToParent(
							m_window->guitar->mapFromScene(m_window->guitar->stringLine(goodPos.str()).p1()))));
		QPointF p2(m_view->mapToScene(m_window->guitar->mapToParent(
							m_window->guitar->mapFromScene(m_window->guitar->stringLine(goodPos.str()).p2()))));
		m_correctAnim->setMorphing(QLineF(p1, p2), m_window->guitar->stringWidth(goodPos.str() - 1));
	}
	m_correctAnim->startAnimations();
	QTimer::singleShot(prevTime, this, SLOT(clearCorrection()));
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void Tcanvas::levelStatusMessage() {
  m_window->setMessageBg(-1); // reset background
  if (m_exam->isExercise())
      m_window->setStatusMessage(tr("You are exercising on level") + ":<br><b>" + m_exam->level()->name + "</b>");
  else
      m_window->setStatusMessage(tr("Exam started on level") + ":<br><b>" + m_exam->level()->name + "</b>");
}


void Tcanvas::clearCanvas() {
  clearConfirmTip();
  clearResultTip();
  if (m_whatTip) {
//     m_window->guitar->setAttribute(Qt::WA_TransparentForMouseEvents, false); // unlock guitar for mouse
    delete m_whatTip;
  }
	delete m_startTip;
	delete m_questionTip;
  clearCertificate();
  delete m_outTuneTip;
  clearMelodyCorrectMessage();
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


void Tcanvas::clearWhatNextTip() { delete m_whatTip; }


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
      return m_window->score->geometry();
    case TQAtype::e_asName:
      return m_window->noteName->geometry();
    case TQAtype::e_asFretPos:
          return m_window->guitar->geometry();
    case TQAtype::e_asSound:
      return m_window->pitchView->geometry();
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
    m_confirmTip->setScale(m_scale * 1.2);
    setConfirmPos();
  }
  if (m_certifyTip) {
    clearCertificate();
		QTimer::singleShot(50, this, SLOT(certificateTip()));
  }
  if (m_outTuneTip) {
		m_outTuneTip->setScale(m_scale);
		setOutTunePos();
  }
}


void Tcanvas::linkActivatedSlot(const QString& link) {
	emit buttonClicked(link);
	if (m_certifyTip)
		clearCertificate();
}


void Tcanvas::correctAnimFinished() {
// 	clearCorrection();
	m_flyEllipse->hide();
	m_window->guitar->setFinger(m_goodPos);
	m_window->guitar->markAnswer(QColor(gl->EanswerColor.name()));
	m_window->update();
}


bool Tcanvas::eventFilter(QObject* obj, QEvent* event) {
	if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (event->type() == QEvent::MouseButtonPress) {
			if (me->button() == Qt::MiddleButton && me->modifiers() | Qt::ShiftModifier &&  me->modifiers() | Qt::AltModifier) {
          if (m_exam && !m_certifyTip)
						emit certificateMagicKeys();
			}
    }
  }     
	return QObject::eventFilter(obj, event);
}

//##################################################################################################
//#################################### PRIVATE #####################################################
//##################################################################################################

int Tcanvas::getMaxTipHeight() {
	if (m_nameFree || m_scoreFree)
		return m_window->score->height() * 0.6;
	else
		return m_window->guitar->height() * 1.1;
}


void Tcanvas::setPosOfTip(TgraphicsTextTip* tip) {
	QRect geoRect;
	if (m_nameFree) { // middle of the noteName
			geoRect = m_window->noteName->geometry();
	} else if (m_scoreFree) {// on the score at its center
			geoRect = m_window->score->geometry();
			fixWidthOverScore(tip);
      if (m_window->score->insertMode() != TmultiScore::e_single && !m_window->guitar->isVisible()) // only score - place it bottom right
        geoRect = QRect(m_view->width() - tip->realW(), m_view->height() - tip->realH(), tip->realW(), tip->realH());
	} else { // middle of the guitar
			geoRect = m_window->guitar->geometry();
      if (m_exam && !m_exam->melodies()) // in single mode put a tip on the right guitar side, below note name
        geoRect.setRect(m_window->noteName->x(), m_window->guitar->y(), m_window->noteName->width(), m_window->guitar->height());
  }
	tip->setPos(qMin(geoRect.x() + (geoRect.width() - tip->realW()) / 2, m_window->width() - tip->realW() - 5.0),
							qMin(geoRect.y() + (geoRect.height() - tip->realH()) / 2, m_window->height() - tip->realH() - 5.0));
	// qMin guards a tip position in scene boundaries 
}


void Tcanvas::setResultPos() {
	m_resultTip->setPos(m_scene->width() * 0.52 + (m_scene->width() * 0.48 - m_resultTip->realW()) / 2, m_scene->height() * 0.01);
}


void Tcanvas::setTryAgainPos() {
	QPointF tl(m_scene->width() * 0.6, m_scene->height() * 0.10); // top left of tip area
	if (m_resultTip) // place it below result tip
		tl.setY(m_resultTip->pos().y() + m_resultTip->realH());
	m_tryAgainTip->setPos(tl.x() + (m_scene->width() * 0.4 - m_scale * m_tryAgainTip->boundingRect().width()) / 2, tl.y());
}


void Tcanvas::setWhatNextPos() {
	int maxTipHeight = getMaxTipHeight();
  if (!m_nameFree && m_whatTip->realH() != maxTipHeight)
			m_whatTip->setScale((qreal)maxTipHeight / m_whatTip->realH());
	if (m_tipPos == e_nameOver) {
		if (m_whatTip->realW() != m_window->width() * 0.45)
			m_whatTip->setScale((m_window->width() * 0.45) / m_whatTip->realW());
		if (m_whatTip->realH() > m_window->score->height())
			m_whatTip->setScale((qreal)(m_window->score->height()) / m_whatTip->realH());
	} else
			fixWidthOverScore(m_whatTip);
	if (m_posOfWhatTips[(int)m_tipPos].isNull()) // calculate tip position only when user doesn't change it
		setPosOfTip(m_whatTip);
	else
		m_whatTip->setFixPos(m_posOfWhatTips[(int)m_tipPos]);
}


void Tcanvas::setStartTipPos() {
// in the middle of a window
  m_startTip->setPos((m_scene->width() - m_startTip->realW()) / 2, (m_scene->height() - m_startTip->realH()) / 2);  
}


void Tcanvas::setConfirmPos() { // right top corner
	m_confirmTip->setPos(m_window->width() - m_confirmTip->realW() - 20, 20);  
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
	int startX = m_window->pitchView->geometry().x();
	if (m_outTuneTip->realW() > m_window->pitchView->geometry().width() / 2)
			m_outTuneTip->setScale(m_outTuneTip->realW() / (m_window->pitchView->geometry().width() / 2));
	if (!m_outTuneTip->data(0).toBool())
		startX += m_window->pitchView->geometry().width() / 2;
	m_outTuneTip->setPos(startX + (m_window->pitchView->geometry().width() / 2 - m_outTuneTip->realW()) / 2, 
												m_window->pitchView->y() - m_outTuneTip->realH());
}


void Tcanvas::updateRelatedPoint() {
	m_relPoint.setX(m_window->score->geometry().x() + (m_window->noteName->geometry().x() - m_window->score->geometry().x()) / 2);
	m_relPoint.setY(m_window->score->geometry().y());
}


void Tcanvas::fixWidthOverScore ( TgraphicsTextTip* tip ) {
	if (m_tipPos == e_scoreOver && tip->realW() > m_window->score->width())
    tip->setScale((qMax((qreal)m_window->score->width() * 0.9, m_window->width() / 3.0) / (tip->boundingRect().width())));
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







