/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#ifndef TRTAUDIOOUT_H
#define TRTAUDIOOUT_H


#include "tabstractplayer.h"
// #include "tscalefile.h"
#include "trtaudioabstract.h"
#include "toggscale.h"
#include <QStringList>

class TaudioParams;

class TaudioOUT : public TabstractPlayer, public TrtAudioAbstract
{
  Q_OBJECT
   
public:
    TaudioOUT(TaudioParams *_params, QString &path, QObject *parent = 0);
    virtual ~TaudioOUT();
    
    static QStringList getAudioDevicesList();
    
          /** Starts playing given note and then returns true, otherwise gets false. */
    bool play(int noteNr);
    void setAudioOutParams(TaudioParams *params);
        /** It sets audio device to value taken from */
    bool setAudioDevice(QString &name);
        /** Immediately stops playing. Emits nothing */
    void stop();
    
protected:
        /** Pointer to this class instance to emit signal from static callBack method. */ 
    static    TaudioOUT *instance;
    ToggScale *oggScale;
    int     ratioOfRate; // ratio of current sample rate to 44100

private slots:
//   void emitNoteFinished() { emit noteFinished(); }
  void stopSlot();
    
private:
  void deleteAudio();
  static int outCallBack(void *outBuffer, void *inBuffer, unsigned int nBufferFrames, double streamTime,
                         RtAudioStreamStatus status, void *userData);

      /** Number of performed samples. */
  static int m_samplesCnt;
      /** Duration of a sound counted in callBack loops */
  static int m_maxCBloops;
      /** Size of a buffer */
  static unsigned int m_bufferFrames;
	static qint16 *m_crossBuffer; // buffer with data of part of previous note to fade out
	static bool m_doCrossFade;
	static float m_cross; // current 'strength' of fading effect


};

#endif // TRTAUDIOOUT_H
