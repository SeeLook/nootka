/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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


#ifndef TRTAUDIOOUT_H
#define TRTAUDIOOUT_H


#include "nootkacoreglobal.h"
#include "tabstractplayer.h"
#include "trtaudio.h"
#include <QtCore/qstringlist.h>


class ToggScale;
class Tnote;


/**
 * Nootka audio output through @p RtAudio
 */
class NOOTKASOUND_EXPORT TaudioOUT : public TabstractPlayer, public TrtAudio
{

  Q_OBJECT

public:
  TaudioOUT(TaudioParams* _params, QObject* parent = nullptr);
  ~TaudioOUT() override;

  static QStringList getAudioDevicesList();

  void stop() override;

  void setAudioOutParams();

protected:
  static bool outCallBack(void* outBuff, void* inBuff, unsigned int nBufferFrames);

  void decodeNextSlot();

  void startPlaying() override;

  void updateSlot() { setAudioOutParams(); }
  void playingFinishedDelay();
  void playingFinishedSlot();

#if defined(Q_OS_WIN)
  void ASIORestartSlot();
#endif


protected:
  static TaudioOUT               *instance; /**< Static pointer of this class instance. */
  ToggScale                      *oggScale;
  int                             ratioOfRate; /**< ratio of current sample rate to 44100 */

private:
  bool                            m_callBackIsBussy;
  bool                            m_singleNotePlayed = false;

};

#endif // TRTAUDIOOUT_H
