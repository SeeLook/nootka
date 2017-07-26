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


#ifndef TMIDIOUT_H
#define TMIDIOUT_H

#include <QtCore/qstringlist.h>
#include <vector>
#include "tabstractplayer.h"
#include "nootkasoundglobal.h"


class RtMidiOut;
class TaudioParams;

/**
 * Midi is played by RtMidi class.
 */
class NOOTKASOUND_EXPORT TmidiOut : public TabstractPlayer
{

  Q_OBJECT

public:
  TmidiOut(TaudioParams *params, QObject *parent = nullptr);
  virtual ~TmidiOut();

  static QStringList getMidiPortsList();

  bool play(int noteNr);

        /**
         * Sets midi parameters:
         * @param portName, if empty system prefered is set (Timidity under Linux)
         * @param instrNr for instrument number in midi nomenclature.
         */
  void setMidiParams();

        /**
         * Deletes midi device if exists.
         * Midi device usually blocks audio devices,
         * so when it exists getAudioDevicesList() doesn't work
         */
  void deleteMidi();

        /**
         * Immediately stops playing. Emits nothing
         */
  void stop();


private:
  TaudioParams                  *m_params;

  RtMidiOut                     *m_midiOut;
  unsigned char                  m_prevMidiNote;
  std::vector<unsigned char>     m_message;
  unsigned int                   m_portNr;
  bool                           m_portOpened;

private:
  void openMidiPort();

private slots:
      /**
       * Turns off played @param m_prevMidiNote
       * If @param m_doEmit is true emits noteFinished() signal.
       */
  void midiNoteOff();
};

#endif // TMIDIOUT_H
