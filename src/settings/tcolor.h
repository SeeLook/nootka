/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TCOLOR_H
#define TCOLOR_H

#include <QColor>

/**
 * It subclass QColor and adds to it some tweaks 
 */
class Tcolor : public QColor
{
public:
  Tcolor();
  Tcolor(Qt::GlobalColor color);
  Tcolor(int r, int g, int b, int a);
  Tcolor(QRgb rgb);
  Tcolor(const QString& name);
  Tcolor(const char* name);
  Tcolor(const QColor& color);
  Tcolor(QColor::Spec spec);

  QColor merge(const QColor& otherColor);
  QColor invert();
  QString argbText();
};

Tcolor::Tcolor() : QColor() {}
Tcolor::Tcolor(Qt::GlobalColor color) : QColor(color) {}
Tcolor::Tcolor(int r, int g, int b, int a) : QColor(r, g, b, a) {}
Tcolor::Tcolor(QRgb rgb) : QColor(rgb) {}
Tcolor::Tcolor(QColor::Spec spec) : QColor(spec) {}
Tcolor::Tcolor(const QString& name) : QColor(name) {}
Tcolor::Tcolor(const char* name) : QColor(name) {}
Tcolor::Tcolor(const QColor& color) : QColor(color) {}




#endif // TCOLOR_H
