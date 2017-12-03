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

/** Glyphs (some)
 * Rests:
 * 0xe102 - whole rest
 * 0xe103 - half rest
 * 0xe108 - quarter rest
 * 0xe10A - eight rest
 * 0xe10B - sixteen rest
 *
 * Flags
 * 0xe21C - eight Up
 * 0xe21D - sixteen Up
 * 0xe221 - eight Down
 * 0xe221 - sixteen Down
 *
 * Note heads:
 * 0xe191 - whole (empty but big)
 * 0xe192 - half (empty)
 * 0xe193 - quarter and above (full)
 * Note symbols (head, stem and flag)
 * 0x0043 - 'C' - whole - head only
 * 0x0044 - 'D' - half (stem up)
 * ... and so on till
 * 0x0047 - 'G' - sixteen
 * stems down starts from
 * 0x004A - 'J' - half - (stem down)
 * ... till 0x004D - 'M' - sixteen
 *
 * Accidentals (all are centered in glyph height but those available by letters: B b # x are placed at a bottom)
 * 0xe123 - double flat (also B)
 * 0xe11a - flat (also b)
 * 0xe10e - sharp (also #)
 * 0xe125 - double sharp (also x)
 * 0xe116
 *
 * digits [0-9] starts from 0x0180
 */


#include "tnoofont.h"
#include <QtCore/qmath.h>


TnooFont::TnooFont(int pointSize) :
  QFont(QStringLiteral("nootka"), pointSize)
{
  setPixelSize(pointSize);
  setBold(false);
  setWeight(50); // Normal
}

//#################################################################################################
//###################              STATIC              ############################################
//#################################################################################################
QString TnooFont::tag(const QString& tag, const QString& text, int fontSize, const QString& extraStyle) {
  QString fSize;
  if (fontSize)
      fSize = QString("font-size: %1px;").arg(fontSize);
  QString ex = extraStyle;
  if (!extraStyle.isEmpty() && !extraStyle.endsWith(QLatin1String(";")))
      ex = extraStyle + QLatin1String(";");
  return QLatin1String("<") + tag + QLatin1String(" style=\"font-family: nootka;") + fSize + ex + QLatin1String("\">")
          + text + QLatin1String("</") + tag + QLatin1String(">");
}


quint16 TnooFont::getCharFromRhythm(quint16 rhythm, bool stemUp, bool rest) {
  int baseChar = 67, stemGap = 0;
  if (rest)
      baseChar = 0xe107;
  else if (!stemUp && rhythm > 1) // stem down only if no rest and half note at least
      stemGap = 6;
  if (rhythm)
    return baseChar + qRound(qLn(static_cast<qreal>(rhythm) / qLn(2.0))) + stemGap;
  else
    return 0xe193;
}






