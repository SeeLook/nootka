/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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

#ifndef TQAGROUP_H
#define TQAGROUP_H


#include "nootkacoreglobal.h"
#include "music/ttechnical.h"
#include "music/tnote.h"


/**
 * This is simple class with
 * @p note (Tnote)
 * and technical information:
 * @li - fret, string through @p pos() returning @p TfingerPos
 * @li - bowing, fingering through @p technical() returning @p Ttechnical
 */
class NOOTKACORE_EXPORT TQAgroup
{

public:

  TfingerPos& pos() { return technical.fingerPos(); }

  Ttechnical technical;
  Tnote note;

      /**
       * Save @p TQAgroup to XML. (not valid) note and position are skipped.
       */
  void toXml(QXmlStreamWriter& xml, const QString& tag = QLatin1String("qa"));
  bool fromXml(QXmlStreamReader& xml);
};


#endif // TQAGROUP_H
