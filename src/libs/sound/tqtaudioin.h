/***************************************************************************
 *   Copyright (C) 2015-2021 by Tomasz Bojczuk                             *
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


#ifndef TAUDIOIN_H
#define TAUDIOIN_H


#include "tcommonlistener.h"
#include <QtMultimedia/qaudiodeviceinfo.h>
#include <QtCore/qtimer.h>
#include <QtCore/qiodevice.h>


class QAudioInput;
class QIODevice;
class TaudioParams;
class TaudioBuffer;
class QTimer;


class TtouchHandler : public QObject
{
  Q_OBJECT
public:
  TtouchHandler(TcommonListener* sniffer);

      /**
       * Stops touch timer
       */
  void skip() { m_touchTimer->stop(); }

  void stopHandling();
  void startHandling();

protected:
  virtual bool eventFilter(QObject* obj, QEvent* event);
  void touchTimerSlot();

private:
  bool                  m_touched = false;
  QTimer               *m_touchTimer;
  TcommonListener      *m_sniffer;
  bool                  m_installed;
};



/**
 * Audio input class for Nootka, based on Qt audio (Qt multimedia)
 */
class NOOTKASOUND_EXPORT TaudioIN : public TcommonListener
{
  Q_OBJECT

public:
  explicit TaudioIN(TaudioParams* params, QObject *parent = 0);
  ~TaudioIN();

      /**
       * Keeps static pointer of TaudioIN instance. static inCallBack uses it to has access.
       */
  static TaudioIN* instance() { return m_instance; }
  static QString inputName() { return m_deviceName; }

      /**
       * Returns list of audio input devices filtered by template audio format.
       */
  static QStringList getAudioDevicesList();

  void updateAudioParams();

      /**
       * Stops handling touch events  to suspend sniffing
       */
  void stopTouchHandle() { m_touchHandler->stopHandling(); }
  void startTouchHandle() { m_touchHandler->startHandling(); }

public slots:
  virtual void startListening();
  virtual void stopListening();

protected slots:
  void bufferReady(const char* data, qint64& dataLenght);
  void stateChangedSlot(QAudio::State s);

private:
      /**
       * Deletes existing @p m_audioIN (when names are different) and creates a new one
       */
  void createInputDevice();

private:
  static TaudioIN       *m_instance;
  static QString         m_deviceName;
  TaudioParams          *m_audioParams;
  QAudioInput           *m_audioIN;
  QAudioDeviceInfo       m_deviceInfo;
  TaudioBuffer          *m_inBuffer;
  TtouchHandler         *m_touchHandler;
};

#endif // TAUDIOIN_H
