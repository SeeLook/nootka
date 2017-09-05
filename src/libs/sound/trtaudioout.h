/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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
class TsingleSound;


/**
 * Nootka audio output through RtAudio
 */
class NOOTKASOUND_EXPORT TaudioOUT : public TabstractPlayer, public TrtAudio
{
  Q_OBJECT

public:
  TaudioOUT(TaudioParams* _params, QObject* parent = nullptr);
  ~TaudioOUT() override;

  static QStringList getAudioDevicesList();

      /**
       * Starts playing given note and then returns true, otherwise gets false.
       */
  bool play(int noteNr) override;

  void playMelody(const QList<Tnote>& notes, int tempo, int firstNote = 0);

  void setAudioOutParams();

  void stop() override;

      /**
       * Number (id) of actually played note, if note list of @p playMelody comes from score
       * this number corresponds with note id on the score
       */
  int playingNoteId() const { return m_playingNoteId; }

signals:
  void nextNoteStarted();

protected:
  static bool outCallBack(void* outBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status);

protected:
  static TaudioOUT               *instance; /**< Static pointer of this class instance. */
  ToggScale                      *oggScale;
  int                             ratioOfRate; /**< ratio of current sample rate to 44100 */

  void decodeNext();

private slots:
  void updateSlot() { setAudioOutParams(); }
  void playingFinishedSlot();

#if defined(Q_OS_WIN)
  void ASIORestartSlot();
#endif

private:
  bool                          m_callBackIsBussy;

  static unsigned int           m_posInNote, m_posInOgg;
  static int                    m_playingNoteNr, m_decodingNoteNr, m_playingNoteId;
  QList<TsingleSound>           m_playList;

};

#endif // TRTAUDIOOUT_H
