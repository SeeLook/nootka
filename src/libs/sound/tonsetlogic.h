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
#include <QtCore/qobject.h>


class NoteData;


/**
 * Simple container of audio data information.
 * Fed by @p setData() method finds and stores:
 * - highest PCM volume
 *
 * TODO: so far nothing more - if so, merge this class with @p TonSetLogic.
 */
class TonsetChunk
{
public:
    TonsetChunk(float* d, int len);
    TonsetChunk(float vol);

  void setData(float* d, int len);

      /**
       * The highest volume (amplitude) occurred in the chunk
       */
  float hiVol() const { return m_hiVol; }

private:
  float               m_hiVol = 0.0;
};

/**
 * Performs logic in looking for note onset and note end.
 * Audio data comes through @p analyzeChunk().
 * @p m_totalMaxVol keeps maximum volume so far occurred
 * and it is base to calculate onset silence thresholds.
 * So onset/end is obtained by PCM volume/energy
 *
 * @p TonsetLogic keeps recent @p CHUNKS_TO_LOOK (3) volume values to compare with.
 * When among those values volume rises above onset threshold
 * state changes to @p e_volOnset
 * then it is @p e_volPending when above silence threshold.
 * When it is in this state for @p minDuration() of chunks @p noteStarted() is @p TRUE.
 * But it has to be accepted @p acceptNote() by @p TpitchFinder to be tracked further,
 * or @p skipNote() to ignore it.
 * When below silence threshold - rest starts: @p restStarted() is @p TRUE then.
 *
 * @p startedAt() and @p finishedAt() return chunk number.
 * The rest is maintained by @p TpitchFinder.
 */
class TonsetLogic
{

  Q_GADGET

public:
    TonsetLogic();
    ~TonsetLogic() {}

  enum EvolumeState {
    e_volInitial = 0,
    e_volOnset, /**< moment when new note was detected */
    e_volPending,
    e_volSilence
  };
  Q_ENUM(EvolumeState);

      /**
       * Method to perform look up for note beginning in audio data.
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

      /** @p TpitchFinder doesn't use that */
  bool restFinished() const { return m_restFinished; }

  quint32 chunkNr() const { return m_chunkNr; }
  quint32 startedAt() const { return m_startedAt; }
  quint32 finishedAt() const { return m_finishedAt; }
  EvolumeState volumeState() const { return m_volumeState; }

  void skipNote();
  void acceptNote();

      /**
       * Resets states of all variables
       */
  void reset();

private:
  QList<TonsetChunk>                m_chunks;
  float                             m_dynamicVal = 0.0f;
  bool                              m_noteStarted = false, m_restStarted = false;
  bool                              m_noteFinished = false, m_restFinished = false;
  bool                              m_noteWasStarted = false, m_restWasStarted = false;
  bool                              m_firstNoteAccepted = false;
  quint32                           m_chunkNr = 0;
  quint32                           m_startedAt = 0, m_finishedAt = 0, m_silenceAt = 0;
  EvolumeState                      m_volumeState = e_volInitial;
  int                               m_minDuration;
  float                             m_totalMaxVol;
};

#endif // TONSETLOGIC_H
