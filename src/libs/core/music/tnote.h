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

#ifndef TNOTE_H
#define TNOTE_H


#include <nootkacoreglobal.h>
#include "trhythm.h"

#include <string>
#include <vector>
#include <QtCore/qdatastream.h>
#include <QtCore/qxmlstream.h>


class Tnote;

typedef std::vector<Tnote> TnotesList;

#define ALTER_BIT_MASK (7)
#define ON_UPPER_BIT_MASK (128)


/**
 * This class provides the descriptions of musical.
 * It helps to convert the letter notation (C D E ...) to solfege (Do Re Mi).
 * It supports accidentals (sharps, flats, double sharps and double flats.
 *
 * Also it stores position on the grand staff (upper/lower staff line) through @p onUpperStaff()
 * Operators == and != compares two notes (their note number, octave and accidental, but not staff position)
 * @p compareNotes() method can skip octaves check
 */
class NOOTKACORE_EXPORT Tnote
{

public:

        /**
         * Enumeration type describes the styles of notation
         */
  enum EnameStyle {
    e_norsk_Hb = 0,     /**< for letters with signs f.e. C# Cx or Cb    !! THIS IS DEFAULT !! */
    e_deutsch_His = 1,  /**< for letters with names f.e. Cis Cisis or Ces H and B (H with flat) */
    e_italiano_Si = 2,  /**< for classical Do Re Mi Fa Sol La Si */
    e_english_Bb = 3,   /**<like @p e_norsk_Hb but with B and Bb (B flat) */
    e_nederl_Bis = 4,   /**< like @p e_deutsch_His but with B ens Bes */
    e_russian_Ci = 5    /**< classical but in Russian: До Ре Ми Фа Соль Ля Си */
  };

        /**
         * @p Ealter enumeration type describes all signs which can be before note in score.
         * It can be: @li e_None = 3 @li e_Sharp = 1 @li e_DoubleSharp=2
         * @li e_Flat= -1 @li e_DoubleFlat= -2 @li e_Natural=0
         */
  enum Ealter : char {
    e_Natural = 0, e_Sharp = 1, e_DoubleSharp = 2, e_Flat = -1, e_DoubleFlat = -2, e_None = 3
  };

        /**
         * note (step) is a number in "diatonic notation" (see constructor).
         */
  inline char note() const { return p_note; }
  inline void setNote(char n) { p_note = n; }

        /**
         * Octave number is @p 0 for "small octave",  @p -1 for "Great" @p 1 for "one-line".
         */
  inline char octave() const { return p_octave; }
  inline void setOctave(char o) { p_octave = o; }

        /**
         * @p accidental means raising or dropping a note, so it ca be:
         * @li 2 for double sharp (x)
         * @li 1 for sharp (#)
         * @li 0 for natural
         * @li -1 for flat (b)
         * @li -2 for double flat (bb)
         */
  inline char alter() const { return (p_alterValue & ALTER_BIT_MASK) - 3; }

      /**
       * @p a HAS TO BE in range [-3, 3] (even if value -3 makes no sense)
       * otherwise it may overwrite @p onUpperStaff() state
       */
  inline void setAlter(char a) { p_alterValue &= ~ALTER_BIT_MASK; p_alterValue |= a + 3; }

  inline bool onUpperStaff() const { return !(p_alterValue & ON_UPPER_BIT_MASK); }
  inline void setOnUpperStaff(bool onUp) {
   if (onUp)
     p_alterValue &= ~128;
   else
     p_alterValue |= 128;
  }

      /**
       * returns std string representing letter of accidental (alter) symbol
       */
  static std::string alterSymbol(int id);

        /**
         * Construct object of Tnote from number of note, number of octave
         * and optionally accidental and rhythm (none by default).
         * The note number is:
         * @li "1" for C
         * @li "2" for D1
         * @li .......
         * @li 7 for B (H in Deutsh)
         * If accidental is not defined, the note is natural.
         */
  Tnote(char diatonNote, char oct, char accid = 0, const Trhythm& r = Trhythm(Trhythm::NoRhythm)) :
      rtm(r),
      p_note(diatonNote),
      p_octave(oct)
    {
      setAlter(accid);
    }

        /**
         * The simple constructor, creates the note instance with 0 note and no rhythm.
         * It makes no sense in musical notation. It's needed for vectors.
         */
  Tnote() : rtm(Trhythm(Trhythm::NoRhythm)) {}

