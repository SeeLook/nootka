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


#ifndef TQAUNIT_H
#define TQAUNIT_H

#include <nootkacoreglobal.h>
#include <tfingerpos.h>
#include <music/tnote.h>
#include <music/tkeysignature.h>
#include <exam/tqatype.h>

class Tattempt;

class Tmelody;

/** 
 * This class describes single question and given answer.
 * By default a melody element (pointer) is empty and attempts as well.
 * But after @p newAttempt() and @p addMelody() those elements exist inside 
 * until destructor work.
 */
class NOOTKACORE_EXPORT TQAunit
{

public:
	
    TQAunit();
		
		~TQAunit();

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
    QString timeText() { return timeToText(time); } /** Gives ready to insert string with time value. */
    double getTime() { return (double)time / 10.0; } /** Returns time value divided by 10*/
    
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
		void newAttempt(); /** Creates and adds new @class Tattempt to the attempts list. */
		int attemptsCount() { if (m_attempts) return m_attempts->size(); else return 0; }
		Tattempt* attempt(int nr) { return m_attempts->at(nr); } /** Pointer to given attempt */
		Tattempt* lastAtt() { return m_attempts->last(); } /** Pointer to the last attempt */
		
		void addMelody(const QString& title); /** Adds melody of replaces existing one. */
		
		void toXml(QXmlStreamWriter& xml);
		bool formXml(QXmlStreamReader& xml);
    
protected:
    quint8 							 valid;
    quint8 							 style;
		
private:
		Tmelody		 					*m_melody;
		QList<Tattempt*> 		*m_attempts;

};

NOOTKACORE_EXPORT QDataStream &operator<< (QDataStream &out, TQAunit &qaUnit);
NOOTKACORE_EXPORT bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit);

#endif // TQAUNIT_H
