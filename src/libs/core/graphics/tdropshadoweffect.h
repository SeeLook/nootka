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


#ifndef TDROPSHADOWEFFECT_H
#define TDROPSHADOWEFFECT_H

#include <nootkacoreglobal.h>
#include <QGraphicsEffect>

/**
 * Shadow effect used for all tips.
 * Base idea was taken from: http://stackoverflow.com/questions/23718827/qt-shadow-around-window
*/
class NOOTKACORE_EXPORT TdropShadowEffect : public QGraphicsEffect
{

public:
  TdropShadowEffect(const QColor& color = -1);

  void setDistance(qreal distance) { m_distance = distance; updateBoundingRect(); }
  qreal distance() const { return m_distance; }

  void setBlurRadius(qreal blurRadius) { m_blurRadius = blurRadius; updateBoundingRect(); }
  qreal blurRadius() const { return m_blurRadius; }

  void setColor(const QColor& color) { m_color = color; m_color.setAlpha(200); }
  QColor& color() { return m_color; }

protected:
  virtual void draw(QPainter* painter);
  virtual QRectF boundingRectFor(const QRectF& sourceRect) const;

private:
  qreal  m_distance;
  qreal  m_blurRadius;
  QColor m_color;
};

#endif // TDROPSHADOWEFFECT_H
