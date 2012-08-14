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
#include "tgraphicstexttip.h"
#include "mainwindow.h"
#include "tnotepixmap.h"
#include "tglobals.h"
#include <texamhelp.h>
#include <QDebug>



extern Tglobals *gl;

Tcanvas::Tcanvas(MainWindow* parent) :
  QGraphicsView(parent),
  m_parent(parent),
  m_resultTip(0),
  m_whatTip(0),
  m_scale(1)
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(("background: transparent"));
  setRenderHint(QPainter::TextAntialiasing, true);
    
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


void Tcanvas::resultTip(TQAunit* answer, int time) {
  QColor answColor;
  if (answer->isCorrect())
      answColor = gl->EanswerColor;
  else
    if (answer->isWrong())
        answColor = gl->EquestionColor;
    else
        answColor = QColor(124, 0 ,124, 30);
    
  m_resultTip = new TgraphicsTextTip(wasAnswerOKtext(answer, answColor, bigFont())/*, answColor*/);
  m_scene->addItem(m_resultTip);
  m_resultTip->setScale(m_scale);
  setPosOfResultTip();
  if (time)
    QTimer::singleShot(time, this, SLOT(clearResultTip()));
}


void Tcanvas::whatNextTip(bool isCorrect, bool onRight) {
  QString whatNextText = TexamHelp::toGetQuestTxt() + ":<br>" + TexamHelp::clickSomeButtonTxt(gl->path+"picts/next-icon.png") + 
  ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt();
  if (!isCorrect)
      whatNextText += "<br>" + tr("To correct an answer") + " " + TexamHelp::clickSomeButtonTxt(gl->path+"picts/prev-icon.png") +
      " " + TexamHelp::orPressBkSTxt();
  
  m_whatTip = new TgraphicsTextTip(whatNextText, palette().window().color());
  m_scene->addItem(m_whatTip);
  m_whatTip->setScale(m_scale /** ((m_scene->height() / 3) / m_whatTip->boundingRect().height())*/);
  setPosOfWhatTip();
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
}


//######################################################################
//#################################### PROTECTED #######################
//######################################################################

void Tcanvas::sizeChanged(QSize newSize) {
  setGeometry(geometry().x(), geometry().y(), newSize.width(), newSize.height());
  m_scene->setSceneRect(geometry());
  m_scale = ((double)newSize.height() / 580.0) / m_scale;
  if (m_resultTip) {
      m_resultTip->setScale(m_scale);;
      setPosOfResultTip();
  }
  if (m_whatTip) {
      m_whatTip->setScale(m_scale);
      setPosOfWhatTip();
  }

}

//######################################################################
//#################################### PRIVATE #########################
//######################################################################

void Tcanvas::setPosOfResultTip() {
  m_resultTip->setPos((m_scene->width() - m_scale * m_resultTip->boundingRect().width()) / 2,
                  qRound((double)m_scene->height() * 0.7 ) -(m_scale * m_resultTip->boundingRect().height()));  
}

void Tcanvas::setPosOfWhatTip() {
//   qDebug() << m_whatTip->mapToScene(m_whatTip->boundingRect().width(), m_whatTip->boundingRect().height()) << m_whatTip->boundingRect().size();
//   QPointF ss = m_whatTip->mapToScene(m_whatTip->boundingRect().width(), m_whatTip->boundingRect().width());
  m_whatTip->setPos((m_scene->width() - m_scale * (m_whatTip->boundingRect().width())) / 2,
                  m_scene->height() - (m_scale * (m_whatTip->boundingRect().height() + 3)));
//   m_whatTip->setPos((m_scene->width() - (ss.x())) / 2,
//                   m_scene->height() - ((ss.y() + 3)));  
}



void Tcanvas::clearResultTip() {
    if (m_resultTip) {
      delete m_resultTip;
      m_resultTip = 0;
    }
}



