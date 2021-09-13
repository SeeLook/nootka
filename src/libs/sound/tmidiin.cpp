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

#include "tmidiin.h"
#include "taudioparams.h"
#include "rt/RtMidi.h"

#include <QtCore/qdebug.h>


#define MIDI_NOTE_ON (144)
#define MIDI_NOTE_OFF (128)

//#################################################################################################
//###################                STATIC            ############################################
//#################################################################################################
QStringList TmidiIn::getMidiInPorts() {
  RtMidiIn *midiIn = nullptr;
  try {
      midiIn = new RtMidiIn();
  } catch (RtMidiError &error) {
      qDebug() << "[TmidiIn] No MIDI devices available!" << QString::fromStdString(error.getMessage());
  }

  QStringList portList;
  if (midiIn && midiIn->getPortCount()) {
    for (int i = 0; i < midiIn->getPortCount(); i++)
      portList << QString::fromStdString(midiIn->getPortName(i));
  }
  delete midiIn;
  return portList;
}


void TmidiIn::midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
  Q_UNUSED(userData)
  if (message->size() > 1) {
    if (message->at(0) == MIDI_NOTE_ON) {
        Tnote n;
        n.fromMidi(message->at(1));
        qDebug() << "MIDI note started" << n.toText();
    } else if (message->at(0) == MIDI_NOTE_OFF) {
        qDebug() << "MIDI note finished" << deltatime;
    }
  }
}


//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################
TmidiIn::TmidiIn(TaudioParams *params, QObject *parent) :
  TcommonListener(params, parent)
{
  p_snifferType = e_midi;
  setMidiParams();
}


TmidiIn::~TmidiIn()
{
  deleteMidi();
}


void TmidiIn::setMidiParams() {
  deleteMidi();
  try {
      m_midiIn = new RtMidiIn(RtMidi::UNSPECIFIED, "Nootka MIDI input");
  } catch (RtMidiError &error) {
      qDebug() << "[TmidiIn] Can't initialize MIDI input";
      return;
  }

  if (m_midiIn && m_midiIn->getPortCount() > 0) {
    m_portNr = 0;
    if (!p_audioParams->midiOutPortName.isEmpty()) {
      for (int i = 0; i < m_midiIn->getPortCount(); i++) {
        if (QString::fromStdString(m_midiIn->getPortName(i)).contains(p_audioParams->midiOutPortName)) {
          m_portNr = i;
          break;
        }
      }
    }

    openMidiPort();
    qDebug() << "[TmidiIn] MIDI device:" << p_audioParams->midiInPortName;
  }
}


void TmidiIn::deleteMidi() {
  if (m_midiIn) {
    delete m_midiIn;
    m_midiIn = nullptr;
  }
}


void TmidiIn::startListening() {
  openMidiPort();
  m_midiIn->setCallback(midiCallback);
}


void TmidiIn::stopListening() {
  m_midiIn->cancelCallback();
  m_midiIn->closePort();
}

//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################
void TmidiIn::openMidiPort() {
  if (m_midiIn->isPortOpen())
    return;

  try {
      m_midiIn->openPort(m_portNr, "Nootka MIDI input");
  } catch (RtMidiError &error) {
      qDebug() << "[TmidiOut] Cannot open MIDI input port";
      return;
  }
  p_audioParams->midiInPortName = QString::fromStdString(m_midiIn->getPortName(m_portNr));
  m_midiIn->ignoreTypes(false, false, false);
}
