/***************************************************************************
 *   Copyright (C) 2014-2020 by Tomasz Bojczuk                             *
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

#include <QtCore/qobject.h>


class TnoteStruct;


/**
 * This class manages melodies during exams/exercises.
 * When user plays score, @p newMelody() is called to initialize list of played notes
 * stored in @p listened().
 * Then played note is started, so @p noteStarted() should be invoked to increase an index
 * When note is finished it is stored in the list through @p setNote().
 * When user click a score note to start playing from it @p wasIndexChenged() can be called.
 * then @p wasIndexChenged() returns @p TRUE but invoking @p noteStarted() 
 * will reset it to @p FALSE again.
 * This way can be counted how many times user started to play a melody.
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

  QList<TnoteStruct>& listened() { return m_listened; }

      /**
       * Clears list of notes and adds @p length number of empty notes. Sets index to 0.
       */
  void newMelody(int length);

      /**
       * Increases current index, note is listened.
       */
  void noteStarted();

      /**
       * Sets note of current index to given value.
       */
  void setNote(const TnoteStruct& n);

      /**
       * Index of current note in the list.
       */
  int currentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int id);
  bool wasIndexChanged() { return m_indexChanged; }

      /**
       * List of fixed notes in melody during exercise
       */
  QList<bool>& attemptFix() { return m_attemptFix; }

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

  QList<TnoteStruct>        m_listened;
  int                       m_currentIndex;
  int                       m_indexOfSaved; /**< Number of note that was recently saved through @p setNote() */
  bool                      m_indexChanged;
  int                       m_numberOfFixed;

  QList<bool>               m_attemptFix;
  int                       m_repeatCounter = 1;
  int                       m_lastMelodyId = 0;
};

#endif // TEXAMMELODY_H
