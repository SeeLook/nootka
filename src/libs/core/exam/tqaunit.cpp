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


#include "tqaunit.h"
#include "tattempt.h"
#include <music/tmelody.h>
#include <QXmlStreamReader>
#include <QDebug>

TQAunit::TQAunit()
{
	qa.pos = TfingerPos();
	qa.note = Tnote(0,0,0);
	style = 50; // (2+1) * 16 + 2 // 2 is e_italiano_Si cast to int
	valid = 0; // correct in assume
	qa_2.note = Tnote(0,0,0);
	qa_2.pos = TfingerPos();
	m_melody = 0;
	m_attempts = 0;
}


TQAunit::TQAunit(const TQAunit& otherUnit)
{
	questionAs = otherUnit.questionAs;
	answerAs = otherUnit.answerAs;
	qa = otherUnit.qa;
	setStyle(otherUnit.styleOfQuestion(), otherUnit.styleOfAnswer());
	setMistake(otherUnit.mistake());
	qa_2 = otherUnit.qa_2;
	if (otherUnit.melody() || otherUnit.attemptsCount())
		qDebug() << "TQAunit is going to be copied when pointer inside exist.\nTROUBLES ARE GUARANTEED!\nTo avoid them keep TQAunit instance in some global scope and use reference or pointer to it.";
	m_melody = 0;
	m_attempts = 0;
}


TQAunit::~TQAunit()
{
	if (m_attempts) {
		for (int i = 0; i < m_attempts->size(); ++i)
			delete m_attempts->at(i);
		delete m_attempts;
	}
	if (m_melody)
		delete m_melody;
}


void TQAunit::setMistake(Emistake mis) {
	switch (mis) {
    case e_correct : valid = 0; break;
    case e_wrongAccid : valid |= 1; break;
    case e_wrongKey : valid |= 2; break;
    case e_wrongOctave : valid |= 4; break;
    case e_wrongStyle : valid |= 8; break;
    case e_wrongPos : valid |= 16; break;
    case e_wrongString : valid |= 32; break;
		case e_wrongIntonation : valid |= 128; break;
    case e_wrongNote : valid = 64; break; // If this kind of mistake is committed
        //  all above has no sense so '=' instead '|=' is correct
	}
}


void TQAunit::newAttempt() {
	if (!m_attempts)
		m_attempts = new QList<Tattempt*>;
	(*m_attempts) << new Tattempt();
}


void TQAunit::addMelody(const QString& title) {
	if (m_melody)
		delete m_melody;
	m_melody = new Tmelody(title);
}


void TQAunit::toXml(QXmlStreamWriter& xml)
{

}


bool TQAunit::formXml(QXmlStreamReader& xml)
{
	bool ok = true;

	return ok;
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

