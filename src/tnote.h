/***************************************************************************
 *   Copyright (C) 2006 by Tomasz Bojczuk   *
 *   tomaszbojczuk@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TNOTE_H
#define TNOTE_H

#include <string>
#include <vector>
#include <QDataStream>

class Tnote;

typedef std::vector<Tnote> TnotesList;



const std::string Letters [7] = {"C","D","E","F","G","A","H"};
const std::string Solmization[7] = {"Do", "Re", "Mi", "Fa", "Sol", "La", "Si"};
const std::string signsAcid[5] = {"bb","b","","#","x",};

//enum Enote { e_C=1, e_D=2, e_E=3, e_F=4, e_G=5, e_A=6, e_H=7, e_0=0 };



 ///////////////////CLASS Tnotes /////////////////////////////////////////
/**
* This class privides the descriptions of classical notation for different variables in apps.
* It helps to convert the letter notation (C D E ...) for Do Re Mi.
* It suports acidentals (sharps, flats, double sharps and double flats.
* General format for each note is its number. 
*
*  @short A class, that support classical music notation for variables in apps
*  @author Tomasz Bojczuk <tomaszbojczuk@gmail.com>
*/
class Tnote
{
public:
/** Enum type EnameStyle describes the styles of notation:
		* @li e_italiano_Si - for classical Do Re Mi ...
		* @li e_norsk_Hb - for letters with signs f.e. C# Cx or Cb    !! THIS IS DEFAULT !!
                * @li e_deutsch_His - for letters with names f.e. Cis Cizis or Ces H and B
		* (as H with flat
		* @li e_english_Bb like @p e_norsk_Hb but with B and Bb (B flat)
                * @li e_nederl_Bis like @p e_deutsch_His but with B ens Bes
		*/
enum EnameStyle {e_norsk_Hb = 0, e_deutsch_His, e_italiano_Si, e_english_Bb, e_nederl_Bis};

/** Eacidentals enum type describes all signs which can be before note in score.
* It can be: @li e_None = 3 @li e_Sharp = 1 @li e_DoubleSharp=2
* @li e_Flat= -1 @li e_DoubleFlat= -2 @li e_Natural=0
*/
enum Eacidentals { e_Natural = 0, e_Sharp=1, e_DoubleSharp=2,
		   e_Flat=-1, e_DoubleFlat=-2, e_None=3 };

// PUBLIC VARIABLES DESCRIBES THE NOTES
		/** note variable is a number in "diatonal notation" (see constructor).*/
char note;
		/** Octave number is @p 0 for "small octave",  @p -1 for "Great" @p 1 for "one-line". */
char octave;
		/** @param acidental means raiseing or falling note, so it ca be:
		* @li 2 for double sharp (x)
		* @li 1 for sharp (#)
		* @li 0 for natural
		* @li -1 for flat (b)
		* @li -2 for double flat (bb) */
char acidental;

//	CONSTRUCTORS
		/** Construct object of Tnote from number of note, number of octave and optional
		* acidental. The note number is: .
		* @li "1" for C 
		* @li "2" for D1 
		* @li .......
		* @li 7 for B (H in Deutsh)
		* If acidental is not defined, the note is natural.	*/
Tnote (char m_diatonNote, char m_octave, char m_acidental = 0);
		/** The simple constructor, creates the note instance with 0 note - 
		* It's no sence in musical notation. It's needed for vectors*/
Tnote ();
		/** Construct object of Tnote from number, that represents:
		* @li "1" for C1 in first octave
		* @li "2" for Cis 
		* @li .......
		* @li 13 for C2 in next octave
		* @li -12 for C in little octave etc....
		* The sharp acidental is default. If You need others, you can use convToFlat. */
Tnote (short chromaticNrOfNote);
~Tnote ();
	
//--Functions--

bool operator==( const Tnote N2 );
bool operator!=( const Tnote N2 );

void convToSharp ();
void convToFlat ();

		/** @return List of pointers to Tnote objects, which are the same (in sound sence),
		* like note represens by class. (C# = Db, or Cx = D = Ebb)  
		* @param enableDbAccids if @p TRUE - checks substitutes with double accids	*/
TnotesList getTheSameNotes (bool enableDbAccids );


Tnote showWithDoubleSharp ();
Tnote showWithSharp ();
	 /** It returns Tnote object with no acidentals, if it's possibile,
	  * or with sharps if not.
	  * @return Tnote  */
Tnote showAsNatural ();
Tnote showWithFlat ();
Tnote showWithDoubleFlat ();
	 /** This method compares actual note, with otherNote.
	  * @param otherNote 
	  * @param ignoreOctave .If 1 (TRUE) the octave values are ignored,
	  * and method compares only number of note and acidental.
	  * @return 1 for TRUE or 0 if notes are different
	  */
short compareNotes(Tnote otherNote, short ignoreOctave = 0);
     /**
      * Return std::string with note in format determines by notation
      * @param showOctave 
      * @return DEPRECATED
      */
std::string printNote ( bool showOctave = 1);

	std::string getName (EnameStyle notation = e_norsk_Hb, bool showOctave = 1);
	std::string getName (Tnote eNote, EnameStyle notation = e_norsk_Hb, bool showOctave = 1);

	 /**
	  *  Return @param chromaticNrOfNote 
	  * @return short 
	  */
short getChromaticNrOfNote ();

protected:

};
    /** This function is substitute of >> operator for @class Tnote.
    * It checks is Tnote valid, and return @value bool about it. */
bool getNoteFromStream(QDataStream &in, Tnote &n);

QDataStream &operator<< (QDataStream &out, Tnote &n);

QDataStream &operator>> (QDataStream &in, Tnote &n);


#endif
