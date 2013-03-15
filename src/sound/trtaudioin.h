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

class RtAudio;
class TaudioParams;
class TpitchFinder;


/** This class manages audio input for Nootka.
 * It emits noteDetected(Tnote) signal when some note is detected
 * and fundamentalFreq(float) with freq of detected note.
 * @method calculateNoiseLevel() can be used to obtain noise. */
class TaudioIN : public QObject
{
    Q_OBJECT
public:
    explicit TaudioIN(TaudioParams *params, QObject *parent = 0);
    ~TaudioIN();
        /** Returns list of audio input devices 
        * filtered by template audio format */
	static QStringList getAudioDevicesList();
		
	QString deviceName() { return m_devName; }
	
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
	qint16 maxPeak() { return m_maxPeak; }
	  /** Starts capturing audio to calculate max level. 
	   * After 1000ms singleShot of Qtimer calls calc(),
	   * and signal noiseLevel(qint16 level) with max peak is emited. */
	void calculateNoiseLevel();
	  /** Sets device parameters stores in struct SaudioInParams. 
	   * SaudioInParams::deviceName is ignored. It have to be set separately
	   * by setAudioDevice() method. 	   */
	void setParameters(TaudioParams *params);	
    /** Pauses TaudioIN */
  void wait();
    /** Wakes up TaudioIN after pause called by wait() */
  void go();
//   void pause();
  void setIsVoice(bool isV);
    /** Sets range of notes which are detected. */
  void setAmbitus(Tnote loNote, Tnote hiNote);

signals:
	void noteDetected(Tnote note);
	void noiseLevel(qint16 level);
	void fundamentalFreq(float freq);


private slots:
	
// 	void readToCalc();
// 	void calc();
  void pitchFreqFound(float pitch, float freq);
  
  
private:
  void initInput();
  
  static int inCallBack(void *outBuffer, void *inBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData);
  
  static float *m_floatBuff;
  static TpitchFinder *m_pitch;
      /** Number of TaudioIN instances in Nootka. 
       * It avoids deleting m_floatBuff and m_pitch when other insatnces are deleted. */
  static int m_instancesNr;
  
  static quint32 m_floatsWriten;
  static qint16 m_maxPeak, m_maxP;
      /** Size of a buffer */
  static unsigned int m_bufferFrames;
  
//   QList<qint16> m_peakList;
  unsigned int m_sampleRate;
  RtAudio *m_rtAudio;
  RtAudio::StreamParameters m_inParams;
  RtAudio::StreamOptions *m_streamOptions;
  TaudioParams *m_params;
  QString m_devName;

  bool m_paused;
	
};

#endif // TRTAUDIOIN_H
