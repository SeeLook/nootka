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


//#################################################################################################
//###################                STATIC            ############################################
//#################################################################################################
QStringList TmidiIn::getMidiInPorts() {
  RtMidiIn *midiIn = nullptr;
  try {
      midiIn = new RtMidiIn();
  } catch ( RtMidiError &error ) {
      qDebug() << "No MIDI input devices available";
  }

  QStringList portList;
  if (midiIn && midiIn->getPortCount()) {
    for (int i = 0; i < midiIn->getPortCount(); i++)
      portList << QString::fromStdString(midiIn->getPortName(i));
  }
  delete midiIn;
  return portList;
}


//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################
TmidiIn::TmidiIn(TaudioParams *params, QObject *parent) :
  TcommonListener(params, parent)
{
}


TmidiIn::~TmidiIn()
{
}


void TmidiIn::setMidiParams() {
  deleteMidi();
  try {
      m_midiIn = new RtMidiIn(RtMidi::UNSPECIFIED, "Nootka_MIDI_In");
  } catch (RtMidiError &error) {
      qDebug() << "[TmidiOut] Can't initialize MIDI input";
      return;
  }

  if (m_midiIn && m_midiIn->getPortCount() > 0) {
    m_portNr = 0;
    if (!p_audioParams->midiPortName.isEmpty()) {
      for (int i = 0; i < m_midiIn->getPortCount(); i++) {
        if (QString::fromStdString(m_midiIn->getPortName(i)).contains(p_audioParams->midiPortName)) {
          m_portNr = i;
          break;
        }
      }
    }

    openMidiPort();
    qDebug() << "[TmidiOut] MIDI device:" << p_audioParams->midiPortName << "instrument:" << static_cast<int>(p_audioParams->midiInstrNr);
  }
}


void TmidiIn::deleteMidi() {
  delete m_midiIn;
  m_midiIn = nullptr;
}


//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################
void TmidiIn::openMidiPort() {
}
