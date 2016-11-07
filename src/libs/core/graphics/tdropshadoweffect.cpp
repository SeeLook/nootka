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


#include "tdropshadoweffect.h"
#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QDebug>



TdropShadowEffect::TdropShadowEffect(const QColor& color) :
  QGraphicsEffect(),
  m_distance(4.0),
  m_blurRadius(20.0)
{
//   setBlurRadius(10);
//   setOffset(4, 4);
  if (color == -1)
    setColor(qApp->palette().text().color());
  else
    setColor(color);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

QT_BEGIN_NAMESPACE
  extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0 );
QT_END_NAMESPACE

void TdropShadowEffect::draw(QPainter* painter) {
  // if nothing to show outside the item, just draw source
  if ((blurRadius() + distance()) <= 0) {
    drawSource(painter);
    return;
  }

  QPoint offset;
  const QPixmap px = sourcePixmap(Qt::DeviceCoordinates, &offset, QGraphicsEffect::PadToEffectiveBoundingRect);
  if (px.isNull()) // return if no source
    return;
  QTransform restoreTransform = painter->worldTransform(); // save world transform
  painter->setWorldTransform(QTransform());
  QSize szi(px.size().width() + 2 * distance(), px.size().height() + 2 * distance()); // Calculate size for the background image

  QImage tmp(szi, QImage::Format_ARGB32_Premultiplied);
  QPixmap scaled = px.scaled(szi);
  tmp.fill(0);
  QPainter tmpPainter(&tmp);
  tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
  tmpPainter.drawPixmap(QPointF(-distance(), -distance()), scaled);
  tmpPainter.end();

  // blur the alpha channel
  QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
  blurred.fill(0);
  QPainter blurPainter(&blurred);
  qt_blurImage(&blurPainter, tmp, blurRadius(), false, true);
  blurPainter.end();

  tmp = blurred;

  // blacken the image...
  tmpPainter.begin(&tmp);
  tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  tmpPainter.fillRect(tmp.rect(), color());
  tmpPainter.end();

  painter->drawImage(offset, tmp); // draw the blurred shadow...
  painter->drawPixmap(offset, px, QRectF()); // draw the actual pixmap...
  painter->setWorldTransform(restoreTransform); // restore world transform
}


QRectF TdropShadowEffect::boundingRectFor(const QRectF& sourceRect) const {
  qreal delta = (blurRadius() + distance()) * 2;
  return sourceRect.united(sourceRect.adjusted(-delta, -delta, 2 * delta, 2 * delta));
}




