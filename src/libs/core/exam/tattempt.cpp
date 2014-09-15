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
#include <QVariant>


Tattempt::Tattempt() :
	m_playedCounter(0)
{

}


void Tattempt::add(quint32 mistake, quint32 time) {
	mistakes << mistake;
	if (time)
		times << time;
}


Tattempt::~Tattempt()
{

}


void Tattempt::toXml(QXmlStreamWriter& xml) {
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
	xml.writeEndElement(); // a
}


bool Tattempt::fromXml(QXmlStreamReader& xml) {
	bool ok = true;
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
		else
			xml.skipCurrentElement();
	}
	return ok;
}








