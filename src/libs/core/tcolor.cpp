/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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

#include "tcolor.h"
#include <QtGui/qpalette.h>


QColor Tcolor::shadow = Qt::gray;


void Tcolor::setShadow(const QPalette& pal) {
#if defined(Q_OS_MAC)
    QColor shadowC(pal.text().color());
    shadowC.setAlpha(50);
    shadow = merge(shadowC, pal.base().color());
#else
    shadow = pal.shadow().color();
#endif
}


QColor Tcolor::merge(const QColor& C1, const QColor& C2) {
  qreal al = iV(C1.alpha()) + iV(C2.alpha() * (1 - iV(C1.alpha())));
  return QColor(((iV(C1.red()) * iV(C1.alpha()) + iV(C2.red()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                ((iV(C1.green()) * iV(C1.alpha()) + iV(C2.green()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                ((iV(C1.blue()) * iV(C1.alpha()) + iV(C2.blue()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                qMin(255, (int)(255 * al)));
}


QColor Tcolor::invert(const QColor& color) {
  QColor C = color;
  if (C.isValid())
      C.setRgb(qRgb(255 - C.red(), 255 - C.green(), 255 - C.blue()));
  return C;
}


QString Tcolor::rgbaText(const QColor& color, const QString& styleTag) {
  return QString(styleTag + "rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}


QString Tcolor::bgTag(const QColor& color) {
  if (color != -1 && color.alpha() > 0)
      return rgbaText(color, "background-color:");
  else
      return QString("background-color: transparent; ");
}



