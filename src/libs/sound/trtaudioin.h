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

#ifndef TRTAUDIOIN_H
#define TRTAUDIOIN_H

#include "nootkacoreglobal.h"
#include <QObject>
#include <QStringList>
#include <QDebug>
#include <music/tnote.h>
#include <music/tnotestruct.h>
#include "rt/RtAudio.h"
#include "trtaudio.h"
#include "tpitchfinder.h"


class TpitchFinder;


/** 
 * This class manages audio input for Nootka.
 * It emits @p noteStarted() when new note was started playing (and its duration is appropriate)
 * and emits @p noteFinished() when note finished.
 * It wraps TpitchFinder values - adjust middle a offset and rejects notes out or range (ambitus)
 * 
 * It can be paused - pitch detection is performed but signals are not sending
 * or it can be stopped - incoming audio data is ignored
 */
class NOOTKASOUND_EXPORT TaudioIN : public QObject, public TrtAudio
{
    Q_OBJECT
    
public:
    explicit TaudioIN(TaudioParams *params, QObject *parent = 0);
    ~TaudioIN();
	
      /** State of input audio device:
       * @p e_listening - when input captures data and emits signals
       * @p e_paused - when data is capturing but signals about detected pitches are not emitting
       * @p e_stopped - capturing data is stopped. 
       * Achieved by calling @p stopListening() and @p startListening()
       */
  enum Estate {
    e_listening = 0, e_paused = 1, e_stopped = 2  
    
  };
  
  static TaudioIN* instance() { return m_instance; }
    
        /** Returns list of audio input devices filtered by template audio format */
	static QStringList getAudioDevicesList(); 
	
      /** Stops emitting signals about pitch detection, but detection is still performed.
			 * It also resets last chunk pitch to ignore detection
       * It helps to sniff whole sound/note from begin to its end. */
  void pause() { m_LastChunkPitch = 0.0; if (m_state == e_listening) setState(e_paused); }
  
      /** Starts emitting @param noteDetected and @param fundamentalFreq signals again. */
  void unPause() { if (m_state == e_paused) setState(e_listening); }
  bool isPaused() { return m_state == e_paused; }
  bool isStoped() { return m_state == e_stopped; }
  
  Estate state() { return m_state; }
  
  float volume() { return m_volume; } /** Current volume of detecting sound or 0 if silence */
	
	  /** Sets device parameters stores in structure SaudioInParams. 
	   * SaudioInParams::deviceName is ignored. It have to be set separately by setAudioDevice() method. 	   */
	void setAudioInParams();	
	
			/** Sets minimal volume needed that note will be detected. Overrides global setting.  */
  void setMinimalVolume(float minVol);

  void setAmbitus(Tnote loNote, Tnote hiNote);   /** Sets range of notes which are detected. */
	
	Tnote& loNote() { return m_loNote; } /** Returns lower boundary note of ambitus */
	Tnote& hiNote() { return m_hiNote; } /** Returns upper boundary note of ambitus */
	
	qreal lastNotePitch() { return m_lastNote.pitchF; } /** Pitch of last detected note in double precision. */
	TnoteStruct& lastNote() { return m_lastNote; } /** Pitch, frequency and duration of the last detected note. */
	
	float lastChunkPitch() { return m_LastChunkPitch; }
	
	bool inRange(qreal pitch) {
		if (pitch >= m_loPitch && pitch <= m_hiPitch)	return true;
		else return false;
	}		/** Returns @p TRUE when @p pitch is in ambitus */
	
	bool noteWasStarted() { return m_noteWasStarted; } /** @p TRUE when note started but not finished. */
	
			/** Sets pitch detection method. Currently three are available:
			 * 0 - MPM (Philip McLeod Method) - default
			 * 1 - auto-correlation 
			 * 2 - MPM modified cepstrum.
			 * Currently set value is available through global. */
	void setDetectionMethod(int method);
  
      /** Stores user action when he stopped sniffing himself. */
  void setStoppedByUser(bool userStop) { m_stoppedByUser = userStop; }
  bool stoppedByUser() { return m_stoppedByUser; }

  quint8 intonationAccuracy(); /** Returns intonation accuracy sets in global audio settings. */
  void setIntonationAccuracy(qint8 intAcc); /** Sets global value of intonation accuracy. It doesn't refresh intonation view. */
	
signals:	
	void noteStarted(const TnoteStruct&); /** Emitted when note was played and its duration is longer than minimal duration */
  void noteFinished(const TnoteStruct&); /** When already started note fade out */
  void stateChanged(int); /** When device changes its state. It can be cast on @p Estate enumeration. */
  

public slots:
	void startListening();
	void stopListening();
  
protected:
	static bool inCallBack(void* inBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& st);
  

private slots:
  void pitchInChunkSlot(float pitch);
  void volumeSlot(float vol) { m_volume = vol; }
	void updateSlot() { setAudioInParams(); }
	void noteStartedSlot(qreal pitch, qreal freq, qreal duration);
	void noteFinishedSlot(TnoteStruct* lastNote);
  void playingFinishedSlot();
  
  void setState(Estate st) { m_state = st; emit stateChanged((int)st); }
#if defined(Q_OS_WIN)
  void ASIORestartSlot();
#endif
  
private:  
      /** Keeps static pointer of TaudioIN instance. static inCallBack uses it to has access. */
  static        		TaudioIN* m_instance;
      /** is set to @p FALSE when destructor starts. It prevents to performs callbacks routines then. */
  static bool       m_goingDelete;
	
  TpitchFinder  	 *m_pitch;
  float         		m_volume;
	Tnote							m_loNote, m_hiNote; 		/** Boundary notes of the ambitus. */
	TnoteStruct				m_lastNote;
	float							m_LastChunkPitch; /** Pitch from recent processed chunk or 0.0 if silence */
	bool              m_stoppedByUser;
	qreal 						m_loPitch, m_hiPitch;
	bool							m_noteWasStarted;
	int								m_currentRange; /** Current range of detected note - see @class TaudioParams */
	Estate            m_state;
};

#endif // TRTAUDIOIN_H

