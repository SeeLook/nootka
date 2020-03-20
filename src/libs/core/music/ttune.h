/***************************************************************************
 *   Copyright (C) 2006-2020 by Tomasz Bojczuk                             *
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

#ifndef TTUNE_H
#define TTUNE_H

#include <nootkacoreglobal.h>
#include "tnote.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/qobject.h>


/**
 * It stores six notes of Tnote objects, which can be set
 * in constructor and get by overloaded [] operator.
*/
class NOOTKACORE_EXPORT Ttune
{

  Q_GADGET

  Q_PROPERTY(QString name READ tuningName)
  Q_PROPERTY(int stringNumber READ stringNr)

public:

  enum Etunings : qint8 {
    NoTuning = -100, /**< Undefined - initial state. */
    Scale = -2, /**< Instrument scale - only first two strings points lower and higher notes */
    Custom = -1, /**< User defined tuning */
    Standard_EADGBE = 0,
    Dropped_D_DADGBE = 1,
    DummyLute_DADFshBE = 2,
    Open_DADGAD = 3,
    Kouyanbaba_DADADF = 4,

    Bass4_EADG = 100,
    Bass4_5ths_CGDA = 101,
    Bass5_BEADG = 102,
    Bass6_BEADGC = 103
  };
  Q_ENUM(Etunings)

      /**
       * @p tuneName is the name, @p S(1-6) are notes.
       * Empty notes (Tnote()) can control strings number
       * when empty - it is moved to the end of a array and stringNr() is less.
       * This way only a number of string [from 1 to 6] is supported.
       */
  Ttune(const QString& tuneName = QString(), const Tnote& S1 = Tnote() , const Tnote& S2 = Tnote(),
          const Tnote& S3 = Tnote(), const Tnote& S4 = Tnote(), const Tnote& S5 = Tnote(), const Tnote& S6 = Tnote(), Etunings tunType = NoTuning);


  QString name; /**< It is a name of the tune */

  QString tuningName() const { return name; }

      /**
       * Number of strings for current tune/guitar
       */
  Q_INVOKABLE quint8 stringNr() const { return m_strNumber; }

      /**
       * When tune has less than 3 strings and "scale" as a name it represents a scale of an instrument
       * and it is not guitar.
       */
  bool isGuitar() const { return m_strNumber > 2; }

  Etunings type() const { return p_tuning; }

      /**
       * Substitute of [] operator - returns note of given string.
       */
  Q_INVOKABLE Tnote str(quint8 stringNr) const { return stringsArray[stringNr - 1]; }

      /**
       * Chromatic number of a given @p stringNr string
       */
  Q_INVOKABLE int strChromatic(int stringNr) const { return static_cast<int>(str(stringNr).chromatic()); }

  static Ttune stdTune; /**< standard EADGBE tuning template */
  static Ttune tunes[4]; /**< templates for guitar tuning */

      /**
       * Templates for bass guitar tuning
       * As long as bass guitar transposes one octave down
       * keep in mind that those string notes are not transposed.
       */
  static Ttune bassTunes[4];

      /**
       * Copies given tuning to this one.
       */
  void copy(Ttune& t);

  static QString definedName(Etunings t);

      /**
       * This method simply rises tuning one octave up.
       * The ONLY ONE PURPOSE of this method is to convert/fix
       * bass dropped down clef of old Nootka to ordinary bass clef.
       * It automatically detects tuning type @p Etunings
       */
  void riseOctaveUp();

      /**
       * Returns tuning type form given tuning @p t
       */
  static Etunings findTuning(const Ttune& t);

      /**
       * Makes translations in defined tunes. Has to be invoked once per app launch
       */
  static void prepareDefinedTunes();

  friend QDataStream &operator<< (QDataStream &out, const Ttune &t);
  friend QDataStream &operator>> (QDataStream &in, Ttune &t);

      /** 
       * Method responses for converting tuning to XML structure..
       * When @p isExam is @p TRUE it is wrapped in <tuning id="0"> tag.
       * Attribute @p id determining kind of tune.
       * Only when tuning is different than all defined tuning suitable XML elements are written.
       * When @p isExam is @p FALSE <staff-details> is a main key and all elements are saved.
       */
  void toXml(QXmlStreamWriter& xml, bool isExam = true);
  bool fromXml(QXmlStreamReader& xml, bool isExam = true);

      /**
       * Overloaded operator [] allows to use statement
       * @li Ttune @p your_variable[number_of_a_string]
       * @p stringNr is real string number (1 to 6)
       */
  Tnote &operator[] (quint8 stringNr) { return stringsArray[stringNr - 1]; }
  bool operator== (Ttune& T2) const {
      return ( stringsArray[0] == T2[1] && stringsArray[1] == T2[2] && stringsArray[2] == T2[3] &&
                stringsArray[3] == T2[4] && stringsArray[4] == T2[5] && stringsArray[5] == T2[6] );
  }
  bool operator!= (Ttune& T2) const {
      return ( stringsArray[0] != T2[1] || stringsArray[1] != T2[2] || stringsArray[2] != T2[3] ||
              stringsArray[3] != T2[4] || stringsArray[4] != T2[5] || stringsArray[5] != T2[6] );
  }

protected:
  Tnote stringsArray[6]; /**< Array of Tnote that represent six strings */

      /**
       * This method is called by constructor and operator.
       * It calculates number of strings by selecting string with defined notes
       * and moving empty ones to the end of stringsArray.
       * THIS IS ONLY WAY TO MANAGE STRINGS NUMBER.
       */
  void determineStringsNumber();

  Etunings       p_tuning;

private:
  quint8         m_strNumber;

};

Q_DECLARE_METATYPE(Ttune)


NOOTKACORE_EXPORT QDataStream &operator<< (QDataStream &out, const Ttune &t);
NOOTKACORE_EXPORT QDataStream &operator>> (QDataStream &in, Ttune &t);

#endif
