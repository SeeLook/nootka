/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
#include <music/tkeysignature.h>
#include <music/tclef.h>
#include <music/tinstrument.h>
#include <QString>
#include <QDataStream>

class QXmlStreamWriter;
class QFile;
class QWidget;

		/** Displays dialog message about existence of newer Nootka version than current. */
NOOTKACORE_EXPORT void newerNootkaMessage(const QString& fileName, QWidget* parent = 0);

/** 
* This class describes exam level.
*/
class NOOTKACORE_EXPORT Tlevel
{
public:
        /** default constructor creates a "complex" level (master of masters) */
    Tlevel();
		
	//------------------------- Managing level versions ------------------------------------------------------
		static const qint32 levelVersion; /** First version, also with early using of intonation and instruments */
		static const qint32 currentVersion; /** Current level version identifier */
		static bool isLevelVersion(quint32 ver); /** Returns true when given value match to level versions. */
		
				/** Generates level version identifier number from given simple version number (1, 2 etc.)
				 * It doesn't verify does given number make sense! */
		static qint32 getVersionId(quint8 verNr) { return levelVersion + (verNr - 1) * 2; }
		
				/** Returns number of level version like 1, 2 or so */
		static int levelVersionNr(qint32 ver);
		
				/** Returns true when given value 'could be' or 'it is' some version of exam level.
				 * This way level created with newer Nootka version can be detected. */
		static bool couldBeLevel(qint32 ver);
		
				/** Saves level to given file and returns true for success or opposite. */
		static bool saveToFile(Tlevel &level, const QString& levelFile);
		
				/** Shows message box with error if file cannot be opened.*/
    static void fileIOerrorMsg(QFile &f, QWidget *parent = 0);

				/** Writes Tnote 'pitch' XML node into given @p tag to XML stream */
		static void tnoteToXml(const QString& tag, Tnote& n, QXmlStreamWriter& xml);
		static Tnote tnoteFromXml(QXmlStreamReader& xml);

  //--------------------------- level parameters ------------------------------------------------------------
    QString name; /** Level name */
    QString desc; /** description */
    TQAtype questionAs;
    TQAtype answersAs[4];
  // QUESTIONS - as note
    bool withSharps, withFlats, withDblAcc;
    bool useKeySign;
    bool isSingleKey;
		quint8 intonation; // This shares byte with isSingleKey in file
    TkeySignature loKey, hiKey; /** range of key signature */
    bool manualKey; /** manually selecting key in answers */
    bool forceAccids;
  // QUESTIONS - as name
    bool requireOctave;
    bool requireStyle;
  // ANSWERS - as position on fingerboard
    bool showStrNr; /** Shows a string number in questions*/
  // RANGE
    Tnote loNote; /** Lowest level note */ 
    Tnote hiNote; /** Highest level note */
//     bool isNoteLo, isNoteHi ---- since 0.8.90 version those values are changed to Tclef
		Tclef clef;
    char loFret, hiFret;
//     bool isFretHi ----  since 0.8.90 version this value is changed to Einstrument
		Einstrument instrument;
    bool usedStrings[6];
    bool onlyLowPos;
    bool onlyCurrKey;
	//------------------------------------------------------------------------------------------------------------
		
				/** Indicates when instrument read from file needs user action to be properly obtained.
				 * It occurs when read value is 255 for level version 1 */
		bool hasInstrToFix;
    
  // some helpers
    bool canBeScore(); // True if answer or question is note on a score
    bool canBeName(); // True if answer or question is note name
    bool canBeGuitar(); // True if answer or question is position on a guitar
    bool canBeSound(); // True if answer or question is played or sang sound
		bool answerIsNote(); // True if answer is note on a score in any question type
		bool answerIsName(); // True if answer is note name in any question type
		bool answerIsGuitar(); // True if answer is position on the guitar in any question type
		bool answerIsSound(); // True if answer is played sound in any question type
		
				/** True when level note range is in given number range represented scale of instrument. */
		bool inScaleOf(int loNoteNr, int hiNoteNr);
		
				/** Overloaded method with scale in Tnote objects */
		bool inScaleOf(Tnote &loN, Tnote &hiN) { return inScaleOf(loN.getChromaticNrOfNote(), hiN.getChromaticNrOfNote()); }
		bool inScaleOf(); /** Overloaded method where instrument scale is taken from Tglobals  */
		
				/** Examines level scale, note by note to find lowest and highest frets used.
				 * Obtained range is returned through references.
				 * Returns true when both frets are in instrument capabilities,
				 * or false if not - then references remind untouched.
				 * This method doesn't change any level value. */
		bool adjustFretsToScale(char& loF, char& hiF);
		
				/** Writes level parameters into 'level' node of XML stream */
		void writeToXml(QXmlStreamWriter& xml);
		
		bool loadFromXml(QXmlStreamReader& xmlFile);
		
				/** Reads 'qaType' key from XML. Determines level var by id and sets it */
		void qaTypeFromXml(QXmlStreamReader& xml);
		
	//------------------------- to fix a level ---------------------------------------------------
		
				/** Returns detected clef from level versions before 0.8.90 */
		Tclef fixClef(quint16 cl);
		
				/** Fixes instrument value taken from file (stream) created before level Version 2 */
		Einstrument fixInstrument(quint8 instr);
		
				/** Checks what kind of instrument should be used by level.
				 * In contrary to fixInstrument() method it doesn't check Tglobals state of instrument
				 * just takes given parameter. */
		Einstrument detectInstrument(Einstrument currInstr);

};

NOOTKACORE_EXPORT QDataStream &operator<< (QDataStream &out, Tlevel &lev);

		/** Reads level data from given stream to @p lev. Respects @p ver - version */
NOOTKACORE_EXPORT bool getLevelFromStream(QDataStream& in, Tlevel& lev, qint32 ver);


#endif // TEXAMLEVEL_H
