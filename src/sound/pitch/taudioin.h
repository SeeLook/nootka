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
//#include
class TpitchFinder;


class TaudioIN : public QObject
{
    Q_OBJECT
public:
    explicit TaudioIN(QObject *parent = 0);
    ~TaudioIN();

    static QStringList getAudioDevicesList();
	static QAudioFormat templAudioFormat;

    void setAudioDevice(const QString &devN);
	void startListening();
	void stopListening();
	qint16 maxPeak() { return m_maxPeak; }
	  /** Starts capturing audio to calculate max level. 
	   * After 1000ms singleShot of Qtimer calls calc,
	   * and signal noiseLevel(qint16 level) is emited.
	   */
	void calculateNoiseLevel();
	void setNoiseLevel(qint16 noise) { m_noiseLevel = noise; }
	qint16 noiseLevel() { return m_noiseLevel; }
	

signals:
	void pitchFound(float pitch);
	void noiseLevel(qint16 level);


private slots:
	void audioDataReady();
	void readToCalc();
	void calc();

  
private:
  void initInput();
  
  
    QAudioDeviceInfo m_deviceInfo;
    QAudioInput *m_audioInput;
	QIODevice *m_IOaudioDevice;
	QByteArray m_buffer;
	float *m_floatBuff;
	quint32 m_floatsWriten;
	qint16 m_maxPeak, m_noiseLevel;
	TpitchFinder *m_pitch;
	QList<qint16> m_peakList;
	QString m_devName;
};

#endif // TAUDIOIN_H
