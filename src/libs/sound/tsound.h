/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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

#ifndef TSOUND_H
#define TSOUND_H


#include "nootkasoundglobal.h"
#include <QtCore/qobject.h>
#include <music/tnote.h>
#include <QtCore/qelapsedtimer.h>


class Tglobals;
class Tchunk;
class TnoteStruct;
class Tmelody;
class TabstractPlayer;
class TaudioIN;


#define   SOUND   Tsound::instance()


/**
 * @class Tsound is a wrapper of @p TaudioIN & @p TaudioOUT classes
 * to manage them. It enables/disables them depends on @p Tglobals,
 * pauses sniffing when playback is proceeding.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p SOUND, also available from QML under this name.
 * It exposes needed properties and methods to QML
 */
class NOOTKASOUND_EXPORT Tsound : public QObject
{

  Q_OBJECT

  Q_PROPERTY(int tempo READ tempo WRITE setTempo NOTIFY tempoChanged)
  Q_PROPERTY(int beatUnit READ beatUnit WRITE setBeatUnit NOTIFY tempoChanged)
  Q_PROPERTY(int quantization READ quantization WRITE setQuantization NOTIFY quantizationChanged)
  Q_PROPERTY(bool stoppedByUser READ stoppedByUser WRITE setStoppedByUser NOTIFY stoppedByUserChanged)
  Q_PROPERTY(bool listening READ listening NOTIFY listeningChanged)
  Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
  Q_PROPERTY(Tnote finishedNote READ note NOTIFY noteFinished)
  Q_PROPERTY(bool tunerMode READ tunerMode NOTIFY tunerModeChanged)
  Q_PROPERTY(bool tickBeforePlay READ tickBeforePlay WRITE setTickBeforePlay NOTIFY tickStateChanged)
  Q_PROPERTY(bool tickDuringPlay READ tickDuringPlay WRITE setTickDuringPlay NOTIFY tickStateChanged)
  Q_PROPERTY(int playingNoteId READ playingNoteId NOTIFY playingNoteIdChanged)
  Q_PROPERTY(bool metroRunning READ metroRunning NOTIFY metroRunningChanged)

  friend class TtunerDialogItem;

public:
  explicit Tsound(QObject *parent = nullptr);
  virtual ~Tsound();

  TabstractPlayer *player;
  TaudioIN  *sniffer;

  static Tsound* instance() { return m_instance; }

  Q_INVOKABLE static QString soundTouchVersion();

      /**
       * Initializes sound stack with delay to keep GUI creation quick.
       * It has to be called before audio input and output will be used
       */
  Q_INVOKABLE void init();

  Q_INVOKABLE void play(const Tnote& note);

  void playMelody(Tmelody* mel, int transposition = 0);

  void playNoteList(QList<Tnote>& notes, int firstNote, int countdownDuration = 0);

  bool isPlayable();

  bool isSniffable() { return (sniffer ? true : false) ; }

  bool melodyIsPlaying();

      /**
       * Normalized volume of detected note
       */
  Q_INVOKABLE qreal inputVol();

      /**
       * Value representing fractional part of the pitch in current chunk,
       * it expressing a clearness of the pitch
       */
  Q_INVOKABLE qreal pitchDeviation();

  Q_INVOKABLE QStringList inputDevices() const;
  Q_INVOKABLE QStringList outputDevices() const;

  Q_INVOKABLE QString currentInDevName() const;
  Q_INVOKABLE QString currentOutDevName() const;

  Q_INVOKABLE void setJACKorASIO(bool setOn);

  Q_INVOKABLE void acceptSettings();

  Q_INVOKABLE void stopListen();
  Q_INVOKABLE void startListen(bool skipPreTicking = true);

      /**
       * Stops both, playing and listening
       */
  Q_INVOKABLE void stop();

      /**
       * Returns recently detected note.
       */
  Q_INVOKABLE Tnote note() const { return m_detectedNote; }

      /**
       * Returns recently detected pitch of note (midi number).
       */
  float pitch();

  int tempo() const { return m_tempo; }
  void setTempo(int t);

  int beatUnit() const { return m_beatUnit; }
  void setBeatUnit(int bu);

      /**
       * Currently set meter in main score.
       * Due to main score lays above sound in libraries hierarchy,
       * main score controls it when its meter changes
       */
  Q_INVOKABLE int currentMeter() const { return m_currentMeter; }
  Q_INVOKABLE void setCurrentMeter(int curMet);

  Q_INVOKABLE void setMetronome(int metronomeTempo, int metronomeBeat);

