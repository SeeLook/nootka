/***************************************************************************
 *   Copyright (C) 2006-2013 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "tnote.h"

#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>

std::string IntToString(int num) {
  std::ostringstream myStream;
  myStream << num << std::flush;
  return(myStream.str());
}


std::string CharToString(char chr) {
  std::ostringstream myStream;
  myStream << (int) chr << std::flush;
  return(myStream.str());
}


QString accidInSpan(char accid) {
	QString accTxt = "";
	switch (accid) {
		case -2: accTxt = "B"; break;
		case -1: accTxt = "b"; break;
		case 1: accTxt = "#"; break;
		case 2: accTxt = "x"; break;
		default: accTxt = "";
	}
	return  QString("<span style=\"font-family: nootka;\">%1</span>").arg(accTxt);
}

/*static*/
std::string Tnote::m_solmization[7] = {"Do", "Re", "Mi", "Fa", "Sol", "La", "Si"};
std::string Tnote::m_solmizationRu[7] = {"До", "Ре", "Ми", "Фа", "Соль", "Ля", "Си"};
// QString Tnote::m_KodalySharps[6] = {"Di", "Ri", "", "Fi", "Si", "Li"};
// QString Tnote::m_KodalyFlats[7] = {"", "Ra", "Me", "", "Se", "Le", "Te" };

Tnote::EnameStyle Tnote::defaultStyle = Tnote::e_norsk_Hb;


//#############################################################################################
//################################ PUBLIC #####################################################
//#############################################################################################
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


