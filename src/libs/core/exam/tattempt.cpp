/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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

#include "tattempt.h"
#include "tqaunit.h"
#include <QtCore/qvariant.h>


Tattempt::Tattempt() :
  m_playedCounter(0),
  m_sum(0),
  m_totalTime(0),
  m_prepTime(0)
{
}


void Tattempt::add(quint32 mistake) {
  mistakes << mistake;
  m_sum |= mistake;
}


Tattempt::~Tattempt()
{}


void Tattempt::updateEffectiveness() {
  if (mistakes.size()) {
      qreal effSum = 0.0;
      m_sum = 0;
      for (int i = 0; i < mistakes.size(); ++i) {
        quint32 m = mistakes[i];
        m_sum |= m;
        if (m == TQAunit::e_correct)
          effSum += CORRECT_EFF;
        else if (!(m & TQAunit::e_wrongNote) && !(m & TQAunit::e_wrongPos))
          effSum += NOTBAD_EFF;
      }
      m_effectiveness = effSum / (qreal)mistakes.size();
  } else
      m_effectiveness = 0.0;

  if (effectiveness() >= 50.0) {
    if (m_sum & TQAunit::e_wrongNote) { // subtract e_wrongNote if summary has sufficient effectiveness
      m_sum = m_sum - TQAunit::e_wrongNote; // attempt was successful
      m_sum |= TQAunit::e_littleNotes; // but has little valid notes
    }
  }
}


void Tattempt::toXml(QXmlStreamWriter& xml) const {
  xml.writeStartElement(QLatin1String("a")); // a like attempt
    if (mistakes.size()) {
      xml.writeStartElement(QLatin1String("mistakes"));
      for (int i = 0; i < mistakes.size(); ++i)
        xml.writeTextElement(QLatin1String("m"), QVariant(mistakes[i]).toString());
      xml.writeEndElement(); // mistakes
    }
    if (m_playedCounter)
      xml.writeTextElement(QLatin1String("p"), QVariant(m_playedCounter).toString());
    if (m_totalTime)
      xml.writeTextElement(QLatin1String("tt"), QVariant(m_totalTime).toString());
    if (m_prepTime)
      xml.writeTextElement(QLatin1String("pt"), QVariant(m_prepTime).toString());
  xml.writeEndElement(); // a
}


void Tattempt::fromXml(QXmlStreamReader& xml) {
  m_playedCounter = 0;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("mistakes")) {
        mistakes.clear();
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("m"))
            mistakes << xml.readElementText().toInt();
          else
            xml.skipCurrentElement();
        }
        updateEffectiveness();
    } else if (xml.name() == QLatin1String("p"))
        m_playedCounter = xml.readElementText().toInt();
    else if (xml.name() == QLatin1String("tt"))
        m_totalTime = xml.readElementText().toInt();
    else if (xml.name() == QLatin1String("pt"))
        m_prepTime = xml.readElementText().toInt();
    else
      xml.skipCurrentElement();
  }
}








