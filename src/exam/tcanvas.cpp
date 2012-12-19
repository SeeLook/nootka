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
#include "tpixmaker.h"
#include "tscorewidget.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include "tpitchview.h"
#include "tquestionaswdg.h"
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#define PIXICONSIZE (32)

extern Tglobals *gl;

Tcanvas::Tcanvas(MainWindow* parent) :
  QGraphicsView(parent),
  m_parent(parent),
  m_resultTip(0), m_startTip(0), m_whatTip(0),
  m_questionTip(0), m_tryAgainTip(0),
  m_scale(1),
  m_flyAnswer(0), m_animation(0),
  m_flyNote(0),
  m_qaPossib(1)
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(("background: transparent"));
  setRenderHint(QPainter::TextAntialiasing, true);
//  setMouseTracking(true);
  m_scene = new QGraphicsScene();
  setScene(m_scene);
  sizeChanged(parent->centralWidget()->size());
//   m_animation = new QParallelAnimationGroup(this);
//   m_flyAnswer = new TanimedTextItem();
//   m_flyAnswer->setFont(QFont("nootka", width() / 18));
//   m_flyAnswer->setBrush(QColor(gl->EanswerColor.name()));
//   scene()->addItem(m_flyAnswer);
//   m_flyAnswer->setZValue(100);
//   m_flyAnswer->hide();
//   QPropertyAnimation *movPos = new QPropertyAnimation(m_flyAnswer, "pos");
//     movPos->setDuration(600);
// //     movPos->setEasingCurve(QEasingCurve::InOutExpo);
//     QPropertyAnimation *movScale = new QPropertyAnimation(m_flyAnswer, "scale");
//     movScale->setDuration(600);
//     movScale->setStartValue(4.0);
//     movScale->setEndValue(0.3);
// //     movScale->setEasingCurve(QEasingCurve::InCirc);
//     QPropertyAnimation *movAlpha = new QPropertyAnimation(m_flyAnswer, "alpha");
//     movAlpha->setDuration(600);
//     movAlpha->setStartValue(255);
//     movAlpha->setEndValue(0);
//     movAlpha->setEasingCurve(QEasingCurve::InExpo);
//     m_animation->addAnimation(movPos);
//     m_animation->addAnimation(movScale);
//     m_animation->addAnimation(movAlpha);
//   
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
  return TexamHelp::toGetQuestTxt() + ":<br>" + TexamHelp::clickSomeButtonTxt(pixToHtml(gl->path + "picts/nextQuest.png", PIXICONSIZE)) + 
  ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt();
}


void Tcanvas::startTip() {
  m_startTip = new TgraphicsTextTip(QString("<p style=\"font-size: %1px;\">").arg(qRound((qreal)bigFont() * 0.75)) + startTipText() + ".<br>" + TexamHelp::toStopExamTxt(pixToHtml(gl->path + "picts/stopExam.png", PIXICONSIZE)) + "</p>", palette().highlight().color());
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
      whatNextText += "<br>" + tr("To correct an answer") + " " + TexamHelp::clickSomeButtonTxt(pixToHtml(gl->path + "picts/prevQuest.png", PIXICONSIZE)) +
      " " + TexamHelp::orPressBkSTxt();
  whatNextText += "<br>" + TexamHelp::toStopExamTxt(pixToHtml(gl->path + "picts/stopExam.png", PIXICONSIZE));
  
  m_whatTip = new TgraphicsTextTip(whatNextText, palette().highlight().color());
  m_scene->addItem(m_whatTip);
  m_whatTip->setFont(tipFont(0.35));
  m_whatTip->setScale(m_scale);
  setPosOfWhatTip();
}