Tnote::Tnote (short chromaticNrOfNote)
{
   switch ((chromaticNrOfNote + 143) % 12 + 1)	{
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
   octave = ((chromaticNrOfNote+143) / 12 - 12) ;
}


Tnote::~Tnote ()
{}


short Tnote::getChromaticNrOfNote() {
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
    return a + (Tnote::octave)*12 + Tnote::acidental;
}


Tnote Tnote::showAsNatural() {
	return Tnote(Tnote(note,octave,acidental).getChromaticNrOfNote());
}


Tnote Tnote::showWithFlat() {
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


Tnote Tnote::showWithSharp() {
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


Tnote Tnote::showWithDoubleSharp() {
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


Tnote Tnote::showWithDoubleFlat() {
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


short Tnote::compareNotes( Tnote otherNote, short ignoreOctave ) {
	if (!ignoreOctave)
		if ( (note == otherNote.note) && ( acidental == otherNote.acidental)
			&& (octave == otherNote.octave) ) return 1;
		else return 0;
	else
		if ( (note == otherNote.note) && ( acidental == otherNote.acidental) )
			return 1;
		else return 0;
}


TnotesList Tnote::getTheSameNotes(bool enableDbAccids) {
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


std::string Tnote::getName( EnameStyle notation, bool showOctave ) {
	std::string noteStr;
    if (note < 1 || note > 7) {
        std::cout << "Oops !! getName() with note=0\n";
        return "none";
    }
	switch (notation) {
      case e_italiano_Si:
					noteStr = m_solmization[note - 1] + signsAcid[acidental + 2];
					break;
			case e_russian_Ci:
					noteStr = m_solmizationRu[note - 1] + signsAcid[acidental + 2];
					break;
      case e_deutsch_His:
					noteStr = Letters[note - 1];
					switch (acidental)	{
						case e_Natural: break;
						case e_DoubleSharp: noteStr = noteStr + "isis"; break;
						case e_Sharp: noteStr = noteStr + "is"; break;
						case e_DoubleFlat: switch (note)	{
							case 3: noteStr = noteStr + "ses"; break;
							case 6: noteStr = noteStr + "sas"; break;
							default: noteStr = noteStr + "eses"; break;
							}
							break;
						case e_Flat: switch (note)	{
							case 3: noteStr = noteStr + "s"; break;
							case 6: noteStr = noteStr + "s"; break;
							case 7: noteStr = "B"; break;
							default: noteStr = noteStr + "es"; break;
								}
								break;
					}
					break;
	  case e_nederl_Bis:
				noteStr = Letters[note-1];
				if (note == 7) noteStr = "B";
				switch( acidental ){
					case e_Natural: break;
					case e_DoubleSharp: noteStr = noteStr + "isis"; break;
					case e_Sharp: noteStr = noteStr + "is"; break;
					case e_DoubleFlat:
						switch (note)	{
						case 3: noteStr = noteStr + "ses"; break;
						case 6: noteStr = noteStr + "ses"; break;
						default: noteStr = noteStr + "eses"; break;
						}
						break;
					case e_Flat:
						switch (note)	{
						case 3: noteStr = noteStr + "s"; break;
						case 6: noteStr = noteStr + "s"; break;
						default: noteStr = noteStr + "es"; break;
						}
						break;
				}
				break;
	  default:
				noteStr = Letters[note - 1];
				if ((notation == e_english_Bb) && (note == 7)) noteStr = "B";
						noteStr = noteStr + signsAcid[acidental + 2];
				break;
   }
	if (showOctave) 
			noteStr = noteStr + CharToString(octave);
	return noteStr;
}


std::string Tnote::getName( Tnote eNote, EnameStyle notation, bool showOctave )
{
	note = eNote.note;
	acidental = eNote.acidental;
	octave = eNote.octave;
	return getName(notation, showOctave);
}


QString Tnote::toText(Tnote::EnameStyle notation, bool showOctave) {
	return QString::fromUtf8(getName(notation, showOctave).data()); 
}


QString Tnote::toRichText(Tnote::EnameStyle notation, bool showOctave) {
  QString result = toText(notation, false);
    if (notation == Tnote::e_italiano_Si ||
			  notation == Tnote::e_russian_Ci ||
        notation == Tnote::e_english_Bb ||
        notation == Tnote::e_norsk_Hb ) {
        if (acidental) {
//             int a = 1;
//             if (acidental == -2) a = 2;
// 						result.insert(result.size() - a,"<sub><i>");
// 						result.insert(result.size(),"</i></sub>");
					result.replace(QString::fromStdString(signsAcid[acidental + 2]), QString("<sub>%1</sub>").arg(accidInSpan(acidental)));
        }
    }
    if (acidental == -2)
				result.replace("B", "!");
    result = result.toLower(); // it converts double flat (B) to single flat (b)
		if (acidental == -2)
				result.replace("!", "B");
    if (showOctave) {
        if (octave < 0) { //first letter capitalize
					QString l1 = result.mid(0, 1).toUpper();
					result.replace(0, 1, l1);
					if (octave < -1)
							result = result + QString("<sub>%1</sub>").arg(int(octave*(-1)-1));
        }
        if (octave > 0)
            result = result + QString("<sup>%1</sup>").arg((int)octave);
    }
    return result;
}


bool Tnote::operator ==( const Tnote N2 )
{
        return ( note == N2.note && octave == N2.octave && acidental == N2.acidental);
}

bool Tnote::operator !=( const Tnote N2 )
{
    return ( note != N2.note || octave != N2.octave || acidental != N2.acidental);
}

bool getNoteFromStream(QDataStream &in, Tnote &n) {
    bool ok = true;
    qint8 nn, oo, aa;
    in >> nn >> oo >> aa;
    if (nn < 1 || nn > 8 || aa < -2 || aa > 2) {
        nn = 1; aa = 0; oo = 0;
        ok = false;
    }
    n = Tnote(nn, oo, aa);
    return ok;
}

QDataStream &operator << (QDataStream &out, const Tnote &n) {
    out << (qint8)n.note << (qint8)n.octave << (qint8)n.acidental;
    return out;
}

QDataStream &operator>> (QDataStream &in, Tnote &n) {
    qint8 nn, oo, aa;
    in >> nn >> oo >> aa;
    n = Tnote(nn, oo, aa);
    return in;
}
