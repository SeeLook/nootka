/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk            *
 *   tomaszbojczuk@gmail.com                 *
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
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "taudioout.h"
#include "taudioparams.h"
#include "tnote.h"
#include "RtMidi.h"
#include <QTimer>
#include <QFile>
#include <QStringList>


//------------------ static methods ------------------------------------------------------
QStringList TaudioOUT::getAudioDevicesList() {
	templAudioFormat.setChannelCount(2);
  templAudioFormat.setSampleSize(16);
  templAudioFormat.setSampleRate(44100);
  templAudioFormat.setSampleType(QAudioFormat::SignedInt);
  templAudioFormat.setCodec("audio/pcm");
  QStringList devList;
  QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].isFormatSupported(templAudioFormat))
            devList << dL[i].deviceName();
    }
  return devList;

}

QStringList TaudioOUT::getMidiPortsList() {
  RtMidiOut *midiOut = 0;
  try {
    midiOut = new RtMidiOut();
  }
  catch ( RtError &error ) {
      error.printMessage();
  }
  QStringList portList;
  if (midiOut->getPortCount())
    for (int i = 0; i < midiOut->getPortCount(); i++)
      portList << QString::fromStdString(midiOut->getPortName(i));
  delete midiOut;
  return portList;  
}


//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams* params, QObject* parent) :
  QObject(parent)
{

}

TaudioOUT::~TaudioOUT()
{

}


//---------------------------------------------------------------------------------------
void TaudioOUT::midiNoteOff() {
  m_midiTimer->stop();
  m_message[0] = 128; // note Off
  m_message[1] = m_prevMidiNote;
  m_message[2] = 0; // volume
  m_midiOut->sendMessage(&m_message);
  m_prevMidiNote = 0;
}



