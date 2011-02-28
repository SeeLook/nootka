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
#include <QString>

//#include <sstream>
//#include <string>
#include <iostream>

// int operator ==( const Tnote & N1, const Tnote & N2 )
// {
// 	return ( N1.note == N2.note && N1.octave == N2.octave && N1.acidental == N2.acidental);
// }

//std::string IntToString(int num)
//{
//  std::ostringstream myStream;
//  myStream << num << std::flush;
//  return(myStream.str());
//}

//std::string CharToString(char chr)
//{
//  std::ostringstream myStream;
//  myStream << (int) chr << std::flush;
//  return(myStream.str());
//}
/**static*/
const QString Tnote::Letters[7] = {"C","D","E","F","G","A","H"};
const QString Tnote::Solmization[7] = {"Do", "Re", "Mi", "Fa", "Sol", "La", "Si"};
const QString Tnote::signsAcid[5] = {"bb","b","","#","x",};


//****************** IMPLEMENTATION *************************************

Tnote::Tnote ()
{
        m_number = 0;
        m_octave = 0;
        m_accid = Tnote::e_Natural;
}

Tnote::Tnote(char _noteNr, char _octave, char _accidental)
{
        m_number = _noteNr;
        m_octave = _octave;
        m_accid = _accidental;
}

Tnote::Tnote (short chromaticNrOfNote)
{
   switch ((chromaticNrOfNote+143) % 12 +1)	{
	/** @todo Do somthing with it */
   case 1: m_number = 1; m_accid = Tnote::e_None; break;
        case 2: m_number = 1; m_accid = Tnote::e_Sharp; break;
        case 3: m_number = 2; m_accid = Tnote::e_None; break;
        case 4: m_number = 2; m_accid = Tnote::e_Sharp; break;
        case 5: m_number = 3; m_accid = Tnote::e_None; break;
        case 6: m_number = 4; m_accid = Tnote::e_None; break;
        case 7: m_number = 4; m_accid = Tnote::e_Sharp; break;
        case 8: m_number = 5; m_accid = Tnote::e_None; break;
        case 9: m_number = 5; m_accid = Tnote::e_Sharp; break;
        case 10: m_number = 6; m_accid = Tnote::e_None; break;
        case 11: m_number = 6; m_accid = Tnote::e_Sharp; break;
        case 12: m_number = 7; m_accid = Tnote::e_None; break;
//	case 1: note = 1; acidental = 0; break;
//	case 2: note = 1; acidental = 1; break;
//	case 3: note = 2; acidental = 0; break;
//	case 4: note = 2; acidental = 1; break;
//	case 5: note = 3; acidental = 0; break;
//	case 6: note = 4; acidental = 0; break;
//	case 7: note = 4; acidental = 1; break;
//	case 8: note = 5; acidental = 0; break;
//	case 9: note = 5; acidental = 1; break;
//	case 10: note = 6; acidental = 0; break;
//	case 11: note = 6; acidental = 1; break;
//	case 12: note = 7; acidental = 0; break;
   }	
   m_octave = ((chromaticNrOfNote+143) / 12 - 11) ;
}


Tnote::~Tnote ()
{
}

//std::string Tnote::printNote ( bool showOctave)
//{
//   std::string nuta = Letters[note-1];
//   if ( acidental != e_None ) {
//   		switch (acidental)	{
//   		  case e_Sharp : nuta += "#"; break;
//   		  case e_DoubleSharp : nuta += "x"; break;
//   		  case e_Flat : nuta += "b"; break;
//   		  case e_DoubleFlat : nuta += "bb"; break;
//   		  case e_Natural :  break;
//   		}
//   	}
//   if (showOctave) nuta = nuta +  CharToString (octave);
//   return nuta;
//}


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
    switch  (Tnote::number())	{
        case 1: a = 1; break;	// note C
        case 2: a = 3; break;	// D
        case 3: a = 5; break;	// E
        case 4: a = 6; break;	// F
        case 5: a = 8; break;	// G
        case 6: a = 10; break;	// A
        case 7: a = 12; break;	// H
    }
    return a + (octave()-1)*12 + accidental();
}



Tnote Tnote::showAsNatural() {
        return Tnote(Tnote(number(),octave(),accidental()).getChromaticNrOfNote());
}

