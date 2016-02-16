/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include "texam.h"
#include <music/tmelody.h>
#include <cmath>
#include <QXmlStreamReader>
#include <QDebug>

//######################################################################
//#######################    CONSTRUCTORS    ###########################
//######################################################################


TQAunit::TQAunit(Texam* exam)
{
	qa.pos = TfingerPos();
	qa.note = Tnote(0,0,0);
	style = 50; // (2+1) * 16 + 2 // 2 is e_italiano_Si cast to int
	valid = 0; // correct in assume
	time = 0;
	qa_2.note = Tnote(0,0,0);
	qa_2.pos = TfingerPos();
	m_melody = 0;
	attemptList = 0;
	m_effectiveness = 0.0;
	m_exam = exam;
	m_srcMelody = e_noMelody;
	idOfMelody = -1;
	m_answered = false;
}


void TQAunit::copy(const TQAunit& otherUnit) {
  questionAs = otherUnit.questionAs;
  answerAs = otherUnit.answerAs;
  qa = otherUnit.qa;
  setStyle(otherUnit.styleOfQuestion(), otherUnit.styleOfAnswer());
  setMistake(otherUnit.mistake());
  qa_2 = otherUnit.qa_2;
  key = otherUnit.key;
  time = otherUnit.time;
  if (otherUnit.melody() || otherUnit.attemptsCount()) {
    deleteMelody();
    m_melody = 0;
    attemptList = 0;
    idOfMelody = -1;
    m_srcMelody = e_noMelody;
  } else {
    m_melody = otherUnit.melody();
    attemptList = otherUnit.attemptList;
    idOfMelody = otherUnit.idOfMelody;
    m_srcMelody = otherUnit.melodySource();
  }
  m_answered = otherUnit.answered();
  m_effectiveness = otherUnit.effectiveness();
  m_exam = otherUnit.exam();
}


TQAunit::~TQAunit()
{
	if (attemptList) {
		for (int i = 0; i < attemptList->size(); ++i)
			delete attemptList->at(i);
		delete attemptList;
	}
	deleteMelody();
}

//######################################################################
//#######################       PUBLIC       ###########################
//######################################################################

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
		case e_littleNotes : valid |= 256; break;
		case e_poorEffect : valid |= 512; break;
		case e_veryPoor : valid = 1024; break; // If this kind of mistakes are committed...
		case e_wrongNote : valid = 64; break; //  all above has no sense so '=' instead '|=' is correct
	}
}


void TQAunit::newAttempt() {
	if (!attemptList)
		attemptList = new QList<Tattempt*>;
	(*attemptList) << new Tattempt();
}


int TQAunit::totalPlayBacks() {
	int result = 0;
	if (attemptList) {
		for (int i = 0; i < attemptList->size(); ++i)
			result += attemptList->at(i)->playedCount();
	}
	return result;
}



void TQAunit::addMelody(const QString& title) {
	deleteMelody();
	m_srcMelody = e_thisUnit;
	m_melody = new Tmelody(title);
}


void TQAunit::addMelody(Tmelody* mel, TQAunit::EmelodySrc source, int id) {
	deleteMelody();
	m_srcMelody = source;
	m_melody = mel;
	idOfMelody = id;
}



void TQAunit::updateEffectiveness() {
	if (attemptList && attemptsCount()) { // melody
		double fee = pow(ATTEMPT_FEE, (double)(attemptsCount() - 1));  // decrease effectiveness by 4% for every additional attempt
		m_effectiveness = lastAttempt()->effectiveness() * fee; // first attempt - power = 0, fee is 1
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
    if (time == 0)
      qDebug() << "Answer time is 0 - faster than light speed?";
		xml.writeTextElement("m", QVariant(mistake()).toString());
		if (!answered()) // in most cases saved unit is answered
			xml.writeTextElement("answered", QVariant(answered()).toString());
		if (qa_2.note.isValid() || qa_2.pos.isValid())
				qaGroupToXml(qa_2, xml, "qa2");
		if (melody()) {
			xml.writeStartElement("melody");
				if (m_srcMelody == e_thisUnit) {
						xml.writeAttribute("title", melody()->title());
						melody()->toXml(xml);
				} else if (m_srcMelody == e_otherUnit)
						xml.writeAttribute("qNr", QVariant(idOfMelody).toString());
				else if (m_srcMelody == e_list)
						xml.writeAttribute("id", QVariant(idOfMelody).toString());
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

/** We no need to worry about reset TQAunit because it is invoked just after its constructor - it is clean already. */
bool TQAunit::fromXml(QXmlStreamReader& xml) {
	bool ok = true;
	m_answered = true; // this state is common and not saved, so if <answered> key exists it is false then.
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
		else if (xml.name() == "answered")
			m_answered = QVariant(xml.readElementText()).toBool(); // or m_answered = false
		else if (xml.name() == "melody") {
			if (xml.attributes().hasAttribute("title")) {
				addMelody(xml.attributes().value("title").toString());
				if (!melody()->fromXml(xml)) {
					qDebug() << "TQAunit has wrong melody";
					ok = false;
					delete m_melody;
					m_melody = 0;
				}
			} else if (xml.attributes().hasAttribute("qNr")) {
					int qNr = xml.attributes().value("qNr").toInt();
					if (qNr < m_exam->count())
						addMelody(m_exam->answList()->at(qNr)->melody(), e_otherUnit, qNr);
					else {
						ok = false;
						qDebug() << "TQAunit has a melody that points to question number which doesn't exist in exam list.";
					}
				xml.skipCurrentElement();
			}
		} else if (xml.name() == "attempts") {
				while (xml.readNextStartElement()) {
					if (xml.name() == "a") {
						newAttempt();
						lastAttempt()->fromXml(xml);
						if (lastAttempt()->isEmpty()) { // Empty attempts are never written - they never occurs
								qDebug() << "TQAunit has wrong attempt" << attemptsCount();
								ok = false;
								attemptList->removeLast();
						}
					} else
							xml.skipCurrentElement();
				}
		}
		else
			xml.skipCurrentElement();
	}
	updateEffectiveness();
	return ok;
}


void TQAunit::deleteMelody() {
	if (m_melody && m_srcMelody == e_thisUnit)
		delete m_melody;
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

