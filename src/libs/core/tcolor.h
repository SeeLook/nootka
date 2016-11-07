/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TCOLOR_H
#define TCOLOR_H

#include <nootkacoreglobal.h>
#include <QColor>
#include <QWidget>

/**
 * A set of static methods with some tweaks of color:
 * - color inversion by invert()
 * - merge current color with another merge(QColor ,QColor)
 * - returns QString with CSS style tag of rgba
 * - shadow() returns shadow color of tips for current palette and underlying Os
 */
class NOOTKACORE_EXPORT Tcolor
{

public:
                      /** Merge given colors. */
  static QColor merge(const QColor& C1, const QColor& C2) {
  qreal al = iV(C1.alpha()) + iV(C2.alpha() * (1 - iV(C1.alpha())));
  return QColor(((iV(C1.red()) * iV(C1.alpha()) + iV(C2.red()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                ((iV(C1.green()) * iV(C1.alpha()) + iV(C2.green()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                ((iV(C1.blue()) * iV(C1.alpha()) + iV(C2.blue()) * iV(C2.alpha()) * (1 - iV(C1.alpha()))) / al) * 255,
                qMin(255, (int)(255 * al)));
}

                    /** Returns inverted copy of given color */
  static QColor invert(const QColor& color) {
          QColor C = color;
          if (C.isValid())
              C.setRgb(qRgb(255 - C.red(), 255 - C.green(), 255 - C.blue()));
          return C;
      }

                    /** Returns css style tag with rgba values of color.
                    *
                    * @p styleTag rgba(red, green, blue, alpha);
                    */
  static QString rgbaText(const QColor& color, const QString& styleTag = "") {
            return QString(styleTag + "rgba(%1, %2, %3, %4);").
                  arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
          }

                    /** Returns
                    * background-color: rgba(red, green, blue, alpha) */
  static QString bgTag(const QColor& color) {
            if (color != -1 && color.alpha() > 0)
                return rgbaText(color, "background-color:");
            else
                return QString("background-color: transparent; ");
          }

                /** Converts value (0 - 255) to (0.0 - 1.0)  */
  static qreal iV(int ch) { return ch / 255.0; }

  static QColor shadow;

                /** Sets default shadow color of tips */
  static void setShadow(const QPalette& pal);

};



#endif // TCOLOR_H
