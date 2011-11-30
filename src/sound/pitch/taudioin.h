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
#include "saudioinparams.h"
#include "tnote.h"

// class Tnote;
class TpitchFinder;



class TaudioIN : public QObject
{
    Q_OBJECT
public:
    explicit TaudioIN(QObject *parent = 0);
    ~TaudioIN();
	
    static QStringList getAudioDevicesList();
	static QAudioFormat templAudioFormat;
	QString deviceName();
	
	bool isAvailAble() {return (m_IOaudioDevice ? true : false) ; }

    bool setAudioDevice(const QString &devN);
	void startListening();
	void stopListening();
	qint16 maxPeak() { return m_maxPeak; }
	  /** Starts capturing audio to calculate max level. 
	   * After 1000ms singleShot of Qtimer calls calc(),
	   * and signal noiseLevel(qint16 level) with max peak is emited.
	   */
	void calculateNoiseLevel();
	  /** Sets device parameters stores in struct SaudioInParams. 
	   * SaudioInParams::deviceName is ignored. It have to be set separately
	   * by setAudioDevice() method. 	   */
	void setParameters(SaudioInParams &params);
// 	SaudioInParams parameters() { return m_params; } // if needed make it as reference and const also
	

signals:
	void noteDetected(Tnote note);
	void noiseLevel(qint16 level);
	void fundamentalFreq(float freq);


private slots:
	void audioDataReady();
	void readToCalc();
	void calc();
	void pitchSlot(float pitch);
	void freqSlot(float freq);
	void noteStopedSlot();

  
private:
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
	SaudioInParams m_params;
	
	
};

#endif // TAUDIOIN_H
