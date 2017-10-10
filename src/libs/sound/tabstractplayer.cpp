/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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


#include "tabstractplayer.h"
#include <music/tnote.h>


unsigned int            TabstractPlayer::p_posInNote = 0;
unsigned int            TabstractPlayer::p_posInOgg = 0;
int                     TabstractPlayer::p_playingNoteNr = -1;
int                     TabstractPlayer::p_decodingNoteNr = -1;
int                     TabstractPlayer::p_playingNoteId = -1;


TabstractPlayer::TabstractPlayer(QObject* parent) :
  QObject(parent),
  playable(false),
  doEmit(false)
{
}

void TabstractPlayer::deleteMidi() {}


void TabstractPlayer::setMidiParams() {}


void TabstractPlayer::preparePlayList(const QList<Tnote>& notes, int tempo, int firstNote, int sampleRate, int transposition, int a440diff) {
  playList().clear();
  for (int n = firstNote; n < notes.count(); ++n) {
    const Tnote& tmpN = notes.at(n);
    int samplesDuration = qRound(((tmpN.duration() > 0 ? tmpN.duration() / 24.0 : 1.0) * (60000.0 / tempo)) * (sampleRate / 1000.0));
    if (tmpN.rtm.tie() > Trhythm::e_tieStart) { // append duration if tie is continued or at end
      if (playList().isEmpty())
        continue; // do not start playing in the middle of tied notes
        playList().last().samplesCount += samplesDuration;
    } else
      playList() << TsingleSound(n, tmpN.isValid() ? tmpN.chromatic() + transposition + a440diff : REST_NR, samplesDuration);
  }
}





