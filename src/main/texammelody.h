/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

#ifndef TEXAMMELODY_H
#define TEXAMMELODY_H


#include <music/tnotestruct.h>
#include "tnotetoplay.h"
#include <QtCore/qobject.h>


class Tmelody;


/**
 * This class manages melodies during exams/exercises.
 * When user plays score, @p newMelody() is called to initialize list of played notes
 * stored in @p played() list.
 * Then played note is started, so @p noteStarted() should be invoked to increase the index.
 * When note is finished it is stored in the list through @p setNote().
 * When user click a score note to start playing from it @p wasIndexChanged() can be called.
 * then @p wasIndexChenged() returns @p TRUE but invoking @p noteStarted()
 * will reset it to @p FALSE again.
 * This way we may count how many times user started to play a melody.
 *
 * @p toPlay() is a list of @p Tmelody notes but with folded tied notes and multi rests.
 * But is also keeps index of corresponding note in the real melody and in the score.
 * Internally in @p m_realList list index of each note
 * of that shortened played list is kept.
 *
 * It also manages of a list of fixed melody notes during correction.
 * List is stored in @p attemptFix() and can be reset by @p clearToFix().
 * Then @p setFixed() sets given note to @p TRUE and @p fixed() returns its state.
 */
class TexamMelody : public QObject
{

  Q_OBJECT

public:

  TexamMelody(QObject* parent);
  ~TexamMelody();

  QVector<TnoteStruct>& played() { return m_playedList; }
  TnoteStruct* currentNote() { return m_playedList.isEmpty() || m_currentIndex < 0 ? nullptr : &m_playedList[m_currentIndex]; }

  QVector<TnoteToPlay>& toPlay() { return m_toPlayList; }

  TnoteToPlay* nextToPlay() { return m_currentIndex + 1 < m_toPlayList.size() ? &m_toPlayList[m_currentIndex + 1] : nullptr; }
  TnoteToPlay* currentToPlay() { return m_currentIndex > -1 ? &m_toPlayList[m_currentIndex] : nullptr; }
  int nextToPlayIndex() { return m_currentIndex > -1 && m_currentIndex < m_toPlayList.count() ? m_toPlayList[m_currentIndex].index() : -1; }

      /**
       * Number of notes to play and to be played.
       * Both lists must have the same number of notes
       */
  int count() const { return m_playedList.count(); }

      /**
       * Returns played note corresponding to note number (id) on the score
       */
  TnoteStruct* listenedFromReal(int realId) { return realId >= m_realList.count() ? nullptr : &m_playedList[m_realList[realId]]; }

  void newMelody(Tmelody* m, int transposition = 0);

      /**
       * Increases current index, note is listened.
       */
  void noteStarted();

      /**
       * Sets note of current index to given value.
       */
  void setNote(const TnoteStruct& n);

  Tnote pitch(int id) const;

      /**
       * Index of current note in the list.
       */
  int currentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int realId);
  bool wasIndexChanged() { return m_indexChanged; }

      /**
       * Index of a note in real melody/score corresponding to currently played one
       */
  int realNoteId() const { return m_toPlayList[m_currentIndex].index(); }

      /**
       * List of fixed notes in melody during exercise
       */
  QVector<bool>& attemptFix() { return m_attemptFix; }

      /**
       * Clears the list and sets all given number elements to false.
       */
  void clearToFix(int notesCount);
  bool fixed(int noteNr) { return m_attemptFix[noteNr]; }

      /**
       * Sets given note number to true (fixed)
       */
  void setFixed(int noteNr);

      /**
       * Number of already fixed notes.
       */
  int numberOfFixed() { return m_numberOfFixed; }

      /**
       * It is @p TRUE when latest note was saved
       */
  bool wasLatestNoteSet() const { return m_indexOfSaved == m_currentIndex; }

      /**
       * Counts how many times the same melody was asked
       */
  int repeatCounter() const { return m_repeatCounter; }
  void setRepeat(int r) { m_repeatCounter = r; }

      /**
       * Increases repeat counter
       */
  void nextRepeat() { m_repeatCounter++; }

  int lastMelodyId() const { return m_lastMelodyId; }
  void setLastMelodyId(int li) { m_lastMelodyId = li; }

private:

  QVector<TnoteStruct>      m_playedList;
  QVector<int>              m_realList;
  QVector<TnoteToPlay>      m_toPlayList;
  int                       m_currentIndex;
  int                       m_indexOfSaved; /**< Number of note that was recently saved through @p setNote() */
  bool                      m_indexChanged;
  int                       m_numberOfFixed;
  bool                      m_tieOrRests = false; /**< @p True if melody contains tie or multiple rests */

  QVector<bool>             m_attemptFix;
  int                       m_repeatCounter = 1;
  int                       m_lastMelodyId = 0;
};

#endif // TEXAMMELODY_H
