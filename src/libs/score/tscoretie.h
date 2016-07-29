/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef TSCORETIE_H
#define TSCORETIE_H


#include <nootkacoreglobal.h>


class QGraphicsSimpleTextItem;
class TscoreNote;


/**
 * The bow connecting two notes (ligature)
 * It could be created by constructor but there is static method
 * @p TscoreTie::check() with @p TscoreNote as a parameter
 * that checks given note and next one to it are notes the same
 * and creates tie then, or if there is a tie already - deletes it.
 * Also @p check() method sets @p TscoreNote::tie() to point on this instance
 *
 * There is single bow adjusted to notes position if they lay at the same staff.
 * When second note lays on the next one, an additional bow is added - before second note.
 * @p checkStaves method manages it. It is invoked only from @p TscoreMeasure::setStaff.
 *
 * @p TscoreTie class also manages logical states of ties in @p TscoreNote::note.rtm.tie.
 * Constructor and destrucor keep it up to date
 */
class NOOTKACORE_EXPORT TscoreTie
{

  friend class TscoreNote;

public:
  explicit TscoreTie(TscoreNote* sn1, TscoreNote* sn2);
  virtual ~TscoreTie();

      /**
       * Initial static method to create new tie with next note to given one
       * or delete the tie if given note has it already.
       */
  static void check(TscoreNote* sn);


  TscoreNote* firstNote() { return m_firstNote; }
  TscoreNote* secondNote() { return m_scondNote; }

      /**
       * Compares are staves of notes the same.
       * Adds extra tie if different.
       */
  void checkStaves();

protected:
  void updateLength(); /**< Updates ties length according to current position */

private:
  QGraphicsSimpleTextItem* newTie(TscoreNote* parentNote);

private:
  TscoreNote                               *m_firstNote, *m_scondNote;
  QGraphicsSimpleTextItem                  *m_tieItem;
  QGraphicsSimpleTextItem                  *m_extraTieItem = nullptr; /**< Additional tie when 2nd note lays on another staff  */
};

#endif // TSCORETIE_H
