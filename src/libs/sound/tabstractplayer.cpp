/***************************************************************************
 *   Copyright (C) 2013-2020 by Tomasz Bojczuk                             *
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
#include <tpath.h>

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#include <QtCore/qfile.h>
#include <QtCore/qdatastream.h>

#if defined (Q_OS_WIN)
  #include <fcntl.h>
  #include <io.h>
#endif

#include <QtCore/qdebug.h>
#include "checktime.h"


TplayerThread::TplayerThread(TabstractPlayer* pl) :
  QThread(pl),
  m_player(pl)
{
}


void TplayerThread::run() {
  if (!m_playList.isEmpty() && m_player->p_playingNoteNr <= m_playList.size() - 1 && m_player->p_posInNote < m_playList[m_player->p_playingNoteNr].samplesCount) {
      // something still is playing
      m_player->p_prevNote = m_playList[m_player->p_playingNoteNr].number;
  } else {
      m_player->p_prevNote = -100;
      m_player->p_shiftOfPrev = 0;
      m_player->p_lastPosOfPrev = 0;
  }
  m_player->p_playingNoteNr = 0;
  m_player->p_decodingNoteNr = 0;
  m_player->p_doEmit = true;
  m_playList.clear();
  if (m_noteToPlay != REST_NR) { // single note
      playList() << TsingleSound(0, m_noteToPlay + static_cast<int>(m_player->p_audioParams->a440diff), qRound(m_player->p_oggScale->sampleRate() * 1.5));
      m_noteToPlay = REST_NR;
  } else if (m_listToPlay) { // note list (i.e. score notes)
      preparePlayList(m_listToPlay, m_player->p_tempo, m_firstNote, m_player->p_oggScale->sampleRate(),
                      GLOB->transposition(), static_cast<int>(m_player->p_audioParams->a440diff));
      m_listToPlay = nullptr;
  } else if (m_melodyToPlay) { // melody (from Tmelody)
      qreal rateFactor = m_player->p_oggScale->sampleRate() / 1000.0;
      if (m_player->p_countdownDur > 0)
        playList() << TsingleSound(CNTDWN_ID, REST_NR, qRound(((m_player->p_countdownDur / 24.0) * (60000.0 / m_melodyToPlay->quarterTempo())) * rateFactor));
      for (int n = 0; n < m_melodyToPlay->length(); ++n) {
        const Tnote& tmpN = m_melodyToPlay->note(n)->p();
        int samplesDur =
          qRound(((tmpN.duration() > 0 ? tmpN.duration() / 24.0 : 1.0) * (60000.0 / m_melodyToPlay->quarterTempo())) * rateFactor);
        if (tmpN.rtm.tie() > Trhythm::e_tieStart) { // append duration if tie is continued or at end
            if (playList().isEmpty())
              continue; // do not start playing in the middle of tied notes
              playList().last().samplesCount += samplesDur;
        } else
            playList() << TsingleSound(n,
                                      tmpN.isValid() ? tmpN.chromatic() + GLOB->transposition() + m_transposition + m_player->p_audioParams->a440diff : REST_NR,
                                      samplesDur
                                      );
      }
      m_melodyToPlay = nullptr;
  }

  if (!playList().isEmpty()) { // naughty user might want to play tied note at the score end
      m_player->p_playingNoteId = playList().first().id;
      emit audioDataReady();
  }
}


void TplayerThread::preparePlayList(QList<Tnote>* notes, int tempo, int firstNote, int sampleRate, int transposition, int a440diff) {
  playList().clear();
  if (m_player->p_countdownDur > 0)
    playList() << TsingleSound(CNTDWN_ID, REST_NR, qRound(((m_player->p_countdownDur / 24.0) * (60000.0 / tempo)) * (sampleRate / 1000.0)));
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



//#################################################################################################
//###################        TabstractPlayer class     ############################################
//#################################################################################################

unsigned int            TabstractPlayer::p_posInNote = 0;
unsigned int            TabstractPlayer::p_posInOgg = 0;
int                     TabstractPlayer::p_playingNoteNr = -1;
int                     TabstractPlayer::p_decodingNoteNr = -1;
int                     TabstractPlayer::p_playingNoteId = -1;
int                     TabstractPlayer::p_prevNote = -100;
unsigned int            TabstractPlayer::p_shiftOfPrev = 0;
unsigned int            TabstractPlayer::p_lastPosOfPrev = 0;

unsigned int            TabstractPlayer::p_beatPeriod = 0;
unsigned int            TabstractPlayer::p_beatBytes = 7984; // beat file frames number (initial, finally obtained from file)
unsigned int            TabstractPlayer::p_beatOffset = 0;
bool                    TabstractPlayer::p_lastNotePlayed = false;
int                     TabstractPlayer::p_ticksCountBefore = 0;

QString                 TabstractPlayer::soundTouchVersion() { return SOUNDTOUCH_VERSION; }


TabstractPlayer::TabstractPlayer(QObject* parent) :
  QObject(parent),
  p_playable(false),
  p_doEmit(false)
{
  m_playThreaad = new TplayerThread(this);
  connect(m_playThreaad, &TplayerThread::audioDataReady, this, &TabstractPlayer::startPlaying, Qt::DirectConnection);
  // Invoke startPlaying in this thread to keep main thread working smooth
}


TabstractPlayer::~TabstractPlayer() {
  m_playThreaad->quit();
  m_playThreaad->wait();
  if (m_beatArray)
    delete[] m_beatArray;
}


bool TabstractPlayer::playNote(int noteNr) {
  if (!p_playable)
    return false;
  m_playThreaad->wait();
  m_playThreaad->setNoteToPLay(noteNr);
  m_playThreaad->start();
  return true;
}


bool TabstractPlayer::playNotes(QList<Tnote>* notes, int tempo, int firstNote, int countdownDur) {
  if (!p_playable)
    return false;
  m_playThreaad->wait();
  m_playThreaad->setListToPlay(notes);
  p_tempo = tempo;
  m_playThreaad->setFirstNote(firstNote);
  p_countdownDur = countdownDur;
  m_playThreaad->start();
  return true;
}


bool TabstractPlayer::playMelody(Tmelody* melody, int transposition, int countdownDur) {
  if (!p_playable)
    return false;
  m_playThreaad->wait();
  m_playThreaad->setMelodyToPlay(melody);
  m_playThreaad->setTransposition(transposition);
  p_countdownDur = countdownDur;
  m_playThreaad->start();
  return true;
}


/**
 * TODO: there is no any re-sampling, for 44100/48000 is bearable but for higher rates could be funny
 */
