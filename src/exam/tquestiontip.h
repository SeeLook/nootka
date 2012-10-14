/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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
 
#ifndef TQUESTIONTIP_H
#define TQUESTIONTIP_H

#include "tgraphicstexttip.h"
#include "tnote.h"

class Texam;
class TexamLevel;
class TQAunit;


class TquestionTip : public TgraphicsTextTip
{
   Q_OBJECT
   
public:
        /** Constructs tip with question content. */
    TquestionTip(Texam *exam, double scale = 0);
    ~TquestionTip();
    
    static QString getTextHowAccid(Tnote::Eacidentals accid);
        /** Returns translated text on (strNr) string in Nootka font. */
    static QString onStringTxt(quint8 strNr);
    
    bool freeScore() { return m_scoreFree; } // true when question is not on score
    bool freeName() { return m_nameFree; } // true when question is not on note name
    bool freeGuitar() { return m_guitarFree; } // true when question is not on guitar
    
protected:    
        /** Returns html-formated question text. */
    QString getQuestion(TQAunit &question, int questNr, TexamLevel *level, double scale = 0);
    QString getNiceNoteName(Tnote note, Tnote::EnameStyle style);
        
    
private:
    bool m_scoreFree, m_nameFree, m_guitarFree; // Indicate where has to be tip
    Tnote::Eacidentals m_forcedAccid; // When different than Tnote::e_Natural text is shown
};


#endif // TQUESTIONTIP_H