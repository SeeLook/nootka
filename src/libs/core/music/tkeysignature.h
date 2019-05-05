/***************************************************************************
*   Copyright (C) 2011-2018 by Tomasz Bojczuk                             *
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


#ifndef TKEYSIGNATURE_H
#define TKEYSIGNATURE_H

#include <nootkacoreglobal.h>
#include <QtCore/qstring.h>
#include "tnote.h"


class QXmlStreamReader;
class QXmlStreamWriter;


/**
 * This class describes a key signature.
 * It keeps it in char type. Also it keeps info about mode (minor/major)
 */
class NOOTKACORE_EXPORT TkeySignature
{
public:
  TkeySignature();
  TkeySignature(char keyS, bool isMinor = false);

      /**
       * Lists of keys names
       */
  static QString majorNames[15], minorNames[15];

      /**
       * Array with definitions of each scale.
       * @p scalesDefArr[7] is C-major/a-minor and has 7 times 0 (no accids).
       * The order os from 7b [0] to 7# [15]
       */
  static const char scalesDefArr[15][7];

      /**
       * Those smart arrays keep which note in @p scalesDefArr is 1-st in key scale.
       * F.e. For G-major is @p majorKeys[8] = 4,
       * like @p scalesDefArr[8][4] point on accid of a G note.
       * This is faster way then calculate it and takes less memory.
       */
  static const char majorKeys[15], minorKeys[15];

      /**
       * This method fulfills majorNames and minorNames arrays
       * appropriate keys names depends on naming style
       * and suffixes.
       * Convention of suffixes is:
       * - empty string "" means - suffix id default for language and taken from translations
       * - string with space " " means - suffix is empty
       * - any other string means - it is as it is
       */
  static void setNameStyle(Tnote::EnameStyle style, const QString& majSuf = QString(), const QString& minSuf = QString());
  static QString majorSufixTxt() { return QObject::tr("major"); }
  static QString minorSufixTxt() { return QObject::tr("minor"); }

  static QString getMajorName(char key) { return majorNames[key + 7]; }
  static QString getMinorName(char key) { return minorNames[key + 7]; }
  QString getMajorName() const { return majorNames[m_key+7]; }
  QString getMinorName() const { return minorNames[m_key+7]; }

    /**
     * Returns minor or major name depend on m_isMinor state.
     */
  QString getName() const { return m_isMinor ? getMinorName() : getMajorName(); }

      /**
       * Return QString with accidentals number and their symbol.
       * f.e.: 7# for @p m_key = 7 (Cis-major).
       * @p inHtml points when HTML syntax is used. If true # and b symbols
       * are formatted to better view.
       */
  QString accidNumber(bool inHtml = false) const;

      /**
       * Checks is given note @p n in given key signature @p k.
       * In the method it checks all possibilities of the note n (flats, sharps, natural,
       * and returns either founded note or Tnote(0,0,0) if didn't find.
       */
  static Tnote inKey(TkeySignature k, const Tnote& n);
  Tnote inKey(const Tnote& n) const;
  bool isMinor() const { return m_isMinor; }
  void setMinor(bool isMinor) { m_isMinor = isMinor; }

  char value() const { return m_key; }

    /**
     * Returns base (tonic) note of this key signature, i.e. for a-minor key it is note 'a'.
     * By default note is in small octave, but it can be set through @p octave parameter.
     */
  Tnote tonicNote(int octave = 0) const;

    /**
     * Adds 'key' key to XML stream compatible with MusicXML format with current key signature
     * <key>
     * <fifths>-2</fifths>
     * <mode>major</mode>
     * </key>  
     */
  void toXml(QXmlStreamWriter& xml);

      /**
       * Reads this key signature from XML stream
       */
  void fromXml(QXmlStreamReader& xml);

  bool operator==(const TkeySignature& k) const { return m_key == k.value() && m_isMinor == k.isMinor(); }
  bool operator!=(const TkeySignature& k) const { return m_key != k.value() || m_isMinor != k.isMinor(); }

      /**
       * Returns difference in semitones between @p otherKey and this key signature.
       * It compares major variants of both keys to get proper difference for transposition
       */
  int difference(const TkeySignature& otherKey) const;

private:
  char m_key;
  bool m_isMinor;

      /**
       * Common part for static and no static inKey() methods.
       */
  static Tnote inKeyPrivate(char val, const Tnote& n);
};


NOOTKACORE_EXPORT QDataStream &operator<< (QDataStream &out, TkeySignature &key);

    /**
     * This function is substitute of >> operator for @p TkeySignature.
     * It checks is Tnote valid, and return @value bool about it.
     */
NOOTKACORE_EXPORT bool getKeyFromStream(QDataStream &in, TkeySignature &k);


#endif // TKEYSIGNATURE_H

