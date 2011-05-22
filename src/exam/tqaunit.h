/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TQAUNIT_H
#define TQAUNIT_H
#include "tfingerpos.h"
#include "tnote.h"
#include "tqatype.h"
#include "tkeysignature.h"


    /** This class describes single question and given answer.*/
class TQAunit
{
public:
    TQAunit();

    struct TQAgroup {
        TfingerPos pos;
        Tnote note;
    };

    enum Emistake { e_correct = 0,
		    e_wrongAccid = 1, //ocurs during enharmonic conversion
		    e_wrongKey = 2,
		    e_wrongOctave = 4,
		    e_wrongStyle = 8, //for further releases when typeing of note name will be implemented
		    e_wrongPos = 16, // when sound is correct but in wrong position
                
		    e_wrongNote = 64 // the highest crime  
    };

    TQAgroup qa;
    TQAtype::Etype questionAs;
    TQAtype::Etype answerAs;
    Tnote::EnameStyle style;
    TkeySignature key;
    Emistake correct;
    quint16 time;
//    Tnote note2; // second note for case when question and answer are notes in a score.
    // pos and note as answers
    
    bool isCorrect() { return correct == 0; }
    bool wrongAccid() { return correct & 1; }
    bool wrongKey() { return correct & 2;}
    bool wrongOctave() { return correct & 4; }
    bool wrongStyle() { return correct & 8; }
    bool wrongPos() { return correct & 16; } 
    bool wrongNote() {return correct & 64; }
    
    
};

#endif // TQAUNIT_H
