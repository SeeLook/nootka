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

#ifndef TONSETLOGIC_H
#define TONSETLOGIC_H


#include <QtCore/qlist.h>
// #include <QtCore/qobject.h>


class NoteData;


class TonSetChunk
{
public:
  TonSetChunk(float* d, int len);
  TonSetChunk(float vol);

  void setData(float* d, int len);

      /**
       * The highest volume (amplitude) occurred in the chunk
       */
  float hiVol() const { return m_hiVol; }

private:
  float               m_hiVol = 0.0;
};

/**
 * Performs logic in finding when note starts and stops.
 * Audio data comes through @p analyzeChunk().
 *
 */
class TonSetLogic
{

//   Q_GADGET

public:
  TonSetLogic();
  ~TonSetLogic();

  enum EnoteVolume {
    e_volInitial = 0,
    e_volOnSet, /**< moment when new note was detected */
    e_volPending,
    e_volSilence
  };
//   Q_ENUM(EnoteVolume);

      /**
       * The method to perform look up for note beginning in audio data.
       */
  void analyzeChunk(float* d, int len);

      /**
       * The energy or dynamic of volume change.
       * Only positive changes are registered (when volume grows).
       * It indicates when new note is beginning.
       */
  float dynamicValue() const { return m_dynamicVal; }
  float pcmVolume() const { return m_chunks.isEmpty() ? 0.0 : m_chunks.last().hiVol(); }

  int minDuration() const { return m_minDuration; }
  void setMinDuration(int md) { m_minDuration = md; }

  bool noteStarted() const { return m_noteStarted; }
  bool noteFinished() const { return m_noteFinished; }
  bool restStarted() const { return m_restStarted; }
  bool restFinished() const { return m_restFinished; }

  quint32 chunkNr() const { return m_chunkNr; }
  quint32 startedAt() const { return m_startedAt; }
  quint32 finishedAt() const { return m_finishedAt; }
  EnoteVolume noteVolume() const { return m_noteVolume; }

      /**
       * Resets states of all variables
       */
  void reset();

private:
  QList<TonSetChunk>                m_chunks;
  float                             m_dynamicVal = 0.0f;
  bool                              m_noteStarted = false, m_restStarted = false;
  bool                              m_noteFinished = false, m_restFinished = false;
  bool                              m_noteWasStarted = false, m_restWasStarted = false;
  quint32                           m_chunkNr = 0;
  quint32                           m_startedAt = 0, m_finishedAt = 0, m_silenceAt = 0;
  EnoteVolume                       m_noteVolume = e_volInitial;
  int                               m_minDuration;
  float                             m_totalMaxVol;
};

#endif // TONSETLOGIC_H
