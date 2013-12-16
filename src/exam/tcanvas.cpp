/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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
#include "tqaunit.h"
#include "texam.h"
#include "tquestiontip.h"
#include <animations/tcombinedanim.h>
#include "tnootkacertificate.h"
#include "tgraphicstexttip.h"
#include "mainwindow.h"
#include "tnotepixmap.h"
#include "tglobals.h"
#include "texamhelp.h"
#include "tpixmaker.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include "tpitchview.h"
#include "tquestionaswdg.h"
#include "tmainscore.h"
#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>



#define PIXICONSIZE (32)

extern Tglobals *gl;


Tcanvas::Tcanvas(MainWindow* parent) :
  QGraphicsView(parent->centralWidget()),
  m_parent(parent),
  m_resultTip(0), m_startTip(0), m_whatTip(0),
  m_questionTip(0), m_tryAgainTip(0), m_confirmTip(0),
  m_certifyTip(0),
  m_scale(1),
  m_correctAnim(0), m_flyEllipse(0),
  m_outTuneTip(0),
  m_scene(0),
  m_timerToConfirm(new QTimer(this))
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setMouseTracking(true);
  setInteractive(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(("background: transparent"));
  setRenderHint(QPainter::TextAntialiasing, true);
  m_scene = new QGraphicsScene(this);
  setScene(m_scene);
	m_newSize = parent->centralWidget()->size();
  sizeChanged();
  connect(parent, SIGNAL(sizeChanged(QSize)), this, SLOT(sizeChangedDelayed(QSize)));
  connect(m_timerToConfirm, SIGNAL(timeout()), this, SLOT(showConfirmTip()));
}

Tcanvas::~Tcanvas()
{}

//######################################################################
//##################################### TIPS ###########################
//######################################################################

int Tcanvas::bigFont() {
  return (fontMetrics().boundingRect("A").height() * 2);
}

QFont Tcanvas::tipFont(qreal factor) {
  QFont f = font();
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
// 	TscaledAnim *scaleTipAnim = new TscaledAnim(m_resultTip);
// 	m_resultTip->setScale(m_scale / 10.0);
  m_resultTip->setScale(m_scale);
  setResultPos();
// 	scaleTipAnim->startScaling(m_scale);
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
     TexamHelp::toStopExamTxt("<a href=\"stopExam\"> " + pixToHtml(gl->path + "picts/stopExam.png", PIXICONSIZE) + "</a>") + "</p>", palette().highlight().color());
   m_scene->addItem(m_startTip);
   m_startTip->setScale(m_scale);
   m_startTip->setTextInteractionFlags(Qt::TextBrowserInteraction);
   connect(m_startTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
   setStartTipPos();
}


void Tcanvas::certificateTip() {
	if (m_questionTip) {
    delete m_questionTip;
    m_questionTip = 0;
  }		
	if (!m_certifyTip) {
			m_certifyTip = new TnootkaCertificate(this, gl->path, m_exam);
			connect(m_certifyTip, SIGNAL(userAction(QString)), this, SLOT(linkActivatedSlot(QString)));
		//   m_finishTip->rotate(-7);
	}
}


