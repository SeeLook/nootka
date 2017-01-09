/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#ifndef TCLEF_H
#define TCLEF_H

#include "../scorekglobal.h"
#include <QString>

class QXmlStreamReader;
class QXmlStreamWriter;


class SCOREK_EXPORT Tclef
{

public:
    enum Etype {
      e_none = 0, // clef not defined
      e_treble_G = 1, // common treble clef
      e_bass_F = 2, // bass clef
      e_alto_C = 4,
      e_treble_G_8down = 8, // treble clef with "8" digit below (guitar)
      e_bass_F_8down = 16, // bass clef with "8" digit below (bass guitar)
      e_tenor_C = 32,
      e_pianoStaff = 128 // exactly is not a clef
    };

    Tclef(Etype type = e_treble_G);

    Etype type() { return m_type; }
    void setClef(Etype type) { m_type = type; }

    QString name(); // short name of a clef
    QString desc(); // a clef description

    /** Adds 'clef' key to XML stream compatible with MusicXML format with current clef
       * <clef>
       * <sign>G</sign>
       * <line>2</line>
       * <clef-octave-change>-1</clef-octave-change>
       * </clef>  */
  void toXml(QXmlStreamWriter& xml);
  void fromXml(QXmlStreamReader& xml); /** Reads this clef from XML stream  */

  static Etype defaultType; /** Default clef type for whole application */


private:
    Etype m_type;

};


#endif // TCLEF_H