Tnote Tnote::showWithFlat() {
    if (accidental() != Tnote::e_Flat)	{
        Tnote outputNote = Tnote::showAsNatural();
// Tnote::showAsNatural() allways returns notes with sharps or neutral, and never returns H# or E#, but C or F,
        if (outputNote.accidental() == Tnote::e_Sharp)	{
            outputNote.setNumber(outputNote.number()+1);
            outputNote.setAccidental(Tnote::e_Flat);
        } else //so only for H changed to Cb we have to increment octave,
            if (outputNote.number() == 7 || outputNote.number() == 3) {
                if (outputNote.number() == 7) {
                    outputNote.setOctave(outputNote.octave()+1);
                    outputNote.setNumber(1);
                }
                if (outputNote.number() == 3) outputNote.setNumber(4);
                //and for E simply convert to Fb
                outputNote.setAccidental(Tnote::e_Flat);
            }
        std::cout << "flat " << outputNote.getName(Tnote::e_deutsch_His,true).toStdString() << "\n";
        return outputNote;
    }
    else
        return Tnote(number(),octave(),accidental());
}

Tnote Tnote::showWithSharp() {
    if (accidental() != Tnote::e_Sharp)	{
        Tnote outputNote = Tnote::showAsNatural();
        if (outputNote.accidental() == Tnote::e_Natural) {
            if (outputNote.number() == 4) {
                outputNote.setNumber(3);
                outputNote.setAccidental(Tnote::e_Sharp);
            } else
                if (outputNote.number() == 1) {
                    outputNote.setOctave(outputNote.octave()-1);
                    outputNote.setNumber(7);
                    outputNote.setAccidental(Tnote::e_Sharp);
                }
        }
        std::cout << "sharp " << outputNote.getName(Tnote::e_deutsch_His,true).toStdString() << "\n";
        return outputNote;
        }
    else return Tnote(number(),octave(),accidental());
}

Tnote Tnote::showWithDoubleSharp() {
    if (m_accid != Tnote::e_DoubleSharp) {
        Tnote outputNote = Tnote::showAsNatural();
        if (outputNote.accidental() == Tnote::e_Natural) {
            if (outputNote.number() == 4) {
                outputNote.setAccidental(Tnote::e_Sharp);
                outputNote.setNumber(3);
            }
            else
                if (outputNote.number() == 1) {
                    outputNote.setAccidental(Tnote::e_Sharp);
                    outputNote.setNumber(7);
                    outputNote.setOctave(outputNote.octave()-1);
                }
                else {
                    outputNote.setAccidental(Tnote::e_DoubleSharp);
                    outputNote.setNumber(outputNote.number()-1);
                }
        }
        std::cout << "dbl# " << outputNote.getName(Tnote::e_deutsch_His,true).toStdString() << "\n";
        return outputNote;
    } else
        return Tnote(number(),octave(),accidental());
}

/** @todo Where is ceses from b and feses from es ???? */
Tnote Tnote::showWithDoubleFlat() {
    if (accidental() != Tnote::e_DoubleFlat) {
        Tnote outputNote = Tnote(number(),octave(),accidental());
        if (outputNote.accidental() == Tnote::e_Flat &&
            ( (outputNote.number() == 3) || (outputNote.number() == 7)) ) {
                if (outputNote.number() == 3) {
                    outputNote.setAccidental(Tnote::e_DoubleFlat);
                    outputNote.setNumber(4);
                } else {
                    outputNote.setAccidental(Tnote::e_DoubleFlat);
                    outputNote.setNumber(1);
                    outputNote.setOctave(outputNote.octave()+1);
                  }
                } else {
                    outputNote = outputNote.showAsNatural();
                    if (outputNote.accidental() == Tnote::e_Natural) {
                        if (outputNote.number() == 3) {
                            outputNote.setAccidental(Tnote::e_Flat);
                            outputNote.setNumber(4);
                        } else
                            if (outputNote.number() == 7) {
                                outputNote.setAccidental(Tnote::e_Flat);
                                outputNote.setNumber(1);
                                outputNote.setOctave(outputNote.octave()+1);
                            } else {
                                outputNote.setAccidental(Tnote::e_DoubleFlat);
                                outputNote.setNumber(outputNote.number()+1);
                              }
                        } else
                            if (outputNote.accidental() == Tnote::e_Sharp) {
                                if (outputNote.number() == 2) {
                                    outputNote.setAccidental(Tnote::e_DoubleFlat);
                                    outputNote.setNumber(4);
                                } else
                                    if (outputNote.number() == 6) {
                                        outputNote.setAccidental(Tnote::e_DoubleFlat);
                                        outputNote.setNumber(1);
                                        outputNote.setOctave(outputNote.octave()+1);
                                    } else
                                        outputNote = outputNote.showWithFlat();
                                }
                }
            std::cout << "dblb " << outputNote.getName(Tnote::e_deutsch_His,true).toStdString() << "\n";
            return outputNote;
        }
        else return Tnote(number(),octave(),accidental());
}

