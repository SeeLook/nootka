/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tmelodyitem.h"
#include <tmtr.h>
#include <tpath.h>
#include <touch/ttouchmenu.h>
#include <QAction>
#include <QPen>
#include <QPainter>


TmelodyItem* TmelodyItem::m_instance = 0;


TmelodyItem::TmelodyItem(QAction* playAction, QAction* recordAction, QAction* listenAction) :
  QGraphicsObject(0),
  m_playAct(playAction),
  m_recAct(recordAction),
  m_sniffAct(listenAction),
  m_touched(false)
{
  m_instance = this;
  setAcceptTouchEvents(true);
  m_playDot = createDot(1);
  m_recDot = createDot(2);
  m_snifDot = createDot(3);

  m_scoreMenuAct = new QAction(QIcon(Tpath::img("score")), tr("score menu"), this);
  connect(m_scoreMenuAct, SIGNAL(triggered()), this, SIGNAL(scoreMenuSignal()));
  m_mainMenuAct = new QAction(QIcon(Tpath::img("nootka")), tr("main menu"), this);
  connect(m_mainMenuAct, SIGNAL(triggered()), this, SIGNAL(mainMenuSignal()));
}


TmelodyItem::~TmelodyItem()
{
  m_instance = 0;
}



void TmelodyItem::setPlaying(bool isPlay) {
  if (isPlay)
    setDotColor(m_playDot, Qt::green);
  else
    setDotColor(m_playDot, qApp->palette().text().color());
}


void TmelodyItem::setRecording(bool isRecord) {
  if (isRecord)
    setDotColor(m_recDot, Qt::red);
  else
    setDotColor(m_recDot, qApp->palette().text().color());
}


void TmelodyItem::setListening(bool isListen) {
  if (isListen)
    setDotColor(m_snifDot, qApp->palette().highlight().color());
  else
    setDotColor(m_snifDot, qApp->palette().text().color());
}


QRectF TmelodyItem::boundingRect() const {
  return QRectF(0, 0, Tmtr::fingerPixels(), Tmtr::fingerPixels());
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodyItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  QColor bg(m_touched ? qApp->palette().highlight().color() : qApp->palette().base().color());
  bg.setAlpha(200);
  painter->setBrush(bg);
  painter->setPen(Qt::NoPen);
  painter->drawRoundedRect(boundingRect().adjusted(0, 0, -Tmtr::fingerPixels() / 2, 0), 30, 30, Qt::RelativeSize);
}


QGraphicsEllipseItem* TmelodyItem::createDot(int dotNr) {
  QGraphicsEllipseItem *dot = new QGraphicsEllipseItem(0, 0, boundingRect().width() / 6, boundingRect().width() / 6, this);
  setDotColor(dot, qApp->palette().text().color());
  dot->setPos((boundingRect().width() / 2 - (dot->boundingRect().width())) / 2,
              dotNr * ((boundingRect().height() - 3 * dot->boundingRect().height()) / 4) + (dotNr - 1) * dot->boundingRect().height());
  return dot;
}


void TmelodyItem::setDotColor(QGraphicsEllipseItem* dot, const QColor& c) {
  QColor dotColor = c;
  dotColor.setAlpha(225);
  dot->setPen(QPen(dotColor, 1.0));
  dotColor.setAlpha(150);
  dot->setBrush(QBrush(dotColor));
}


void TmelodyItem::mousePressEvent(QGraphicsSceneMouseEvent*) {
  m_touched = true;
  update();
}


void TmelodyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent*) {
  TtouchMenu menu;
  menu.setAnimDuration(200);
  menu.addAction(m_playAct);
  menu.addAction(m_recAct);
  menu.addAction(m_sniffAct);
  menu.addAction(m_scoreMenuAct);
  menu.addAction(m_mainMenuAct);
  int xx = x() + Tmtr::fingerPixels() * 0.7;
  m_touched = false;
  menu.exec(QPoint(xx, 2), QPoint(xx, -menu.sizeHint().height()));
}


