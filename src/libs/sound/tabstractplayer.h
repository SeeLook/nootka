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


#ifndef TABSTRACTPLAYER_H
#define TABSTRACTPLAYER_H


#include <nootkasoundglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>


class QTimer;
class Tnote;


#define REST_NR (127)


class NOOTKASOUND_EXPORT TsingleSound {
public:
  TsingleSound(int nId = -1, qint16 nr = REST_NR, quint32 samples = 0) : id(nId), number(nr), samplesCount(samples) {}
  int id; /** Note id in the melody */
  qint8 number; /**< number of note (chromatic) - none (rest) by default */
  quint32 samplesCount; /**< Number of samples the note takes */
};


/**
 * Base abstract class for sound output (playing scale).
 */
class NOOTKASOUND_EXPORT TabstractPlayer : public QObject
{

  Q_OBJECT

public:
  TabstractPlayer(QObject *parent = nullptr);

  bool isPlayable() const { return playable; }

  bool isPlaying() const { return p_isPlaying; }

      /**
        * Starts playing given note and then returns true, otherwise gets false.
        */
  virtual bool play(int noteNr) = 0;

      /**
       * Immediately stops playing. Emits nothing
       */

  virtual void stop() = 0;

      /**
       * Does nothing in audio player subclass.
       */
  virtual void deleteMidi();
  virtual void setMidiParams();

  enum EplayerType { e_audio, e_midi };

  EplayerType type() { return playerType; }

  QList<TsingleSound>& playList() { return m_playList; }

      /**
       * Number (id) of actually played note, if note list of @p playMelody comes from score
       * this number corresponds with note id on the score
       */
  int playingNoteId() const { return p_playingNoteId; }

signals:
      /**
       * This signal is emitted when playing of a note/melody is finished.
       */
  void noteFinished();

  void nextNoteStarted();


protected:
  void setType(EplayerType type) { playerType = type; }

  void preparePlayList(const QList<Tnote>& notes, int tempo, int firstNote, int sampleRate, int transposition, int a440Ddiff);

protected:
  bool             playable;

      /**
       * Determines whether noteFinished() signal is emited in offTimer timeOut() slot.
       * Slot is also called by stop() method and then signal can't be emitted.
       */
  bool                         doEmit;
  QTimer                      *offTimer;
  EplayerType                  playerType;
  bool                         p_isPlaying = false;
  static unsigned int          p_posInNote, p_posInOgg;
  static int                   p_playingNoteNr, p_decodingNoteNr, p_playingNoteId;

private:
  QList<TsingleSound>           m_playList;

};

#endif // TABSTRACTPLAYER_H
