/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "tpaneitem.h"
#include <tnoofont.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>


#if defined (Q_OS_ANDROID)
  #define FONT_SIZE (4)
#else
  #define FONT_SIZE (3)
#endif


TpaneItem::TpaneItem(int charNr, TscoreScene* sc) :
  TscoreItem(sc),
  m_symbol(QString(QChar(charNr))),
  m_color(qApp->palette().text().color()),
  m_selected(false)
{
  setAcceptHoverEvents(true);
  m_font = TnooFont(FONT_SIZE);
}


void TpaneItem::setFont(const QFont& f) {
  prepareGeometryChange();
  m_font = f;
}


QRectF TpaneItem::boundingRect() const {
  return QRectF(0.0, 0.0, WIDTH, FONT_SIZE + 0.5);
}


void TpaneItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  if (m_selected) {
      painter->setPen(Qt::NoPen);
      painter->setBrush(qApp->palette().highlight().color());
      painter->drawRect(boundingRect());
      painter->setPen(qApp->palette().highlightedText().color());
  } else
      painter->setPen(m_color);
  painter->setFont(m_font);
  painter->drawText(QRectF(0.0, 0.25, boundingRect().width(), boundingRect().height()), Qt::AlignCenter,  m_symbol);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TpaneItem::mousePressEvent(QGraphicsSceneMouseEvent*) {
  setGraphicsEffect(nullptr);
  emit clicked();
}


void TpaneItem::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
  QGraphicsDropShadowEffect *hiBlur = new QGraphicsDropShadowEffect();
    hiBlur->setColor(qApp->palette().highlight().color());
    hiBlur->setOffset(1.0, 1.0);
    hiBlur->setBlurRadius(7.0);
  setGraphicsEffect(hiBlur);
  emit entered(this);
}


void TpaneItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
  setGraphicsEffect(nullptr);
  emit leaved(this);
}


void TpaneItem::touched(const QPointF&) {
  mousePressEvent(nullptr);
}


