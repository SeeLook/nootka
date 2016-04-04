/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tmelodyitem.h"
#include "tqtaudioin.h"
#include "tqtaudioout.h"
#include <tmtr.h>
#include <tpath.h>
#include <graphics/tnotepixmap.h>
#include <graphics/tdropshadoweffect.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtGui/qpen.h>
#include <QtGui/qpainter.h>
#include <QtCore/qtimer.h>
#include <QtCore/qmath.h>

#include <QtCore/qdebug.h>


#define SHORT_TAP_TIME (300)
#define BG_ALPHA (200) // alpha of items background color


/**
 * NOTICE!!!!!!!!!!!
 * It works only with tqtaudioin - means only under Android
 */


/**
 * Note-head shaped item with icon inside.
 */
class TflyItem : public QGraphicsItem
{

public:
  TflyItem(const QIcon& icon, QGraphicsItem* parent = 0) :
      QGraphicsItem(parent),
      m_pixmap(icon.pixmap(Tmtr::fingerPixels() * 0.85))
  {
    setBgColor(Qt::black);
    setGraphicsEffect(new TdropShadowEffect);
  }

  enum { FlyItemType = UserType + 2 };
  virtual int type() const { return FlyItemType; }

  void setBgColor(const QColor& c) { m_bgColor = QColor(c.red(), c.green(), c.blue(), BG_ALPHA); update(); }

  virtual QRectF boundingRect() const {
    return QRectF(0, 0, Tmtr::fingerPixels() * 1.4, Tmtr::fingerPixels() * 1.4);
  }

  virtual QPainterPath shape() const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
  }

protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QBrush(m_bgColor));
    painter->drawEllipse(0, 0, boundingRect().width(), boundingRect().height());
    painter->drawPixmap((boundingRect().width() - m_pixmap.width()) / 2,
                        (boundingRect().height() - m_pixmap.height()) / 2, m_pixmap);
  }

private:
  QPixmap             m_pixmap;
  QColor              m_bgColor;
};


//#################################################################################################
//###################             TmelodyItem          ############################################
//#################################################################################################

TmelodyItem* TmelodyItem::m_instance = 0;

TmelodyItem::TmelodyItem() :
  QGraphicsObject(0),
  m_touched(false),
  m_selectedAction(0)
{
  m_instance = this;
//  setAcceptTouchEvents(true);
  m_playDot = createDot(1);
  m_recDot = createDot(2);
  m_snifDot = createDot(3);
  m_timer = new QTimer(this);
  m_timer->setSingleShot(true);
  m_timer->setTimerType(Qt::PreciseTimer);
  m_timer->setInterval(SHORT_TAP_TIME);
  connect(m_timer, &QTimer::timeout, this, &TmelodyItem::createFlyActions);
}


TmelodyItem::~TmelodyItem()
{
  m_instance = 0;
}


bool TmelodyItem::audioInEnabled() {
  return (bool)TaudioIN::instance();
}


bool TmelodyItem::audioOutEnabled() {
  return (bool)TaudioOUT::instance();
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
  QColor bg(m_touched ? qApp->palette().highlight().color() : Qt::white);
  bg.setAlpha(BG_ALPHA);
  painter->setRenderHints(QPainter::Antialiasing);
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


void TmelodyItem::createFlyActions() {
  m_timer->stop();
  if (m_flyList.isEmpty()) {
    m_selectedAction = 0;
    qreal angle = qDegreesToRadians(86.0) / (m_actions.size() - 1);
    qreal off = qDegreesToRadians(88.0);
    qreal r = Tmtr::fingerPixels() * 4;
    for (int i = 0; i < m_actions.size(); ++i) {
      if (m_actions[i]->isEnabled()) {
        auto it = new TflyItem(m_actions[i]->icon(), this);
        it->setData(0, i);
        m_flyList << it;
        if (m_actions.size() > 1)
          it->setPos(qSin(off - i * angle) * r, qCos(off - i * angle) * r);
        else
          it->setPos(10, r); // when there is only one item
      }
    }
  }
}

//#################################################################################################
//###################              EVENTS              ############################################
//#################################################################################################

void TmelodyItem::mousePressEvent(QGraphicsSceneMouseEvent*) {
  m_currentIt = 0;
  m_touched = true;
  update();
  emit touched();
  m_timer->start();
}


void TmelodyItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  if (m_touched && event->pos().x() > Tmtr::fingerPixels() || event->pos().y() > Tmtr::fingerPixels()) {
    // condition is correct as long as melody item is at (0, 0) position
    if (m_timer->isActive()) {
        createFlyActions(); // finger was moved over the melody item buy flying items have been not created yet.
    } else {
        auto it = scene()->itemAt(mapToScene(event->pos()), transform());
        if (it->type() == TflyItem::FlyItemType) {
          auto flyIt = qgraphicsitem_cast<TflyItem*>(it);
          if (flyIt != m_currentIt) {
            m_selectedAction = m_actions.at(flyIt->data(0).toInt());
            flyIt->setBgColor(qApp->palette().highlight().color());
            if (m_currentIt) // revert selection of previous fly item
                m_currentIt->setBgColor(Qt::black);
            m_currentIt = flyIt;
          }
        }
    }
  }
  QGraphicsItem::mouseMoveEvent(event);
}


void TmelodyItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  m_timer->stop();
  for (int i = 0 ; i < m_flyList.size(); ++i)
    delete m_flyList[i];
  m_flyList.clear();
  m_touched = false;
  update();
  if (m_selectedAction) {
      m_selectedAction->trigger();
      m_selectedAction = 0;
  } else if (event->pos().x() < Tmtr::fingerPixels() && event->pos().y() < Tmtr::fingerPixels()) {
    // send a signal only when finger was not moved over the melody tip
      QTimer::singleShot(5, [=]{ emit menuSignal(); }); // delay fixes garbing touch by scene when menu is ignored
  }
  QGraphicsItem::mouseReleaseEvent(event);
}






