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

#include "tguitarview.h"
#include "tfingerboard.h"
#include <tmtr.h>
#include <graphics/tdropshadoweffect.h>
#include <tpath.h>
#include <QtWidgets/qgraphicsproxywidget.h>
#include <QtWidgets/qscrollbar.h>
#include <QtCore/qdebug.h>
#if defined (Q_OS_ANDROID)
  #include <touch/ttouchproxy.h>
  #include <touch/ttouchparams.h>
  #include <ttouchmessage.h>
#endif


// TODO: Implement left-handed guitar view (someday)

TguitarView::TguitarView(QGraphicsView* guitar, QGraphicsView* parent) :
  QGraphicsView(0, 0),
  m_parent(parent),
  m_proxy(0),
  m_mark(0),
  m_couldBeTouch(false),
  m_touchStartedHere(false),
  m_isPreview(false)
{
  m_parent = parent;
  m_guitar = static_cast<TfingerBoard*>(guitar);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  setStyleSheet("background-color: transparent;");
  setScene(m_guitar->scene());
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  hide();
  connect(m_guitar, &TfingerBoard::enabilityChanged, this, &TguitarView::guitarAvailChanged);
}


TguitarView::~TguitarView()
{
  if (m_mark)
    delete m_mark;
}


bool TguitarView::guitarEnabled() {
  return m_guitar->guitarEnabled();
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
      QColor fretHi = m_guitar->selectedColor();
      fretHi.setAlpha(50);
      m_mark = m_guitar->scene()->addRect(QRectF(), Qt::NoPen, QBrush(fretHi));
      m_mark->setZValue(0);
      m_mark->hide();
    }
  }
#else
  m_isPreview = false;
#endif
  return m_isPreview;
}


bool TguitarView::mapTouchEvent(QTouchEvent* te) {
  m_guitar->deleteBeyondTip(); // in any case delete 'note beyond guitar scale' tip
#if defined (Q_OS_ANDROID)
  if (isPreview()) { // PREVIEW MODE
      if (!TtouchParams::i()->guitarWasTouched) { // display hint how to use fret preview
          tMessage->setMessage(TtouchProxy::touchGuitarHelp(), 0);
          TtouchParams::i()->guitarWasTouched = true;
      }
      switch (te->type()) {
        case QEvent::TouchBegin: {
          if (proxy()->isVisible()) // already displayed - can be scrolled or selected
            m_couldBeTouch = true;
          break;
        }
        case QEvent::TouchUpdate: {
          if (isVisible() && m_touchStartedHere &&
              QLineF(te->touchPoints().first().pos(), te->touchPoints().first().startPos()).length() > Tmtr::fingerPixels() / 2)
          { // scrolling - so not selected
              horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                              (te->touchPoints()[0].pos().x() - te->touchPoints()[0].lastPos().x()));
              quint8 currFret = m_guitar->pointToFinger(QPoint(horizontalScrollBar()->value() / transform().m11(),
                                                               m_guitar->height() / 2)).fret();
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
          if (isVisible()) {
            if (m_touchStartedHere) { // preview was touched so it might be scrolled or touched
              if (m_couldBeTouch) { // it was touched - fake press the guitar
                if (m_parent->itemAt(te->touchPoints().first().startPos().toPoint()) == proxy()) {
                  QPointF mtgv = proxy()->mapFromScene(te->touchPoints().first().pos()); // mapped from global scene to guitar view
                  m_guitar->fakePress(QPointF((mtgv.x() + horizontalScrollBar()->value()) / transform().m11(),
                                              mtgv.y() / transform().m11()).toPoint());
                }
              } else // touched but scrolled - update displayed fret
                  updateContextPosition();
            } else // guitar (parent widget) was touched, so hide the preview
                hide();
          } else // not visible -display it at touch position
              displayAt(te->touchPoints().first().pos());
          m_touchStartedHere = false;
          m_couldBeTouch = false;
          break;
        }
        default:
          break;
      }
  } else
#endif
  { // NO PREVIEW - only convert touch in guitar into mouse press
      if (te->type() == QEvent::TouchEnd)
          m_guitar->fakePress(QPoint(te->touchPoints().first().pos().x() - m_guitar->x(),
                                     te->touchPoints().first().pos().y() - m_guitar->y()));
  }
  return true;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TguitarView::guitarAvailChanged(bool guitEnabled) {
  if (guitEnabled) {

  } else {
    if (isVisible()) {
      hide();
    }
  }
}


#if defined (Q_OS_ANDROID)
void TguitarView::displayAt(const QPointF& scenePos) {
  m_fret = m_guitar->pointToFinger(QPoint(scenePos.x(), m_guitar->height() / 2)).fret();
  updateContextPosition();
  proxy()->setPos(m_fret ? m_guitar->fretPositionX(m_fret) + 9.0 : m_parent->width() - width(), m_parent->height() - height() - 4);
  proxy()->show();
  updateMarkPosition();
}


void TguitarView::updateMarkPosition() {
  if (m_fret) {
    m_mark->setRect(0, 0,
                    m_guitar->fretPositionX(m_fret) - (m_fret > 1 ? m_guitar->fretPositionX(m_fret - 1) + 9.0 : m_guitar->fbRect().x() + 2.0),
                    m_guitar->fbRect().height() - 6.0);
    m_mark->setPos((m_fret > 1 ? m_guitar->fretPositionX(m_fret - 1) + 7.0 : m_guitar->fbRect().x()), m_guitar->fbRect().y() + 3.0);
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
//     if (!m_guitar->isRightHanded()) {
//       painter.translate(width(), 0);
//       painter.scale(-1, 1);
//     }
    if (m_guitar->guitarTypeId() == 1) { // No need to check for non guitar - in such a case it never goes here
      QPixmap guitarPixmap = QPixmap(Tpath::img("body")).scaled(
                  (m_parent->width() / 2) * transform().m11(), height() * 2.445714285714286, Qt::IgnoreAspectRatio);
      painter.drawPixmap((m_guitar->posX12fret() + 7) * transform().m11() - horizontalScrollBar()->value(),
                         height() - guitarPixmap.height(), guitarPixmap);
    } else {
      QPixmap guitarPixmap;
      if (m_guitar->guitarTypeId() == 2)
          guitarPixmap = QPixmap(Tpath::img("body-electro")).scaled(644 * ((height() * 2.9) / 614), height() * 2.9);
      else
          guitarPixmap = QPixmap(Tpath::img("body-bass")).scaled(640 * ((height() * 2.9) / 535), height() * 2.9);
      painter.drawPixmap((m_guitar->fbRect().right() - m_guitar->height() * 1.449532710280374) * transform().m11() - horizontalScrollBar()->value(),
                         height() - guitarPixmap.height() , guitarPixmap);
//       if (!gl->GisRightHanded)
//         painter.resetTransform();
      qreal pickCoef = (((height() * 2.9) / 614.0) * 0.5);
      QPixmap rosettePixmap = QPixmap(Tpath::img("pickup")).scaled(291 * pickCoef, 468 * pickCoef, Qt::KeepAspectRatio);
      painter.drawPixmap(m_guitar->pickRect()->x() * transform().m11() - horizontalScrollBar()->value(),
                         6 * transform().m11(), rosettePixmap);
    }
  }
  QGraphicsView::paintEvent(event);
}


void TguitarView::hideEvent(QHideEvent* event) {
  if (m_mark)
    m_mark->hide();
  m_touchStartedHere = false;
  m_couldBeTouch = false;
  QGraphicsView::hideEvent(event);
}
#endif




