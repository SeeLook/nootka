/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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
#include "toggscale.h"
#include <music/tnote.h>
#include <music/tmelody.h>
#include <taudioparams.h>
#include <tglobals.h>
#include <QtCore/qthread.h>

#include <QtCore/qdebug.h>


unsigned int            TabstractPlayer::p_posInNote = 0;
unsigned int            TabstractPlayer::p_posInOgg = 0;
int                     TabstractPlayer::p_playingNoteNr = -1;
int                     TabstractPlayer::p_decodingNoteNr = -1;
int                     TabstractPlayer::p_playingNoteId = -1;


TabstractPlayer::TabstractPlayer(QObject* parent) :
  QObject(parent),
  p_playable(false),
  p_doEmit(false)
{
  m_thread = new QThread();
  moveToThread(m_thread);
  connect(m_thread, &QThread::started, this, &TabstractPlayer::playThreadSlot);
}


TabstractPlayer::~TabstractPlayer() {
  if (m_thread->isRunning()) {
    m_thread->terminate();
    m_thread->wait();
  }
  delete m_thread;
}


bool TabstractPlayer::playNote(int noteNr) {
  if (!p_playable)
    return false;
  m_thread->wait();
  m_noteToPlay = noteNr;
  m_thread->start();
  return true;
}


bool TabstractPlayer::playNotes(QList<Tnote>* notes, int tempo, int firstNote) {
  if (!p_playable)
    return false;
  m_thread->wait();
  m_listToPlay = notes;
  p_tempo = tempo;
  m_firstNote = firstNote;
  m_thread->start();
  return true;
}


bool TabstractPlayer::playMelody(Tmelody* melody, int transposition) {
  if (!p_playable)
    return false;
  m_thread->wait();
  m_melodyToPlay = melody;
  m_transposition = transposition;
  m_thread->start();
  return true;
}


void TabstractPlayer::preparePlayList(QList<Tnote>* notes, int tempo, int firstNote, int sampleRate, int transposition, int a440diff) {
  playList().clear();
  for (int n = firstNote; n < notes->count(); ++n) {
    const Tnote& tmpN = notes->at(n);
    int samplesDuration = qRound(((tmpN.duration() > 0 ? tmpN.duration() / 24.0 : 1.0) * (60000.0 / tempo)) * (sampleRate / 1000.0));
    if (tmpN.rtm.tie() > Trhythm::e_tieStart) { // append duration if tie is continued or at end
      if (playList().isEmpty())
        continue; // do not start playing in the middle of tied notes
        playList().last().samplesCount += samplesDuration;
    } else
        playList() << TsingleSound(n, tmpN.isValid() ? tmpN.chromatic() + transposition + a440diff : REST_NR, samplesDuration);
  }
}


void TabstractPlayer::playThreadSlot() {
  p_playingNoteNr = 0;
  p_decodingNoteNr = 0;
  p_posInNote = 0;
  p_posInOgg = 0;
  p_doEmit = true;
  m_playList.clear();
  if (m_noteToPlay != REST_NR) { // single note
      playList() << TsingleSound(0, m_noteToPlay + static_cast<int>(p_audioParams->a440diff), qRound(p_oggScale->sampleRate() * 1.5));
      m_noteToPlay = REST_NR;
  } else if (m_listToPlay) { // note list (i.e. score notes)
      preparePlayList(m_listToPlay, p_tempo, m_firstNote, p_oggScale->sampleRate(), GLOB->transposition(), static_cast<int>(p_audioParams->a440diff));
      m_listToPlay = nullptr;
  } else if (m_melodyToPlay) { // melody (from Tmelody)
      for (int n = 0; n < m_melodyToPlay->length(); ++n) {
        const Tnote& tmpN = m_melodyToPlay->note(n)->p();
        int samplesDuration = qRound(((tmpN.duration() > 0 ? tmpN.duration() / 24.0 : 1.0) * (60000.0 / m_melodyToPlay->tempo())) * (p_oggScale->sampleRate() / 1000.0));
        if (tmpN.rtm.tie() > Trhythm::e_tieStart) { // append duration if tie is continued or at end
            if (playList().isEmpty())
              continue; // do not start playing in the middle of tied notes
              playList().last().samplesCount += samplesDuration;
        } else
            playList() << TsingleSound(n,
                                       tmpN.isValid() ? tmpN.chromatic() + GLOB->transposition() + m_transposition + p_audioParams->a440diff : REST_NR,
                                       samplesDuration);
      }
      m_melodyToPlay = nullptr;
  }
  if (!playList().isEmpty()) { // naughty user might want to play tied note at the score end
    p_playingNoteId = playList().first().id;
    startPlaying();
  }
  m_thread->quit();
}




