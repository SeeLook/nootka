/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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

#ifndef TEXERCISES_H
#define TEXERCISES_H


#include <QtWidgets/qdialog.h>

class QRadioButton;
class Texam;


/**
 * Class for managing exercising (exam like mode)
 * It gets answers state through @p checkAnswer(), checks is it necessary to suggest an exam
 * and emits @p messageDisplayed() signal during message.
 */
class Texercises : public QObject
{

  Q_OBJECT

public:
  Texercises(Texam *exam, QObject *parent = nullptr);

  void checkAnswer();

      /**
       * Sets checking of exercising progress enabled when @p obligate value is bigger than 0.
       * Given value should be qaPossibilities() calculated by @class TexecutorSupply.
       * Special cases are when exercises are melodies - it is controlled by @p melody.
       */
  void setSuggestionEnabled(int qaPosibilities, bool melody = false);

      /**
       * Returns user decision when he got message about starting an exam.
       * It affects global setting for further suggestions.
       */
  bool suggestInFuture() { return m_checkInFuture; }

      /**
       * Returns true when user decided to start exam after suggestion.
       */
  bool readyToExam() const { return m_readyToExam; }

      /**
       * Stores index of corrected note on the score.
       * By default it is -1 --> no note was corrected.
       * Score emits signal when correction finish
       */
  void setCorrectedNoteId(int noteId = -1) { m_correctedNoteId = noteId; }
  int idOfCorrectedNote() { return m_correctedNoteId; }

signals:
  void messageDisplayed();

      /**
       * Signal emitted after message with information about desire to start an exam.
       */
  void messageClosed(bool);

private:
  Texam         *m_exam;
  bool           m_checkInFuture = false;
  bool           m_checkNow = false;
  bool           m_readyToExam = false;
  int            m_max; /**< Number of questions in a cycle */
  int            m_currentGood; /**< Number of good answers since last mistake */;
  int            m_prevMistake;
  int            m_correctedNoteId = -1;

};



class TsuggestExam : public QDialog
{

  Q_OBJECT

public:
  explicit TsuggestExam();

  enum Esuggest {
    e_readyToExam, e_forAmoment, e_checkEntireMelody, e_notThisTime, e_neverEver
  };

      /**
       * Call this dialog and returns user decision
       */
  Esuggest suggest(bool entireVisible = false);

private:
  QRadioButton       *m_readyExamRadio, *m_checkEntireRadio, *m_notNowRadio, *m_notThisExRadio, *m_neverAskRadio;
  Esuggest            m_userResponse;
};

#endif // TEXERCISES_H