      /**
       * Runs metronome routines placed in @p player but only when @p tickBeforePlay or @p tickDuringPlay are set.
       * If @p preTicksNr is set and @p tickBeforePlay is enabled,
       * calculates tick number to countdown before playing or listening.
       * Emits @p countdownPrepare(int preTicksNr) to initialize QML part of pre-ticking
       */
  void runMetronome(int preTicksNr = 0);

      /**
       * Quantization value determines accuracy of detecting rhythm of played note by its duration.
       */
  void setQuantization(int q);
  int quantization() const { return m_quantVal; }

  bool stoppedByUser() const;
  void setStoppedByUser(bool sbu);

  bool listening() const;

  bool playing() const;

  void pauseSinffing();
  void unPauseSniffing();
  bool isSnifferPaused();
  bool isSniferStopped();

  bool tickBeforePlay() const;
  void setTickBeforePlay(bool tbp);

  bool tickDuringPlay() const;
  void setTickDuringPlay(bool tdp);

  int playingNoteId() const;

      /**
       * Property about state of a metronome.
       * It is automatically initialized when @p runMetronome() is invoked
       * and disabled when playing and listening are not performed
       */
  bool metroRunning() { return m_metronomeIsRun; }

      /**
       * Prepares sound to exam.
       * Given notes in params are level range notes and are put to sniffer ambitus.
       */
  void prepareToExam(Tnote loNote, Tnote hiNote);
  void restoreAfterExam();
  Q_INVOKABLE void stopPlaying();

      /**
       * Instrument scale extended of perfect 4th up and down.
       */
  void setDefaultAmbitus();

      /**
       * Locks/unlocks connections between pitch detection and other widgets (score, instrument)
       */
  bool tunerMode() const { return m_tunerMode; }
  void setTunerMode(bool isTuner);

#if defined (Q_OS_ANDROID)
  Q_INVOKABLE int maxVolRange() const;
  Q_INVOKABLE int currentVol() const;
  Q_INVOKABLE void setVol(int v);
  Q_INVOKABLE void setTouchHandling(bool th);
#else
  void setDumpFileName(const QString& fName);
#endif

signals:
  void noteStarted(const Tnote&);
  void noteStartedEntire(const TnoteStruct&);
  void noteFinished();
  void noteFinishedEntire(const TnoteStruct&);
  void plaingFinished();
  void tempoChanged();
  void quantizationChanged();
  void stoppedByUserChanged();
  void listeningChanged();
  void playingChanged();
  void tunerModeChanged();
  void tickStateChanged();
  void playingNoteIdChanged();
  void metroRunningChanged();
  void countdownPrepare(int tickCount);
  void volumeKeyPressed();
  void volumeUpPressed();
  void volumeDownPressed();

      /**
       * When sound got initialized at the very beginning
       */
  void initialized();

protected:
#if defined (Q_OS_ANDROID)
  bool eventFilter(QObject* watched, QEvent* event) override;
  QElapsedTimer m_volKeyTimer; /**< Time between volume keys release to avoid too many. */
#endif

private:
  void createPlayer();
  void createSniffer();
  void deletePlayer();
  void deleteSniffer();
  void restoreSniffer(); /**< Brings back sniffer & pitch view state as such as before settings dialog */

  void stopMetronome();

  Tnote                   m_detectedNote; /**< detected note */
  bool                    m_examMode = false;
  bool                    m_tunerMode = false;
  Tnote                   m_prevLoNote, m_prevHiNote; /**< notes form sniffer ambitus stored during an exam */
  bool                    m_stopSniffOnce, m_userState;;
  Tmelody                *m_playedMelody;
  int                     m_tempo;
  int                     m_beatUnit = 0; /**< corresponds with Tmeter::EbeatUnit enum. Quarter by default */
  int                     m_currentMeter = 0;
  int                     m_quantVal;
  bool                    m_metronomeIsRun = false;

  static Tsound          *m_instance;

#if defined (Q_OS_ANDROID)
  int                     m_maxVol; /**< Maximal volume of Android device */
  int                     m_currVol; /**< Current volume of Android device */
#endif

private:
  void playingStartedSlot();
      /**
       * Is performed when note or melody stops playing, then sniffing is unlocked
       */
  void playingFinishedSlot();

  void noteStartedSlot(const TnoteStruct& note);
  void noteFinishedSlot(const TnoteStruct& note);

      /**
       * Checks current played note by audio out and selects it on the score,
       * It calls itself by timer until played note is not the last on the score
       */
  void selectNextNote();

};

#endif // TSOUND_H
