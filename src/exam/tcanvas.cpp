/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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
#include "tanimedtextitem.h"
#include "tgraphicstexttip.h"
#include "mainwindow.h"
#include "tnotepixmap.h"
#include "tglobals.h"
#include "texamhelp.h"
#include "tscorewidget.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include "tpitchview.h"
#include <QDebug>
#include <QTimer>
#include <qpropertyanimation.h>



extern Tglobals *gl;

Tcanvas::Tcanvas(MainWindow* parent) :
  QGraphicsView(parent),
  m_parent(parent),
  m_resultTip(0), m_startTip(0), m_whatTip(0),
  m_questionTip(0), m_tryAgainTip(0),
  m_scale(1),
  m_flyQuestion(0), m_flyAnswer(0)
{
//   setAttribute(Qt::WA_TransparentForMouseEvents);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(("background: transparent"));
  setRenderHint(QPainter::TextAntialiasing, true);
  setMouseTracking(true);
    
  m_scene = new QGraphicsScene();
  setScene(m_scene);
  sizeChanged(parent->centralWidget()->size());
  
  connect(parent, SIGNAL(sizeChanged(QSize)), this, SLOT(sizeChanged(QSize)));
}

Tcanvas::~Tcanvas()
{}

//######################################################################
//#################################### PUBLIC #########################
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
  if (m_resultTip)
    delete m_resultTip;    
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
  setPosOfResultTip();
  if (time)
    QTimer::singleShot(time, this, SLOT(clearResultTip()));
}


void Tcanvas::tryAgainTip(int time) {
  m_tryAgainTip = new TgraphicsTextTip(QString("<span style=\"color: %1; font-size: %2px;\">")
      .arg(gl->EquestionColor.name()).arg(bigFont()) + tr("Try again!") + "</span>");
  m_scene->addItem(m_tryAgainTip);
  m_tryAgainTip->setZValue(100);
  m_tryAgainTip->setScale(m_scale);
  setPosOfTryAgainTip();
  QTimer::singleShot(time, this, SLOT(clearTryAgainTip()));
}


QString Tcanvas::startTipText() {
  return TexamHelp::toGetQuestTxt() + ":<br>" + TexamHelp::clickSomeButtonTxt(gl->path+"picts/next-icon.png") + 
  ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt();
}


void Tcanvas::startTip() {
  m_startTip = new TgraphicsTextTip(QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75)) + startTipText() + "</p>", palette().highlight().color());
  m_scene->addItem(m_startTip);
  m_startTip->setScale(m_scale);
  setPosOfStartTip();
}


void Tcanvas::whatNextTip(bool isCorrect, bool onRight) {
  if (m_questionTip) {
    delete m_questionTip;
    m_questionTip = 0;
  }
  QString whatNextText = startTipText();
  if (!isCorrect)
      whatNextText += "<br>" + tr("To correct an answer") + " " + TexamHelp::clickSomeButtonTxt(gl->path+"picts/prev-icon.png") +
      " " + TexamHelp::orPressBkSTxt();
  
  m_whatTip = new TgraphicsTextTip(whatNextText, palette().highlight().color());
  m_scene->addItem(m_whatTip);
  m_whatTip->setFont(tipFont(0.35));
  m_whatTip->setScale(m_scale);
  setPosOfWhatTip();
}

void Tcanvas::noteTip(int time) {

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
  if (m_questionTip)
    delete m_questionTip;
  m_questionTip = new TquestionTip(exam, m_scale);
  m_scene->addItem(m_questionTip);
  setPosOfQuestionTip();
  markQuestion(exam->curQ().questionAs, exam->curQ().answerAs);
}


void Tcanvas::addTip(TgraphicsTextTip* tip) {
  m_scene->addItem(tip);  
}

