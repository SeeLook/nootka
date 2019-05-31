/***************************************************************************
 *   Copyright (C) 2019 by Tomasz Bojczuk                                  *
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
 * Paints pie chart with slices representing integer values of @p kindOfMistakes() list
 */
class TpieChartItem : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(QList<int> values READ values WRITE setValues NOTIFY valuesChanged)

public:
  explicit TpieChartItem(QQuickItem* parent = nullptr);

  void paint(QPainter* painter) override;

  QList<int> values() { return m_valueList; }
  void setValues(QList<int> valList);

signals:
  void valuesChanged();

private:
  QList<int>                   m_valueList;
  qreal                        m_sum = 0.0;

};

#endif // TPIECHARTITEM_H
