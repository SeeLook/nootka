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

#include "tguitarview.h"
#include "tfingerboard.h"
#include <tmtr.h>
#include <graphics/tdropshadoweffect.h>
#include <tpath.h>
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include <QDebug>


TguitarView::TguitarView(QGraphicsView* guitar, QGraphicsView* parent) :
  QGraphicsView(0, 0),
  m_parent(parent),
  m_couldBeTouch(false),
  m_wasTouched(false)
{
  m_parent = parent;
//   setAttribute(Qt::WA_AcceptTouchEvents);
  m_guitar = static_cast<TfingerBoard*>(guitar);
  setStyleSheet("background-color: transparent;");
  setScene(m_guitar->scene());
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFixedSize(m_guitar->averFretWidth() * 2.5, Tmtr::fingerPixels() * 4);
  qreal factor = (qreal)height() / (qreal)m_guitar->viewport()->height();
  scale(factor, factor);
  m_proxy = parent->scene()->addWidget(this);
  m_proxy->setZValue(50);
  m_proxy->hide();
  m_proxy->setGraphicsEffect(new TdropShadowEffect());
  QPolygonF polygon;
  polygon << QPointF(0.0, 0.0)
          << QPointF(m_guitar->fbRect().y() / 2, m_guitar->fbRect().y())
          << QPointF(m_guitar->fbRect().y(), 0.0)
          << QPointF(0.0, 0.0);
  m_mark = m_guitar->scene()->addPolygon(polygon, Qt::NoPen, QBrush(qApp->palette().highlight().color()));
  m_mark->hide();
}


TguitarView::~TguitarView()
{
  delete m_mark;
}


void TguitarView::displayAt(const QPointF& scenePos) {
  m_fret = m_guitar->pointToFinger(QPoint(scenePos.x(), m_guitar->height() / 2)).fret();
  updateContextPosition();
  proxy()->setPos(m_fret ? m_guitar->fretPositionX(m_fret) : m_parent->width() - width(), m_parent->height() - height() - 4);
  proxy()->show();
  updateMarkPosition();
}


bool TguitarView::mapTouchEvent(QTouchEvent* te) {
  switch (te->type()) {
    case QEvent::TouchBegin: {
      if (proxy()->isVisible()) // already displayed - prepare for scrolling
        m_couldBeTouch = true;
      break;
    }
    case QEvent::TouchUpdate: {
      if (QLineF(te->touchPoints().first().pos(), te->touchPoints().first().startPos()).length() > Tmtr::fingerPixels() / 2) {
          horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                          (te->touchPoints()[0].pos().x() - te->touchPoints()[0].lastPos().x()));
          quint8 currFret = m_guitar->pointToFinger(QPoint(horizontalScrollBar()->value() / transform().m11(), m_guitar->height() / 2)).fret();
          if (currFret != m_fret) {
            m_fret = currFret;
            updateMarkPosition();
            proxy()->setPos(m_fret ? m_guitar->fretPositionX(m_fret) : m_parent->width() - width(), m_parent->height() - height() - 4);
          }
          m_couldBeTouch = false;
      }
      break;
    }
    case QEvent::TouchEnd: {
      if (m_couldBeTouch) {
        if (m_parent->itemAt(te->touchPoints().first().startPos().toPoint()) == proxy()) {
          QPointF mtgv = proxy()->mapFromScene(te->touchPoints().first().pos()); // mapped from global scene to guitar view
          m_guitar->fakePress(QPointF((mtgv.x() + horizontalScrollBar()->value()) / transform().m11(), mtgv.y() / transform().m11()).toPoint());
        }
      } else if (m_wasTouched)
          updateContextPosition();
      m_wasTouched = false;
      m_couldBeTouch = false;
      break;
    }
    default:
      break;
  }
  return true;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TguitarView::updateMarkPosition() {
  if (m_fret) {
    m_mark->setPos(m_guitar->fretPositionX(m_fret) - 25.0, 0.0);
    m_mark->show();
  } else
    m_mark->hide();
}


void TguitarView::updateContextPosition() {
  if (m_fret) {
      if (m_fret == 1)
        horizontalScrollBar()->setValue(1);
      else
        horizontalScrollBar()->setValue(m_guitar->fretPositionX(m_fret - 1) * transform().m11());
  } else {
      horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
  }
}


void TguitarView::paintEvent(QPaintEvent* event) {
  if (horizontalScrollBar()->value() >= m_guitar->fbRect().x() + m_guitar->fbRect().width()) {
    QPainter painter(viewport());
//     if (!gl->GisRightHanded) {
//       painter.translate(width(), 0);
//       painter.scale(-1, 1);
//     }
//     if (gl->instrument == e_classicalGuitar || gl->instrument == e_noInstrument) {
      QPixmap guitarPixmap = QPixmap(Tpath::img("body")).scaled(
                  (m_parent->width() / 2) * transform().m11(), height() * 2.445714285714286, Qt::IgnoreAspectRatio);
      painter.drawPixmap((m_guitar->posX12fret() + 7) * transform().m11() - horizontalScrollBar()->value(),
                         height() - guitarPixmap.height(), guitarPixmap);
//     } else {
//       qreal ratio = (guitar->height() * 3.3) / 535;
//       painter.drawPixmap(guitar->fbRect().right() - 235 * ratio, height() - m_bgPixmap.height() , m_bgPixmap);
//       if (!gl->GisRightHanded)
//         painter.resetTransform();
//       painter.drawPixmap(guitar->pickRect()->x(), guitar->pickRect()->y(), m_rosettePixmap);
//     }
  }
  QGraphicsView::paintEvent(event);
}






