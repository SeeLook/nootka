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
#include "tnote.h"

#include <sstream>
#include <string>
#include <iostream>

// int operator ==( const Tnote & N1, const Tnote & N2 )
// {
// 	return ( N1.note == N2.note && N1.octave == N2.octave && N1.acidental == N2.acidental);
// }

std::string IntToString(int num)
{
  std::ostringstream myStream;
  myStream << num << std::flush;
  return(myStream.str());
}

std::string CharToString(char chr)
{
  std::ostringstream myStream;
  myStream << (int) chr << std::flush;
  return(myStream.str());
}


//****************** IMPLEMENTATION *************************************

Tnote::Tnote ()
{
	note = 0;
	octave = 0;
	acidental = Tnote::e_Natural;
}

Tnote::Tnote( char m_diatonNote, char m_octave, char m_acidental)
{
	note = m_diatonNote;
	octave = m_octave;
	acidental = m_acidental;
}

/**
 * @todo restore this comments
 * @param chromaticNrOfNote 
 * @return 
 */
Tnote::Tnote (short chromaticNrOfNote)
{
   switch ((chromaticNrOfNote+143) % 12 +1)	{
	/** @todo Do somthing with it */
// 	case 1: note = 1; acidental = e_None; break;
// 	case 2: note = 1; acidental = e_Sharp; break;
// 	case 3: note = 2; acidental = e_None; break;
// 	case 4: note = 2; acidental = e_Sharp; break;
// 	case 5: note = 3; acidental = e_None; break;
// 	case 6: note = 4; acidental = e_None; break;
// 	case 7: note = 4; acidental = e_Sharp; break;
// 	case 8: note = 5; acidental = e_None; break;
// 	case 9: note = 5; acidental = e_Sharp; break;
// 	case 10: note = 6; acidental = e_None; break;
// 	case 11: note = 6; acidental = e_Sharp; break;
// 	case 12: note = 7; acidental = e_None; break;
	case 1: note = 1; acidental = 0; break;
	case 2: note = 1; acidental = 1; break;
	case 3: note = 2; acidental = 0; break;
	case 4: note = 2; acidental = 1; break;
	case 5: note = 3; acidental = 0; break;
	case 6: note = 4; acidental = 0; break;
	case 7: note = 4; acidental = 1; break;
	case 8: note = 5; acidental = 0; break;
	case 9: note = 5; acidental = 1; break;
	case 10: note = 6; acidental = 0; break;
	case 11: note = 6; acidental = 1; break;
	case 12: note = 7; acidental = 0; break;
   }	
   octave = ((chromaticNrOfNote+143) / 12 - 11) ;
}


Tnote::~Tnote ()
{
}

std::string Tnote::printNote ( bool showOctave)
{
   std::string nuta = Letters[note-1];
   if ( acidental != e_None ) {
   		switch (acidental)	{
   		  case e_Sharp : nuta += "#"; break;
   		  case e_DoubleSharp : nuta += "x"; break;
   		  case e_Flat : nuta += "b"; break;
   		  case e_DoubleFlat : nuta += "bb"; break;
   		  case e_Natural :  break;
   		}
   	}
   if (showOctave) nuta = nuta +  CharToString (octave);
   return nuta;
}


/**
 * One thing is strange
 * /code
 * return a + (Tnote::octave-1)*12 + Tnote::acidental;
 * /endcode
 * We subtract 1 for octave number because it is for guitar notation
 * @todo Remove this strangeness to TscoreWidget class, witch have to support tke clefs
 */
short Tnote::getChromaticNrOfNote( )
{
	char a;
	switch  (Tnote::note)	{
		case 1: a = 1; break;	// note C
		case 2: a = 3; break;	// D
		case 3: a = 5; break;	// E
		case 4: a = 6; break;	// F
		case 5: a = 8; break;	// G
		case 6: a = 10; break;	// A
		case 7: a = 12; break;	// H
	}
	return a + (Tnote::octave-1)*12 + Tnote::acidental;
}



Tnote Tnote::showAsNatural( )
{
	return Tnote(Tnote(note,octave,acidental).getChromaticNrOfNote());
}

