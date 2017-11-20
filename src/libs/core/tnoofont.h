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

#ifndef TNOOFONT_H
#define TNOOFONT_H

#include <nootkacoreglobal.h>
#include <QtGui/qfont.h>


/**
 * This is QFont with nootka.ttf initialized with size of 20 pixels
 * It has many helpers to quickly generate HTML spans with nootka as font and so
 */
class NOOTKACORE_EXPORT TnooFont : public QFont
{

public:
  TnooFont(int pointSize = 20);

      /**
       * Returns given text wrapped with given HTML tag and:
       * - font size (if set)
       * - extra Style (if set)
       * like:
       * <tag style="font-family: nootka; extraStyle; font-size: XXpx;">text</tag>
       */
  static QString tag(const QString& tag, const QString& text, int fontSize = 0, const QString& extraStyle = QString());

      /**
       * tag() method with span tag
       */
  static QString span(const QString& text, int fontSize = 0, const QString& extraStyle = QString()) {
                  return tag(QLatin1String("span"), text, fontSize, extraStyle); }

      /**
       * Bare digits starts from UNI-0x180 in nootka.ttf
       * due to glyphs of ordinary numbers are used for strings (circled).
       * This method returns proper string of given digit
       */
  static QString digit(quint8 d) { return (d / 10 ? QString(QChar(0x0180 + d / 10)) : QString()) + QString(QChar(0x0180 + d % 10)); }

      /**
       * Converts rhythm value (0, 1, 2, 4 - 16) into uni-code char number in Nootka font
       * 0 (no rhythm returns full note head symbol)
       * set @p stemUp to false to get symbols with stem down.
       * Returned characters are optimized for score (staff lines height),
       * to get just note symbol use 'n' and 'N' (stem down) instead
       */
  static quint16 getCharFromRhythm(quint16 rhythm, bool stemUp = true, bool rest = false);

};

#endif // TNOOFONT_H