        /**
         * Construct object of Tnote from number, that represents:
         * @li "1" for C1 in first octave
         * @li "2" for Cis
         * @li .......
         * @li 13 for C2 in next octave
         * @li -12 for C in little octave etc....
         * The sharp accidental is default. If other are needed, use @p showWithFlat().
         * Sets rhythm to @p r
         */
  Tnote(short chromaticNrOfNote, const Trhythm& r = Trhythm(Trhythm::NoRhythm)) :
    rtm(r)
  {
    setChromatic(chromaticNrOfNote);
  }

      /**
       * Constructs @class Tnote from other Tnote but with different rhythm @p r
       */
  Tnote(const Tnote& other, const Trhythm& r) :
    rtm(r),
    p_note(other.note()),
    p_octave(other.octave()),
    p_alterValue(other.p_alterValue)
  {}


  Trhythm rtm; /**< Easy access to rhythm object */
  Trhythm::Erhythm rhythm() const { return rtm.rhythm(); }

      /**
       * Sets rhythm parameters, Resets all previous values!
       */
  void setRhythm(Trhythm::Erhythm r, bool rest = false, bool dot = false, bool triplet = false) {
    rtm.setRhythm(r, rest, dot, triplet);
  }

  void setRhythm(const Trhythm& r) { rtm.setRhythm(r); }

      /**
       * Converts given value into rhythm
       */
  void setRhythm(quint16 durationValue) { rtm.setRhythm(durationValue); }

      /**
       * Changes rhythmic value only, state of dot, triplet, beams remains unchanged
       */
  void setRhythmValue(Trhythm::Erhythm nVal) { rtm.setRhythmValue(nVal); }

      /**
       * It converts std::string into rhythm value. Doesn't change state of triplet, dot or beam.
       */
  void setRhythmValue(const std::string& nVal) { rtm.setRhythmValue(nVal); }

      /**
       * Rhythm value cast to int: i.e. quarter is 4, half is 2 and so on
       */
  int weight() const { return rtm.weight(); }

      /**
       * Whole note is 96, half is 48, quarter is 24 and with dot is 36. Single eight triplet is 8.
       * Base value is defined in @p RVALUE macro
       */
  int duration() const { return rtm.duration(); }
  bool isRest() const { return rtm.isRest(); }
  void setRest(bool rest) { rtm.setRest(rest); }
  bool hasDot() const { return rtm.hasDot(); }
  void setDot(bool dot) { rtm.setDot(dot); }
  bool isTriplet() const { return rtm.isTriplet(); }
  void setTriplet(bool tri) { rtm.setTriplet(tri); }

      /*
       * Returns @p TRUE when note is valid. There are used 'undefined' notes with 0 - they are invalid.
       */
  bool isValid() const { return (note() > 0 && note() < 8); }

      /**
       * Static value determines default name style for a note
       */
  static EnameStyle defaultStyle;

      /**
       * Determines whether to use International Pitch Notation,
       * when octaves are numbered from 0 (-3 here, sub contra octave).
       * This is global static switch, a change influences any further note name.
       * By default it is @p FALSE
       */
  static bool scientificOctaves;

  bool operator==(const Tnote& N2) const {
    return (note() == N2.note() && octave() == N2.octave() && p_alterValue == N2.p_alterValue && rtm == N2.rtm);
  }

  bool operator!=(const Tnote& N2) const {
    return ( note() != N2.note() || octave() != N2.octave() || p_alterValue != N2.p_alterValue || rtm != N2.rtm);
  }

        /**
         * Splits current note on two given rhythmic values
         */
  Tnote split(const Trhythm& r1, const Trhythm& r2) {
    setRhythm(r1);
    return Tnote(note(), octave(), p_alterValue, r2);
  }

        /**
         * @return List of Tnote objects, which are the same (in sound sense),
         * like note represents by class. (C# = Db, or Cx = D = Ebb)
         * @param enableDbAccids if @p TRUE - checks substitutes with double accidentals
         */
  TnotesList getTheSameNotes (bool enableDbAccids) const;

      /** Return this note converted into double-sharp or the same note if not possible. */
  Tnote showWithDoubleSharp() const;

      /** Return this note converted into sharp or the same note if not possible. */
  Tnote showWithSharp() const;

