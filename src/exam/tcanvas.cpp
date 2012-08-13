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
#include <QDebug>



extern Tglobals *gl;

Tcanvas::Tcanvas(MainWindow* parent) :
  QGraphicsView(parent),
  m_parent(parent),
  m_scale(1)
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(("background: transparent"));
  setRenderHint(QPainter::TextAntialiasing, true);
    
  setGeometry(parent->centralWidget()->geometry());
  m_scene = new QGraphicsScene();
  setScene(m_scene);
  m_scene->setSceneRect(geometry());
  sizeChanged(parent->centralWidget()->size());
  
  connect(parent, SIGNAL(sizeChanged(QSize)), this, SLOT(sizeChanged(QSize)));
}

Tcanvas::~Tcanvas()
{}

int Tcanvas::bigFont() {
  return fontMetrics().boundingRect("A").height() * 2;
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
    
  TgraphicsTextTip *m_resultTip = new TgraphicsTextTip(wasAnswerOKtext(answer, answColor, bigFont()), answColor);
  m_scene->addItem(m_resultTip);
  m_resultTip->setPos(mapToScene((m_parent->centralWidget()->width() - m_resultTip->boundingRect().width()) / 2,
                  qRound((double)m_parent->centralWidget()->height() * 0.5)));  
}


void Tcanvas::addTip(TgraphicsTextTip* tip)
{
  m_scene->addItem(tip);  
}

void Tcanvas::clearCanvas() {
  m_scene->clear();
}


/****************************************************************************
** PROTECTED
****************************************************************************/

void Tcanvas::sizeChanged(QSize newSize) {
  m_scale = (double)newSize.height() / 580.0 / m_scale;
  scale(m_scale, m_scale);
  qDebug() << transform();
  for (int i = 0; i < m_scene->items().size(); i++)
    m_scene->items().operator[](i)->setPos(m_scene->items().operator[](i)->pos().x() * m_scale, 
                m_scene->items().operator[](i)->pos().y() * m_scale);

}



void Tcanvas::clearResultTip() {

}