void Tcanvas::whatNextTip(bool isCorrect, bool toCorrection) {
  if (m_questionTip) {
    delete m_questionTip;
    m_questionTip = 0;
  }
  if (m_whatTip)
		delete m_whatTip;
  QString whatNextText = startTipText();
  if (!isCorrect)
      whatNextText += "<br>" + tr("To correct an answer") + " " + 
      TexamHelp::clickSomeButtonTxt("<a href=\"prevQuest\">" + pixToHtml(gl->path + "picts/prevQuest.png", PIXICONSIZE) + "</a>") +
      " " + TexamHelp::orPressBackSpace();
	if (toCorrection)
		whatNextText += "<br>" + tr("To see corrected answer") + " " + 
			TexamHelp::clickSomeButtonTxt("<a href=\"correct\">" + pixToHtml(gl->path + "picts/correct.png", PIXICONSIZE) + "</a>") +
			TexamHelp::orPressEnterKey();
  whatNextText += "<br>" + TexamHelp::toStopExamTxt("<a href=\"stopExam\">" + pixToHtml(gl->path + "picts/stopExam.png", PIXICONSIZE) + "</a>");
  
  m_whatTip = new TgraphicsTextTip(whatNextText, palette().highlight().color());
	m_whatTip->setTextWidth(qMin(m_maxTipWidth, m_parent->score->width()));
  m_scene->addItem(m_whatTip);
  m_whatTip->setFont(tipFont(0.35));
  m_whatTip->setScale(m_scale);
  m_parent->guitar->setAttribute(Qt::WA_TransparentForMouseEvents, true); // to activate click on tip
  m_whatTip->setTextInteractionFlags(Qt::TextBrowserInteraction);
  connect(m_whatTip, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
  setWhatNextPos();
}


void Tcanvas::noteTip(int time) {

}


void Tcanvas::confirmTip(int time) {
  m_timerToConfirm->start(time + 1); // add 1 to show it immediately when time = 0
}


void Tcanvas::showConfirmTip() {
  m_timerToConfirm->stop();
	if (!m_confirmTip) {
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
  if (m_startTip) {
    delete m_startTip;
    m_startTip = 0;
  }
  if (m_whatTip) {
    delete m_whatTip;
    m_whatTip = 0;
  }
  if (m_outTuneTip) {
		delete m_outTuneTip;
		m_outTuneTip = 0;
  }
  if (m_questionTip)
    delete m_questionTip;
  m_questionTip = new TquestionTip(exam, m_scale);
	m_questionTip->setTextWidth(m_maxTipWidth);
  m_scene->addItem(m_questionTip);
	m_guitarFree = m_questionTip->freeGuitar();
	m_nameFree = m_questionTip->freeName();
	m_scoreFree = m_questionTip->freeScore();
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


void Tcanvas::correctToGuitar(TQAtype::Etype &question, int prevTime, TfingerPos& goodPos) {
	if (m_correctAnim)
		return;
	m_goodPos = goodPos;
	m_flyEllipse = new QGraphicsEllipseItem;
	m_flyEllipse->setPen(Qt::NoPen);
	m_flyEllipse->setBrush(QBrush(QColor(gl->EquestionColor.name())));
	m_scene->addItem(m_flyEllipse);
	if (question == TQAtype::e_asNote) {
			m_flyEllipse->setRect(m_parent->score->noteRect(1)); // 1 - answer note segment	
			m_flyEllipse->setPos(mapToScene(m_parent->score->notePos(1)));
	} else if (question == TQAtype::e_asName) {
			m_flyEllipse->setRect(QRectF(0, 0, m_parent->noteName->labelRect().height() * 2, m_parent->noteName->labelRect().height()));
			m_flyEllipse->setPos(mapToScene(m_parent->noteName->mapToParent(m_parent->noteName->labelRect().topLeft())));
	} else if (question == TQAtype::e_asSound) {
			m_flyEllipse->setRect(QRectF(0, 0, m_parent->pitchView->height() * 2, m_parent->pitchView->height()));
			m_flyEllipse->setPos(mapToScene((m_parent->pitchView->geometry().topLeft() + 
				QPoint(m_parent->pitchView->width() / 2 , 0))));
	}
	
	m_correctAnim = new TcombinedAnim(m_flyEllipse, this);
	m_correctAnim->setDuration(600);
	connect(m_correctAnim, SIGNAL(finished()), this, SLOT(correctAnimFinished()));
	m_correctAnim->setMoving(m_flyEllipse->pos(), 
					mapToScene(m_parent->guitar->mapToParent(m_parent->guitar->mapFromScene(m_parent->guitar->fretToPos(goodPos)))));
	m_correctAnim->moving()->setEasingCurveType(QEasingCurve::InOutBack);
	if (goodPos.fret() != 0) {
			m_correctAnim->setScaling(m_parent->guitar->fingerRect().width() / m_flyEllipse->rect().width(), 2.0);
			m_correctAnim->scaling()->setEasingCurveType(QEasingCurve::OutQuint);
	}
	m_correctAnim->setColoring(QColor(gl->EanswerColor.name()));
	if (goodPos.fret() == 0) {
		QPointF p1(mapToScene(m_parent->guitar->mapToParent(
							m_parent->guitar->mapFromScene(m_parent->guitar->stringLine(goodPos.str()).p1()))));
		QPointF p2(mapToScene(m_parent->guitar->mapToParent(
							m_parent->guitar->mapFromScene(m_parent->guitar->stringLine(goodPos.str()).p2()))));
		m_correctAnim->setMorphing(QLineF(p1, p2), m_parent->guitar->stringWidth(goodPos.str() - 1));
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
    m_parent->guitar->setAttribute(Qt::WA_TransparentForMouseEvents, false); // unlock guitar for mouse
    delete m_whatTip;
    m_whatTip = 0;
  }
  if (m_startTip) {
    delete m_startTip;
    m_startTip = 0;
  }
  if (m_questionTip) {
    delete m_questionTip;
    m_questionTip = 0;
  }
  if (m_certifyTip) {
    delete m_certifyTip;
    m_certifyTip = 0;
  }
  if (m_outTuneTip) {
		delete m_outTuneTip;
		m_outTuneTip = 0;
  }
}


void Tcanvas::clearResultTip() {
    if (m_resultTip) {
      delete m_resultTip;
      m_resultTip = 0;
    }
}

void Tcanvas::clearTryAgainTip() {
    if (m_tryAgainTip){
      delete m_tryAgainTip;
      m_tryAgainTip = 0;
    }
}

void Tcanvas::clearConfirmTip() {
    m_timerToConfirm->stop();
    if (m_confirmTip){
      delete m_confirmTip;
      m_confirmTip = 0;
    }
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



void Tcanvas::markAnswer(TQAtype::Etype qType, TQAtype::Etype aType) {
}
  
  
const QRect& Tcanvas::getRect(TQAtype::Etype kindOf) {
  switch (kindOf) {
    case TQAtype::e_asNote:
      return m_parent->score->geometry();
    case TQAtype::e_asName:
      return m_parent->noteName->geometry();
    case TQAtype::e_asFretPos:
          return m_parent->guitar->geometry();
    case TQAtype::e_asSound:
      return m_parent->pitchView->geometry();
  }
}


//######################################################################
//#################################### PROTECTED #######################
//######################################################################


void Tcanvas::sizeChangedDelayed(QSize newSize) {
	m_newSize = newSize;
	QTimer::singleShot(2, this, SLOT(sizeChanged()));
}


void Tcanvas::sizeChanged() {
  setGeometry(geometry().x(), geometry().y(), m_newSize.width(), m_newSize.height());
  int hi;
  if (m_scene->height())
    hi = m_scene->height();
  else
    hi = 580;
  m_scene->setSceneRect(geometry());
  m_scale =m_scale * ((double)m_newSize.height() / hi);
	m_maxTipWidth = width() / 3;
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
	m_parent->guitar->setFinger(m_goodPos);
	m_parent->guitar->markAnswer(QColor(gl->EanswerColor.name()));
}



bool Tcanvas::event(QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
    event->setAccepted(false);
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    me->setAccepted(false);
    if (event->type() == QEvent::MouseButtonPress)
        QGraphicsView::mousePressEvent(me);
    else
        QGraphicsView::mouseReleaseEvent(me);
  }     
  return QGraphicsView::event(event);
}


//##################################################################################################
//#################################### PRIVATE #####################################################
//##################################################################################################

int Tcanvas::getMaxTipHeight() {
	if (m_nameFree)
			return m_parent->noteName->height() * 1.1;
	else if (m_scoreFree)
			return m_parent->score->height() / 2;
	else {
		if (m_parent->pitchView->isVisible())
				return m_parent->guitar->height();
		else
				return m_parent->guitar->height() + (m_parent->guitar->geometry().y() - m_parent->noteName->geometry().bottom()) / 2;
	}
}


void Tcanvas::setPosOfTip(TgraphicsTextTip* tip) {
	QRect geoRect;
	if (m_nameFree)  // middle of the noteName
			geoRect = m_parent->noteName->geometry();
	else if (m_scoreFree) {// on the score at its center
			geoRect = m_parent->score->geometry();
			if (tip->boundingRect().width() * tip->scale() > m_parent->score->width())
				tip->setScale(((qreal)m_parent->score->width() / (tip->boundingRect().width())));
	} else { // middle of the guitar
			geoRect = m_parent->guitar->geometry();
			if (!m_parent->pitchView->isVisible()) // tip can be bigger
				geoRect = QRect(m_parent->noteName->geometry().x() - 20, 
							m_parent->guitar->geometry().y() - (m_parent->guitar->geometry().top() - m_parent->noteName->geometry().bottom()) / 2,
							m_parent->guitar->width() - m_parent->noteName->width() + 20,
							m_parent->guitar->height() + (m_parent->guitar->geometry().top() - m_parent->noteName->geometry().bottom()) / 2);
		}
	tip->setPos(geoRect.x() + (geoRect.width() - tip->boundingRect().width() * tip->scale()) / 2,
		geoRect.y() + (geoRect.height() - tip->boundingRect().height() * tip->scale()) / 2 );
}


void Tcanvas::setResultPos() {
  m_resultTip->setPos(m_parent->relatedPoint().x() + (((m_scene->width() - m_parent->relatedPoint().x()) -
                                                       m_scale * m_resultTip->boundingRect().width())) / 2,
                      m_parent->relatedPoint().y());
}


void Tcanvas::setTryAgainPos() {
  m_tryAgainTip->setPos(m_parent->relatedPoint().x() + (((m_scene->width() - m_parent->relatedPoint().x()) -
											m_scale * m_tryAgainTip->boundingRect().width())) / 2,
                      m_parent->noteName->geometry().top() - m_scale * m_tryAgainTip->boundingRect().height());
}


void Tcanvas::setWhatNextPos() {
	int maxTipHeight = getMaxTipHeight();
  if (m_whatTip->boundingRect().height() * m_whatTip->scale() != maxTipHeight)
				m_whatTip->setScale((qreal)maxTipHeight / (m_whatTip->boundingRect().height() * m_whatTip->scale()));
	setPosOfTip(m_whatTip);
}


void Tcanvas::setStartTipPos() {
// in the middle of a window
  m_startTip->setPos((m_scene->width() - m_scale * (m_startTip->boundingRect().width())) / 2,
                  (m_scene->height() - m_scale * (m_startTip->boundingRect().height())) / 2 );  
}


void Tcanvas::setConfirmPos() { // middle of noteName and somewhere above
    m_confirmTip->setPos(m_parent->noteName->geometry().x() + (m_parent->noteName->width() - m_confirmTip->boundingRect().width()) / 2,
												 m_parent->relatedPoint().y());  
}


void Tcanvas::setQuestionPos() {
	int maxTipHeight = getMaxTipHeight();
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
}


void Tcanvas::setOutTunePos() {
	int startX = m_parent->pitchView->geometry().x();
	if (m_outTuneTip->boundingRect().width() * m_outTuneTip->scale() > m_parent->pitchView->geometry().width() / 2)
			m_outTuneTip->setScale((m_outTuneTip->boundingRect().width() * m_outTuneTip->scale()) / 
							(m_parent->pitchView->geometry().width() / 2));
	if (!m_outTuneTip->data(0).toBool())
		startX += m_parent->pitchView->geometry().width() / 2;
	m_outTuneTip->setPos(startX + (m_parent->pitchView->geometry().width() / 2 - m_outTuneTip->boundingRect().width()) / 2, 
		m_parent->pitchView->y() - m_outTuneTip->boundingRect().height() * m_outTuneTip->scale());
}








