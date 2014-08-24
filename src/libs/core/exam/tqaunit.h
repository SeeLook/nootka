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
 * WARRING! Melody (Tmelody) and Attempts list objects are never copied with operator=
 */
class NOOTKACORE_EXPORT TQAunit
{

public:
	
    TQAunit();
		TQAunit(const TQAunit& otherUnit);
		
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
		
				/** Sets mistakes from value. It is the same:
				 * setMistake(e_wrongKey); setMistake(e_wrongOctave);
				 * and 
				 * setMistake(e_wrongKey | e_wrongOctave);
				 * or
				 * setMistake(6);	 */
		void setMistake(quint8 misVal) { valid = misVal; }
		quint8 mistake() const { return valid; } /** set of mistakes as Boolean sum of Emistake */

    TQAgroup qa;
    TQAtype::Etype questionAs;
    TQAtype::Etype answerAs;
        
    Tnote::EnameStyle styleOfQuestion() const { return Tnote::EnameStyle(style / 16 - 1);  }
    Tnote::EnameStyle styleOfAnswer() const { return Tnote::EnameStyle(style % 16);  }
    void setStyle(Tnote::EnameStyle questionStyle, Tnote::EnameStyle answerStyle) {
      style = ((quint8)questionStyle + 1) * 16 + (quint8)answerStyle;  }
    TkeySignature key;
    quint16 time; // time of answer multiple by 10
    TQAgroup qa_2; // expected answers when question and answer types are the same

    friend QDataStream &operator<< (QDataStream &out, TQAunit &qaUnit);
    friend bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit);
    
    bool isCorrect() const { return valid == 0; }
    bool wrongAccid() const { return valid & 1; }
    bool wrongKey() const { return valid & 2; }
    bool wrongOctave() const { return valid & 4; }
    bool wrongStyle() const { return valid & 8; }
    bool wrongPos() const { return valid & 16; }
    bool wrongString() const { return valid & 32; }
    bool wrongNote() const {return valid & 64; }
    bool wrongIntonation() const {return valid & 128; }
    
    bool questionAsNote() const { return questionAs == TQAtype::e_asNote; } /** questionAs == TQAtype::e_asNote; */
    bool questionAsName() const { return questionAs == TQAtype::e_asName; } /** questionAs == TQAtype::e_asName; */
    bool questionAsFret() const { return questionAs == TQAtype::e_asFretPos; } /** questionAs == TQAtype::e_asFretPos; */
    bool questionAsSound() const { return questionAs == TQAtype::e_asSound; } /** questionAs == TQAtype::e_asSound; */
    bool answerAsNote() const { return answerAs == TQAtype::e_asNote; } /** answerAs == TQAtype::e_asNote; */
    bool answerAsName() const { return answerAs == TQAtype::e_asName; } /** answerAs == TQAtype::e_asName; */
    bool answerAsFret() const { return answerAs == TQAtype::e_asFretPos; } /** answerAs == TQAtype::e_asFretPos; */
    bool answerAsSound() const { return answerAs == TQAtype::e_asSound; } /** answerAs == TQAtype::e_asSound; */
    
    bool isWrong() const { return wrongNote() | wrongPos(); }
    bool isNotSoBad() const { if (valid && !wrongNote() && !wrongPos()) return true;
																else return false;
											}
		void newAttempt(); /** Creates and adds new @class Tattempt to the attempts list. */
		int attemptsCount() const { if (m_attempts) return m_attempts->size(); else return 0; }
		Tattempt* attempt(int nr) { return m_attempts->operator[](nr); } /** Pointer to given attempt */
		Tattempt* lastAttepmt() { return m_attempts->last(); } /** Pointer to the last attempt */
		
		void addMelody(const QString& title); /** Adds melody of replaces existing one. */
		Tmelody* melody() const { return m_melody; }
		
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
