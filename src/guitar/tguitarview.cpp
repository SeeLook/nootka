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


//TODO: guitar has to emit signal when changes being enabled to stop/start touch propagation

TguitarView::TguitarView(QGraphicsView* guitar, QGraphicsView* parent) :
  QGraphicsView(0, 0),
  m_parent(parent),
  m_proxy(0),
  m_mark(0),
  m_couldBeTouch(false),
  m_wasTouched(false)
{
  m_parent = parent;
//   setAttribute(Qt::WA_AcceptTouchEvents);
  m_guitar = static_cast<TfingerBoard*>(guitar);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  setStyleSheet("background-color: transparent;");
  setScene(m_guitar->scene());
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  hide();
}


TguitarView::~TguitarView()
{
  if (m_mark)
    delete m_mark;
}


bool TguitarView::checkIsPreview() {
#if defined (Q_OS_ANDROID)
  m_isPreview = Tmtr::fingerPixels() * 4 > m_guitar->height() * 1.1;
  if (isPreview()) { // as long as it is called only under Android where re-sizing doesn't occur
    if (!m_proxy) { // it is sufficient to call this once - only when proxy is created
      m_proxy = m_parent->scene()->addWidget(this);
      m_proxy->setZValue(50);
      m_proxy->hide();
      m_proxy->setGraphicsEffect(new TdropShadowEffect());
      setFixedSize(m_guitar->averFretWidth() * 2.5, qMax<int>(Tmtr::fingerPixels() * 4, m_guitar->viewport()->height() * 1.1)); // also size of preview
      qreal factor = (qreal)height() / (qreal)m_guitar->viewport()->height(); // and its scale
      scale(factor, factor); // will not change
      QPolygonF polygon;
      polygon << QPointF(0.0, 0.0)
              << QPointF(m_guitar->fbRect().y() / 2, m_guitar->fbRect().y() * 1.5)
              << QPointF(m_guitar->fbRect().y()  * 1.5, 0.0)
              << QPointF(0.0, 0.0);
      m_mark = m_guitar->scene()->addPolygon(polygon, Qt::NoPen, QBrush(qApp->palette().highlight().color()));
      m_mark->hide();
      qDebug() << "view" << qMax<int>(Tmtr::fingerPixels() * 4, m_guitar->viewport()->height() * 1.1);
    }
  }
#else
  m_isPreview = false;
#endif
}


void TguitarView::displayAt(const QPointF& scenePos) {
#if defined (Q_OS_ANDROID)
  m_fret = m_guitar->pointToFinger(QPoint(scenePos.x(), m_guitar->height() / 2)).fret();
  updateContextPosition();
  proxy()->setPos(m_fret ? m_guitar->fretPositionX(m_fret) + 9.0 : m_parent->width() - width(), m_parent->height() - height() - 4);
  proxy()->show();
  updateMarkPosition();
#endif
}


bool TguitarView::mapTouchEvent(QTouchEvent* te) {
#if defined (Q_OS_ANDROID)
  if (isPreview()) {
      if (!isVisible())
          return false;
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
              if ((m_fret == 1 && currFret > 1) || (m_fret != 1 && currFret != m_fret)) {
                m_fret = currFret;
                updateMarkPosition();
                proxy()->setPos(m_fret ? m_guitar->fretPositionX(m_fret) + 9.0 :
                                         m_parent->width() - width(), m_parent->height() - height() - 4);
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
  } else
#endif
  { // no preview - only convert touch in guitar into mouse press
      if (te->type() == QEvent::TouchEnd)
        m_guitar->fakePress(QPoint(te->touchPoints().first().pos().x() - m_guitar->x(), te->touchPoints().first().pos().y() - m_guitar->y()));
  }
  return true;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
#if defined (Q_OS_ANDROID)
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
  if (isPreview() && horizontalScrollBar()->value() >= m_guitar->fbRect().x() + m_guitar->fbRect().width()) {
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


void TguitarView::hideEvent(QHideEvent* event) {
  if (m_mark)
    m_mark->hide();
  QGraphicsView::hideEvent(event);
}
#endif