void Tcanvas::clearCanvas() {
  clearResultTip();
  if (m_whatTip) {
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


TQAtype::Etype m_aType;
void Tcanvas::markQuestion(TQAtype::Etype qType, TQAtype::Etype aType) {
  m_flyQuestion = new TanimedTextItem();
  m_flyQuestion->setText("?");
  m_flyQuestion->setFont(QFont("nootka", width() / 18));
  m_flyQuestion->setBrush(QColor(gl->EquestionColor.name()));
  scene()->addItem(m_flyQuestion);
  QPropertyAnimation *movPos = new QPropertyAnimation(m_flyQuestion, "pos");
  movPos->setDuration(500);
  movPos->setStartValue(getRect(qType).center());
  movPos->setEndValue(geometry().center());
  movPos->setEasingCurve(QEasingCurve::InCirc);
  QPropertyAnimation *movScale = new QPropertyAnimation(m_flyQuestion, "scale");
  movScale->setDuration(500);
  movScale->setStartValue(3);
  movScale->setEndValue(1.5);
  movScale->setEasingCurve(QEasingCurve::InBack);
  movPos->start();
  movScale->start();
  m_aType = aType;
  QTimer::singleShot(550, this, SLOT(delayedAnswer()));
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


void Tcanvas::delayedAnswer() { // [slot]
//     if (m_flyQuestion)
//         delete m_flyQuestion;
    m_flyAnswer = new TanimedTextItem();
    m_flyAnswer->setText("!");
    m_flyAnswer->setFont(QFont("nootka", width() / 18));
    m_flyAnswer->setBrush(QColor(gl->EanswerColor.name()));
    scene()->addItem(m_flyAnswer);
    QPropertyAnimation *movPos = new QPropertyAnimation(m_flyAnswer, "pos");
    movPos->setDuration(500);
    movPos->setStartValue(geometry().center());
    movPos->setEndValue(getRect(m_aType).center());
    movPos->setEasingCurve(QEasingCurve::OutCirc);
    QPropertyAnimation *movScale = new QPropertyAnimation(m_flyAnswer, "scale");
    movScale->setDuration(500);
    movScale->setStartValue(1.5);
    movScale->setEndValue(3);
    movScale->setEasingCurve(QEasingCurve::OutBack);
    movPos->start();
    movScale->start();
}



void Tcanvas::clearNoteTip() {

}


//######################################################################
//#################################### PROTECTED #######################
//######################################################################

void Tcanvas::sizeChanged(QSize newSize) {
  setGeometry(geometry().x(), geometry().y(), newSize.width(), newSize.height());
  int hi;
  if (m_scene->height())
    hi = m_scene->height();
  else
    hi = 580;
  m_scene->setSceneRect(geometry());
  m_scale =m_scale * ((double)newSize.height() / hi);
  if (m_resultTip) {
      m_resultTip->setScale(m_scale);;
      setPosOfResultTip();
  }
  if (m_tryAgainTip) {
    m_tryAgainTip->setScale(m_scale);
    setPosOfTryAgainTip();
  }
  if (m_whatTip) {
      m_whatTip->setScale(m_scale);
      setPosOfWhatTip();
  }
  if (m_startTip) {
    m_startTip->setScale(m_scale);
    setPosOfStartTip();
  }
  if (m_questionTip) {
    delete m_questionTip;
    m_questionTip = new TquestionTip(m_exam, m_scale);
    m_scene->addItem(m_questionTip);
    setPosOfQuestionTip();
  }
  if (m_flyQuestion)
    delete m_flyQuestion;
  if  (m_flyAnswer)
    delete m_flyAnswer;

}

//######################################################################
//#################################### PRIVATE #########################
//######################################################################

void Tcanvas::setPosOfResultTip() {
  m_resultTip->setPos(m_parent->relatedPoint().x() + (((m_scene->width() - m_parent->relatedPoint().x()) -
                                                       m_scale * m_resultTip->boundingRect().width())) / 2,
                      m_parent->relatedPoint().y() - 5);
}


void Tcanvas::setPosOfTryAgainTip() {
  m_tryAgainTip->setPos(m_parent->relatedPoint().x() + (((m_scene->width() - m_parent->relatedPoint().x()) -
                                                       m_scale * m_tryAgainTip->boundingRect().width())) / 2,
                      m_parent->relatedPoint().y() - m_scale * m_tryAgainTip->boundingRect().height());
}



void Tcanvas::setPosOfWhatTip() {
    // in the middle on guitar
  if (m_whatTip->boundingRect().height() < (m_scene->height() * 0.26))
      m_whatTip->setScale((m_scene->height() * 0.24) / m_whatTip->boundingRect().height());
  m_whatTip->setPos((m_scene->width() - (m_whatTip->scale() * m_whatTip->boundingRect().width())) / 2,
                  m_scene->height() * 0.74);
}


void Tcanvas::setPosOfStartTip() {
    // in the middle
  m_startTip->setPos((m_scene->width() - m_scale * (m_startTip->boundingRect().width())) / 2,
                  (m_scene->height() - m_scale * (m_startTip->boundingRect().height())) / 2 );  
}


void Tcanvas::setPosOfQuestionTip() {
  if (m_questionTip->boundingRect().height() > (m_scene->height() * 0.24))
    m_questionTip->setScale((m_scene->height() * 0.24) / m_questionTip->boundingRect().height());
  QPoint pos;
  if (m_questionTip->freeGuitar()) {
      int off = 0;
      if (m_exam->curQ().answerAs == TQAtype::e_asSound)
        off = m_scene->width() / 8;
      pos = QPoint((m_scene->width() - (m_questionTip->boundingRect().width())) / 2 + off, 
                   m_scene->height() * 0.76 + (m_scene->height() * 0.24 - m_questionTip->boundingRect().height()) / 2);
  }
    else
      if (m_questionTip->freeName())
        pos = QPoint((m_scene->width() / 2), (m_scene->height() * 0.73) - m_questionTip->boundingRect().height());
      else // on the score
        pos = QPoint(((double)m_scene->width() * 0.42 - m_questionTip->boundingRect().width()) / 2 ,
                     m_scene->height() / 10 + (m_scene->height() / 2 - m_questionTip->boundingRect().height()) /2 );
      
  m_questionTip->setPos(pos);
}



