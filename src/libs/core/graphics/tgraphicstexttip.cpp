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

#include "tgraphicstexttip.h"
#include "tdropshadoweffect.h"
#include <tcolor.h>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <QDebug>


/* static */
void TgraphicsTextTip::alignCenter(QGraphicsTextItem* tip) {
  tip->setTextWidth(tip->boundingRect().width() * tip->scale());
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignCenter);
  QTextCursor cursor = tip->textCursor();
  cursor.select(QTextCursor::Document);
  cursor.mergeBlockFormat(format);
  cursor.clearSelection();
  tip->setTextCursor(cursor);
}


void TgraphicsTextTip::setDropShadow(QGraphicsTextItem* tip, QColor shadowColor) {
  Q_UNUSED(shadowColor)
  TdropShadowEffect *shadow = new TdropShadowEffect();
  shadow->setColor(qApp->palette().text().color()); // consistent shadow color looks better than colored
  tip->setGraphicsEffect(shadow);
}


//#################################################################################################
//###################            CONSTRUCTORS          ############################################
//#################################################################################################

TgraphicsTextTip::TgraphicsTextTip(const QString& text, QColor bgColor) :
  QGraphicsTextItem(),
  m_bgColor(bgColor),
  m_movable(false), m_mouseClick(false)
{
  setHtml(text);
  setDropShadow(this, bgColor);
  if (m_bgColor != -1)
      m_frameColor = QColor(m_bgColor.name());
  m_baseColor = qApp->palette().base().color();
}


TgraphicsTextTip::TgraphicsTextTip() :
  QGraphicsTextItem(),
  m_bgColor(-1),
  m_movable(false)
{
  setDropShadow(this);
  m_baseColor = qApp->palette().base().color();
}


TgraphicsTextTip::~TgraphicsTextTip()
{
  setCursor(Qt::ArrowCursor); // when exam is managed with keys it may stay look as 'dragging', better back it to 'arrow'
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TgraphicsTextTip::setHtml(const QString& htmlText) {
  QGraphicsTextItem::setHtml(htmlText);
  alignCenter(this);
}


void TgraphicsTextTip::setScale(qreal sc) {
  QFont f = font();
  if (f.pointSize() == -1) { // point size was not set
      if (f.pixelSize() != -1) // check pixel size then
          f.setPixelSize(f.pixelSize() *sc);
  } else
      f.setPointSizeF(sc * f.pointSize());
  setFont(f);
  if (textWidth() != -1)
    setTextWidth(textWidth() * sc);
}


void TgraphicsTextTip::setBgColor(const QColor& col) {
  if (m_bgColor != -1)
      m_frameColor = QColor(col.name());
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
    QColor borderColor = m_frameColor;
    if (m_mouseClick)
      borderColor = borderColor.lighter();
    painter->setPen(QPen(borderColor, 1.5));
    painter->setBrush(QBrush(m_baseColor));
    painter->drawRoundedRect(rect, 2, 2);
    QLinearGradient grad(rect.width() / 2, 0, rect.width() / 2, rect.height());
    grad.setColorAt(0.4, startColor);
    grad.setColorAt(0.9, endColor);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 2, 2);
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
 * All moving methods will work properly only for items with no parent - those ones belong to scene directly
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
  emit entered();
}


void TgraphicsTextTip::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  if (isMovable())
    setCursor(Qt::ArrowCursor);
  emit leaved();
}


void TgraphicsTextTip::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    bool prevClickState = m_mouseClick;
    if (isMovable())
      setCursor(Qt::DragMoveCursor);
    m_lastPos = event->scenePos();
    m_mouseClick = true;
    if (prevClickState != m_mouseClick)
      update();
  }
}


void TgraphicsTextTip::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  if (isMovable() && event->buttons() == Qt::LeftButton) {
    if (!m_lastPos.isNull()) {
      setFixPos(x() + event->scenePos().x() - m_lastPos.x(), y() + event->scenePos().y() - m_lastPos.y());
      emit moved();
    }
    m_lastPos = event->scenePos();
  }
  QGraphicsTextItem::mouseMoveEvent(event);
}


void TgraphicsTextTip::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (isMovable())
    setCursor(Qt::SizeAllCursor);
  if (m_mouseClick) {
    if (m_lastPos == event->scenePos()) {
      clearFocus();
      emit clicked();
    }
    m_mouseClick = false;
    update();
  }
  event->accept();
  QGraphicsTextItem::mouseReleaseEvent(event);
}






