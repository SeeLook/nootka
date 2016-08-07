/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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
#include <qtr.h>
#include <graphics/tnotepixmap.h>
#include <graphics/tdropshadoweffect.h>
#include <graphics/tgraphicstexttip.h>
#include <tfingerpointer.h>
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
  virtual int type() const override { return FlyItemType; }

  void setBgColor(const QColor& c) { m_bgColor = QColor(c.red(), c.green(), c.blue(), BG_ALPHA); update(); }

  virtual QRectF boundingRect() const override {
    return QRectF(0, 0, Tmtr::fingerPixels() * 1.4, Tmtr::fingerPixels() * 1.4);
  }

  virtual QPainterPath shape() const override {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
  }

protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
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
  m_selectedAction(0),
  m_locked(false)
{
  m_instance = this;
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


void TmelodyItem::initialAnim() {
  QRadialGradient gr(0.0, 0.0, scene()->width() * 0.7);
  QColor bg = qApp->palette().highlight().color();
  bg.setAlpha(240);
  gr.setColorAt(0.0, Qt::transparent);
  gr.setColorAt(0.5, Qt::transparent);
  gr.setColorAt(0.8, bg);
  gr.setColorAt(1.0, qApp->palette().highlight().color());
  auto bgRect = scene()->addRect(0.0, 0.0, scene()->width(), scene()->height(), Qt::NoPen, QBrush(gr));

  createFlyActions();
  m_touched = true; // to highlight menu button
  update();
  m_locked = true; // to lock menu button
  auto fingerPoint = new TfingerPointer(bgRect);
  fingerPoint->setZValue(200);
  qreal radius = Tmtr::fingerPixels() * 0.7;
  for (int i = 0; i < m_flyList.size(); ++i) {
    QString hText = QLatin1String("<b>") + m_actions[i]->text() + QLatin1String("</b>");
    if (!m_actions[i]->statusTip().isEmpty())
      hText += QLatin1String("<br>") + m_actions[i]->statusTip().replace(QLatin1String(". "), QLatin1String(".<br>"));
    if (i == 2)
      hText += QLatin1String("<br>") + qTR("TvolumeView", "Switch on/off pitch detection");
    auto hint = new TgraphicsTextTip(hText, qApp->palette().highlight().color());
    scene()->addItem(hint);
    hint->setParentItem(bgRect);
    if (hint->boundingRect().width() + m_flyList[i]->x() + Tmtr::fingerPixels() * 1.7 > scene()->width() * 0.9)
      hint->setTextWidth(scene()->width() * 0.9 - (m_flyList[i]->x() + Tmtr::fingerPixels() * 1.7));
    if (i == 2)
      hint->setPos(m_flyList[i]->x(),
                 m_flyList[i]->y() + m_flyList[i]->boundingRect().height() + 15.0);
    else
      hint->setPos(m_flyList[i]->x() + Tmtr::fingerPixels() * 1.7,
                 m_flyList[i]->y() + (m_flyList[i]->boundingRect().height() - hint->realH()) / 2.0);
    fingerPoint->addMove(QPointF(x() + 10.0, y() + boundingRect().height() / 2.0), QPointF(m_flyList[i]->x() + radius, m_flyList[i]->y() + radius), 300, 750);
  }
  connect(fingerPoint, &TfingerPointer::moved, [=](int s){
    m_flyList[s]->setBgColor(qApp->palette().highlight().color());
    if (s > 0)
      m_flyList[s - 1]->setBgColor(Qt::black);
  });
  connect(fingerPoint, &TfingerPointer::finished, [=](){
    m_flyList.last()->setBgColor(Qt::black);
    QTimer::singleShot(10, [=]{ fingerPoint->start(); });
  });

  auto gotIt = new TgraphicsTextTip(qTR("QDialogButtonBox", "OK") + QLatin1String(" !"));
  gotIt->setDefaultTextColor(qApp->palette().base().color());
  gotIt->setParentItem(bgRect);
  gotIt->setScale(3.0);
  gotIt->setPos(scene()->width() * 0.95 - gotIt->realW(), scene()->height() * 0.95 - gotIt->realH());
  connect(gotIt, &TgraphicsTextTip::clicked, [=]{
    delete bgRect;
    qDeleteAll(m_flyList);
    m_flyList.clear();
    m_touched = false;
    m_locked = false;
    update();
    emit previewFinished();
  });
  fingerPoint->start();
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
  if (m_locked) // this is initial help preview - ignore touching menu button
    return;
  m_currentIt = 0;
  m_touched = true;
  update();
  emit touched();
  m_timer->start();
}


void TmelodyItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  if (m_locked) // this is initial help preview - ignore touching menu button
    return;
  if ((m_touched && event->pos().x() > Tmtr::fingerPixels()) || event->pos().y() > Tmtr::fingerPixels()) {
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
  if (m_locked) // this is initial help preview - ignore touching menu button
    return;
  m_timer->stop();
  qDeleteAll(m_flyList);
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






