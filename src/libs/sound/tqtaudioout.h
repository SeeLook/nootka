/***************************************************************************
 *   Copyright (C) 2015-2020 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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


#ifndef TQTAUDIOOUT_H
#define TQTAUDIOOUT_H


#include "nootkasoundglobal.h"
#include "tabstractplayer.h"
#include <QtCore/qstringlist.h>
#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudiodeviceinfo.h>


class ToggScale;
class TaudioParams;
class QAudioOutput;
class TaudioBuffer;


/**
 * Implementation of sound player using Qt Multimedia
 */
class NOOTKASOUND_EXPORT TaudioOUT : public TabstractPlayer
{

  Q_OBJECT

public:
  TaudioOUT(TaudioParams* _params, QObject* parent = 0);
  virtual ~TaudioOUT();

  static QStringList getAudioDevicesList();
  static QString outputName() { return m_devName; }
  static TaudioOUT* instance() { return m_instance; }

  void startPlaying() override;

  void setAudioOutParams();

      /**
       * Immediately stops playing.
       */
  void stop() override;

  TaudioParams* audioParams() { return m_audioParams; }

protected:
  void createOutputDevice();

  void decodeNext();

signals:
  void finishSignal();

protected:
  ToggScale                      *oggScale;
  int                             ratioOfRate; // ratio of current sample rate to 44100

private:
  void outCallBack(char* data, qint64 maxLen, qint64& wasRead);
//   void updateSlot() { setAudioOutParams(); }
  void playingFinishedSlot();
  void stateChangedSlot(QAudio::State state);
  void startPlayingSlot();


private:
  static QString                   m_devName;
  static TaudioOUT                *m_instance;
  int                              m_bufferFrames, m_sampleRate;
  bool                             m_callBackIsBussy;
  TaudioParams                    *m_audioParams;
  QAudioOutput                    *m_audioOUT;
  TaudioBuffer                    *m_buffer;
  QAudioDeviceInfo                 m_deviceInfo;
};

#endif // TRTAUDIOOUT_H
