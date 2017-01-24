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


#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtCore/QDebug>


class TtickColors : public QObject
{
  Q_OBJECT

public:
  TtickColors(QObject* parent = nullptr);

  Q_INVOKABLE void resize(qreal w);

  Q_INVOKABLE QColor colorAt(int nr) {
    return m_tickColors[qBound(0, nr, m_tickColors.count() - 1)];
    if (nr < 0 || nr > m_tickColors.count() - 1)
      qDebug() << "wrong color number" << nr;
  }

  QColor gradColorAtPoint(float lineX1, float lineX2, QColor startC, QColor endC, float posC);

  static QColor startColor, middleColor, endColor, totalColor, disabledColor;

private:
  QList<QColor>   m_tickColors;
};

#endif