      /** It returns Tnote object with no accidentals, if it's possible, or with sharps if not.  */
  Tnote showAsNatural() const;

      /** Return this note converted into flat or the same note if not possible. */
  Tnote showWithFlat() const;

      /** Return this note converted into double-flat or the same note if not possible. */
  Tnote showWithDoubleFlat() const;

      /**
        * This method compares actual note, with otherNote @p otherNote.
        * @p ignoreOctave, if @p TRUE - the octave values are ignored,
        * and method compares only number of note and accidental.
        *
        * In contrary to == operator, it also ignores @p onUpperStaff() comparison
        */
  bool compareNotes(const Tnote& otherNote, bool ignoreOctave = false) const {
    return note() == otherNote.note() && alter() == otherNote.alter() && (ignoreOctave || octave() == otherNote.octave());
  }

  std::string getName(EnameStyle notation = e_norsk_Hb, bool showOctave = 1) const;

        /** Returns note name converted to QString */
  QString toText (EnameStyle notation, bool showOctave = true) const {
    return QString::fromUtf8(getName(notation, showOctave).data());
  }

      /** Returns note name converted to @p QString */
  QString toText (bool showOctave = true) const { return toText(defaultStyle, showOctave); }

        /** DEPRECATED
         * Returns note name formatted to HTML
         * NOTE: Since QML port, it is note recommended, prefer @p styledName() instead
         */
  QString toRichText(EnameStyle notation, bool showOctave = true) const;

        /** DEPRECATED
         * Returns note name formatted to HTML in default name style sets by @p defaultStyle.
         * NOTE: Since QML port, it is note recommended, prefer @p styledName() instead
         */
  QString toRichText(bool showOctave = true) const { return toRichText(defaultStyle, showOctave); }

      /**
       * Returns name of the current note with glyphs of Scorek font.
       * It doesn't use any tags for better QML performance.
       */
  QString styledName(bool showOctave = true)const;

  short chromatic() const; /**< Returns chromatic number of note f.e. C1 is 60 */
  void setChromatic(short noteNr); /**< Determines note, octave and accidental from chromatic value. */

      /**
       * Adds given @p tag or 'pitch' key to XML stream compatible with MusicXML format with current note
       * Following elements can be prefixed with @p prefix (it is used i.e. to tuning in MusicXML)
       * If @p attr and its @p val is set the attribute is added
       * <pitch attr="val">
       *     <prefix-step>G</prefix-step>
       *     <prefix-octave>2</prefix-octave>
       *     <prefix-alter>-1</prefix-alter>
       * </pitch>
       */
  void toXml(QXmlStreamWriter& xml, const QString& tag = QStringLiteral("pitch"), const QString& prefix = QString(),
            const QString& attr = QString(), const QString& val = QString()) const;

      /**
       * Reads this note from XML stream.
       * It looks every note element prefixed with @p prefix.
       */
  void fromXml(QXmlStreamReader& xml, const QString& prefix = QString());


      /**
       * Returns this note as midi number.
       */
  unsigned int toMidi() const { return chromatic() + 47; }

      /**
       * Sets this note from midi note number.
       */
  void fromMidi(unsigned int midiNote) { setChromatic(midiNote - 47); }

      /**
       * Shifts this note (if valid) about given number of semitones (interval)
       * Negative values shift down (drops), positive ones shift up (rise)
       */
  void transpose(int interval);

      /**
       * Simple rises this note one octave up
       */
  inline void riseOctaveUp() { if (isValid()) p_octave++; }

  static QString shortOctaveName(int o);
  static QString fullOctaveName(int o);

protected:
  char                  p_note = 0;
  char                  p_octave = 0;
  quint8                p_alterValue = 3; /**< Used also to store upper/lower staff placement. */

private:
  static std::string    m_solmization[7];
  static std::string    m_solmizationRu[7];

};

Q_DECLARE_METATYPE(Tnote)

    /**
    * This function is substitute of >> operator for @p Tnote.
    * It checks is Tnote valid, and return Boolean about it. 
    */
NOOTKACORE_EXPORT bool getNoteFromStream(QDataStream &in, Tnote &n);

NOOTKACORE_EXPORT QDataStream &operator<< (QDataStream &out, const Tnote &n);
NOOTKACORE_EXPORT QDataStream &operator>> (QDataStream &in, Tnote &n);


#endif
