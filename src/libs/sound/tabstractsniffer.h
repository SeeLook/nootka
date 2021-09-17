/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#ifndef TABSTRACTSNIFFER_H
#define TABSTRACTSNIFFER_H


#include "nootkasoundglobal.h"
#include <music/tnotestruct.h>

#include <QtCore/qobject.h>


class TaudioParams;


/**
 * @class TabstractSniffer is base class for all Nootka supported input back-ends:
 * RtAudio, Qt Audio and RtMidi.
 * It emits @p noteStarted() when new note was started playing (and its duration is appropriate)
 * and emits @p noteFinished() when note finished.
 */
class NOOTKASOUND_EXPORT TabstractSniffer : public QObject
{

  Q_OBJECT

public:
  TabstractSniffer(TaudioParams* params, QObject* parent = nullptr);

  enum EsnifferType { e_audio, e_midi };

  EsnifferType type() const { return p_snifferType; }
  bool isMIDI() { return p_snifferType == e_midi; }

      /**
       * State of input audio device:
       * @p e_listening - when input captures data and emits signals
       * @p e_paused - when data is capturing but signals about detected pitches are not emitting
       * @p e_stopped - capturing data is stopped.
       * Achieved by calling @p stopListening() and @p startListening()
       */
  enum Estate { e_detecting = 0, e_paused = 1, e_stopped = 2 };

      /**
       * Stops emitting signals about pitch detection, but detection is still performed.
       * It also resets last chunk pitch to ignore detection
       * It helps to sniff whole sound/note from begin to its end.
       */
  void pause() { if (m_state == e_detecting) setState(e_paused); }

  /**
   * Starts emitting @p noteStarted() and @p noteFinished() signals again.
   */
  void unPause() { if (m_state == e_paused) setState(e_detecting); }
  bool isPaused() { return m_state == e_paused; }
  bool isStoped() { return m_state == e_stopped; }

  Estate detectingState() { return m_state; }

      /**
       * This virtual method is responsible for starting listening
       */
  virtual void startListening() = 0;

      /**
       * This virtual method is responsible for stopping listening
       */
  virtual void stopListening() = 0;

      /**
       * Stores user action when he stopped sniffing himself.
       */
  void setStoppedByUser(bool userStop);
  bool stoppedByUser() const { return m_stoppedByUser; }

      /**
       * Sets range of notes which are detected. Others are ignored.
       */
      void setAmbitus(const Tnote& lowestNote, const Tnote& highestNote);

      /**
       * Returns lower boundary note of ambitus
       */
  Tnote& loNote() { return m_loNote; }

      /**
       * Returns upper boundary note of ambitus
       */
  Tnote& hiNote() { return m_hiNote; }


signals:
      /**
       * Emitted when note was played and its duration is longer than minimal duration
       */
  void noteStarted(const TnoteStruct&);

      /**
       * When already started note fade out and finished
       */
  void noteFinished(const TnoteStruct&);

      /**
       * When device changes its state. It can be cast on @p Estate enumeration.
       */
  void stateChanged(int);

protected:
  EsnifferType      p_snifferType = e_audio;
  TaudioParams     *p_audioParams;
  TnoteStruct       p_lastNote;
  bool              p_noteWasStarted = false;

protected:
  void setState(TabstractSniffer::Estate st) { m_state = st; emit stateChanged(static_cast<int>(st)); }

private:
  bool              m_stoppedByUser;

  Estate            m_state;
  Tnote             m_loNote, m_hiNote; /**< Boundary notes of the ambitus. */

};

#endif // TABSTRACTSNIFFER_H
