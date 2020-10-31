/***************************************************************************
 *   Copyright (C) 2013-2020 by Tomasz Bojczuk                             *
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
#include <QtCore/qthread.h>



#define REST_NR (127) /**< Chromatic note number impossible to achieve identifying rest */
#define CNTDWN_ID (-7) /**< Nonexistent index of played note identifying countdown intro is playing */


class NOOTKASOUND_EXPORT TsingleSound {
public:
  TsingleSound(int nId = -1, qint16 nr = REST_NR, quint32 samples = 0) : id(nId), number(nr), samplesCount(samples) {}
  int id; /**< Note id in the melody */
  qint8 number; /**< number of note (chromatic) - none (rest) by default */
  quint32 samplesCount; /**< Number of samples the note takes */
};


class Tmelody;
class QTimer;
class Tnote;
class TaudioParams;
class ToggScale;
class TabstractPlayer;


/**
 * @p QThread subclass to perform audio data preparation for @p TabstractPlayer
 * @p run() creates @p playList() with @p TsingleSound list of notes to play,
 * then signal @p audioDataReady() is emitted to invoke @p TabstractPlayer::startPlaying() over player instance.
 */
class NOOTKASOUND_EXPORT TplayerThread : public QThread
{

  Q_OBJECT

  friend class TabstractPlayer;

public:
  TplayerThread(TabstractPlayer* pl);
  void run() override;

signals:
  void audioDataReady();

protected:
  QList<TsingleSound>& playList() { return m_playList; }

  int noteTopPlay() { return m_noteToPlay; }
  void setNoteToPLay(int ntp) { m_noteToPlay = ntp; }

  int firstNote() { return m_firstNote; }
  void setFirstNote(int fn) { m_firstNote = fn; }

  void setListToPlay(QList<Tnote>* ltp) { m_listToPlay = ltp; }
  void setMelodyToPlay(Tmelody* m) { m_melodyToPlay = m; }
  void setTransposition(int tra) { m_transposition = tra; }

  void preparePlayList(QList<Tnote>* notes, int tempo, int firstNote, int sampleRate, int transposition, int a440Ddiff);

private:
  TabstractPlayer             *m_player; /**< Instance of player (audio Rt, Qt) */
  QList<TsingleSound>          m_playList;
  int                          m_noteToPlay = REST_NR;
  QList<Tnote>                *m_listToPlay = nullptr;
  int                          m_firstNote = 0;
  Tmelody                     *m_melodyToPlay = nullptr;
  int                          m_transposition = 0;

};



/**
 * Abstract class for sound output. All sound back-ends (RtAudio, Qt audio, MIDI) derive from it.
 *
 * To trigger playing three methods are available:
 * @li @p playNote() - for single note
 * @li @p playNotes() - for note list (i.e. score notes)
 * @li @p playMelody() - for melody with defined tempo
 * Those methods just stores stuff to play and trigger thread to start playing.
 */
class NOOTKASOUND_EXPORT TabstractPlayer : public QObject
{

  Q_OBJECT

  friend class TplayerThread;

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
  bool playNotes(QList<Tnote>* notes, int tempo, int firstNote = 0, int countdownDur = 0);
  bool playMelody(Tmelody* melody, int transposition = 0, int countdownDur = 0);

  enum EplayerType { e_audio, e_midi };

  EplayerType type() { return playerType; }

  QList<TsingleSound>& playList() { return m_playThreaad->playList(); }

      /**
       * Number (id) of actually played note, if note list of @p playMelody comes from score
       * this number corresponds with note id on the score
       * or
       * CNTDWN_ID (-7) if empty bar is played before melody
       */
  int playingNoteId() const { return p_playingNoteId; }

  void setMetronome(unsigned int beatTempo);
  void stopMetronome();
  qint16 getBeatsample(unsigned int sampleNr) const { return m_beatArray[sampleNr]; }

  bool tickBeforePlay() const;
  void setTickBeforePlay(bool tbp);

  bool tickDuringPlay() const;
  void setTickDuringPlay(bool tdp);

      /**
       * Number of metronome ticks before melody will be played.
       * Also before audio input will be processed (pitch detection started).
       * It counts down, so becomes null when done,
       * so has to be set every time it is needed.
       */
  static int ticksCountBefore() { return p_ticksCountBefore; }
  static void setTicksCountBefore(int tcb) { p_ticksCountBefore = tcb; }

  bool doTicking() const;

  void setPitchOffset(qreal pitchOff);

      /**
       * Dirty mixing of two given samples
       */
  static qint16 mix(qint16 sampleA, qint16 sampleB);

  static QString soundTouchVersion();

signals:
  void playingStarted();

      /**
       * This signal is emitted when playing of a note/melody is finished.
       */
  void playingFinished();

  void nextNoteStarted();


protected:
  void setType(EplayerType type) { playerType = type; }

  virtual void startPlaying() = 0;

  void preparePlayList(QList<Tnote>* notes, int tempo, int firstNote, int sampleRate, int transposition, int a440diff);

  QThread* playThread() { return m_playThreaad; }

protected:
  bool                         p_playable;

      /**
       * Determines whether @p playingFinished() signal is emited (Mostly in Qt Audio in offTimer timeOut() slot).
       * Slot is also called by stop() method and then signal can't be emitted.
       */
  bool                         p_doEmit;
  QTimer                      *offTimer;
  EplayerType                  playerType;
  bool                         p_isPlaying = false;
  static unsigned int          p_posInNote, p_posInOgg;
  static int                   p_playingNoteNr, p_decodingNoteNr, p_playingNoteId;
  int                          p_tempo = 120;
  int                          p_countdownDur = 0; /**< countdown duration before melody will be played */
  TaudioParams                *p_audioParams = nullptr;
  ToggScale                   *p_oggScale = nullptr;
  static int                   p_prevNote; /**< Previously played note number, when -100 means invalid  */
  static unsigned int          p_shiftOfPrev, p_lastPosOfPrev; /**< Helpers for previously played note */
  static unsigned int          p_beatPeriod; /**< Samples number of single metronome period, 0 - no metronome  */
  static unsigned int          p_beatBytes; /**< Number of bytes in single beat sample */
  static unsigned int          p_beatOffset; /**< Callback position in beat period */
  static bool                  p_lastNotePlayed; /**< @p TRUE set in callback only when last note just has been played */
  static int                   p_ticksCountBefore; /**<   Number of metronome ticks before playing melody or sniffing */

private:
  TplayerThread               *m_playThreaad;
  qint16                      *m_beatArray = nullptr;

private:
  void playThreadSlot();

};

#endif // TABSTRACTPLAYER_H
