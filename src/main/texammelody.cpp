/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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


#include "texammelody.h"
#include <music/tmelody.h>

#include <QtCore/qdebug.h>


TexamMelody::TexamMelody(QObject* parent) :
  QObject(parent)
{
}


TexamMelody::~TexamMelody()
{}


void TexamMelody::newMelody(Tmelody *m, int transposition) {
  m_playedList.clear();
  m_realList.clear();
  m_tieOrRests = false;
  if (m) {
    int i = 0;
    while (i < m->length()) {
      m_playedList << TnoteStruct(Tnote(), 0.0);
      int listenedId = m_playedList.count() - 1;
      m_realList << listenedId;
      if (m->note(i)->p().isRest()) { // skip all rest after this one
          int j = i + 1;
          while (j < m->length() && m->note(j)->p().isRest()) {
            if (m->note(j)->p().isRest())
              m_realList << listenedId;
            j++;
            i++;
          }
      } else if (m->note(i)->p().rtm.tie()) {
          int j = i + 1;
          while (j < m->length() && m->note(j)->p().rtm.tie() && m->note(j)->p().rtm.tie() != Trhythm::e_tieStart) {
            if (m->note(j)->p().rtm.tie() != Trhythm::e_tieStart)
              m_realList << listenedId;
            j++;
            i++;
          }
      }
      i++;
    }
    if (m_realList.count() != m->length())
      qDebug() << "[TexamMelody] FIXME! Melody length and real list doesn't match";
  }
  m_currentIndex = -1;
  m_indexOfSaved = -1;
  m_indexChanged = false;
  m_numberOfFixed = 0;

  m_toPlayList.clear();
  if (m) {
    m_toPlayList = TnoteToPlay::fromMelody(m, transposition);
    if (m->length() > m_toPlayList.size())
      m_tieOrRests = true;
  }
  if (m_toPlayList.size() != m_playedList.size())
    qDebug() << "[TexamMelody] FIXME! Lists have different sizes!";
}


void TexamMelody::noteStarted() {
  if (m_indexOfSaved < m_currentIndex)
    return;

  m_indexChanged = false;
  m_currentIndex++;
  if (m_currentIndex >= m_playedList.size()) {
    m_currentIndex = m_playedList.size() - 1;
    qDebug() << "[TexamMelody] reached end of melody.";
  }
}


void TexamMelody::setNote(const TnoteStruct& n) {
  if (m_currentIndex < 0 || m_currentIndex > m_playedList.count() - 1) {
    qDebug() << "[TexamMelody::setNote] note index out of range" << m_currentIndex;
    return;
  }
  m_playedList[m_currentIndex] = n; // TODO use simpler struc to store data, just copy it here
  m_indexOfSaved = m_currentIndex;
}


void TexamMelody::setCurrentIndex(int realId) {
  if (!m_playedList.isEmpty()) {
      if (realId >= 0 && realId < m_playedList.size()) {
          m_currentIndex = 
          m_realList[realId] - 1; // decrease it because noteStarted() will increase it again
          m_indexChanged = true;
          m_indexOfSaved = m_currentIndex;
      } else
          qDebug() << "[TexamMelody::setCurrentIndex] Index out of range!";
  } else {
      qDebug() << "[TexamMelody::setCurrentIndex] list is empty, cannot change index!";
  }
}


void TexamMelody::setFixed(int noteNr) {
  if (!m_attemptFix[noteNr]) // if not fixed yet
    m_numberOfFixed++; // increase number of fixed
  m_attemptFix[noteNr] = true;
}


void TexamMelody::clearToFix(int notesCount) {
  m_attemptFix.clear();
  for (int i = 0 ; i < notesCount; ++i)
    m_attemptFix << false;
}




