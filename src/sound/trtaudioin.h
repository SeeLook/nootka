/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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

#include <QObject>
#include <QStringList>
#include "tnote.h"
#include "rt/RtAudio.h"
#include "trtaudioabstract.h"

class TpitchFinder;


/** This class manages audio input for Nootka.
 * It emits noteDetected(Tnote) signal when some note is detected
 * and fundamentalFreq(float) with freq of detected note.
 * @method calculateNoiseLevel() can be used to obtain noise. */
class TaudioIN : public QObject, public TrtAudioAbstract
{
    Q_OBJECT
public:
    explicit TaudioIN(TaudioParams *params, QObject *parent = 0);
    ~TaudioIN();
        /** Returns list of audio input devices 
        * filtered by template audio format */
	static QStringList getAudioDevicesList();
	
// 	bool isAvailable() {return (m_IOaudioDevice ? true : false) ; }

	bool setAudioDevice(const QString &devN);
	void startListening();
	void stopListening();
      /** Stops emiting signals about pitch detection,
       * but detection is still performed. 
       * It helps to sniff whole sound/note from begin to its end. */
  void pause() { m_paused = true; }
      /** Starts emiting @param noteDetected and @param fundamentalFreq signals again. */
  void unPause() { m_paused = false; }
  bool isPaused() { return m_paused; }
	float maxPeak() { return m_maxPeak; }
	  /** Sets device parameters stores in struct SaudioInParams. 
	   * SaudioInParams::deviceName is ignored. It have to be set separately
	   * by setAudioDevice() method. 	   */
	void setParameters(TaudioParams *params);	
    /** Do the same as @param startListening() but for backword compatibility 
     * with QtMultimedia that function remains  */
  void wait() { stopListening(); }
    /** Do the same as @param stopListening() but for backword compatibility 
     * with QtMultimedia that function remains  */
  void go() { startListening(); }

  void setIsVoice(bool isV);
    /** Sets range of notes which are detected. */
  void setAmbitus(Tnote loNote, Tnote hiNote);
	Tnote loNote() { return m_loNote; } // Returns lower boundary note of ambitus 
	Tnote hiNote() { return m_hiNote; } // Returns upper boundary note of ambitus 

signals:
	void noteDetected(Tnote note);
	void noiseLevel(qint16 level);
	void fundamentalFreq(float freq);
  void chunkPitch(float pitch);

  
protected:
  static TaudioIN* instance() { return m_instances[m_thisInstance] ; }

private slots:
	
  void pitchFreqFound(float pitch, float freq);
  void pitchInChunkSlot(float pitch);
  void volumeSlot(float vol);
  
  
private:
  void initInput();
  
  static int inCallBack(void *outBuffer, void *inBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData);
      /** Keeps pointers for all (two) created instances of TaudioIN
       * ststic inCallBack uses it to has access. */
  static        QList<TaudioIN*> m_instances;
  static        int m_thisInstance;
  
  float         *m_floatBuff;
  TpitchFinder  *m_pitch;
      
  quint32       m_floatsWriten;
  qint16        m_maxP;
  float         m_maxPeak;
      /** Size of a buffer */
  unsigned int  m_bufferFrames;  
  bool          m_paused;
			/** Boundary notes of the ambitus. */
	Tnote					m_loNote, m_hiNote;
	
};

#endif // TRTAUDIOIN_H

