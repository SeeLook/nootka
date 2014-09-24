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
	m_effectiveness = 0.0;
}


TQAunit::TQAunit(const TQAunit& otherUnit)
{
	questionAs = otherUnit.questionAs;
	answerAs = otherUnit.answerAs;
	qa = otherUnit.qa;
	setStyle(otherUnit.styleOfQuestion(), otherUnit.styleOfAnswer());
	setMistake(otherUnit.mistake());
	qa_2 = otherUnit.qa_2;
	key = otherUnit.key;
	time = otherUnit.time;
	if (otherUnit.melody() || otherUnit.attemptsCount())
		qDebug() << "TQAunit is going to be copied when pointer inside exist.\nTROUBLES ARE GUARANTEED!\nTo avoid them keep TQAunit instance in some global scope and use reference or pointer to it.";
	m_melody = 0;
	m_attempts = 0;
	m_effectiveness = otherUnit.effectiveness();
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
		case e_fixed : valid |= 256; break;
		case e_wrongNote : valid = 64; break; // If this kind of mistake is committed
        //  all above has no sense so '=' instead '|=' is correct
	}
}


void TQAunit::newAttempt() {
	if (!m_attempts)
		m_attempts = new QList<Tattempt*>;
	(*m_attempts) << new Tattempt();
}


int TQAunit::totalPlayBacks() {
	int result = 0;
	if (m_attempts) {
		for (int i = 0; i < m_attempts->size(); ++i)
			result += m_attempts->at(i)->playedCount();
	}
	return result;
}



void TQAunit::addMelody(const QString& title) {
	if (m_melody)
		delete m_melody;
	m_melody = new Tmelody(title);
}


void TQAunit::updateEffectiveness() {
	if (m_attempts && attemptsCount()) { // melody
		qreal sum = 0.0;
		for (int i = 0; i < attemptsCount(); ++i)
			sum += attempt(i)->effectiveness();
		m_effectiveness = sum / attemptsCount();
	} else { // single note
		m_effectiveness = CORRECT_EFF;
		if (isNotSoBad())
				m_effectiveness = NOTBAD_EFF;
		else if (isWrong())
			m_effectiveness = 0.0;
	}
}


void TQAunit::toXml(QXmlStreamWriter& xml) {
	xml.writeStartElement("u"); //u like unit
		if (qa.note.isValid() || qa.pos.isValid())
			qaGroupToXml(qa, xml);
		xml.writeTextElement("q", QVariant((qint8)questionAs).toString());
		xml.writeTextElement("a", QVariant((qint8)answerAs).toString());
		xml.writeTextElement("s", QVariant(style).toString());
		if (key.value() || key.isMinor()) // skip key signature when C-major (default), fromXml() will guess it
			key.toXml(xml);
		xml.writeTextElement("t", QVariant(time).toString());
		xml.writeTextElement("m", QVariant(mistake()).toString());
	if (qa_2.note.isValid() || qa_2.pos.isValid())
			qaGroupToXml(qa_2, xml, "qa2");
	if (melody()) {
		xml.writeStartElement("melody");
			xml.writeAttribute("title", melody()->title());
			melody()->toXml(xml);
		xml.writeEndElement();
		xml.writeStartElement("attempts");
		for (int i = 0; i < attemptsCount(); ++i) {
			if (!attempt(i)->isEmpty()) // do not save empty attempts
				attempt(i)->toXml(xml);		
		}
		xml.writeEndElement(); // attempts
	}
	xml.writeEndElement(); // u
}

/** We no need to worry about reset TQAunit because it is invoked just after its constructor - it clean yet. */
bool TQAunit::fromXml(QXmlStreamReader& xml) {
	bool ok = true;
	while (xml.readNextStartElement()) {
		if (xml.name() == "qa")
			qaGroupFromXml(qa, xml);
		else if (xml.name() == "qa2")
			qaGroupFromXml(qa_2, xml);
		else if (xml.name() == "q")
			questionAs = (TQAtype::Etype)xml.readElementText().toInt();
		else if (xml.name() == "a")
			answerAs = (TQAtype::Etype)xml.readElementText().toInt();
		else if (xml.name() == "s")
			style = (TQAtype::Etype)xml.readElementText().toInt();
		else if (xml.name() == "key")
			key.fromXml(xml);
		else if (xml.name() == "t")
			time = xml.readElementText().toInt();
		else if (xml.name() == "m")
			valid = xml.readElementText().toInt();
		else if (xml.name() == "melody") {
			addMelody(xml.attributes().value("title").toString());
			if (!melody()->fromXml(xml)) {
				qDebug() << "TQAunit has wrong melody";
				ok = false;
				delete m_melody;
				m_melody = 0;
			}
		} else if (xml.name() == "attempts") {
				while (xml.readNextStartElement()) {
					if (xml.name() == "a") {
						newAttempt();
						lastAttempt()->fromXml(xml);
						if (lastAttempt()->isEmpty()) { // Empty attempts are never written - they never occurs
								qDebug() << "TQAunit has wrong attempt" << attemptsCount();
								ok = false;
								m_attempts->removeLast();
						}
					} else
							xml.skipCurrentElement();
				}
				updateEffectiveness();
		}
		else
			xml.skipCurrentElement();
	}	
	return ok;
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
	quint8 valid_uint8;
	in >> valid_uint8;
	qaUnit.valid = valid_uint8;
//     in >> qaUnit.valid;
	qaUnit.updateEffectiveness();
	return ok;
}

