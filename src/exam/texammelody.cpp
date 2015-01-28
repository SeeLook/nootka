/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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


#include "texammelody.h"
#include <music/tnotestruct.h>
#include <QDebug>


TexamMelody::TexamMelody(QObject* parent) :
  QObject(parent)
{
}


TexamMelody::~TexamMelody()
{}


void TexamMelody::newMelody(int length) {
  m_listened.clear();
  if (length) {
    for (int i = 0; i < length; ++i) {
      m_listened << TnoteStruct(Tnote(), 0);
    }
  }
  m_currentIndex = -1;
  m_indexChanged = false;
  m_numberOfFixed = 0;
}


void TexamMelody::noteStarted() {
  m_indexChanged = false;
  m_currentIndex++;
  if (m_currentIndex >= listened().size()) {
    m_currentIndex = listened().size() - 1;
    qDebug() << "TexamMelody reached end of melody.";
  }
}


void TexamMelody::setNote(const TnoteStruct& n) {
  m_listened[m_currentIndex] = n;
}


void TexamMelody::setCurrentIndex(int id) {
  if (!m_listened.isEmpty()) {
    if (id >= 0 && id < m_listened.size()) {
      m_currentIndex = id - 1; // decrease it because noteStarted() will increase it again
      m_indexChanged = true;
    } else
        qDebug() << "TexamMelody Index out of range!";
  } else {
    qDebug() << "TexamMelody list is empty, cannot change index!";
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




