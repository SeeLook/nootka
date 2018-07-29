/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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

#ifndef TATTEMPT_H
#define TATTEMPT_H

#include "nootkacoreglobal.h"
#include <QtCore/qxmlstream.h>
#include <QtCore/qlist.h>


/**
 * This class describes an attempt to 'resolve/guess' an exercise/exam question
 * which is a note sequence (melody)
 * Not all members has usage for given question/answer type
 */
class NOOTKACORE_EXPORT Tattempt
{

public:
  Tattempt();

  virtual ~Tattempt();

      /**
       * Adds time and mistake of a note to the lists.
       */
  void add(quint32 mistake);

      /**
       * Type of mistake of every note in a melody
       */
  QList<quint32> mistakes;

      /**
       * How many times user played or listened to a question melody.
       */
  quint16 playedCount() const { return m_playedCounter; }

      /**
       * Increases playback counter
       */
  void melodyWasPlayed() { m_playedCounter++; }

      /**
       * @p TRUE when no mistakes and times in the lists and playback counter is 0
       */
  bool isEmpty() const { return mistakes.isEmpty() && m_playedCounter == 0; }

      /**
       * Total answer time of this attempt.
       */
  quint32 totalTime() const { return m_totalTime; }
  void setTotalTime(quint32 tt) { m_totalTime = tt; }

      /**
       * Time spent to prepare playing answer. Between question time start and detected first played note.
       */
  quint32 prepareTime() const { return m_prepTime; }
  void setPrepareTime(quint32 prepTime) { m_prepTime = prepTime; }

      /**
       * Logical sum of all mistakes in the attempt
       */
  quint32 summary() const { return m_sum; }

      /**
       * All notes were correct.
       */
  bool isCorrect() const { return m_sum == 0; }

      /**
       * Some note(s) are 'not bad' but none is wrong.
       */
  bool isNotBad() { return m_sum && !(m_sum & 64 || m_sum & 16); }

      /**
       * Some note(s) are wrong.
       */
  bool isWrong() { return m_sum & 64; }

  void toXml(QXmlStreamWriter& xml) const;
  void fromXml(QXmlStreamReader& xml);

      /**
       * Effectiveness of the attempt.
       */
  qreal effectiveness() const { return m_effectiveness; }

      /**
       * Calculates an effectiveness from mistakes, and logical mistakes sum.
       */
  void updateEffectiveness();

private:

  quint16              m_playedCounter;
  qreal                m_effectiveness;
  quint32              m_sum, m_totalTime, m_prepTime;

};

#endif // TATTEMPT_H