Tnote Tnote::showWithFlat( )
{
	if (acidental != Tnote::e_Flat)	{
		Tnote outputNote = Tnote::showAsNatural();
// Tnote::showAsNatural() allways returns notes with sharps or neutral, and never returns H# or E#, but
// C or F,
		if (outputNote.acidental == Tnote::e_Sharp)	{
				outputNote.note++;
			outputNote.acidental = Tnote::e_Flat;
		}
		else //so only for H changed to Cb we have to increment octave, 
		if (outputNote.note == 7 || outputNote.note == 3)	{ 
		  if (outputNote.note == 7)	{outputNote.octave++; outputNote.note = 1;}
		  if (outputNote.note == 3) outputNote.note = 4;//and for E simply convert to Fb
		  outputNote.acidental = Tnote::e_Flat;
		}
		return outputNote;
	}
	else	return Tnote(note,octave,acidental);
}

Tnote Tnote::showWithSharp( )
{
	if (acidental != Tnote::e_Sharp)	{
		Tnote outputNote = Tnote::showAsNatural();
		if (outputNote.acidental == Tnote::e_Natural)	{
			if (outputNote.note == 4) { outputNote.note = 3; outputNote.acidental = Tnote::e_Sharp;}
			else
			  if (outputNote.note == 1)
				{outputNote.octave--; outputNote.note = 7; outputNote.acidental = Tnote::e_Sharp;}
		}
	return outputNote;
	}
	else return Tnote(note,octave,acidental);
}

Tnote Tnote::showWithDoubleSharp( )
{
	if (acidental != Tnote::e_DoubleSharp)	{
		Tnote outputNote = Tnote::showAsNatural();
		if (outputNote.acidental == Tnote::e_Natural)	{
		  if (outputNote.note == 4) {outputNote.acidental = Tnote::e_Sharp; outputNote.note = 3;}
		  else
			if (outputNote.note == 1)
				{outputNote.acidental = Tnote::e_Sharp; outputNote.note = 7; outputNote.octave--;}
			else
			  {outputNote.acidental = Tnote::e_DoubleSharp; outputNote.note--;}
		}
		return outputNote;
	}
	else return Tnote(note,octave,acidental);
}

/** @todo Where is ceses from b and feses from es ???? */
Tnote Tnote::showWithDoubleFlat( )
{
	if (acidental != Tnote::e_DoubleFlat)	{
		Tnote outputNote = Tnote(note,octave,acidental);
		if (outputNote.acidental == Tnote::e_Flat && ( (outputNote.note == 3) || (outputNote.note == 7)) )	{
			if (outputNote.note == 3) {outputNote.acidental = Tnote::e_DoubleFlat; outputNote.note = 4;}
			else
				{outputNote.acidental = Tnote::e_DoubleFlat; outputNote.note = 1; outputNote.octave++;}
		}
		else	{
			outputNote = outputNote.showAsNatural();
			//Tnote outputNote = Tnote::showAsNatural();
			if (outputNote.acidental == Tnote::e_Natural)	{
		  		if (outputNote.note == 3) {outputNote.acidental = Tnote::e_Flat; outputNote.note = 4;}
				else
				if (outputNote.note == 7)
					{outputNote.acidental = Tnote::e_Flat; outputNote.note = 1; outputNote.octave++;}
				else
			  		{outputNote.acidental = Tnote::e_DoubleFlat; outputNote.note++;}
			}
			else
				if (outputNote.acidental == Tnote::e_Sharp)	{
					if (outputNote.note == 2)
						{outputNote.acidental = Tnote::e_DoubleFlat; outputNote.note = 4;}
					else
					  if (outputNote.note == 6)
						{outputNote.acidental = Tnote::e_DoubleFlat; outputNote.note = 1; outputNote.octave++;}
					  else
						outputNote = outputNote.showWithFlat();
				}
		}
		return outputNote;
	}
	else return Tnote(note,octave,acidental);
}

