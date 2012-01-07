/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk            *
 *   tomaszbojczuk@gmail.com                 *
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
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TAUDIOOUT_H
#define TAUDIOOUT_H

// #include "RtMidi.h"
#include <vector>
#include <QString>
#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
// #include 

class RtMidiOut;
class TaudioParams;
class QTimer;

/** TaudioOUT is a class which plays single guitar sounds.
 * It can play real audio sound or midi.
 * Audio is taken from file classical-guitar.wav, in method getAudioData checks file
 * and load data to m_audioArr.
 * Trick is that data is mono 22050 but output is stereo 44100. All this magic
 * is in paCallBack because of requirements of some audio devices.
 * So far playing is bounded to range C in Contra octave to e in 3-line.
 * 
 * Midi is played by RtMidi class.
*/
class TaudioOUT: public QObject
{
  
  Q_OBJECT

public:
    TaudioOUT(TaudioParams *params, QString &path, QObject *parent = 0);
    ~TaudioOUT();

  static QStringList getAudioDevicesList();
  static QStringList getMidiPortsList();
      /** Template audio format is 
      * 2 chanells (stereo) 
      * 44100 samples per second
      * signed int resolution (16bit)
      * pcm data */
	static QAudioFormat templAudioFormat;

  void play(int noteNr);
  void setAudioOutParams(TaudioParams *params);
      /** It sets audio device to value taken from */
  bool setAudioDevice(QString &name);
    /** Sets midi parameters:
    * @param portName, if empty system prefered is set (Timidity under Linux) 
    * @param instrNr for instrument number in midi nomenclature. */
  void setMidiParams();
  bool isPlayable() { return m_playable; }
    /** Deletes midi device if exists. 
     * Midi device usually blocks audio devices, so when it exists getAudioDevicesList() doesn't work */
  void deleteMidi();
  
signals:
    /** This signal is emited when playing of a note is finished. */
  void noteFinished();

private:
      /** Loads wav file with scale to m_audioArr. If everything is ok returns true */
  bool loadAudioData();
  void deleteAudio();
      /** Sets template values to templAudioFormat*/
  static void prepTemplFormat();
  
  bool m_playable;
  QTimer *m_timer;
  TaudioParams *m_params;
  
//########## audio #############
	QAudioDeviceInfo m_deviceInfo;
  QAudioOutput *m_audioOutput;
  QIODevice *m_IOaudioDevice;
  QByteArray m_buffer;
  QString m_devName;
      /** Path to wav file with sounds */
  QString m_wavFile;
  quint32 m_sampleRate;
  qint16 *m_audioArr;
      /** position of a note in @param m_audioArr */
  int m_noteOffset;
  int m_samplesCnt;
  bool m_isPlayed;

//########## midi #############
  RtMidiOut *m_midiOut;
  unsigned char m_prevMidiNote;
  std::vector<unsigned char> m_message;
  bool m_doEmit;
  
private slots:
    /** Turns off played @param m_prevMidiNote
     * If @param m_doEmit is true emits noteFinished() signal. */
  void midiNoteOff();
    /** m_timer calls this to prepare audio for device*/
  void timeForAudio();
//   void playBuffer();
  void stateSlot(QAudio::State st);

};

#endif // TAUDIOOUT_H
