/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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

#include "trtaudioin.h"
#include "tpitchfinder.h"
#include <taudioparams.h>
#include <QDebug>


/*static */
QStringList TaudioIN::getAudioDevicesList() {
    QStringList devList;
    createRtAudio();
    int devCnt = rtDevice()->getDeviceCount();
    if (devCnt < 1)
        return devList;
    for (int i = 0; i < devCnt; i++) {
        RtAudio::DeviceInfo devInfo;
        try {
          devInfo = rtDevice()->getDeviceInfo(i);
        }
        catch (RtAudioError& e) {
          qDebug() << "error when probing input device" << i;
          continue;
        }
        if (devInfo.probed && devInfo.inputChannels > 0)
          devList << QString::fromLocal8Bit(devInfo.name.data());
    }
    if (rtDevice()->getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
				devList.prepend("ALSA default");
    return devList;
}


bool TaudioIN::inCallBack(void* inBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
		if (instance()->m_stopped)
				return true;
    if (status)
        qDebug() << "Stream over detected!";
    qint16 *in = (qint16*)inBuff;
		float* inF = (float*)inBuff;
		qint16 value;
    for (int i = 0; i < nBufferFrames; i++) {
					value = *(in + i);
					*(instance()->m_floatBuff + instance()->m_floatsWriten) = float(value) / 32768.0f;
        if (instance()->m_floatsWriten == instance()->m_pitch->aGl()->framesPerChunk - 1) {
						instance()->m_pitch->searchIn(instance()->m_floatBuff);
						instance()->m_floatsWriten = -1;
      }
      instance()->m_floatsWriten++;
    }
    return false;
}

QList<TaudioIN*> 			TaudioIN::m_instances = QList<TaudioIN*>();
int 									TaudioIN::m_thisInstance = -1;

//------------------------------------------------------------------------------------
//------------          constructor     ----------------------------------------------
//------------------------------------------------------------------------------------
TaudioIN::TaudioIN(TaudioParams* params, QObject* parent) :
    QObject(parent),
    TrtAudio(params, e_input, inCallBack),
    m_floatBuff(0),
    m_pitch(0),
    m_floatsWriten(0),
    m_maxPeak(0),
    m_paused(false), m_stopped(true),
    m_lastPich(0.0f)
{
  m_instances << this;
  m_pitch = new TpitchFinder();
  m_thisInstance = m_instances.size() - 1;
  setAudioInParams();
  
  connect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
  connect(m_pitch, SIGNAL(pichInChunk(float)), this, SLOT(pitchInChunkSlot(float)));
  connect(m_pitch, SIGNAL(volume(float)), this, SLOT(volumeSlot(float)));
}

TaudioIN::~TaudioIN()
{
  disconnect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
// 	closeStram();
  delete m_pitch;
  if (m_floatBuff)
    delete (m_floatBuff);
  m_instances.removeLast();
  m_thisInstance = m_instances.size() - 1;
}

//------------------------------------------------------------------------------------
//------------          methods         ----------------------------------------------
//------------------------------------------------------------------------------------

void TaudioIN::setAudioInParams() {
	qDebug() << "setAudioInParams";
  m_pitch->setIsVoice(audioParams()->isVoice);
	setMinimalVolume(audioParams()->minimalVol);
	m_pitch->setMinimalDuration(audioParams()->minDuration);

	m_pitch->setSampleRate(sampleRate(), audioParams()->range); // framesPerChunk is determined here
	delete m_floatBuff;
	m_floatBuff = new float[m_pitch->aGl()->framesPerChunk];
	initInput();
}



void TaudioIN::initInput() {
  m_floatsWriten = 0;
  m_maxPeak = 0;
}


void TaudioIN::startListening() {
    initInput();
		m_stopped = false;
		startStream();
}

void TaudioIN::stopListening() {
		m_stopped = true;
		m_paused = false;
}


void TaudioIN::setMinimalVolume(float minVol) {
		m_pitch->setMinimalVolume(minVol);
		audioParams()->minimalVol = minVol;
}



void TaudioIN::setIsVoice(bool isV) {
  m_pitch->setIsVoice(isV);
}


/** Range of notes is increased one note down and up.
 * This 46 and 48 are its sign. 
 * Normally 47 is offset of midi note to Nootka Tnote. */
void TaudioIN::setAmbitus(Tnote loNote, Tnote hiNote) {
  m_pitch->setAmbitus(loNote.getChromaticNrOfNote() + 46, hiNote.getChromaticNrOfNote() + 48);
	m_loNote = loNote;
	m_hiNote = hiNote;
// 	qDebug() << "Ambitus set to:" << loNote.toText() << "--" << hiNote.toText();
}

//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

void TaudioIN::pitchInChunkSlot(float pitch) {
	if (m_paused)
			return;
  if (pitch == 0.0)
			m_LastChunkPitch = 0.0;
  else
			m_LastChunkPitch = pitch - audioParams()->a440diff;
// 	emit chunkPitch(m_LastChunkPitch);
}


void TaudioIN::pitchFreqFound(float pitch, float freq) {
  if (!m_paused) {
			m_lastPich = pitch - audioParams()->a440diff;
			if (pitch >= m_pitch->aGl()->loPitch && pitch <= m_pitch->aGl()->topPitch) {
					emit noteDetected(Tnote(qRound(pitch - audioParams()->a440diff) - 47));
			}
			emit fundamentalFreq(freq);
  } else 
			m_lastPich = 0.0f;
}


void TaudioIN::volumeSlot(float vol) {
	m_maxPeak = vol;
}



