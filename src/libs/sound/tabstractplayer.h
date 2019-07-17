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


#ifndef TABSTRACTPLAYER_H
#define TABSTRACTPLAYER_H


#include <nootkasoundglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>


class QTimer;
class Tnote;
class QThread;
class TaudioParams;
class ToggScale;
class Tmelody;


#define REST_NR (127)


class NOOTKASOUND_EXPORT TsingleSound {
public:
  TsingleSound(int nId = -1, qint16 nr = REST_NR, quint32 samples = 0) : id(nId), number(nr), samplesCount(samples) {}
  int id; /**< Note id in the melody */
  qint8 number; /**< number of note (chromatic) - none (rest) by default */
  quint32 samplesCount; /**< Number of samples the note takes */
};


/**
 * Abstract class for sound output. All sound back-ends (RtAudio, Qt audio, MIDI) derive from it.
 *
 * To trigger playing three methods are available:
 * @li @p playNote() - for single note
 * @li @p playNotes() - for note list (i.e. score notes)
 * @li @p playMelody() - for melody with defined tempo
 *
 * Those methods just stores stuff to play and trigger thread to start playing.
 * @p playThreadSlot() creates @p playList() with @p TsingleSound list of notes to play,
 * then abstract @p startPlaying() playing is called which has to be re-implemented by players.
 */
class NOOTKASOUND_EXPORT TabstractPlayer : public QObject
{

  Q_OBJECT

public:
  TabstractPlayer(QObject *parent = nullptr);
  ~TabstractPlayer() override;

  bool isPlayable() const { return p_playable; }

  bool isPlaying() const { return p_isPlaying; }

      /**
       * Immediately stops playing. Emits nothing
       */
  virtual void stop() = 0;

      /**
       * Does nothing in audio player subclass.
       */
  virtual void deleteMidi() {}
  virtual void setMidiParams() {}

  bool playNote(int noteNr);
  bool playNotes(QList<Tnote>* notes, int tempo, int firstNote = 0);
  bool playMelody(Tmelody* melody, int transposition = 0);

  enum EplayerType { e_audio, e_midi };

  EplayerType type() { return playerType; }

  QList<TsingleSound>& playList() { return m_playList; }

      /**
       * Number (id) of actually played note, if note list of @p playMelody comes from score
       * this number corresponds with note id on the score
       */
  int playingNoteId() const { return p_playingNoteId; }

  void runMetronome(unsigned int beatTempo);
  void stopMetronome();
  qint16 getBeatsample(unsigned int sampleNr) const { return m_beatArray[sampleNr]; }

  bool tickBeforePlay() const;
  void setTickBeforePlay(bool tbp);

  bool tickDuringPlay() const;
  void setTickDuringPlay(bool tdp);

signals:
      /**
       * This signal is emitted when playing of a note/melody is finished.
       */
  void noteFinished();

  void nextNoteStarted();


protected:
  void setType(EplayerType type) { playerType = type; }

  virtual void startPlaying() = 0;

  void preparePlayList(QList<Tnote>* notes, int tempo, int firstNote, int sampleRate, int transposition, int a440Ddiff);

  QThread* playThread() { return m_thread; }

protected:
  bool                         p_playable;

      /**
       * Determines whether noteFinished() signal is emited in offTimer timeOut() slot.
       * Slot is also called by stop() method and then signal can't be emitted.
       */
  bool                         p_doEmit;
  QTimer                      *offTimer;
  EplayerType                  playerType;
  bool                         p_isPlaying = false;
  static unsigned int          p_posInNote, p_posInOgg;
  static int                   p_playingNoteNr, p_decodingNoteNr, p_playingNoteId;
  int                          p_tempo = 120;
  TaudioParams                *p_audioParams = nullptr;
  ToggScale                   *p_oggScale = nullptr;
  static int                   p_prevNote; /**< Previously played note number, when -100 means invalid  */
  static unsigned int          p_shiftOfPrev, p_lastPosOfPrev; /**< Helpers for previously played note */
  static unsigned int          p_beatPeriod; /**< Samples number of single metronome period, 0 - no metronome  */
  static unsigned int          p_beatBytes; /**< Number of bytes in single beat sample */
  static unsigned int          p_beatOffset; /**< Callback position in beat period */
  static bool                  p_lastNotePlayed; /**< @p TRUE set in callback only when last note just has been played */

private:
  QList<TsingleSound>          m_playList;
  QThread                     *m_thread;
  int                          m_noteToPlay = REST_NR;
  QList<Tnote>                 m_emptyList;
  QList<Tnote>                *m_listToPlay = nullptr;
  int                          m_firstNote = 0;
  Tmelody                     *m_melodyToPlay = nullptr;
  int                          m_transposition = 0;
  qint16                      *m_beatArray = nullptr;

private:
  void playThreadSlot();

};

#endif // TABSTRACTPLAYER_H
