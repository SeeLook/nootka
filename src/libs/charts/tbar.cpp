/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#include "tbar.h"
#include "tgroupedqaunit.h"
#include "tquestionpoint.h"
#include "tstatisticstip.h"
#include <graphics/tdropshadoweffect.h>
#include <QPainter>
#include <QGraphicsColorizeEffect>
#include <QGraphicsScene>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>

#define WIDTH (30) // default width of a bar

TstatisticsTip::Ekind Tbar::m_tipType = TstatisticsTip::e_full;


Tbar::Tbar(qreal height, TgroupedQAunit* qaGroup, TstatisticsTip::Ekind tipType) :
    TtipHandler(),
    m_height(height),
    m_qaGroup(qaGroup),
    m_isUnderMouse(false)
{
    m_tipType = tipType;
    m_wrongAt = (qreal)m_qaGroup->mistakes() / (qreal)m_qaGroup->size();
    m_notBadAt = (qreal)m_qaGroup->notBad() / (qreal)m_qaGroup->size();
    TdropShadowEffect *shadow = new TdropShadowEffect();
    setGraphicsEffect(shadow);
}


Tbar::~Tbar()
{}


//####################################################################################
//##################### protected methods ############################################
//####################################################################################

void Tbar::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    QRectF rect = boundingRect();
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
    if (m_isUnderMouse)
        painter->setPen(QPen(QColor(0, 192, 192), 2));
    else
        painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 1, 1);
//     QLinearGradient shad(0, 0, rect.width(), 0);
//     QColor shCol = TquestionPoint::bgColor();
//     shCol.setAlpha(255);
//     grad.setColorAt(0.0, shCol);
//     shCol.setAlpha(50);
//     grad.setColorAt(1.0, shCol);
//     painter->setBrush(QBrush(shad));
//     painter->drawRoundedRect(rect, 1, 1);
}


QRectF Tbar::boundingRect() const {
    QRectF rect(WIDTH / -2, - m_height, WIDTH, m_height); // orign is in the centre on the bottom
    return rect;
}



void Tbar::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  m_isUnderMouse = true;
  if (tip) {
    if (tip == initObject())
        return;
    else
      deleteTip();
  }
  tip = new TstatisticsTip(m_qaGroup, m_tipType);
  handleTip(event->scenePos());
}


void Tbar::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    m_isUnderMouse = false;
    update();
    TtipHandler::hoverLeaveEvent(event);
}


void Tbar::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (!tip)
      hoverEnterEvent(event);
}







