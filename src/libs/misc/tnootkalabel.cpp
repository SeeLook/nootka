/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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

#include "tnootkalabel.h"
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtGui/qevent.h>


TnootkaLabel::TnootkaLabel(const QString& pixmapPath, QWidget* parent, QColor bgColor, const QString& version) :
  QGraphicsView(parent),
  m_bgColor(bgColor),
  m_hoverColor(-1)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet(QStringLiteral("background: transparent; border-radius: 10px;"));
  setRenderHint(QPainter::TextAntialiasing, true);

  QGraphicsScene *scene = new QGraphicsScene(this);
  setScene(scene);

  m_pixItem = new QGraphicsPixmapItem(QPixmap(pixmapPath));
  m_pixItem->setTransformationMode(Qt::SmoothTransformation);
  scene->addItem(m_pixItem);
  if (!version.isEmpty()) {
    QGraphicsSimpleTextItem *ver = new QGraphicsSimpleTextItem(version);
    scene->addItem(ver);
    ver->setBrush(Qt::white);
    QFont f = ver->font();
    f.setBold(true);
    ver->setZValue(255);
    ver->setFont(f);
    ver->setScale((m_pixItem->pixmap().height() / 3.5) / ver->boundingRect().height());
    ver->setPos((m_pixItem->boundingRect().width() - ver->boundingRect().width() * ver->scale()) / 2.0,
                m_pixItem->pixmap().height() - ver->boundingRect().height() * ver->scale());
  }
  if (parent && parent->height() > m_pixItem->pixmap().height())
    resize(m_pixItem->pixmap().size());
  else {
    qreal factor = qreal(parent->height()) / qreal(m_pixItem->pixmap().height());
    scale(factor, factor);
  }

  m_effect = new QGraphicsColorizeEffect();
  if (m_bgColor == -1)
    m_bgColor = palette().window().color();
  m_effect->setColor(m_bgColor);
  m_pixItem->setGraphicsEffect(m_effect);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

//#################################################################################################
//###################              EVENTS              ############################################
//#################################################################################################

bool TnootkaLabel::event(QEvent* event) {
  if (m_hoverColor != -1) {
    if (event->type() == QEvent::Enter)
      m_effect->setColor(m_hoverColor);
    else if (event->type() == QEvent::Leave)
      m_effect->setColor(m_bgColor);
  }
  return QGraphicsView::event(event);
}


void TnootkaLabel::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton)
    emit clicked();
}


void TnootkaLabel::resizeEvent(QResizeEvent* event) {
  if (height() < m_pixItem->pixmap().height() * 1.5) {
    qreal factor = (qreal(height()) / qreal(m_pixItem->pixmap().height())) / transform().m11();
    scale(factor, factor);
    setMinimumWidth(qRound(m_pixItem->pixmap().width() * transform().m11()));
  }
  QGraphicsView::resizeEvent(event);
}



