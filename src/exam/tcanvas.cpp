/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include "tquestiontip.h"
#include <animations/tcombinedanim.h>
#include "tnootkacertificate.h"
#include <texamparams.h>
#include <graphics/tgraphicstexttip.h>
#include "mainwindow.h"
#include <graphics/tnotepixmap.h>
#include "tglobals.h"
#include <help/texamhelp.h>
#include <notename/tnotename.h>
#include <guitar/tfingerboard.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#include <widgets/tquestionaswdg.h>
#include <score/tmainscore.h>
// #include <widgets/tanimedchbox.h>
#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>



#define PIXICONSIZE (32)

extern Tglobals *gl;


Tcanvas::Tcanvas(QGraphicsView* view, MainWindow* parent) :
  QObject(parent->centralWidget()),
  m_view(view),
  m_window(parent),
  m_certifyTip(0),
  m_exam(0),
  m_scale(1),
  m_flyEllipse(0),
  m_timerToConfirm(new QTimer(this))
{
  
  m_scene = m_view->scene();
	m_newSize = m_scene->sceneRect().size().toSize();
  sizeChanged();
	connect(m_scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sizeChangedDelayed(QRectF)));
  connect(m_timerToConfirm, SIGNAL(timeout()), this, SLOT(showConfirmTip()));
}

Tcanvas::~Tcanvas()
{}

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
  QColor answColor;
  if (answer->isCorrect())
      answColor = gl->EanswerColor;
  else
    if (answer->isWrong())
        answColor = gl->EquestionColor;
    else
        answColor = gl->EnotBadColor;
    
  m_resultTip = new TgraphicsTextTip(wasAnswerOKtext(answer, answColor, bigFont()));
  m_scene->addItem(m_resultTip);
  m_resultTip->setZValue(100);
  m_resultTip->setScale(m_scale);
  setResultPos();
  // In exercise mode display detected note when it was incorrect TODO exam run-time settings
  if (/*m_window->correctChB->isVisible() && */gl->E->showWrongPlayed && 
			answer->answerAs == TQAtype::e_asSound && !answer->isCorrect() && m_window->sound->note().note) {
					int tt = 5000;
					if (time) // will be deleted with tip or after 5 s when tip remains
						tt = time;
					m_window->setStatusMessage("<table valign=\"middle\" align=\"center\"><tr><td> " + wrapPixToHtml
							(m_window->sound->note(), m_exam->level()->clef.type(), TkeySignature(0), m_window->centralWidget()->height() / 260.0) + 
							QString("<span style=\"color: %1;\"><big>").arg(answColor.name()) + 
							tr("%1 was detected", "note name").arg(m_window->sound->note().toRichText()) + "</big></span></td></tr></table>", tt);
  }
  if (time)
			QTimer::singleShot(time, this, SLOT(clearResultTip()));
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
    TexamHelp::clickSomeButtonTxt("<a href=\"nextQuest\">" + pixToHtml(gl->path + "picts/nextQuest.png", PIXICONSIZE) + "</a>") + 
    ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt();
}


void Tcanvas::startTip() {
   m_startTip = new TgraphicsTextTip(QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75)) + startTipText() + ".<br>" +
     TexamHelp::toStopExamTxt("<a href=\"stopExam\"> " + pixToHtml(gl->path + "picts/stopExam.png", PIXICONSIZE) + "</a>") + "</p>", m_window->palette().highlight().color());
   m_scene->addItem(m_startTip);
   m_startTip->setScale(m_scale);
   m_startTip->setTextInteractionFlags(Qt::TextBrowserInteraction);
   connect(m_startTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
   setStartTipPos();
}


void Tcanvas::certificateTip() {
	delete m_questionTip;
	if (!m_certifyTip) {
			m_certifyTip = new TnootkaCertificate(m_view, gl->path, m_exam);
			connect(m_certifyTip, SIGNAL(userAction(QString)), this, SLOT(linkActivatedSlot(QString)));
	}
}


