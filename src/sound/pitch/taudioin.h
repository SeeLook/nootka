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
	qint16 maxPeak() { return m_maxPeak; }
	

signals:
	void pitchFound(float pitch);


private slots:
	void audioDataReady();

  
private:
    QAudioDeviceInfo m_deviceInfo;
    QAudioInput *m_audioInput;
	QIODevice *m_IOaudioDevice;
	QByteArray m_buffer;
	float *m_floatBuff;
	quint32 m_floatsWriten;
	qint16 m_maxPeak;
	TpitchFinder *m_pitch;


};

#endif // TAUDIOIN_H
