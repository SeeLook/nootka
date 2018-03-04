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

#ifndef TSOUND_H
#define TSOUND_H


#include "nootkasoundglobal.h"
#include <QtCore/qobject.h>
#include <music/tnote.h>


class Tglobals;
class Tchunk;
class TnoteStruct;
class Tmelody;
class TabstractPlayer;
class TaudioIN;


#define   SOUND   Tsound::instance()


/**
 * Tsound is a wrapper of TaudioIN & TaudioOUT classes
 * to manage them. It enables/disables them depends on Tglobals,
 * pauses sniffing when playback is proceeding.
 * Also it has got TpitchView to show volume meter & pitch detection state.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p SOUND
 */
class NOOTKASOUND_EXPORT Tsound : public QObject
{

  Q_OBJECT

  Q_PROPERTY(int tempo READ tempo WRITE setTempo NOTIFY tempoChanged)
  Q_PROPERTY(int quantization READ quantization WRITE setQuantization NOTIFY quantizationChanged)
  Q_PROPERTY(bool stoppedByUser READ stoppedByUser WRITE setStoppedByUser NOTIFY stoppedByUserChanged)
  Q_PROPERTY(bool listening READ listening NOTIFY listeningChanged)
  Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
  Q_PROPERTY(Tnote finishedNote READ note NOTIFY noteFinished)

public:
  explicit Tsound(QObject *parent = nullptr);
  virtual ~Tsound();

  TabstractPlayer *player;
  TaudioIN  *sniffer;

  static Tsound* instance() { return m_instance; }

      /**
       * Initializes sound stack with delay to keep GUI creation quick.
       * It has to be called before audio input and output will be used
       */
  Q_INVOKABLE void init();

  Q_INVOKABLE void play(const Tnote& note);

  void playMelody(Tmelody* mel);

  void playScoreNotes(const QList<Tnote>& notes, int firstNote = 0);

  Q_INVOKABLE void playScore();

  bool isPlayable();

  bool isSniffable() { return (sniffer ? true : false) ; }

  bool melodyIsPlaying() { return m_melodyNoteIndex > -1; }

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

    /**
     * Before Nootka config dialog is created a few things have to be done.
     * stop sniffing, playing, delete midi, which blocks audio devices,
     */
//   Q_INVOKABLE void prepareToConf();

      /**
       * Also, when user will discard config, it has to restore its state.
       */
//   Q_INVOKABLE void restoreAfterConf();
  Q_INVOKABLE void acceptSettings();

  Q_INVOKABLE void stopListen();
  Q_INVOKABLE void startListen();

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

#if !defined (Q_OS_ANDROID)
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

      /**
       * When sound got initialized at the very beginning
       */
  void initialized();

private:
  void createPlayer();
  void createSniffer();
  void deletePlayer();
  void deleteSniffer();
  void restoreSniffer(); /**< Brings back sniffer & pitch view state as such as before settings dialog */

  Tnote                   m_detectedNote; /**< detected note */
  bool                    m_examMode;
  Tnote                   m_prevLoNote, m_prevHiNote; /**< notes form sniffer ambitus stored during an exam */
  bool                    m_stopSniffOnce, m_userState;;
  int                     m_melodyNoteIndex;
  Tmelody                *m_playedMelody;
  int                     m_tempo;
  int                     m_quantVal;

  static Tsound          *m_instance;

private slots:
    /*
     * Is performed when note stops playing, then sniffing is unlocked
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
