/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


class QXmlStreamReader;
class QXmlStreamWriter;


class NOOTKACORE_EXPORT Tclef
{
  Q_GADGET

  Q_PROPERTY(EclefType type READ type WRITE setClef)

public:
  enum EclefType {
    NoClef = 0,             /**< clef not defined or neutral clef (no pitch) */
    Treble_G = 1,           /**< common treble clef */
    Bass_F = 2,             /**< bass clef */
    Alto_C = 4,
    Treble_G_8down = 8,     /**< treble clef with "8" digit below (guitar) */
    Bass_F_8down = 16,      /**< bass clef with "8" digit below (bass guitar) */
    Tenor_C = 32,
    PianoStaffClefs = 128   /**< exactly it is not a clef */
  };

  Q_ENUM(EclefType)

  Tclef(EclefType type = Treble_G) : m_type(type) {}
  Tclef(const Tclef& other) { m_type = other.type(); }
  ~Tclef() {}

  EclefType type() const { return m_type; }
  void setClef(EclefType type) { m_type = type; }

  Q_INVOKABLE QString name() const; /**< short name of a clef */
  Q_INVOKABLE QString desc() const; /**< a clef description */
  Q_INVOKABLE QString glyph() const;
  Q_INVOKABLE QString glyphOnStaff() const; /**< Clef symbol on the staff */

     /**
      * Adds 'clef' key to XML stream compatible with MusicXML format with current clef
      * <clef>
      * <sign>G</sign>
      * <line>2</line>
      * <clef-octave-change>-1</clef-octave-change>
      * </clef>
      */
void toXml(QXmlStreamWriter& xml);
void fromXml(QXmlStreamReader& xml); /** Reads this clef from XML stream  */

static EclefType defaultType; /** Default clef type for whole application */


private:
  EclefType m_type;

};

#endif // TCLEF_H