bool Tnote::compareNotes( Tnote otherNote, bool ignoreOctave ) {
    if (!ignoreOctave)
        if ( (number() == otherNote.number()) && ( accidental() == otherNote.accidental())
                        && (octave() == otherNote.octave()) ) return true;
            else return false;
    else
        if ( (number() == otherNote.number()) && ( accidental() == otherNote.accidental()) )
           return true;
        else return false;
}


TnotesList Tnote::getTheSameNotes( bool enableDbAccids ) {
    TnotesList notes;
    short cnt; //counter of notes. With double accids is 5 (4) without 3 (2)
    notes << (Tnote(m_number, m_octave, m_accid));
    if (notes[0].accidental() != Tnote::e_Natural) notes << (notes[0].showAsNatural());
    if (notes[0].accidental() != Tnote::e_Sharp) notes << (notes[0].showWithSharp());
    if (notes[0].accidental() != Tnote::e_Flat) notes << (notes[0].showWithFlat());
    if (enableDbAccids)	{
        if (notes[0].accidental() != Tnote::e_DoubleSharp)
            notes << (notes[0].showWithDoubleSharp());
        if (notes[0].accidental() != Tnote::e_DoubleFlat)
            notes << (notes[0].showWithDoubleFlat());
        cnt = 4;
    }
    else cnt = 2;
    for( int m=0; m < cnt; m++)	{
//        std::cout << m << notes[m].getName(Tnote::e_deutsch_His,true).toStdString() << "\n";
        for( int n=cnt; n > m; n--) {
            if ((notes[m].number() != 0) && (notes[n].number() != 0) )
                if(notes[m].compareNotes(notes[n])) notes[n].setNumber(0);
        }
    }
    QList<Tnote>::iterator m = notes.end();
    while( m != notes.begin() ) {
        --m;
        if ( !(*m).number() ) notes.erase(m);
    }
    notes.erase(notes.begin());
//    std::cout << "size: " << notes.size() << "\n";
    return notes;

}

QString Tnote::getName(EnameStyle nameStyle, bool showOctave)
{
  if (number()) {
    QString nuta;
    switch (nameStyle) {
      case e_italiano_Si:
         nuta = Solmization[m_number-1]+signsAcid[m_accid+2];
         break;
      case e_deutsch_His:
         nuta = Letters[m_number-1];
         switch (m_accid) {
             case e_Natural: break;
             case e_DoubleSharp: nuta = nuta + "isis"; break;
             case e_Sharp: nuta = nuta + "is"; break;
             case e_DoubleFlat: switch (m_number) {
                case 3: nuta = nuta + "ses"; break;
                case 6: nuta = nuta + "sas"; break;
                default: nuta = nuta + "eses"; break;
             }
              break;
             case e_Flat: switch (m_number)	{
                case 3: nuta = nuta + "s"; break;
                case 6: nuta = nuta + "s"; break;
                case 7: nuta = "B"; break;
                default: nuta = nuta + "es"; break;
                }
              break;
          }
          break;
        case e_nederl_Bis:
          nuta = Letters[m_number-1];
          if (m_number == 7) nuta = "B";
          switch( m_accid ){
              case e_Natural: break;
              case e_DoubleSharp: nuta = nuta + "isis"; break;
              case e_Sharp: nuta = nuta + "is"; break;
              case e_DoubleFlat: switch (m_number)	{
                 case 3: nuta = nuta + "ses"; break;
                 case 6: nuta = nuta + "ses"; break;
                 default: nuta = nuta + "eses"; break;
              }
              break;
        case e_Flat:
            switch (m_number) {
                case 3: nuta = nuta + "s"; break;
                case 6: nuta = nuta + "s"; break;
                default: nuta = nuta + "es"; break;
            }
            break;
         }
         break;
         default:
            nuta = Letters[m_number-1];
            if ((nameStyle == e_english_Bb) && (m_number == 7)) nuta = "B";
            nuta = nuta + signsAcid[m_accid+2];
            break;
    }
    if (showOctave) nuta = nuta + QString("%1").arg((int)m_octave);
    return nuta;
  }
  else return "";
}

QString Tnote::getName(Tnote eNote, EnameStyle nameStyle, bool showOctave) {
    m_number = eNote.number();
    m_accid = eNote.accidental();
    m_octave = eNote.octave();
    return getName(nameStyle,showOctave);
}

bool Tnote::operator ==(Tnote N2)
{
    return ( m_number == N2.number() && m_octave == N2.octave() && m_accid == N2.accidental());
}