void Tcanvas::whatNextTip(bool isCorrect, bool toCorrection) {
	delete m_questionTip;
	delete m_whatTip;
  QString whatNextText = startTipText();
//   if (!m_window->autoRepeatChB->isChecked()) TODO exam run-time settings
//       m_window->autoRepeatChB->startAnimation(3);
  if (!isCorrect)
      whatNextText += "<br>" + tr("To correct an answer") + " " + 
      TexamHelp::clickSomeButtonTxt("<a href=\"prevQuest\">" + pixToHtml(gl->path + "picts/prevQuest.png", PIXICONSIZE) + "</a>") +
      " " + TexamHelp::orPressBackSpace();
	if (toCorrection) {
		whatNextText += "<br>" + tr("To see corrected answer") + " " + 
			TexamHelp::clickSomeButtonTxt("<a href=\"correct\">" + pixToHtml(gl->path + "picts/correct.png", PIXICONSIZE) + "</a>") +
			TexamHelp::orPressEnterKey();
//       if (!m_window->correctChB->isChecked()) TODO exam run-time settings
//           m_window->correctChB->startAnimation(3);
  }
  whatNextText += "<br>" + TexamHelp::toStopExamTxt("<a href=\"stopExam\">" + pixToHtml(gl->path + "picts/stopExam.png", PIXICONSIZE) + "</a>");
  
  m_whatTip = new TgraphicsTextTip(whatNextText, m_window->palette().highlight().color());
	if (m_guitarFree) // tip is wide there, otherwise text is word-wrapped and is narrowest but higher
			m_whatTip->setTextWidth(m_maxTipWidth);
  m_scene->addItem(m_whatTip);
//   m_whatTip->setFont(tipFont(0.35));
//   m_window->guitar->setAttribute(Qt::WA_TransparentForMouseEvents, true); // to activate click on tip
  m_whatTip->setTextInteractionFlags(Qt::TextBrowserInteraction);
  connect(m_whatTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
  setWhatNextPos();
}


void Tcanvas::confirmTip(int time) {
  m_timerToConfirm->start(time + 1); // add 1 to show it immediately when time = 0
}


void Tcanvas::showConfirmTip() {
  m_timerToConfirm->stop();
	if (!m_confirmTip) {
//     m_window->expertAnswChB->startAnimation(3); TODO exam run-time settings
		m_confirmTip = new TgraphicsTextTip(tr("To check the answer confirm it:") + "<br>- " + 
			TexamHelp::clickSomeButtonTxt("<a href=\"checkAnswer\">" + pixToHtml(gl->path + "picts/check.png", PIXICONSIZE) + "</a>") +
			"<br>- " + TexamHelp::pressEnterKey() + "<br>- " + TexamHelp::orRightButtTxt() + "<br>" +
			tr("Check in exam help %1 how to do it automatically").arg("<a href=\"examHelp\">" + 
			pixToHtml(gl->path + "picts/help.png", PIXICONSIZE) + "</a>"), gl->EanswerColor);
		m_confirmTip->setScale(m_scale);
		m_scene->addItem(m_confirmTip);
		m_confirmTip->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_confirmTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
		setConfirmPos();
	}
}


void Tcanvas::questionTip(Texam* exam) {
  m_exam = exam;
	delete m_startTip;  
  delete m_whatTip;
	delete m_outTuneTip;
	delete m_questionTip;
  m_questionTip = new TquestionTip(exam, m_scale);
	m_questionTip->setTextWidth(m_maxTipWidth);
  m_scene->addItem(m_questionTip);
	m_guitarFree = m_questionTip->freeGuitar();
	m_nameFree = m_questionTip->freeName();
	m_scoreFree = m_questionTip->freeScore();
// 	m_questionTip->show();
  setQuestionPos();
// 	m_questionTip->setZValue(255);
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

//######################################################################
//##################################### PUBLIC METHODS #################
//######################################################################

void Tcanvas::clearCanvas() {
  clearConfirmTip();
  clearResultTip();
  if (m_whatTip) {
//     m_window->guitar->setAttribute(Qt::WA_TransparentForMouseEvents, false); // unlock guitar for mouse
    delete m_whatTip;
  }
	delete m_startTip;
	delete m_questionTip;
	delete m_certifyTip;
  delete m_outTuneTip;
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
}


void Tcanvas::clearWhatNextTip() { delete m_whatTip; }


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


//######################################################################
//#################################### PROTECTED #######################
//######################################################################


void Tcanvas::sizeChangedDelayed(const QRectF& newRect) {
	m_newSize = newRect.size().toSize();
	QTimer::singleShot(2, this, SLOT(sizeChanged()));
}


void Tcanvas::sizeChanged() {
	m_window->score->resize(m_window->score->widthToHeight(m_window->score->height()), m_window->score->height());
	updateRelatedPoint();
  int hi;
  if (m_scene->height())
    hi = m_scene->height();
  else
    hi = 580;
//   m_scene->setSceneRect(geometry());
  m_scale = m_scale * ((double)m_newSize.height() / hi);
	m_maxTipWidth = m_view->width() / 3;
  if (m_resultTip) {
      m_resultTip->setScale(m_scale);;
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
    delete m_questionTip;
    m_questionTip = new TquestionTip(m_exam, m_scale);
    m_scene->addItem(m_questionTip);
    setQuestionPos();
  }
  if (m_confirmTip) {
    m_confirmTip->setScale(m_scale);
    setConfirmPos();
  }
  if (m_certifyTip) {
    clearCertificate();
		certificateTip();		
  }
  if (m_outTuneTip) {
		m_outTuneTip->setScale(m_scale);
		setOutTunePos();
  }		
}


void Tcanvas::linkActivatedSlot(QString link) {
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


/*
bool Tcanvas::event(QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
    event->setAccepted(false);
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    me->setAccepted(false);
    if (event->type() == QEvent::MouseButtonPress) {
        QGraphicsView::mousePressEvent(me);
        if (me->button() == Qt::MiddleButton && me->modifiers().testFlag(Qt::ShiftModifier) &&  me->modifiers().testFlag(Qt::AltModifier)) {
            if (m_exam)
              emit certificateMagicKeys();
        }
    } else
        QGraphicsView::mouseReleaseEvent(me);
  }     
  return QGraphicsView::event(event);
}*/


//##################################################################################################
//#################################### PRIVATE #####################################################
//##################################################################################################

int Tcanvas::getMaxTipHeight() {
	if (m_nameFree)
			return m_window->noteName->height();
	else if (m_scoreFree)
			return m_window->score->height() * 0.75;
	else
			return m_window->guitar->height();
}


void Tcanvas::setPosOfTip(TgraphicsTextTip* tip) {
	QRect geoRect;
	if (m_nameFree) { // middle of the noteName
			geoRect = m_window->noteName->geometry();
			if (tip == m_whatTip) {
				if (tip->boundingRect().width() * tip->scale() != m_window->noteName->geometry().width() - 20)
					tip->setScale((m_window->noteName->geometry().width() - 20) / (tip->boundingRect().width() * tip->scale()));
				if (tip->boundingRect().height() * tip->scale() > m_window->noteName->height())
					tip->setScale((qreal)(m_window->noteName->height()) / (tip->boundingRect().height()));
			}
	} else if (m_scoreFree) {// on the score at its center
			geoRect = m_window->score->geometry();
			if (tip->boundingRect().width() * tip->scale() > m_window->score->width())
				tip->setScale((qMax((qreal)m_window->score->width(), m_window->width() / 3.0) / (tip->boundingRect().width())));
	} else { // middle of the guitar
			geoRect = m_window->guitar->geometry();
			if (!m_window->pitchView->isVisible() || !m_window->guitar->isVisible()) // tip can be bigger
				geoRect = QRect(m_window->noteName->geometry().x() - 20,
							m_window->guitar->geometry().y() - (m_window->guitar->geometry().top() - m_window->noteName->geometry().bottom()) / 2,
							m_window->guitar->width() - m_window->noteName->geometry().x() - 20,
							m_window->guitar->height() + (m_window->guitar->geometry().top() - m_window->noteName->geometry().bottom()) / 2);
		}
	tip->setPos(geoRect.x() + (geoRect.width() - tip->boundingRect().width() * tip->scale()) / 2,
		geoRect.y() + (geoRect.height() - tip->boundingRect().height() * tip->scale()) / 2 );
}


void Tcanvas::setResultPos() {
  m_resultTip->setPos(m_relPoint.x() + (((m_scene->width() - m_relPoint.x()) - m_scale * m_resultTip->boundingRect().width())) / 2,
                      m_relPoint.y());
}


void Tcanvas::setTryAgainPos() {
  m_tryAgainTip->setPos(m_relPoint.x() + (((m_scene->width() - m_relPoint.x()) -
											m_scale * m_tryAgainTip->boundingRect().width())) / 2,
                      m_window->noteName->geometry().top() - m_scale * m_tryAgainTip->boundingRect().height());
}


void Tcanvas::setWhatNextPos() {
	int maxTipHeight = getMaxTipHeight();
  if (!m_nameFree && m_whatTip->boundingRect().height() * m_whatTip->scale() != maxTipHeight)
			m_whatTip->setScale((qreal)maxTipHeight / (m_whatTip->boundingRect().height() * m_whatTip->scale()));
	setPosOfTip(m_whatTip);
}


void Tcanvas::setStartTipPos() {
// in the middle of a window
  m_startTip->setPos((m_scene->width() - m_scale * (m_startTip->boundingRect().width())) / 2,
                  (m_scene->height() - m_scale * (m_startTip->boundingRect().height())) / 2 );  
}


void Tcanvas::setConfirmPos() { // middle of noteName and somewhere above
    m_confirmTip->setPos(m_window->noteName->geometry().x() + (m_window->noteName->width() - m_confirmTip->boundingRect().width()) / 2,
												 m_relPoint.y());  
}


void Tcanvas::setQuestionPos() {
	int maxTipHeight = getMaxTipHeight() * 1.1;
	qreal fineScale;
	if (m_questionTip->boundingRect().height() * m_questionTip->scale() > maxTipHeight) { // check is scaling needed
			fineScale = (qreal)maxTipHeight / m_questionTip->boundingRect().height();
			qreal scaleStep = 0.0;
			while (m_questionTip->boundingRect().height() * m_questionTip->scale() > maxTipHeight) {
					delete m_questionTip;
					m_questionTip = new TquestionTip(m_exam, m_scale * fineScale - scaleStep);
					m_questionTip->setTextWidth(m_maxTipWidth);
					m_scene->addItem(m_questionTip);
					scaleStep += 0.1;
			}
	}
	setPosOfTip(m_questionTip);
	m_questionTip->show();
}


void Tcanvas::setOutTunePos() {
	int startX = m_window->pitchView->geometry().x();
	if (m_outTuneTip->boundingRect().width() * m_outTuneTip->scale() > m_window->pitchView->geometry().width() / 2)
			m_outTuneTip->setScale((m_outTuneTip->boundingRect().width() * m_outTuneTip->scale()) / 
							(m_window->pitchView->geometry().width() / 2));
	if (!m_outTuneTip->data(0).toBool())
		startX += m_window->pitchView->geometry().width() / 2;
	m_outTuneTip->setPos(startX + (m_window->pitchView->geometry().width() / 2 - m_outTuneTip->boundingRect().width()) / 2, 
		m_window->pitchView->y() - m_outTuneTip->boundingRect().height() * m_outTuneTip->scale());
}


void Tcanvas::updateRelatedPoint() {
	m_relPoint.setX(m_window->score->geometry().x() + (m_window->noteName->geometry().x() - m_window->score->geometry().x()) / 2);
	m_relPoint.setY(m_window->score->geometry().y());
}