short Tnote::compareNotes( Tnote otherNote, short ignoreOctave )
{
	if (!ignoreOctave)
		if ( (note == otherNote.note) && ( acidental == otherNote.acidental)
			&& (octave == otherNote.octave) ) return 1;
		else return 0;
	else
		if ( (note == otherNote.note) && ( acidental == otherNote.acidental) )
			return 1;
		else return 0;
}


TnotesList Tnote::getTheSameNotes( bool enableDbAccids )
{
	TnotesList notesL;
	short cnt;//counter of notes. With double accids is 5 (4) without 3 (2)
	notesL.push_back(Tnote(note,octave,acidental));
	if (notesL[0].acidental != Tnote::e_Natural) notesL.push_back(notesL[0].showAsNatural());
	if (notesL[0].acidental != Tnote::e_Sharp) notesL.push_back(notesL[0].showWithSharp());
	if (notesL[0].acidental != Tnote::e_Flat) notesL.push_back(notesL[0].showWithFlat());
	if (enableDbAccids)	{
		if (notesL[0].acidental != Tnote::e_DoubleSharp) notesL.push_back(notesL[0].showWithDoubleSharp());
		if (notesL[0].acidental != Tnote::e_DoubleFlat) notesL.push_back(notesL[0].showWithDoubleFlat());
		cnt = 4;
	}
	else cnt = 2;
	for( int m=0; m < cnt; m++)	{
		for( int n=cnt; n > m; n--){
			if ((notesL[m].note != 0) && (notesL[n].note != 0) )
				if(notesL[m].compareNotes(notesL[n]) )	notesL[n].note = 0;
		}
	}
	TnotesList::iterator m = notesL.end();
	while( m != notesL.begin() ){
		--m;
		if (!(*m).note) notesL.erase(m);
	}
	//notesL.erase(notesL.begin());
	return notesL;
}

std::string Tnote::getName( Enotation notation, bool showOctave )
{
	std::string nuta;
	switch (notation) {
      case e_italiano_Si:
		nuta = Solmization[note-1]+signsAcid[acidental+2];
		break;
      case e_deutsch_His:
		nuta = Letters[note-1];
		switch (acidental)	{
			case e_Natural: break;
			case e_DoubleSharp: nuta = nuta + "isis"; break;
			case e_Sharp: nuta = nuta + "is"; break;
			case e_DoubleFlat: switch (note)	{
				case 3: nuta = nuta + "ses"; break;
				case 6: nuta = nuta + "sas"; break;
				default: nuta = nuta + "eses"; break;
				}
				break;
			case e_Flat: switch (note)	{
				case 3: nuta = nuta + "s"; break;
				case 6: nuta = nuta + "s"; break;
				case 7: nuta = "B"; break;
				default: nuta = nuta + "es"; break;
					}
					break;
		}
	  break;
	  case e_nederl_Bis:
		nuta = Letters[note-1];
		if (note == 7) nuta = "B";
		switch( acidental ){
			case e_Natural: break;
			case e_DoubleSharp: nuta = nuta + "isis"; break;
			case e_Sharp: nuta = nuta + "is"; break;
			case e_DoubleFlat:
			  switch (note)	{
				case 3: nuta = nuta + "ses"; break;
				case 6: nuta = nuta + "ses"; break;
				default: nuta = nuta + "eses"; break;
			  }
				break;
			case e_Flat:
			  switch (note)	{
				case 3: nuta = nuta + "s"; break;
				case 6: nuta = nuta + "s"; break;
				default: nuta = nuta + "es"; break;
			  }
				break;
		}
	  break;
	  default:
		nuta = Letters[note-1];
		if ((notation == e_english_Bb) && (note == 7)) nuta = "B";
		nuta = nuta + signsAcid[acidental+2];
		break;
   }
	if (showOctave) nuta = nuta + CharToString(octave);
	return nuta;
}

std::string Tnote::getName( Tnote eNote, Enotation notation, bool showOctave )
{
	note = eNote.note;
	acidental = eNote.acidental;
	octave = eNote.octave;
	return getName(notation,showOctave);
}

bool Tnote::operator ==( const Tnote & N2 )
{
	return ( note == N2.note && octave == N2.octave && acidental == N2.acidental);
}




