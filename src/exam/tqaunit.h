/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
		    e_wrongAccid = 1, //occurs during enharmonic conversion
		    e_wrongKey = 2,
		    e_wrongOctave = 4,
		    e_wrongStyle = 8, //for further releases when typing of note name will be implemented
		    e_wrongPos = 16, // when wrong position
        e_wrongString = 32, // when sound is proper but not on required string
		    e_wrongNote = 64, // the highest crime
		    e_wrongIntonation = 128 // when detected sound is out of range of intonation accuracy
    };

        /** Returns string with time divided by 10. 
        * Usually time is stored in value multiplied by 10. 
        * @param prec defines digit number after point. */
    static QString timeToText(int time10, int prec = 1) { return QString("%1").arg((qreal)time10 / 10, 0, 'f', prec); }
        /** Gives ready to insert string with time value. */
    QString timeText() { return timeToText(time); }
        /** Returns time value divided by 10*/
    double getTime() { return (double)time / 10.0; }
    
    void setMistake(Emistake mis);

    TQAgroup qa;
    TQAtype::Etype questionAs;
    TQAtype::Etype answerAs;
        
    Tnote::EnameStyle styleOfQuestion() { return Tnote::EnameStyle(style / 16 - 1);  }
    Tnote::EnameStyle styleOfAnswer() { return Tnote::EnameStyle(style % 16);  }
    void setStyle(Tnote::EnameStyle questionStyle, Tnote::EnameStyle answerStyle) {
      style = ((quint8)questionStyle + 1) * 16 + (quint8)answerStyle;  }
    TkeySignature key;
    quint16 time; // time of answer multiple by 10
    TQAgroup qa_2; // expected answers when question and answer types are the same

    friend QDataStream &operator<< (QDataStream &out, TQAunit &qaUnit);
//    friend QDataStream &operator>> (QDataStream &in, TQAunit &qaUnit);
    friend bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit);
    
    bool isCorrect() { return valid == 0; }
    bool wrongAccid() { return valid & 1; }
    bool wrongKey() { return valid & 2;}
    bool wrongOctave() { return valid & 4; }
    bool wrongStyle() { return valid & 8; }
    bool wrongPos() { return valid & 16; }
    bool wrongString() { return valid & 32; }
    bool wrongNote() {return valid & 64; }
    bool wrongIntonation() {return valid & 128; }
    
    bool isWrong() { return wrongNote() | wrongPos(); }
    bool isNotSoBad() { if (valid && !wrongNote() && !wrongPos()) return true;
                            else return false;
    }
    
protected:
    quint8 valid;
    quint8 style;

};

QDataStream &operator<< (QDataStream &out, TQAunit &qaUnit);
//QDataStream &operator>> (QDataStream &in, TQAunit &qaUnit);
bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit);

#endif // TQAUNIT_H
