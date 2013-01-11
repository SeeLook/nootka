/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tbar.h"
#include <QPainter>
#include <QGraphicsColorizeEffect>
#include <QGraphicsScene>
#include <QApplication>
#include <QGraphicsView>
#include "tquestionpoint.h"
#include "tgraphicstexttip.h"
#include "tdropshadoweffect.h"
#include "tstatisticstip.h"
#include "tgroupedqaunit.h"
#include "texamview.h"
#include <QDebug>

#define WIDTH (30) // default width of a bar


Tbar::Tbar(qreal height, TgroupedQAunit* qaGroup) :
    m_height(height),
    m_qaGroup(qaGroup),
    m_tip(0)
{
    setAcceptHoverEvents(true);
    m_wrongAt = (qreal)m_qaGroup->mistakes() / (qreal)m_qaGroup->size();
    m_notBadAt = (qreal)m_qaGroup->notBad() / (qreal)m_qaGroup->size();
    TdropShadowEffect *shadow = new TdropShadowEffect();
    setGraphicsEffect(shadow);
}


//####################################################################################
//##################### protected methods ############################################
//####################################################################################

void Tbar::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    QRectF rect = boundingRect();
//     rect.translate(1, -1);
    qreal nextAt = 0.0;
    QColor endColor;
    QLinearGradient grad(0, -rect.height(), 0, 0);
    if (m_wrongAt) {
      grad.setColorAt(0.0, TquestionPoint::wrongColor());
      nextAt += m_wrongAt;
      grad.setColorAt(nextAt - 0.01, TquestionPoint::wrongColor());
      endColor = TquestionPoint::wrongColor();
    }      
    if (m_notBadAt) {
      grad.setColorAt(nextAt, TquestionPoint::notBadColor());
      nextAt += m_notBadAt;
      grad.setColorAt(nextAt - 0.01, TquestionPoint::notBadColor());
      endColor = TquestionPoint::notBadColor();
    }
    if (m_qaGroup->mistakes() + m_qaGroup->notBad() < m_qaGroup->size()) {
      grad.setColorAt(nextAt, TquestionPoint::goodColor());
      endColor = TquestionPoint::goodColor();
    }
    grad.setColorAt(1.0, endColor);
    if (m_tip)
        painter->setPen(QPen(TquestionPoint::notBadColor(), 1));
    else
        painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 1, 1);
    
}


QRectF Tbar::boundingRect() const {
    QRectF rect(WIDTH / -2, - m_height, WIDTH, m_height); // orign is in the centre on the bottom
    return rect;
}



void Tbar::hoverEnterEvent(QGraphicsSceneHoverEvent* )
{
    if (!m_tip) {
      m_tip = new TstatisticsTip(m_qaGroup);
      scene()->addItem(m_tip);
      m_tip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
      QPointF p = pos();
      QSize s = scene()->views()[0]->size();
      p.setY(p.y() - m_height / 2);
      QPointF mapedP = scene()->views()[0]->mapFromScene(p);
      // determine where to display tip when point is near a view boundaries
      if (mapedP.x() > (s.width() / 2) ) // tip on the left
          p.setX(p.x() - m_tip->boundingRect().width() / scene()->views()[0]->transform().m22());
     if (mapedP.y() > (s.height() / 2) )
         p.setY(p.y() - m_height / scene()->views()[0]->transform().m22());
      m_tip->setPos(p);
      m_tip->setZValue(70);
      update();
    }
}


void Tbar::hoverLeaveEvent(QGraphicsSceneHoverEvent* )
{
    if (m_tip) {
      m_tip->deleteLater();
      m_tip = 0;
      update();
      scene()->update();
    }
}






