/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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
#include "taudioparams.h"
#include <tinitcorelib.h>
#if defined(Q_OS_WIN)
  #include "tasioemitter.h"
#endif



/*static */
QStringList TaudioIN::getAudioDevicesList() {
	QStringList devList;
	createRtAudio();
  if (getCurrentApi() == RtAudio::LINUX_ALSA)
      closeStream(); // close ALSA stream to get full list of devices
	int devCnt = getDeviceCount();
	if (devCnt < 1)
			return devList;
	for (int i = 0; i < devCnt; i++) {
			RtAudio::DeviceInfo devInfo;
			if (!getDeviceInfo(devInfo, i))
				continue;
			if (devInfo.probed && devInfo.inputChannels > 0)
				devList << convDevName(devInfo);
	}
	if (getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
			devList.prepend("ALSA default");
	return devList;
}


TaudioIN*        			TaudioIN::m_instance = 0;
bool                  TaudioIN::m_goingDelete = false;

//#################################################################################################
//###################              CONSTRUCTOR         ############################################
//#################################################################################################

TaudioIN::TaudioIN(TaudioParams* params, QObject* parent) :
  QObject(parent),
  TrtAudio(params, e_input, inCallBack),
  m_pitch(0),
  m_volume(0.0),
  m_stoppedByUser(false),
  m_loPitch(15), m_hiPitch(140),
  m_noteWasStarted(false),
  m_currentRange(1),
  m_state(e_stopped)
{
  if (m_instance) {
    qDebug() << "Nothing of this kind... TaudioIN already exist!";
    return;
  }
  m_instance = this;
  m_pitch = new TpitchFinder();
  setAudioInParams();
	m_goingDelete = false;
	forceUpdate = true;
  
	connect(m_pitch, &TpitchFinder::noteStarted, this, &TaudioIN::noteStartedSlot);
	connect(m_pitch, &TpitchFinder::noteFinished, this, &TaudioIN::noteFinishedSlot);
	connect(m_pitch, &TpitchFinder::pitchInChunk, this, &TaudioIN::pitchInChunkSlot);
	connect(m_pitch, &TpitchFinder::volume, this, &TaudioIN::volumeSlot);
	connect(ao(), &TaudioObject::paramsUpdated, this, &TaudioIN::updateSlot);
  connect(ao(), &TaudioObject::playingFinished, this, &TaudioIN::playingFinishedSlot);
}

TaudioIN::~TaudioIN()
{
  stopListening();
	m_goingDelete = true;
  closeStream();
	m_pitch->blockSignals(true);
// 	m_pitch->deleteLater();
  delete m_pitch;
  m_instance = 0;
  deleteInParams();
  resetCallBack();
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TaudioIN::setAudioInParams() {
  setDetectionMethod(audioParams()->detectMethod);
	setMinimalVolume(audioParams()->minimalVol);
	m_pitch->setMinimalDuration(audioParams()->minDuration);
  m_pitch->setSplitByVolChange(audioParams()->minSplitVol > 0.0);
  m_pitch->setSplitVolume(audioParams()->minSplitVol / 100.0);
  m_pitch->setSkipStillerVal(audioParams()->skipStillerVal / 100.0);
  m_pitch->aGl()->equalLoudness = audioParams()->equalLoudness;

	m_pitch->setSampleRate(inRate(), m_currentRange); // framesPerChunk is determined here
	m_volume = 0.0;
#if defined(Q_OS_WIN)
  if (getCurrentApi() == RtAudio::WINDOWS_ASIO)
    connect(rtDevice()->emitter(), &TASIOEmitter::resetASIO, this, &TaudioIN::ASIORestartSlot, Qt::UniqueConnection);
#endif
//   qDebug() << "setAudioInParams" << "\nrate:" << sampleRate() << "\nmethod:" << audioParams()->detectMethod
//            << "\nmin duration" << audioParams()->minDuration << "\nmin volume" << audioParams()->minimalVol
//            << "\nsplit volume" << (m_pitch->isSplitByVolume() ? m_pitch->minVolumeToSplit() * 100.0 : 0.0)
//            << "\nskip volume" << m_pitch->skipStillerValue() * 100.0
//            << "\nnoise filter:" << m_pitch->aGl()->equalLoudness;
}


void TaudioIN::setMinimalVolume(float minVol) {
	m_pitch->setMinimalVolume(minVol);
	audioParams()->minimalVol = minVol;
}


/**
 * Range of notes is increased semitone down and up.
 * This 46 and 48 are its sign.
 * Normally 47 is offset of midi note to Nootka Tnote. 
*/
void TaudioIN::setAmbitus(Tnote loNote, Tnote hiNote) {
	m_loPitch = loNote.toMidi() - 1;
	m_hiPitch = hiNote.toMidi() + 1;
	m_loNote = loNote;
	m_hiNote = hiNote;
	TpitchFinder::Erange range = loNote.chromatic() > Tnote(5, -2, 0).chromatic() ? TpitchFinder::e_middle : TpitchFinder::e_low;
// 	if (loNote.chromatic() > Tnote(6, 0, 0).chromatic())
// 		range = TpitchFinder::e_high;
// 	else if (loNote.chromatic() > Tnote(5, -2, 0).chromatic())
// 		range = TpitchFinder::e_middle;
// 	else
// 		range = TpitchFinder::e_low;
	if (static_cast<int>(range) != m_currentRange) {
		m_currentRange = static_cast<int>(range);
		bool isStop = isStoped();
		stopListening();
		m_pitch->setSampleRate(m_pitch->aGl()->rate, m_currentRange);
		if (!isStop)
			startListening();
	}
// 	qDebug() << "Ambitus set to:" << Tnote(m_loPitch - 47).toText() << "--" << Tnote(m_hiPitch - 47).toText();
}


void TaudioIN::setDetectionMethod(int method) {
	method = qBound<int>(0, method, 2);
	m_pitch->aGl()->analysisType = EanalysisModes(method);
	audioParams()->detectMethod = method;
}


quint8 TaudioIN::intonationAccuracy() {
  return audioParams()->intonation;
}


void TaudioIN::setIntonationAccuracy(qint8 intAcc) {
  audioParams()->intonation = qBound<quint8>(0, intAcc, 5);
}


//#################################################################################################
//###################         PUBLIC SLOTS             ############################################
//#################################################################################################

void TaudioIN::startListening() {
	if (!streamParams()) {
			qDebug() << "Can not start listening due to uninitialized input";
			return;
	}
	if (state() != e_listening) {
    m_volume = 0.0;
    if (!m_stoppedByUser) {
      if (areStreamsSplit() && state() != e_listening)
        openStream();
      if (startStream())
        setState(e_listening);
// 			qDebug() << "start listening";
		}
  }
}


void TaudioIN::stopListening() {
  if (state() != e_stopped) {
//     qDebug() << "stop listening";
    m_volume = 0.0;
    m_LastChunkPitch = 0.0;
		if (areStreamsSplit() || rtDevice()->getCurrentApi() != RtAudio::LINUX_PULSE)
			abortStream();
    setState(e_stopped);
    m_pitch->stop(true);
  }
}


void TaudioIN::pitchInChunkSlot(float pitch) {
	if (isPaused())
			return;
  if (pitch == 0.0f)
			m_LastChunkPitch = 0.0f;
  else
			m_LastChunkPitch = pitch - audioParams()->a440diff;
}


void TaudioIN::noteStartedSlot(qreal pitch, qreal freq, qreal duration) {
	if (!isPaused()) {
			m_lastNote.set(pitch - audioParams()->a440diff, freq, duration);
			if (inRange(m_lastNote.pitchF)) {
				m_noteWasStarted = true;
//         qDebug() << "[TaudioIN] started" << pitch << m_lastNote.pitch.toText();
				emit noteStarted(m_lastNote);
			} // else
//         qDebug() << "[TaudioIN] started but out of range";
  } else
			m_lastNote.set(); // reset last detected note structure
}


void TaudioIN::noteFinishedSlot(TnoteStruct* lastNote) {
	m_noteWasStarted = false;
	if (!isPaused()) {
      qreal midiPitch = lastNote->getAverage(3, // non guitar pitch is average of all pitches
                              Tcore::gl()->instrument == e_noInstrument ? lastNote->pitches()->size() : m_pitch->minChunksNumber());
      m_lastNote.set(midiPitch - audioParams()->a440diff, pitch2freq(midiPitch), lastNote->duration);
			if (inRange(m_lastNote.pitchF)) {
//         qDebug() << "[TaudioIN] finished" << m_lastNote.pitchF << m_lastNote.pitch.toText();
				emit noteFinished(m_lastNote);
      }
  } else 
			m_lastNote.set(); // reset last detected note structure
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
bool TaudioIN::inCallBack(void* inBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& st) {
  if (m_goingDelete || instance()->isStoped())
    return true;
  if (st)
    qDebug() << "input buffer underflow";

  instance()->m_pitch->copyToBuffer(inBuff, nBufferFrames);
  return false;
}

//#################################################################################################
//###################               PRIVATE SLOTS      ############################################
//#################################################################################################

void TaudioIN::playingFinishedSlot() {
  if (m_state == e_listening) {
    openStream();
    startStream();
  }
}

#if defined(Q_OS_WIN)
void TaudioIN::ASIORestartSlot() {
  restartASIO();
}
#endif









