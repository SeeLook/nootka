/***************************************************************************
 *   Copyright (C) 2019-2021 by Tomasz Bojczuk                             *
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

#ifndef TPIECHARTITEM_H
#define TPIECHARTITEM_H


#include <QtQuick/qquickpainteditem.h>


class QPainter;


/**
 * Paints pie chart with slices representing integer values of @p values() list
 */
class TpieChartItem : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(QList<int> values READ values WRITE setValues NOTIFY valuesChanged)
  Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors NOTIFY colorsChanged)

public:
  explicit TpieChartItem(QQuickItem* parent = nullptr);

  void paint(QPainter* painter) override;

      /**
       * List of values.
       * All values are summarized and every slice is proportional to the sum.
       * Null values are ignored during @p paint(),
       * so corresponding colors will be displayed correctly.
       */
  QList<int> values() { return m_valueList; }
  void setValues(QList<int> valList);

      /**
       * Colors list (array) defines color for every slice of the pie.
       * But only if it is set,
       * if not, then darkest variant of the last color in the list is taken.
       * When list is empty, @p Qt::red is used by default.
       * In other words:
       * one can set color for every value in the list,
       * or just set a single color - it darker variants will be used for more values
       * or just live color list empty - red one will be used.
       */
  QList<QColor> colors() const { return m_colors; }
  void setColors(const QList<QColor>& cl);

signals:
  void valuesChanged();
  void colorsChanged();

private:
  QList<int>                   m_valueList;
  qreal                        m_sum = 0.0;
  QList<QColor>                m_colors;

};

#endif // TPIECHARTITEM_H
