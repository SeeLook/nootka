/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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

#include "tmidiout.h"
#include <taudioparams.h>
#include "rt/RtMidi.h"
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


//------------------ static methods ------------------------------------------------------
QStringList TmidiOut::getMidiPortsList()
{
  RtMidiOut *midiOut = nullptr;
  try {
      midiOut = new RtMidiOut();
  } catch ( RtMidiError &error ) {
      qDebug() << "no midi devices available";
  }

  QStringList portList;
  if (midiOut && midiOut->getPortCount()) {
    for (int i = 0; i < midiOut->getPortCount(); i++)
      portList << QString::fromStdString(midiOut->getPortName(i));
  }
  delete midiOut;
  return portList;
}


TmidiOut::TmidiOut(TaudioParams* params, QObject* parent) :
  TabstractPlayer(parent),
  m_midiOut(nullptr),
  m_prevMidiNote(0),
  m_portOpened(false)
{
  p_audioParams = params;
  setType(e_midi);
  m_offTimer = new QTimer(this);
  m_offTimer->setSingleShot(true);
  moveToThread(playThread());
  setMidiParams();
  connect(m_offTimer, &QTimer::timeout, this, &TmidiOut::midiNoteOff, Qt::DirectConnection);
  connect(this, &TmidiOut::wantStop, this, &TmidiOut::stopSlot);
}


TmidiOut::~TmidiOut()
{
  deleteMidi();
}


void TmidiOut::setMidiParams() {
  deleteMidi();
  p_playable = true;
  try {
      m_midiOut = new RtMidiOut(RtMidi::UNSPECIFIED, "Nootka_MIDI_out");
  } catch (RtMidiError &error) {
      qDebug() << "[TmidiOut] Can't initialize MIDI";
      p_playable = false;
      return;
  }

  if (m_midiOut && m_midiOut->getPortCount() > 0) {
      m_portNr = 0;
#if defined(Q_OS_LINUX)
      if (p_audioParams->midiPortName.isEmpty())
        p_audioParams->midiPortName = QStringLiteral("TiMidity");  // TiMidity port is prefered under Linux
#endif
      if (!p_audioParams->midiPortName.isEmpty()) {
        for (int i = 0; i < m_midiOut->getPortCount(); i++) {
          if (QString::fromStdString(m_midiOut->getPortName(i)).contains(p_audioParams->midiPortName)) {
            m_portNr = i;
            break;
          }
        }
      }

      openMidiPort();
      qDebug() << "[TmidiOut] MIDI device:" << p_audioParams->midiPortName << "instrument:" << static_cast<int>(p_audioParams->midiInstrNr);
  } else
      p_playable = false;
}


void TmidiOut::deleteMidi() {
//   qDebug("deleteMidi");
  if (m_midiOut) {
    if (m_offTimer->isActive())
      m_offTimer->stop();
    if (m_portOpened)
      m_midiOut->closePort();
    m_portOpened = false;
    delete m_midiOut;
    m_midiOut = nullptr;
  }
  p_playable = false;
}


void TmidiOut::startPlaying() {
  if (p_isPlaying) {
    p_doEmit = false;
    midiNoteOff();
  }
  p_playingNoteNr = 0;
  p_isPlaying = true;
  emit playingStarted();
  playLoop();
}


void TmidiOut::playLoop() {
  if (p_playingNoteNr < playList().count()) {
    int noteToPlay = playList()[p_playingNoteNr].number;
      if (noteToPlay == REST_NR) {
        noteToPlay = 0;
      }
      p_playingNoteId = playList()[p_playingNoteNr].id;
      play(noteToPlay);
      m_offTimer->start(playList()[p_playingNoteNr].samplesCount);
      p_doEmit = false;
      if (playList().size() > 1)
        emit nextNoteStarted();
  } else {
      p_doEmit = true;
      midiNoteOff();
  }
  p_playingNoteNr++;
}


bool TmidiOut::play (int noteNr) {
  if (!p_playable)
    return false;

  if (!m_portOpened)
    openMidiPort();
  p_doEmit = true;
  int semiToneOff = 0; // entire semitone offset
  quint16 midiBend = 0;
  if (p_audioParams->a440diff != 0.0) {
    semiToneOff = static_cast<int>(p_audioParams->a440diff);
    float fineOff = qAbs(p_audioParams->a440diff) - qAbs(static_cast<float>(semiToneOff)); // float part of offset
    if (fineOff) { // if exist midi bend message is needed
      if (p_audioParams->a440diff < 0) // restore bend direction
        fineOff *= -1;
      midiBend = 8192 + static_cast<quint16>(qRound(4192.0 * fineOff)); // calculate 14-bit bend value
    }
  }
  m_prevMidiNote = noteNr + 47 + semiToneOff;
  m_message[0] = 144; // note On
  m_message[1] = m_prevMidiNote;
  m_message[2] = 100; // volume
  m_midiOut->sendMessage(&m_message);
  if (midiBend) { // let's send bend message
    char msb, lsb;
    lsb = (char)(midiBend % 128); // calculate 7 bits lsb&msb
    msb = (char)(midiBend / 128);
    m_message[0] = 224; // pitch bend
    m_message[1] = lsb;
    m_message[2] = msb;
    m_midiOut->sendMessage(&m_message);
  }
//   if (m_offTimer->isActive())
//     m_offTimer->stop();
//   m_offTimer->start(1500);
  return true;
}


void TmidiOut::stop() {
  emit wantStop();
}


void TmidiOut::stopSlot() {
  if (m_offTimer->isActive()) {
    playThread()->exit();
    m_offTimer->stop();
    p_playingNoteNr = playList().count();
    p_doEmit = false;
    midiNoteOff();
  }
}


void TmidiOut::openMidiPort() {
    if (m_portOpened)
      return;
    try {
        m_midiOut->openPort(m_portNr, "Nootka_MIDI_out");
    } catch (RtMidiError &error) {
        qDebug() << "[TmidiOut] Can't open MIDI port";
        p_playable = false;
        return;
    }
    m_portOpened = true;
    p_audioParams->midiPortName = QString::fromStdString(m_midiOut->getPortName(m_portNr));
    // midi program (instrument) change
    m_message.clear();
    m_message.push_back(192);
    m_message.push_back(p_audioParams->midiInstrNr); // instrument number
    m_midiOut->sendMessage(&m_message);
    // some spacial signals
    m_message[0] = 241;
    m_message[1] = 60;
    m_midiOut->sendMessage(&m_message);

    m_message.push_back(0); // third message param

    m_message[0] = 176;
    m_message[1] = 7;
    m_message[2] = 100; // volume 100;
    m_midiOut->sendMessage(&m_message);
}

//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################

void TmidiOut::midiNoteOff() {
  m_offTimer->stop();
  m_message[0] = 128; // note Off
  m_message[1] = m_prevMidiNote;
  m_message[2] = 0; // volume
  try {
      m_midiOut->sendMessage(&m_message);
  } catch (RtMidiError &error) {
      qDebug() << "[TmidiOut] Can't send MIDI message to fade sound out";
  }
  m_prevMidiNote = 0;
  if (p_playingNoteNr < playList().count()) {
      playLoop();
      return;
  } else {
      p_isPlaying = false;
      playThread()->exit();
  }
//   if (m_portOpened) {
//       try {
//         m_midiOut->closePort();
//       } catch (RtMidiError &error){
//         qDebug() << "can't close MIDI port";
//       }
//         m_portOpened = false;
//     }
  if (p_doEmit)
    emit playingFinished();
}


