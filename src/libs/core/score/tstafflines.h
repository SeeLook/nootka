/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#ifndef TSTAFFLINES_H
#define TSTAFFLINES_H


#include "nootkacoreglobal.h"
#include <QtQuick/qquickpainteditem.h>


/**
 * Paints five staff lines
 */
class NOOTKACORE_EXPORT TstaffLines : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(qreal staffScale READ staffScale WRITE setStaffScale)

public:
  explicit TstaffLines(QQuickItem* parent = nullptr);

  qreal staffScale() { return m_staffScale; }
  void setStaffScale(qreal stScale);

  void paint(QPainter* painter) override;

protected:
  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private:
  qreal                         m_staffScale = 1.0;
};

#endif // TSTAFFLINES_H