void TabstractPlayer::setMetronome(unsigned int beatTempo) {
  if (!m_beatArray) {
    QFile beatFile(Tpath::sound("beat", ".raw48-16"));
    if (!beatFile.exists()) {
      qDebug() << "[TabstractPlayer] Can't open metronome beat file" << beatFile.fileName();
      p_beatPeriod = 0; // disable metronome
      return;
    }
    beatFile.open(QIODevice::ReadOnly);
    QDataStream beatStream(&beatFile);
    m_beatArray = new qint16[beatFile.size() / 2];
    p_beatBytes = beatStream.readRawData(reinterpret_cast<char*>(m_beatArray), beatFile.size()) / 2;
    beatFile.close();
  }
  p_beatOffset = 0;
  p_beatPeriod = beatTempo ? (p_oggScale->sampleRate() * 60) / beatTempo : 0;
}


void TabstractPlayer::stopMetronome() {
  p_beatPeriod = 0;
  p_ticksCountBefore = 0; // stop counting before as well
}


bool TabstractPlayer::tickBeforePlay() const { return p_audioParams && p_audioParams->countBefore; }
void TabstractPlayer::setTickBeforePlay(bool tbp) {
  if (p_audioParams)
    p_audioParams->countBefore = tbp;
}



bool TabstractPlayer::tickDuringPlay() const { return p_audioParams && p_audioParams->audibleMetro; }
void TabstractPlayer::setTickDuringPlay(bool tdp) {
  if (p_audioParams)
    // TODO wait for busy callback to avoid cracks
    p_audioParams->audibleMetro = tdp;
}


bool TabstractPlayer::doTicking() const {
  return p_audioParams && (p_audioParams->audibleMetro || p_audioParams->countBefore);
}


void TabstractPlayer::setPitchOffset(qreal pitchOff) {
  p_oggScale->setPitchOffset(pitchOff);
}


qint16 TabstractPlayer::mix(qint16 sampleA, qint16 sampleB) {
//   return static_cast<qint16>((static_cast<qint32>(sampleA) + static_cast<qint32>(sampleB)) / 2); It cracks
  qint32 a32 = static_cast<qint32>(sampleA), b32 = static_cast<qint32>(sampleB);
  if (sampleA < 0 && sampleB < 0 )
    return static_cast<qint16>((a32 + b32) - ((a32 * b32) / INT16_MIN));
  else if (sampleA > 0 && sampleB > 0 )
    return static_cast<qint16>((a32 + b32) - ((a32 * b32) / INT16_MAX));
  else
    return sampleA + sampleB;
}
