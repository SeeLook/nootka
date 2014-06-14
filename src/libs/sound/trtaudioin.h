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

#ifndef TRTAUDIOIN_H
#define TRTAUDIOIN_H

#include "nootkacoreglobal.h"
#include <QObject>
#include <QStringList>
#include <music/tnote.h>
#include "rt/RtAudio.h"
#include "trtaudio.h"

class TpitchFinder;


/** 
 * This class manages audio input for Nootka.
 * It emits noteDetected(Tnote) signal when some note is detected
 * and fundamentalFreq(float) with freq of detected note.
 * It can be paused - pitch detection is performed but signals are not sending
 * or it can be stopped - incoming audio data is ignored
 */
class NOOTKASOUND_EXPORT TaudioIN : public QObject, public TrtAudio
{
    Q_OBJECT
public:
    explicit TaudioIN(TaudioParams *params, QObject *parent = 0);
    ~TaudioIN();
		
        /** Returns list of audio input devices filtered by template audio format */
	static QStringList getAudioDevicesList();
	
      /** Stops emitting signals about pitch detection, but detection is still performed.
			 * It also resets last chunk pitch to ignore detection
       * It helps to sniff whole sound/note from begin to its end. */
  void pause() { m_paused = true; m_LastChunkPitch = 0.0; }
  
      /** Starts emiting @param noteDetected and @param fundamentalFreq signals again. */
  void unPause() { m_paused = false; }
  bool isPaused() { return m_paused; }
  bool isStoped() { return m_stopped; }
	float maxPeak() { return m_maxPeak; }
	
	  /** Sets device parameters stores in structure SaudioInParams. 
	   * SaudioInParams::deviceName is ignored. It have to be set separately
	   * by setAudioDevice() method. 	   */
	void setAudioInParams();	
	
			/** Sets minimal volume needed that note will be detected. Overrides global setting.  */
  void setMinimalVolume(float minVol);
  void setIsVoice(bool isV);
  void setAmbitus(Tnote loNote, Tnote hiNote);   /** Sets range of notes which are detected. */
	Tnote loNote() { return m_loNote; } // Returns lower boundary note of ambitus 
	Tnote hiNote() { return m_hiNote; } // Returns upper boundary note of ambitus 
	float lastNotePitch() { return m_lastPich; } /** Pitch of last detected note in float precision. */
	float lastChunkPitch() { return m_LastChunkPitch; }

signals:
	void noteDetected(Tnote note);
	void fundamentalFreq(float freq);
//   void chunkPitch(float pitch);

public slots:
	void startListening();
	void stopListening();
  
protected:
  static TaudioIN* instance() { return m_instances[m_thisInstance] ; }
	static bool inCallBack(void* inBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status);

private slots:
	
  void pitchFreqFound(float pitch, float freq);
  void pitchInChunkSlot(float pitch);
  void volumeSlot(float vol);
	void updateSlot() { setAudioInParams(); }
  
  
private:  
      /** Keeps pointers for all (two) created instances of TaudioIN
       * static inCallBack uses it to has access. */
  static        QList<TaudioIN*> m_instances;
  static        int m_thisInstance;
  TpitchFinder  *m_pitch;
  float         m_maxPeak;
  bool          m_paused, m_stopped;
			/** Boundary notes of the ambitus. */
	Tnote					m_loNote, m_hiNote;
	float 				m_lastPich; /** Pitch of last detected note in float precision. */
	float					m_LastChunkPitch; /** Pitch from recent processed chunk or 0.0 if silence */
	
};

#endif // TRTAUDIOIN_H

