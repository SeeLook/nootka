/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tattempt.h"
#include "tqaunit.h"
#include <QVariant>


Tattempt::Tattempt() :
	m_playedCounter(0),
	m_sum(0),
	m_totalTime(0)
{
}


void Tattempt::add(quint32 mistake, quint32 time) {
	mistakes << mistake;
	if (time)
		times << time;
	m_sum |= mistake;
}


Tattempt::~Tattempt()
{
}


void Tattempt::updateEffectiveness() {
	if (mistakes.size()) {
		qreal effSum = 0.0;
		m_sum = 0;
		for (int i = 0; i < mistakes.size(); ++i) {
			m_sum |= mistakes[i];
			if (mistakes[i] == TQAunit::e_correct)
					effSum += CORRECT_EFF;
			else if (!(mistakes[i] & TQAunit::e_wrongNote) && !(mistakes[i] & TQAunit::e_wrongPos))
					effSum += NOTBAD_EFF;
		}
		m_effectiveness = effSum / (qreal)mistakes.size();
	} else
		m_effectiveness = 0.0;
}


void Tattempt::toXml(QXmlStreamWriter& xml) const {
	xml.writeStartElement("a"); // a like attempt
		if (mistakes.size()) {
			xml.writeStartElement("mistakes");
			for (int i = 0; i < mistakes.size(); ++i)
				xml.writeTextElement("m", QVariant(mistakes[i]).toString());
			xml.writeEndElement(); // mistakes
		}
		if (times.size()) {
			xml.writeStartElement("times");
			for (int i = 0; i < times.size(); ++i)
				xml.writeTextElement("t", QVariant(times[i]).toString());
			xml.writeEndElement(); // times
		}
		if (m_playedCounter)
			xml.writeTextElement("p", QVariant(m_playedCounter).toString());
		if (m_totalTime)
			xml.writeTextElement("tt", QVariant(m_totalTime).toString());
	xml.writeEndElement(); // a
}


void Tattempt::fromXml(QXmlStreamReader& xml) {
	m_playedCounter = 0;
	while (xml.readNextStartElement()) {
		if (xml.name() == "mistakes") {
			mistakes.clear();
			while (xml.readNextStartElement()) {
				if (xml.name() == "m")
					mistakes << xml.readElementText().toInt();
				else
					xml.skipCurrentElement();
			}
			updateEffectiveness();
		} else if (xml.name() == "times") {
			times.clear();
			while (xml.readNextStartElement()) {
				if (xml.name() == "t")
					times << xml.readElementText().toInt();
				else
					xml.skipCurrentElement();
			}
		} else if (xml.name() == "p")
				m_playedCounter = xml.readElementText().toInt();
		else if (xml.name() == "tt")
				m_totalTime = xml.readElementText().toInt();
		else
			xml.skipCurrentElement();
	}
}








