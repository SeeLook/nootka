/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#include "timportscore.h"
#include "tmelody.h"
#include "tchunk.h"
#include "score/tscoreobject.h"

#include <QtCore/qdebug.h>


TimportScore* TimportScore::m_instance = nullptr;


TimportScore::TimportScore(Tmelody* melody, QObject *parent) :
  QObject(parent),
  m_melody(melody)
{
  if (m_instance) {
    qDebug() << "[TimportScore] instance already exists!";
    return;
  }
  m_instance = this;
}


TimportScore::~TimportScore()
{
  m_instance = nullptr;
}


void TimportScore::addNote(int partId, int staff, int voice, int snipp, const Tchunk &note) {
  if (partId == m_parts.count() + 1)
      m_parts << new TmelodyPart(nullptr, partId, staff, voice, snipp);
  else if (partId > m_parts.count()) {
      qDebug() << "[TimportScore] Wrong parts order in the score file!" << partId << m_parts.count();
      return;
  }
  auto staffPart = m_parts[partId - 1];
  if (staff == staffPart->count() + 1)
      staffPart->parts << new TmelodyPart(staffPart, partId, staff, voice, snipp);
  else if (staff > staffPart->count()) {
      qDebug() << "[TimportScore] Wrong staves order in the score file!" << staff << staffPart->count();
      return;
  }
  auto voicePart = staffPart->parts[staff - 1];
  if (voice > voicePart->count()) {
    while (voicePart->count() < voice)
      voicePart->parts << new TmelodyPart(voicePart, partId, staff, voicePart->count() + 1, snipp);
  }
  auto snippPart = voicePart->parts[voice - 1];
  if (snipp == snippPart->count() + 1) {
      snippPart->parts << new TmelodyPart(snippPart, partId, staff, voice, snipp);
  }  else if (snipp > snippPart->count()) {
      qDebug() << "[TimportScore] Problem with snippets order in the score file!";
      return;
  }
  auto currSnipp = snippPart->parts[snipp - 1];
  if (!currSnipp->melody()) {
    auto m = new Tmelody(m_melody->title(), m_melody->key());
//     m->setComposer(m_melody->composer());
    m->setMeter(m_melody->meter()->meter());
    m->setTempo(m_melody->tempo());
    m->setClef(m_melody->clef());
    currSnipp->setMelody(m);
  }
  currSnipp->melody()->addNote(note);
  if (!m_hasMoreParts)
    m_hasMoreParts = partId > 1 || staff > 1 || voice > 1 || snipp > 1;
}


void TimportScore::appendPart(TmelodyPart* p) {
  m_partsModel << p;
}


//#################################################################################################
//###################               TpartMelody        ############################################
//#################################################################################################

TmelodyPart::TmelodyPart(TmelodyPart* parent, int partId, int staffNr, int voiceNr, int snippId) :
  m_parent(parent),
  m_partId(partId),
  m_staffNr(staffNr),
  m_voiceNr(voiceNr),
  m_snippet(snippId)
{}


TmelodyPart::~TmelodyPart()
{
  if (m_melody)
    delete m_melody;
  if (!parts.isEmpty()) {
    qDeleteAll(parts);
  }
}


void TmelodyPart::setMelody(Tmelody* m) {
  if (m != m_melody) {
    if (m_melody == nullptr && IMPORT_SCORE)
      IMPORT_SCORE->appendPart(this);
    m_melody = m;
    emit melodyChanged();
  }
}


void TmelodyPart::setScoreObject(TscoreObject* sObj) {
  m_scoreObj = sObj;
  if (m_melody)
    m_scoreObj->setMelody(m_melody);
}

