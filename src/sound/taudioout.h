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


#ifndef TAUDIOOUT_H
#define TAUDIOOUT_H

#include <QString>
#include "tabstractplayer.h"
#include "tscalefile.h"
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

class TaudioParams;
class QTimer;

/** TaudioOUT is a class which plays single guitar sounds.
 * It can play real audio sound or midi.
 * Audio is taken from file classical-guitar.wav, in method getAudioData checks file
 * and load data to m_audioArr.
 * Trick is that data is mono 22050 but output is stereo 44100. 
 * All this magicis in timeForAudio() because some of audio devices supports only 44100/16/2 format.
 * So far playing is bounded to range C in Contra octave to e in 3-line.
*/
class TaudioOUT: public TabstractPlayer, public TscaleFile
{
  
  Q_OBJECT

public:
    TaudioOUT(TaudioParams *params, QString &path, QObject *parent = 0);
    ~TaudioOUT();

  static QStringList getAudioDevicesList();
      /** Template audio format is 
      * 2 chanells (stereo) 
      * 44100 samples per second
      * signed int resolution (16bit)
      * pcm data */
	static QAudioFormat templAudioFormat;

      /** Starts playing given note and then returns true, otherwise gets false. */
  bool play(int noteNr);
  void setAudioOutParams(TaudioParams *params);
      /** It sets audio device to value taken from */
  bool setAudioDevice(QString &name);
      /** Immediately stops playing. Emits nothing */
  void stop();
  

private:
  void deleteAudio();
      /** Sets template values to templAudioFormat*/
  static void prepTemplFormat();
  
  QTimer *offTimer;
  TaudioParams *audioParams;
  
//########## audio #############
	QAudioDeviceInfo m_deviceInfo;
  QAudioOutput *m_audioOutput;
  QIODevice *m_IOaudioDevice;
  QByteArray m_buffer;
  QString deviceName;
      
  int m_noteOffset;
      /** Number of performed samples. */
  int m_samplesCnt;
  bool m_doPlay;
      /** Period of audio timer loop depends on QAudioOutput::periodSize() unit [ms] */
  quint16 m_period;

  
private slots:
    /** m_timer calls this to prepare audio for device*/
  void timeForAudio();
  void deviceStateSlot(QAudio::State auStat);
};

#endif // TAUDIOOUT_H
