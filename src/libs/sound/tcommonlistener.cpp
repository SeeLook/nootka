/***************************************************************************
 *   Copyright (C) 2015-2021 by Tomasz Bojczuk                             *
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


#include "tcommonlistener.h"
#include "tpitchfinder.h"
#include <taudioparams.h>
#include <tglobals.h>
#include <QtCore/qdir.h>
#include <QtCore/qdatetime.h>

#include <QtCore/qdebug.h>


#define LOWEST_PCM (0.2f) // lowest raw PCM volume to start counting
#define HIGHEST_PCM (0.8f)
#define MAX_OUT_NUM (10)


TcommonListener::TcommonListener(TaudioParams* params, QObject* parent) :
  QObject(parent),
  m_audioParams(params),
  m_volume(0.0f),
  m_stoppedByUser(params->stoppedByUser),
  m_loPitch(15), m_hiPitch(140),
  m_noteWasStarted(false),
  m_currentRange(1),
  m_state(e_stopped)
{
  m_pitchFinder = new TpitchFinder();
#if !defined (Q_OS_ANDROID)
  if (!m_audioParams->dumpPath.isEmpty()) {
      QDir dumpDir(m_audioParams->dumpPath);
      if (dumpDir.exists() && QFileInfo(m_audioParams->dumpPath).isWritable()) {
          /**
           * If dump path exists and it is writable,
           * create sub-directory with time stamp name
           * and set it as dump directory for @p TpitchFinder
           */
          QString subDir = QDateTime::currentDateTime().toString(Qt::ISODate);
          dumpDir.mkpath(subDir);
          m_pitchFinder->setDumpDirPath(m_audioParams->dumpPath + QLatin1String("/") + subDir);
          qDebug() << "Dumping audio data into:" << m_pitchFinder->dumpDirPath();
      } else {
          qDebug() << "Problem with dump directory" << m_audioParams->dumpPath << "\nAudio data will not be dumped!";
      }
  }
#endif
  setAudioInParams();

  connect(finder(), &TpitchFinder::noteStarted, this, &TcommonListener::noteStartedSlot);
  connect(finder(), &TpitchFinder::noteFinished, this, &TcommonListener::noteFinishedSlot);
  connect(finder(), &TpitchFinder::pitchInChunk, this, &TcommonListener::pitchInChunkSlot);
  connect(finder(), &TpitchFinder::volume, this, &TcommonListener::volumeSlot);
}


TcommonListener::~TcommonListener() {
  delete m_pitchFinder;
}


void TcommonListener::setStoppedByUser(bool userStop) {
  m_stoppedByUser = userStop;
  m_audioParams->stoppedByUser = userStop;
}


void TcommonListener::setAudioInParams() {
  setDetectionMethod(m_audioParams->detectMethod);
  setMinimalVolume(m_audioParams->minimalVol);
  finder()->setMinimalDuration(m_audioParams->minDuration);
  finder()->aGl()->equalLoudness = m_audioParams->equalLoudness;
  m_volume = 0.0;
}


void TcommonListener::setMinimalVolume(qreal minVol) {
  finder()->setMinimalVolume(static_cast<float>(minVol));
  m_audioParams->minimalVol = minVol;
}


qreal TcommonListener::minimalVolume() {
  return m_audioParams->minimalVol;
}


qreal TcommonListener::chunkTime() const {
  return m_pitchFinder->chunkTime() * 1000.0;
}


void TcommonListener::setAmbitus(Tnote loNote, Tnote hiNote) {
  m_loNote = Tnote(loNote.chromatic() + m_audioParams->transposition);
  m_hiNote = Tnote(hiNote.chromatic() + m_audioParams->transposition);
  m_loPitch = m_loNote.toMidi() - 1;
  m_hiPitch = m_hiNote.toMidi() + 1;
  TpitchFinder::Erange range = m_loNote.chromatic() > Tnote(5, -2, 0).chromatic() ? TpitchFinder::e_middle : TpitchFinder::e_low;
// TODO: seems like we never use e_high (makes sense for flute maybe) - remove dead code then
//  TpitchFinder::Erange range = TpitchFinder::e_middle;
//  if (loNote.chromatic() > Tnote(6, 0, 0).chromatic())
//    range = TpitchFinder::e_high;
//  else if (loNote.chromatic() > Tnote(5, -2, 0).chromatic())
//    range = TpitchFinder::e_middle;
//  else
//    range = TpitchFinder::e_low;
if (static_cast<int>(range) != m_currentRange) {
    m_currentRange = static_cast<int>(range);
    bool isStop = isStoped();
    stopListening();
    finder()->setSampleRate(finder()->aGl()->rate, m_currentRange);
    if (!isStop)
      startListening();
  }
//   qDebug() << "[TcommonListener] Ambitus set to" << range << m_loNote.toText() << "--" << m_hiNote.toText();
}


