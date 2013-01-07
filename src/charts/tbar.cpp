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
#include "tquestionpoint.h"
#include "tgraphicstexttip.h"
#include "tdropshadoweffect.h"
#include "tgroupedqaunit.h"
#include <texamview.h>
// #include <QDebug>

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
        painter->setPen(QPen(QColor(0, 192, 192), 3));
    else
        painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 1, 1);
    
}


QRectF Tbar::boundingRect() const {
    QRectF rect(WIDTH / -2, - m_height, WIDTH, m_height); // orign is in the centre on the bottom
    return rect;
}


QString Tbar::getTipText() {
    QString tipText = QApplication::translate("Tbar", "Statistics for:") + "<br>";
    tipText += "<span style=\"font-size: 20px;\"><b>" + m_qaGroup->description() + "</b></span><hr>";
    tipText += "<table><tr><td>";
    tipText += TexamView::averAnsverTimeTxt() + ": </td><td> <b>" + TexamView::formatReactTime(m_qaGroup->averTime(), true) + "</b></td></tr><tr><td>";
    tipText += ("Tbar", "Questions number:") + QString(" </td><td> <b>%1</b></td></tr><tr><td>").arg(m_qaGroup->size());
    tipText += TexamView::corrAnswersNrTxt() + QString(": </td><td> <b>%1</b></td></tr>")
        .arg(m_qaGroup->size() - m_qaGroup->mistakes() - m_qaGroup->notBad());
    if (m_qaGroup->mistakes())
      tipText += "<tr><td>" + TexamView::mistakesNrTxt() + QString(": </td><td> <b>%1</b></td></tr>").arg(m_qaGroup->mistakes());
    if (m_qaGroup->notBad())
      tipText += "<tr><td>" + TexamView::halfMistakenTxt() + QString(": </td><td> <b>%1</b></td></tr>").arg(m_qaGroup->notBad());
    tipText += "</table>";
    return tipText;
}


void Tbar::hoverEnterEvent(QGraphicsSceneHoverEvent* )
{
    if (!m_tip) {
      m_tip = new TgraphicsTextTip(getTipText(), QColor(0, 192, 192));
      scene()->addItem(m_tip);
      m_tip->setPos(pos().x(), pos().y() - m_height / 2);
      m_tip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
      update();
    }
}


void Tbar::hoverLeaveEvent(QGraphicsSceneHoverEvent* )
{
    if (m_tip) {
      m_tip->deleteLater();
      m_tip = 0;
      update();
    }
}






