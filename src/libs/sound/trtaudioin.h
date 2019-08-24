/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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


#ifndef TRTAUDIOIN_H
#define TRTAUDIOIN_H


#include "trtaudio.h"
#include "tpitchfinder.h"
#include "tcommonlistener.h"


/**
 * This class manages audio input in desktop versions of Nootka.
 * It works through RtAudio framework
 */
class NOOTKASOUND_EXPORT TaudioIN : public TcommonListener, public TrtAudio
{
    Q_OBJECT

public:
    explicit TaudioIN(TaudioParams *params, QObject *parent = nullptr);
    ~TaudioIN();

  static TaudioIN* instance() { return m_instance; }

        /**
         * Returns list of audio input devices filtered by template audio format
         */
  static QStringList getAudioDevicesList();


    /**
     * Sets device parameters stores in structure SaudioInParams.
     * SaudioInParams::deviceName is ignored. It have to be set separately by setAudioDevice() method.
     */
  void setAudioInParams();


public slots:
  virtual void startListening();
  virtual void stopListening();

protected:
  static bool inCallBack(void*, void* inBuff, unsigned int nBufferFrames);


#if defined(Q_OS_WIN)
  void ASIORestartSlot();
#endif

  void playingFinishedSlot();

private:
  static TaudioIN     *m_instance; /**< Keeps static pointer of TaudioIN instance. static inCallBack uses it to has access. */
  static bool          m_goingDelete; /**< is set to @p FALSE when destructor starts. It prevents to performs callbacks routines then. */

};

#endif // TRTAUDIOIN_H