void TcommonListener::setDetectionMethod(int method) {
  method = qBound(0, method, 2);
  finder()->aGl()->analysisType = static_cast<EanalysisModes>(method);
  m_audioParams->detectMethod = method;
}


quint8 TcommonListener::intonationAccuracy() {
  return m_audioParams->intonation;
}


void TcommonListener::setIntonationAccuracy(qint8 intAcc) {
  m_audioParams->intonation = qBound<quint8>(0, intAcc, 5);
}


qreal TcommonListener::pcmVolume() {
  return static_cast<qreal>(m_pitchFinder->pcmVolume());
}


#if !defined (Q_OS_ANDROID)
void TcommonListener::setDumpFileName(const QString& fName) {
  m_pitchFinder->setDumpFileName(fName);
}
#endif

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TcommonListener::pitchInChunkSlot(float pitch) {
  if (isPaused())
      return;
  if (pitch == 0.0)
      m_LastChunkPitch = 0.0;
  else
      m_LastChunkPitch = pitch - m_audioParams->a440diff;
}


void TcommonListener::noteStartedSlot(qreal pitch, qreal freq, qreal duration) {
  if (!isPaused()) {
      if (pitch > 0.0) {
          m_lastNote.set(pitch + m_audioParams->a440diff, freq, duration);
          if (inRange(m_lastNote.pitchF)) {
            m_noteWasStarted = true;
            m_lastNote.pitch.transpose(-m_audioParams->transposition);
            emit noteStarted(m_lastNote);
          }
      } else { // zero pitch means rest
          if (GLOB->rhythmsEnabled()) {
            m_noteWasStarted = true;
            m_lastNote.pitch.setNote(0);
            m_lastNote.pitch.rtm.setRest(true);
            m_lastNote.duration = duration;
            emit noteStarted(m_lastNote);
          }
      }
  } else
      m_lastNote.set(); // reset last detected note structure
}


void TcommonListener::noteFinishedSlot(TnoteStruct* lastNote) {
  m_noteWasStarted = false;
  if (!isPaused()) {
      // non guitar pitch is average of all pitches
      qreal midiPitch = lastNote->pitchF > 0.0 ? lastNote->getAverage(3,
                             GLOB->instrument().type() == Tinstrument::NoInstrument ? lastNote->pitches()->size() : finder()->minChunksNumber()) : 0.0;
      m_lastNote.startChunk = lastNote->startChunk;
      m_lastNote.endChunk = lastNote->endChunk;
      if (lastNote->pitchF > 0.0)
        m_lastNote.set(midiPitch - m_audioParams->a440diff, pitch2freq(midiPitch), lastNote->duration);
      else
        m_lastNote.set(0.0, 0.0, lastNote->duration);
      if (lastNote->pitchF > 0.0) {
          if (inRange(m_lastNote.pitchF)) {
            m_lastNote.pitch.transpose(-m_audioParams->transposition);
            emit noteFinished(m_lastNote);
          }
      } else if (GLOB->rhythmsEnabled()) {
          m_lastNote.pitch.rtm.setRest(true);
          emit noteFinished(m_lastNote);
      }

//       if (lastNote->maxPCMvol < LOWEST_PCM) {
//           m_hiPCMnumber = 0;
//           m_loPCMnumber++;
//           if (m_loPCMnumber > MAX_OUT_NUM) {
//             emit lowPCMvolume();
//             m_loPCMnumber = 0;
//           }
//       } else if (lastNote->maxPCMvol > HIGHEST_PCM) {
//           m_loPCMnumber = 0;
//           m_hiPCMnumber++;
//           if (m_hiPCMnumber > MAX_OUT_NUM) {
//             emit hiPCMvolume();
//             m_hiPCMnumber = 0;
//           }
//       } else { // reset counters
//           m_hiPCMnumber = 0;
//           m_loPCMnumber = 0;
//       }
  } else
      m_lastNote.set(); // reset last detected note structure
}



