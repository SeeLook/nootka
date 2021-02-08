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

#include "tgotit.h"
#include <main/tstartexamitem.h>
#include <tsound.h>
#include <score/tscoreobject.h>

// #include <QtCore/qdebug.h>


TgotIt::TgotIt(QObject *parent) :
  QObject(parent)
{
}


TgotIt::~TgotIt() {
  if (m_gotItType == GotSoundInfo) {
    SOUND->stopListen();
    SOUND->setTunerMode(false);
    SOUND->startListen();
  }
}


void TgotIt::setGotItType(TgotIt::EgotItType gt) {
  if (gt != m_gotItType) {
    switch (gt) {
      case GotSoundInfo:
        SOUND->stopListen();
        SOUND->setTunerMode(true);
        SOUND->startListen();
        break;
      default:
        break;
    }
    m_gotItType = gt;
    emit gotItTypeChanged();
  }
}


void TgotIt::setScore(TscoreObject *sc) {
  m_score = sc;
}


void TgotIt::setCursorAlter(int curAlt) {
  if (m_score)
    m_score->setCursorAlter(curAlt);
}


void TgotIt::setWorkRtmValue(int rtmV) {
  if (m_score)
    m_score->setWorkRtmValue(rtmV);
}


QString TgotIt::exerOrExamHelpTxt(bool withHeader) {
  return TstartExamItem::exerOrExamHelpTxt(withHeader);
}
