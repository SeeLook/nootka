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
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include <QDebug>



TguitarView::TguitarView(QGraphicsView* guitar, QGraphicsView* parent) :
  QGraphicsView(0, 0)
{
  setAttribute(Qt::WA_AcceptTouchEvents);
  m_guitar = static_cast<TfingerBoard*>(guitar);
  setScene(m_guitar->scene());
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFixedSize(parent->width(), parent->height() / 2);
  qreal factor = (qreal)height() / (qreal)m_guitar->viewport()->height();
  scale(factor, factor);
  m_proxy = parent->scene()->addWidget(this);
  m_proxy->setPos(0, parent->height() - height());
  m_proxy->setZValue(50);
  guitar->installEventFilter(this);
}


bool TguitarView::mapTouchEvent(QTouchEvent* te) {
  if (te->type() == QEvent::TouchUpdate) {
    if (QLineF(te->touchPoints().first().pos(), te->touchPoints().first().startPos()).length() > 10) // TODO use screen factor instead 10
      horizontalScrollBar()->setValue (horizontalScrollBar()->value() + (te->touchPoints()[0].lastPos().x() - te->touchPoints()[0].pos().x()));
  } else if (te->type() == QEvent::TouchEnd) {
      if (QLineF(te->touchPoints().first().pos(), te->touchPoints().first().startPos()).length() < 10) { // TODO use screen factor instead 10
        QPointF mtgv = proxy()->mapFromScene(te->touchPoints().first().pos()); // mapped from global scene to guitar view
        m_guitar->fakePress(QPointF((mtgv.x() + horizontalScrollBar()->value()) / transform().m11(), mtgv.y() / transform().m11()).toPoint());
      }
    }
  return true;
}






