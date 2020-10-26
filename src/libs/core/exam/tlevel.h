/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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

#ifndef TEXAMLEVEL_H
#define TEXAMLEVEL_H

#include <nootkacoreglobal.h>
#include "tqatype.h"
#include <music/tnote.h>
#include <music/tmelody.h>
#include <music/tinstrument.h>
#include <QtCore/qdatastream.h>


class QXmlStreamWriter;
class QFile;


/**
 * This class describes exam level.
 */
class NOOTKACORE_EXPORT Tlevel
{
public:
      /**
       * default constructor creates a "complex" level (master of masters)
       */
  Tlevel();

      /**
       * Possible errors during reading level file or XML key.
       */
  enum EerrorType {
      e_level_OK = 0,
      e_levelFixed,     /**< level parameters were fixed */
      e_noLevelInXml,   /**< when XML stream has no <level> tag  */
      e_otherError
  };

      /**
       * Describes how to generate melody
       */
  enum ErandMelody : quint8 {
      e_randFromRange = 1, /**< melody is composed from notes in level range  */
      e_randFromList = 2,  /**< melody is composed from list of notes (@p notesList) */
      e_melodyFromSet = 4  /**< melody from set of melodies (either randomized or asked in the list order). (@p melodySet) */
  };

//------------------------- Managing level versions ------------------------------------------------------
  static const qint32 levelVersion; /**< First version, also with early using of intonation and instruments */
  static const qint32 currentVersion; /**< Current level version identifier */

      /**
       * Returns true when given value match to level versions.
       */
  static bool isLevelVersion(quint32 ver);

      /**
       * Generates level version identifier number from given simple version number (1, 2 etc.)
       * It doesn't verify does given number make sense!
       */
  static qint32 getVersionId(quint8 verNr) { return levelVersion + (verNr - 1) * 2; }

      /**
       * Returns number of level version like 1, 2 or so
       */
  static int levelVersionNr(qint32 ver);

      /**
       * Returns true when given value 'could be' or 'it is' some version of exam level.
       * This way level created with newer Nootka version can be detected.
       */
  static bool couldBeLevel(qint32 ver);

      /**
       * Saves level to given file and returns true for success or opposite.
       */
  static bool saveToFile(Tlevel &level, const QString& levelFile);

      /**
       * Shows message box with error if file cannot be opened.
       */
  static void fileIOerrorMsg(QFile &f);

      /**
       * Reads fret number from current XML key to @fr reference and verifies it.
       * Setts error type when error occurs or lives @p err unchanged when OK.
       */
  static void fretFromXml(QXmlStreamReader& xml, char& fr, Tlevel::EerrorType& err);

  static void skipCurrentXmlKey(QXmlStreamReader& xml);

//--------------------------- level parameters ------------------------------------------------------------
  QString            name; /**< Level name */
  QString            desc; /**< description */
// QUESTIONS
  TQAtype            questionAs;
  TQAtype            answersAs[4];
  bool               forceAccids;
  bool               requireOctave;
  bool               requireStyle;
  bool               showStrNr; /**< Shows a string number in questions */
  quint8             intonation; /**< This shares byte with isSingleKey in file */
  Tclef              clef;
  Tinstrument::Etype instrument;
  bool               onlyLowPos;
  bool               onlyCurrKey;
// ACCIDENTALS
  bool               withSharps, withFlats, withDblAcc;
  bool               useKeySign;
  bool               isSingleKey;
  TkeySignature      loKey, hiKey; /**< range of key signature */
  bool               manualKey; /**< manually selecting key in answers */
// MELODIES
  quint16            melodyLen; /**< Notes count in a melody */
  bool               endsOnTonic;
  bool               requireInTempo;
  ErandMelody        randMelody; /**< How melody is composed (from range, from notes list or from set of melodies) */
  QList<Tnote>       notesList; /**< List with notes from which melody is composed */
  TkeySignature      keyOfrandList; /**< Key signature of note list for composing random melodies */
  QList<Tmelody>     melodySet; /**< List of defined melodies when @p randMelody is @p e_melodyFromSet */
  bool               randOrderInSet; /**< order of melodies in the set, @p TRUE means random */
  quint8             repeatNrInSet; /**< How many times a question is repeated with a melody */
// RHYTHMS
  quint32            basicRhythms;
  quint32            dotsRhythms;
  quint16            meters; /**< Bits of time signatures used in the level */
  quint8             rhythmDiversity;
  quint8             barNumber;
  bool               variableBarNr; /**< Whether bar number grows or not as exam/exercise is progressing */
  bool               useRests;
  bool               useTies;
// RANGE
  Tnote              loNote; /**< Lowest level note */
  Tnote              hiNote; /**< Highest level note */
  char               loFret, hiFret;
  bool               usedStrings[6];


