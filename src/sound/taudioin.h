/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#ifndef TAUDIOIN_H
#define TAUDIOIN_H

#include <QObject>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioFormat>
#include "tnote.h"

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
		/** Template audio format is 
		 * 1 chanell (mono) 
		 * 44100 samples per second
		 * signed int resolution (16bit)
		 * pcm data */
	static QAudioFormat templAudioFormat;
// 	inline QString deviceName();
	
	bool isAvailable() {return (m_IOaudioDevice ? true : false) ; }

	bool setAudioDevice(const QString &devN);
	void startListening();
	void stopListening();
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
  void setIsVoice(bool isV);

signals:
	void noteDetected(Tnote note);
	void noiseLevel(qint16 level);
	void fundamentalFreq(float freq);


private slots:
	void audioDataReady();
	void readToCalc();
	void calc();
  void pitchFreqFound(float pitch, float freq);
	void noteStopedSlot();

  
private:
    /** Sets template values to templAudioFormat*/
  static void prepTemplFormat();
  void initInput();
  
  
	QAudioDeviceInfo m_deviceInfo;
	QAudioInput *m_audioInput;
	QIODevice *m_IOaudioDevice;
	QByteArray m_buffer;
	float *m_floatBuff;
	quint32 m_floatsWriten;
	qint16 m_maxPeak;
	TpitchFinder *m_pitch;
	QList<qint16> m_peakList;
	bool m_noteStarted;
	TaudioParams *m_params;
	
	
};

#endif // TAUDIOIN_H
