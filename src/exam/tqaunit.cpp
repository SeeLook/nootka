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


#include "tqaunit.h"

TQAunit::TQAunit()
{
    qa.pos = TfingerPos();
    qa.note = Tnote(0,0,0);
    style = 50; // (2+1) * 16 + 2 // 2 is e_italiano_Si casted to int
    valid = 0; // correct a priori
    qa_2.note = Tnote(0,0,0);
    qa_2.pos = TfingerPos();
}

TQAunit::~TQAunit() {}

void TQAunit::setMistake(Emistake mis) {
    switch (mis) {
    case e_correct : valid = 0; break;
    case e_wrongAccid : valid |= 1; break;
    case e_wrongKey : valid |= 2; break;
    case e_wrongOctave : valid |= 4; break;
    case e_wrongStyle : valid |= 8; break;
    case e_wrongPos : valid |= 16; break;
    case e_wrongString : valid |= 32; break;
    case e_wrongNote : valid = 64; break; // If this kind of mistake is commited
        //  all above has no sence so '=' instead '|=' is correct
    }
}

QDataStream &operator<< (QDataStream &out, TQAunit &qaUnit) {
    out << qaUnit.qa.note << qaUnit.qa.pos;
    out << (qint8)qaUnit.questionAs << (qint8)qaUnit.answerAs;
    out << qaUnit.style;
    out << qaUnit.key;
    out << qaUnit.time;
    out << qaUnit.qa_2.note << qaUnit.qa_2.pos;
    out << qaUnit.valid;
    return out;
}

//QDataStream &operator>> (QDataStream &in, TQAunit &qaUnit) {
//}

bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit) {
    bool ok = true;
    ok = getNoteFromStream(in, qaUnit.qa.note);
    in >> qaUnit.qa.pos;
    qint8 qu, an;
    in >> qu >> an;
    qaUnit.questionAs = (TQAtype::Etype)qu;
    qaUnit.answerAs = (TQAtype::Etype)an;
    in >> qaUnit.style;
    ok = getKeyFromStream(in, qaUnit.key);
    in >> qaUnit.time;
  // getNoteFromStream is too smart and doesn't allow null Tnote(0,0,0)
  // I have to cheat it....
    bool ok2 = getNoteFromStream(in, qaUnit.qa_2.note);
    if (!ok2)
        qaUnit.qa_2.note = Tnote(0,0,0);
    in >> qaUnit.qa_2.pos;
    in >> qaUnit.valid;
    return ok;
}