      /**
       * Indicates when instrument read from file needs user action to be properly obtained.
       * It occurs when read value is 255 for level version 1
       */
  bool hasInstrToFix;

// some helpers
      /**
       * @p TRUE if answer or question is note on a score
       */
  bool canBeScore() const ;

      /**
       * @p TRUE if answer or question is note name
       */
  bool canBeName() const;

      /**
       * @p TRUE if answer or question is position on a guitar
       */
  bool canBeGuitar() const;

      /**
       * @p TRUE if answer or question is played or sang sound
       */
  bool canBeSound() const ;

      /**
       * @p TRUE when question/answer has more notes and have appropriate types
       */
  bool canBeMelody() const;

      /**
       * @p TRUE if answer is note on a score in any question type
       */
  bool answerIsNote() const;

      /**
       * @p TRUE  if answer is note name in any question type
       */
  bool answerIsName() const;

      /**
       * @p TRUE  if answer is position on the guitar in any question type
       */
  bool answerIsGuitar() const;

      /**
       * @p TRUE  if answer is played sound in any question type
       */
  bool answerIsSound() const;

      /**
       * @p TRUE when the level is set of melodies
       */
  bool isMelodySet() const { return randMelody == e_melodyFromSet && !melodySet.isEmpty(); }

      /**
       * @p TRUE  when level note range is in given number range represented scale of instrument.
       */
  bool inScaleOf(int loNoteNr, int hiNoteNr) const;

      /**
       * Overloaded method with scale in Tnote objects
       */
  bool inScaleOf(const Tnote &loN, const Tnote &hiN) const { return inScaleOf(loN.chromatic(), hiN.chromatic()); }

      /**
       * Overloaded method where instrument scale is taken from @p Tglobals
       */
  bool inScaleOf();

      /**
       * Examines level scale, note by note to find lowest and highest frets used.
       * Obtained range is returned through references.
       * Returns true when both frets are in instrument capabilities,
       * or false if not - then references remind untouched.
       * This method doesn't change any level value.
       */
  bool adjustFretsToScale(char& loF, char& hiF);

      /**
       * Writes level parameters into 'level' node of XML stream
       */
  void writeToXml(QXmlStreamWriter& xml);

      /**
       * Reads level from XML stream
       */
  EerrorType loadFromXml(QXmlStreamReader& xml);

      /**
       * Reads 'qaType' key from XML. Determines level var by id and sets it
       */
  EerrorType qaTypeFromXml(QXmlStreamReader& xml);

      /**
       * @p TRUE when all rhythms parameters (any meter, any rhythmic group and bar number) are set
       */
  bool useRhythms() const;

      /**
       * Returns number of key signatures in the range of keys
       * or @p 1 if no keys in use, or just a single key
       */
  int keysInRange() const;

//------------------------- to fix a level ---------------------------------------------------

      /**
       * Since Nootka 2 (versions 1.5 and above) dropped bass clef
       * was changed to simply bass clef (without any drop),
       * but with instrument transposition one octave down.
       * So all notes (melodies, range and so) HAVE TO BE transposed as well
       * by one octave up (12 semitones).
       */
  void convFromDropedBass();

      /**
       * Returns detected clef from level versions before 0.8.90
       */
  Tclef fixClef(quint16 cl);

      /**
       * Fixes instrument value taken from file (stream) created before level Version 2
       */
  Tinstrument::Etype fixInstrument(quint8 instr);

      /**
       * Checks what kind of instrument should be used by level.
       * In contrary to fixInstrument() method it doesn't check Tglobals state of instrument
       * just takes given parameter.
       */
  Tinstrument::Etype detectInstrument(Tinstrument::Etype currInstr);

  Tlevel::EerrorType fixFretRange(); /**< When loFret is bigger than hiFret it swaps their values */
  Tlevel::EerrorType fixNoteRange(); /**< When loNote is higher than hiNote it swaps their values */
  Tlevel::EerrorType fixKeyRange(); /**< When loKey is bigger than hiKey it swaps their values */

};

    /**
     * Reads level data from given stream to @p lev. Respects @p ver - version
     */
NOOTKACORE_EXPORT bool getLevelFromStream(QDataStream& in, Tlevel& lev, qint32 ver);

Q_DECLARE_METATYPE(Tlevel*)

#endif // TEXAMLEVEL_H

