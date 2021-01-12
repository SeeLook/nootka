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

#ifndef TNOTETOPLAY_H
#define TNOTETOPLAY_H


#include <music/tnote.h>


class Tmelody;


/**
 * @class TnoteToPlay is simple container for a melody note that is either a question or an answer.
 * It stores chromatic number of a note @p pitch(), note @p duration() (summary of tied notes)
 * and @p index() of a note in melody (or other note list) - if note are tied it is index of first one.
 */
class TnoteToPlay
{
public:
  TnoteToPlay(const Tnote& note, int index, int transposition = 0);

      /**
       * Default constructor required by @p QVector.
       */
  TnoteToPlay() {}

  int pitch() const { return m_pitch; }
  int duration() const { return m_duration; }

      /**
       * Index (number) that corresponds with a  melody or a score
       * from which this note structure was created
       */
  int index() const { return m_index; }
  bool isRest() const { return m_pitch == 127; }

      /**
       * Appends duration of given @p note to existing duration
       */
  void append(const Tnote& note) { m_duration += note.duration(); }

      /**
       * Converts given melody list on @p TnoteToPlay objects
       * - stores only chromatic pitch number and duration,
       * folds all ties and multiple rests into single elements
       * with summarized duration.
       * Utilizes transposition if any.
       */
  static QVector<TnoteToPlay> fromMelody(Tmelody* m, int transposition);

private:
  qint16          m_pitch = 127; // rest
  int             m_duration = 0;
  qint16          m_index = 0;
};

#endif // TNOTETOPLAY_H
