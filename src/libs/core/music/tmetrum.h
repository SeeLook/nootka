/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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

#ifndef TMETRUM_H
#define TMETRUM_H

#include <nootkacoreglobal.h>
#include <QtCore/qxmlstream.h>
#include <QtGui/qpixmap.h>


/**
 * Describes musical time signature
 */
class NOOTKACORE_EXPORT Tmetrum
{

public:
	enum Emeter {
		e_none = 0,
		e_2_4 = 1, e_3_4 = 2, e_4_4 = 4, e_5_4 = 8, e_6_4 = 16, e_7_4 = 32,
    e_3_8 = 64, e_5_8 = 128, e_6_8 = 256, e_7_8 = 512, e_9_8 = 1024, e_12_8 = 2048
	};

	Tmetrum(Emeter meter = e_none) : m_meter(meter) {}

	Emeter meter() const { return m_meter; }

  int upper() const; /**< Returns upper digit of time signature  */
  int lower() const; /**< Returns lower digit of time signature  */

  qreal value() const; /**< Returns numeric value representing duration of single measure: 3/4 is 0.75, 4/4 is 1.0, etc. */

  QPixmap pixmap(int fontSize, const QColor& c = -1); /**< Returns QPixmap of this time signature with given font size and color. */

  void toXml(QXmlStreamWriter& xml) const;
  bool fromXml(QXmlStreamReader& xml);

  bool operator==(const Tmetrum& m) const { return meter() == m.meter(); }
  bool operator!=(const Tmetrum& m) const { return meter() != m.meter(); }

private:
	Emeter				m_meter;
};

#endif // TMETRUM_H