void Tcanvas::noteTip(int time) {
  if (m_flyNote)
      m_flyNote->deleteLater();
  QParallelAnimationGroup *animation = new QParallelAnimationGroup(this);
  m_flyNote = new TanimedTextItem();
  m_flyNote->setText("N");
  m_flyNote->setFont(QFont("nootka", width() / 18));
  m_flyNote->setBrush(palette().highlight().color());
  scene()->addItem(m_flyNote);
  QPropertyAnimation *movPos = new QPropertyAnimation(m_flyNote, "pos");
    movPos->setDuration(time);
    movPos->setStartValue(QPoint(width() + 10, qrand() % height()));
    movPos->setEndValue(getRect(TQAtype::e_asSound).center());
    movPos->setEasingCurve(QEasingCurve::OutCirc);
    QPropertyAnimation *movScale = new QPropertyAnimation(m_flyNote, "scale");
    movScale->setDuration(time);
    movScale->setStartValue(5.0);
    movScale->setEndValue(0.01);
    movScale->setEasingCurve(QEasingCurve::InCirc);
//     QPropertyAnimation *movAlpha = new QPropertyAnimation(m_flyAnswer, "alpha");
//     movAlpha->setDuration(time);
//     movAlpha->setStartValue(255);
//     movAlpha->setEndValue(0);
//     movAlpha->setEasingCurve(QEasingCurve::InExpo);
    animation->addAnimation(movPos);
    animation->addAnimation(movScale);
//     animation->addAnimation(movAlpha);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
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
//   if (m_qaPossib > 1)
//       markAnswer(exam->curQ().questionAs, exam->curQ().answerAs);
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


void Tcanvas::markAnswer(TQAtype::Etype qType, TQAtype::Etype aType) {
//     m_flyAnswer->setText(TquestionAsWdg::qaTypeSymbol(qType));
//     if (qType == TQAtype::e_asFretPos) {
//       if (gl->GisRightHanded)
//         m_flyAnswer->setRotation(-90);
//       else 
//         m_flyAnswer->setRotation(90);
//     } else
//       m_flyAnswer->setRotation(0);
//     m_flyAnswer->show();
//     QPoint qCenter, anCenter;
//     int adj = - m_flyAnswer->boundingRect().height();
//     if (qType == TQAtype::e_asFretPos)
//       qCenter = QPoint(width() / 2, height() * 0.85);
//     else {
//       qCenter = getRect(qType).center();
//       adj = m_flyAnswer->boundingRect().height() * 2;
//     }
//     anCenter = getRect(aType).center();
//     QPropertyAnimation* anim = static_cast<QPropertyAnimation*>(m_animation->animationAt(0));
//     anim->setStartValue(QPoint(qCenter.x() - (m_flyAnswer->boundingRect().width() * 2),
//                                 qCenter.y() - adj));
//     anim->setEndValue(QPoint(anCenter.x() - (m_flyAnswer->boundingRect().width() * 0.15),
//                                 anCenter.y() - (m_flyAnswer->boundingRect().height() * 0.15)));
//     m_animation->start();
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
//   if (m_flyAnswer) {
//     m_flyAnswer->hide();
//     m_flyAnswer->setFont(QFont("nootka", width() / 18));
//   }
  if (m_flyNote)
    m_flyNote->hide();
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
  if (m_whatTip->boundingRect().height() != (m_scene->height() * 0.26))
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
                   m_scene->height() * 0.75 + (m_scene->height() * 0.25 - m_questionTip->scale() * m_questionTip->boundingRect().height()) / 2);
  }
    else
      if (m_questionTip->freeName())
          pos = QPoint(m_parent->noteName->geometry().x() + (m_parent->noteName->width() - m_questionTip->boundingRect().width()) / 2,
                       m_parent->noteName->geometry().y() + (m_parent->noteName->height() - m_questionTip->scale() * m_questionTip->boundingRect().height()) - 5);
      else // on the score
        pos = QPoint(((double)m_scene->width() * 0.42 - m_questionTip->boundingRect().width()) / 2 ,
                     m_scene->height() / 10 + (m_scene->height() / 2 - m_questionTip->boundingRect().height()) /2 );
      
  m_questionTip->setPos(pos);
}



