/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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

#include "tgraphicstexttip.h"
#include "tdropshadoweffect.h"
#include <tcolor.h>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <QGraphicsScene>
#include <QDebug>


/* static */
void TgraphicsTextTip::alignCenter(QGraphicsTextItem* tip) {
    tip->setTextWidth(tip->boundingRect().width());
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    QTextCursor cursor = tip->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    cursor.clearSelection();
    tip->setTextCursor(cursor);
}


void TgraphicsTextTip::setDropShadow(QGraphicsTextItem* tip, QColor shadowColor) {
  TdropShadowEffect *shadow = new TdropShadowEffect();
  if (shadowColor != -1)
    shadow->setColor(QColor(shadowColor.name()));
  else
    shadow->setColor(Tcolor::shadow);
  tip->setGraphicsEffect(shadow);
}


//#################################################################################################
//###################            CONSTRUCTORS          ############################################
//#################################################################################################

TgraphicsTextTip::TgraphicsTextTip(QString text, QColor bgColor) :
  QGraphicsTextItem(),
  m_bgColor(bgColor),
  m_movable(false)
{
  setHtml(text);
  setDropShadow(this, bgColor);  
}


TgraphicsTextTip::TgraphicsTextTip() :
  QGraphicsTextItem(),
  m_bgColor(-1),
  m_movable(false)
{
  setDropShadow(this);
}


TgraphicsTextTip::~TgraphicsTextTip()
{
	setCursor(Qt::ArrowCursor); // when exam is managed with keys it may stay look as 'dragging', better back it to 'arrow'
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TgraphicsTextTip::setHtml(QString htmlText) {
  QGraphicsTextItem::setHtml(htmlText);
  alignCenter(this);
}


void TgraphicsTextTip::setBgColor(QColor col) {
  m_bgColor = col; 
  delete graphicsEffect();
  setDropShadow(this, QColor(col.name()));
}


void TgraphicsTextTip::setTextInteractionFlags(Qt::TextInteractionFlags flags) {
	if (flags | Qt::LinksAccessibleByMouse)
		connect(this, &TgraphicsTextTip::linkHovered, this, &TgraphicsTextTip::linkHoveredSlot, Qt::UniqueConnection);
	else
		disconnect(this, &TgraphicsTextTip::linkHovered, this, &TgraphicsTextTip::linkHoveredSlot);
	QGraphicsTextItem::setTextInteractionFlags(flags);
}


void TgraphicsTextTip::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  if (m_bgColor != -1) {
    QRectF rect = boundingRect();
    QColor startColor = m_bgColor;
    startColor.setAlpha(25);
    QColor endColor = startColor;
    endColor.setAlpha(75);
    painter->setPen(QPen(endColor, 1));
		painter->setBrush(QBrush(qApp->palette().base().color()));
    painter->drawRoundedRect(rect, 5, 5);
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0.2, startColor);
    grad.setColorAt(0.95, endColor);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 5, 5);
  }
	QGraphicsTextItem::paint(painter, option, widget);
}


QRectF TgraphicsTextTip::boundingRect() const {
  return QGraphicsTextItem::boundingRect();
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

/** !!!!!!!!!!!!!!!
 * All moving methods will work properly only for items with no parent - ones those belongs to scene directly 
*/
void TgraphicsTextTip::linkHoveredSlot(const QString& link) {
	if (link.isEmpty()) {
		setCursor(m_lastLinkCursor);
	} else {
		m_lastLinkCursor = cursor().shape();
		setCursor(Qt::PointingHandCursor);
	}
}


void TgraphicsTextTip::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	if (isMovable())
		setCursor(Qt::SizeAllCursor);
}


void TgraphicsTextTip::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
	if (isMovable())
		setCursor(Qt::ArrowCursor);
}


void TgraphicsTextTip::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (isMovable() && event->button() == Qt::LeftButton) {
		m_lastPos = event->scenePos();
		setCursor(Qt::DragMoveCursor);
	}
}


void TgraphicsTextTip::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	if (isMovable() && event->buttons() == Qt::LeftButton) {
		if (!m_lastPos.isNull())
				setPos(qBound(0.0, x() + event->scenePos().x() - m_lastPos.x(), scene()->width() - boundingRect().width() * scale()),
							 qBound(0.0, y() + event->scenePos().y() - m_lastPos.y(), scene()->height() - boundingRect().height() * scale()));
		m_lastPos = event->scenePos();
	}
	QGraphicsTextItem::mouseMoveEvent(event);
}


void TgraphicsTextTip::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
	if (isMovable())
		setCursor(Qt::SizeAllCursor);
	event->accept();
	QGraphicsTextItem::mouseReleaseEvent(event);
}






