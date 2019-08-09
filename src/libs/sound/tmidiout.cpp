/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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
  RtMidiOut *midiOut = 0;
  try {
    midiOut = new RtMidiOut();
  }
  catch ( RtMidiError &error ) {
    qDebug() << "no midi devices available";
  }
  QStringList portList;
  if (midiOut && midiOut->getPortCount())
    for (int i = 0; i < midiOut->getPortCount(); i++)
      portList << QString::fromStdString(midiOut->getPortName(i));
  delete midiOut;
  return portList;
}


TmidiOut::TmidiOut(TaudioParams* params, QObject* parent) :
  TabstractPlayer(parent),
  m_midiOut(nullptr),
  m_params(params),
  m_prevMidiNote(0),
  m_portOpened(false)
{
    setType(e_midi);
    offTimer = new QTimer();
    setMidiParams();
    if (p_playable)
        connect(offTimer, SIGNAL(timeout()), this, SLOT(midiNoteOff()));
}


TmidiOut::~TmidiOut()
{
  deleteMidi();
  delete offTimer;
}


void TmidiOut::setMidiParams() {
  deleteMidi();
  offTimer->disconnect();
  p_playable = true;
  try {
    m_midiOut = new RtMidiOut(RtMidi::UNSPECIFIED, "Nootka_MIDI_out");
  }
  catch ( RtMidiError &error ) {
    qDebug() << "can't initialize MIDI";
    p_playable = false;
    return;
  }

  if (m_midiOut && m_midiOut->getPortCount() > 0) {
      m_portNr = 0;
#if defined(Q_OS_LINUX)
      if(m_params->midiPortName == "")
      m_params->midiPortName = "TiMidity";  // TiMidity port is prefered under Linux
#endif
      if (m_params->midiPortName != "") {
          for (int i = 0; i < m_midiOut->getPortCount(); i++) {
            if (QString::fromStdString(m_midiOut->getPortName(i)).contains(m_params->midiPortName)) {
              m_portNr = i;
              break;
            }
          }
      }

      openMidiPort();
      qDebug() << "midi device:" << m_params->midiPortName << "instr:" << (int)m_params->midiInstrNr;
  } else
      p_playable = false;
}


void TmidiOut::deleteMidi() {
//   qDebug("deleteMidi");
  if (m_midiOut) {
    if (offTimer->isActive())
      offTimer->stop();
    if (m_portOpened)
      m_midiOut->closePort();
    m_portOpened = false;
    delete m_midiOut;
    m_midiOut = nullptr;
  }
  p_playable = false;
}


bool TmidiOut::play(int noteNr) {
  if (!p_playable)
      return false;
  if (m_prevMidiNote) {  // note is played and has to be turned off. Volume is pushed.
      p_doEmit = false;
      midiNoteOff();
  }
  if (!m_portOpened)
    openMidiPort();
  p_doEmit = true;
  int semiToneOff = 0; // "whole" semitone offset
  quint16 midiBend = 0;
  if (m_params->a440diff != 0.0) {
      semiToneOff = (int)m_params->a440diff;
      float fineOff = qAbs(m_params->a440diff) - qAbs((float)semiToneOff); // float part of offset
      if (fineOff) { // if exist midi bend message is needed
          if (m_params->a440diff < 0) // restore bend direction
            fineOff *= -1;
          midiBend = 8192 + (quint16)qRound(4192.0 * fineOff); // calculate 14-bit bend value
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
  if (offTimer->isActive())
      offTimer->stop();
  offTimer->start(1500);
  return true;
}


void TmidiOut::stop() {
  if (offTimer->isActive()) {
    offTimer->stop();
    p_doEmit = false;
    midiNoteOff();
  }
}


void TmidiOut::openMidiPort() {
    if (m_portOpened)
      return;
    try {
          m_midiOut->openPort(m_portNr, "Nootka_MIDI_out");
      }
      catch (RtMidiError &error){
          qDebug() << "can't open MIDI port";
          p_playable = false;
          return;
      }
      m_portOpened = true;
      m_params->midiPortName = QString::fromStdString(m_midiOut->getPortName(m_portNr));
      // midi program (instrument) change
      m_message.clear();
      m_message.push_back(192);
      m_message.push_back(m_params->midiInstrNr); // instrument number
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



//---------------------------------------------------------------------------------------
//-------------------------------- slots ------------------------------------------------
//---------------------------------------------------------------------------------------

void TmidiOut::midiNoteOff() {
//   qDebug("midiNoteOff");
  offTimer->stop();
  m_message[0] = 128; // note Off
  m_message[1] = m_prevMidiNote;
  m_message[2] = 0; // volume
  try {
    m_midiOut->sendMessage(&m_message);
  } catch (RtMidiError &error){
    qDebug() << "can't send MIDI message to fade sound out";
  }
  m_prevMidiNote = 0;
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


