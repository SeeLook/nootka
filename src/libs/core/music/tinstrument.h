/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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

#ifndef TINSTRUMENT_H
#define TINSTRUMENT_H


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


#define INSTR_COUNT (8) // number of instruments supported by Nootka


/**
 * Describes instrument
 */
class NOOTKACORE_EXPORT Tinstrument {

  Q_GADGET

  Q_PROPERTY(Etype type READ type WRITE setType)
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(int typeINT READ typeINT)
  Q_PROPERTY(QString glyph READ glyph)
  Q_PROPERTY(int clef READ clef)
  Q_PROPERTY(bool isGuitar READ isGuitar)
  Q_PROPERTY(bool isSax READ isSax)
  Q_PROPERTY(QString qmlFile READ qmlFile)
  Q_PROPERTY(int transposition READ transposition)
  Q_PROPERTY(int fretNumber READ fretNumber)
  Q_PROPERTY(bool isFadeOut READ isFadeOut)

public:

  enum Etype {
    NoInstrument = 0, // 0, however level and exam save it as 255 for backward comparability
    ClassicalGuitar = 1,
    ElectricGuitar = 2,
    BassGuitar = 3,
    Piano = 4,
    Bandoneon = 5,
    AltSax = 6,
    TenorSax = 7
  };
  Q_ENUM(Etype)

  Tinstrument(Etype type = NoInstrument);

  Etype type() const { return m_type; }
  Q_INVOKABLE void setType(Tinstrument::Etype t) { m_type = t; }
  int typeINT() const { return static_cast<int>(m_type); }

    /**
     * Translated name of an instrument.
     */
  QString name() const;

  QString static staticName(Etype t);

      /**
       * letter of instrument symbol (singer glyph for NoInstrument).
       */
  QString glyph() const;

      /**
       * Preferred clef for @p Etype of instrument
       */
  int clef() const;

  bool isGuitar() const { return m_type == ClassicalGuitar || m_type == ElectricGuitar || m_type == BassGuitar; }

  bool isSax() const { return m_type == AltSax || m_type == TenorSax; }

      /**
       * File implementing QML side of the instrument
       */
  QString qmlFile() const;

      /**
       * Default transposition of the instrument
       */
  int transposition() const;

      /**
       * Number of frets for guitars or null for other instruments
       */
  int fretNumber() const;

      /**
       * @p True for guitars and piano - instruments with sound that fades out.
       * In contrary the sound of saxophones, bandoneon, bowed strings, etc.
       * is continuous.
       * @p NoInstrument type is defined as continuous here as well.
       */
  bool isFadeOut() const;

      /**
       * Name of subdirectory in the 'levels' directory
       * with level *.nel files for actual instrument
       */
  QString levelsDir() const;

      /**
       * Returns desired instrument item height calculated from Nootka main window height
       */
  Q_INVOKABLE int getItemHeight(int mainWindowHeight);

private:
  Etype             m_type;

};


#endif //TINSTRUMENT_H

