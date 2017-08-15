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

#ifndef TTICKCOLORS_H
#define TTICKCOLORS_H


#include "nootkasoundglobal.h"
#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtCore/QDebug>


/**
 * @class TtickColors generates list of gradient colors in a line of declared @p width().
 * Number of ticks is obtained by @p width() divided by @p divisor().
 * Then gradient values of @p startColor -> @p middleColor -> @p endColor -> @p totalColor
 * are stored in the list accessible by @p colorAt(tickNr)
 */
class NOOTKASOUND_EXPORT TtickColors : public QObject
{
  Q_OBJECT

  Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(qreal divisor READ divisor WRITE setDivisor NOTIFY divisorChanged)

public:
  TtickColors(QObject* parent = nullptr);

  qreal width() const { return m_width; }
  void setWidth(qreal w);

      /**
       * This is the sum of tick width and  gap distance in pixels
       */
  qreal divisor() const { return m_divisor; }
  void setDivisor(qreal d);

  void resize(qreal w);

  Q_INVOKABLE QColor colorAt(int nr) {
    if (nr < 0 || nr > m_tickColors.count() - 1) {
      qDebug() << "[TtickColors] WRONG COLOR NUMBER!" << nr;
      return QColor();
    }
    return m_tickColors[qBound(0, nr, m_tickColors.count() - 1)];
  }

      /**
       * Calculates color at point @p posC on the line of X points from @p lineX1 to @p lineX2
       * of gradient with colors @p startC to @p endColor. @p posC has to be inside @p lineX1 and @p lineX2
       */
  QColor gradColorAtPoint(qreal lineX1, qreal lineX2, const QColor& startC, const QColor& endC, qreal posC);

  static QColor startColor, middleColor, endColor, totalColor, disabledColor;

signals:
  void widthChanged();
  void divisorChanged();

private:
  QList<QColor>   m_tickColors;
  qreal           m_width;
  qreal           m_divisor;
};

#endif
