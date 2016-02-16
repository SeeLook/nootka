/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


class QAudioInput;
class QIODevice;
class TaudioParams;


/**
 * Audio input class for Nootka based on Qt audio (Qt multimedia)
 */
class NOOTKASOUND_EXPORT TaudioIN : public TcommonListener
{
  Q_OBJECT

public:
  explicit TaudioIN(TaudioParams* params, QObject *parent = 0);
  ~TaudioIN();
  
  static TaudioIN* instance() { return m_instance; }
  static QString inputName() { return m_deviceName; }
  
      /** Returns list of audio input devices filtered by template audio format */
  static QStringList getAudioDevicesList(); 
  
  void updateAudioParams();
  
public slots:
  virtual void startListening();
  virtual void stopListening();


protected slots:
  void dataReady();
  void startThread();
  void stopThread();
  
private:
  void createInputDevice(); /** Deletes existing @p m_audioIN (if any) and creates a new one */

private:
     /** Keeps static pointer of TaudioIN instance. static inCallBack uses it to has access. */
  static TaudioIN       *m_instance;
  static QString         m_deviceName;
  TaudioParams          *m_audioParams;
  QAudioInput           *m_audioIN;
  QAudioDeviceInfo       m_deviceInfo;
  QIODevice             *m_inDevice;
  qint16                *m_buffer;
  QThread               *m_thread;
};

#endif // TAUDIOIN_H
