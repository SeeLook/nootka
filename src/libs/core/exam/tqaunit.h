/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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


#ifndef TQAUNIT_H
#define TQAUNIT_H

#include <nootkacoreglobal.h>
#include "tqagroup.h"
#include <music/tkeysignature.h>
#include <exam/tqatype.h>


#define CORRECT_EFF (100.0) // effectiveness of correct answer
#define NOTBAD_EFF (50.0) // effectiveness of 'not bad' answer
#define ATTEMPT_FEE (0.96) // decrease effectiveness by 4% (100 - 4 = 96) for every additional attempt


class Texam;
class Tattempt;
class Tmelody;


/**
 * This class describes single question and given answer.
 * By default a melody element (pointer) is empty and attempts as well.
 * But after @p newAttempt() and @p addMelody() those elements exist inside
 * until destructor work.
 * WARRING! Melody (Tmelody) and Attempts list objects are never copied with operator=
 */
class NOOTKACORE_EXPORT TQAunit
{

public:

      /**
       * Copy constructor
       */
  TQAunit(const TQAunit& otherUnit) { copy(otherUnit); }

      /**
       * Pointer to exam that keeps this unit
       */
  TQAunit(Texam* exam = nullptr);

  ~TQAunit();

  enum Emistake {
      e_correct = 0,
      e_wrongAccid = 1, /**< occurs during enharmonic conversion */
      e_wrongKey = 2,
      e_wrongOctave = 4,
      e_wrongStyle = 8, /**< for further releases when typing of note name will be implemented */
      e_wrongPos = 16, /**< when wrong position */
      e_wrongString = 32, /**< when sound is proper but not on required string */
      e_wrongNote = 64, /**< the highest crime */
      e_wrongIntonation = 128, /**< when detected sound is out of range of intonation accuracy */
      e_littleNotes = 256, /**< when number of notes in answered melody is little */
      e_poorEffect = 512, /**< effectiveness of an answered melody is less than 70% (but greater than 50%) */
      e_veryPoor = 1024, /**< effectiveness less than 50% - CARDINAL MISTAKE */
      e_wrongRhythm = 2048, /**< more notes has wrong rhythm - CARDINAL MISTAKE */
      e_wrongTempo = 4096 /**< melody was played to slow or to fast */
  };

      /**
       * Copies given unit into this one.
       * IT DOESN'T COPY A MELODY AND ATTEMPTS but only their pointers
       */
  void copy(const TQAunit& otherUnit);

      /**
       * Returns string with time divided by 10.
       * Usually time is stored in value multiplied by 10.
       * @param prec defines digit number after point.
       */
  static QString timeToText(int time10, int prec = 1) { return QString("%1").arg(static_cast<qreal>(time10) / 10, 0, 'f', prec); }

      /**
       * Gives ready to insert string with time value.
       */
  QString timeText() { return timeToText(time); }

      /**
       * Returns time value divided by 10
       */
  double getTime() { return static_cast<double>(time) / 10.0; }

      /**
       * Set a given mistake.
       * If actual effectiveness is necessary invoke updateEffectiveness() after.
       */
  void setMistake(Emistake mis);

      /**
       * Sets mistakes from value. It is the same:
       * setMistake(e_wrongKey); setMistake(e_wrongOctave);
       * and
       * setMistake(e_wrongKey | e_wrongOctave);
       * or
       * setMistake(6);
       * If actual effectiveness is necessary invoke updateEffectiveness() after.
       */
  void setMistake(quint32 misVal) { p_valid = misVal; }

      /**
       * set of mistakes as Boolean sum of @p Emistake
       */
  quint32 mistake() const { return p_valid; }

  TQAgroup qa;
  TQAtype::Etype questionAs;
  TQAtype::Etype answerAs;

  Tnote::EnameStyle styleOfQuestion() const { return Tnote::EnameStyle(style / 16 - 1);  }
  Tnote::EnameStyle styleOfAnswer() const { return Tnote::EnameStyle(style % 16);  }
  void setStyle(Tnote::EnameStyle questionStyle, Tnote::EnameStyle answerStyle) {
    style = (static_cast<quint8>(questionStyle) + 1) * 16 + static_cast<quint8>(answerStyle);
  }

  TkeySignature key;

      /**
       * time of answer multiple by 10
       */
  quint16 time;

      /**
       * expected answers when question and answer types are the same
       */
  TQAgroup qa_2;

  friend bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit);

  bool isCorrect() const { return p_valid == e_correct; }
  bool wrongAccid() const { return p_valid & e_wrongAccid; }
  bool wrongKey() const { return p_valid & e_wrongKey; }
  bool wrongOctave() const { return p_valid & e_wrongOctave; }
  bool wrongStyle() const { return p_valid & e_wrongStyle; }
  bool wrongPos() const { return p_valid & e_wrongPos; }
  bool wrongString() const { return p_valid & e_wrongString; }
  bool wrongNote() const {return p_valid & e_wrongNote; }
  bool wrongIntonation() const {return p_valid & e_wrongIntonation; }

      /**
       * when number of notes in answered melody is little
       */
  bool littleNotes() const {return p_valid & e_littleNotes; }

      /**
       * effectiveness is less than 70% (but greater than 50%)
       */
  bool poorEffect() const {return p_valid & e_poorEffect; }

      /**
       * effectiveness of an answered melody is less than 50%
       */
  bool veryPoor() const {return p_valid & e_veryPoor; }

      /**
       * more notes has wrong rhythm - CARDINAL MISTAKE
       */
  bool wrongRhythm() const { return p_valid & e_wrongRhythm; }

      /**
       * melody was played to slow or to fast
       */
  bool wrongTempo() const { return p_valid & e_wrongTempo; }

      /**
       * questionAs == TQAtype::e_onScore
       */
  bool questionOnScore() const { return questionAs == TQAtype::e_onScore; }

      /**
       * questionAs == TQAtype::e_asName
       */
  bool questionAsName() const { return questionAs == TQAtype::e_asName; }

      /**
       * questionAs == TQAtype::e_onInstr
       */
  bool questionOnInstr() const { return questionAs == TQAtype::e_onInstr; }

      /**
       * questionAs == TQAtype::e_asSound
       */
  bool questionAsSound() const { return questionAs == TQAtype::e_asSound; }

      /**
       * answerAs == TQAtype::e_onScore
       */
  bool answerOnScore() const { return answerAs == TQAtype::e_onScore; }

      /**
       * answerAs == TQAtype::e_asName
       */
  bool answerAsName() const { return answerAs == TQAtype::e_asName; }

      /**
       * answerAs == TQAtype::e_onInstr
       */
  bool answerOnInstr() const { return answerAs == TQAtype::e_onInstr; }

      /**
       * answerAs == TQAtype::e_asSound
       */
  bool answerAsSound() const { return answerAs == TQAtype::e_asSound; }

  bool isWrong() const { return wrongNote() || wrongPos() || veryPoor(); }
  bool isNotSoBad() const { return p_valid && !wrongNote() && !wrongPos(); }

      /**
       * Parent exam or null.
       */
  Texam* exam() const { return m_exam; }

      /**
       * Creates and adds new @class Tattempt to the attempts list.
       */
  void newAttempt();
  int attemptsCount() const { return attemptList ? attemptList->size() : 0; }

      /**
       * Pointer to given attempt
       */
  Tattempt* attempt(int nr) { return attemptList && nr < attemptsCount() ? attemptList->operator[](nr) : nullptr; }

      /**
       * Pointer to the last attempt
       */
  Tattempt* lastAttempt() { return attemptList->last(); }

      /**
       * Returns number of melody playback in all attempts.
       */
  int totalPlayBacks();

      /**
       * Returns effectiveness of this answer.
       * For single note it is 100 for correct, 50 for 'not bad' and 0 if wrong,
       * for melody it is average of all attempts effectiveness
       */
  qreal effectiveness() const { return m_effectiveness; }

      /**
       * Updates an effectiveness value to current answer state
       */
  void updateEffectiveness();

      /**
       * Determines a source of melody. @p e_srcOtherUnit points melody in another unit
       * @p e_srcLevelSet  points melody in level
       */
  enum EmelodySrc {
    e_srcNoMelody, e_srcThisUnit, e_srcOtherUnit, e_srcLevelSet
  };

      /**
       * Adds melody or replaces existing one.
       */
  void addMelody(const QString& title);

      /**
       * Adds melody existing in given source (other unit or a list)
       * and with @p id (question number or index in the list)
       */
  void addMelody(Tmelody* mel, EmelodySrc source, int id);

      /**
       * Pointer to question melody or null if no melody.
       */
  Tmelody* melody() const { return m_melody; }
  EmelodySrc melodySource() const { return m_srcMelody; }

      /**
       * Finishes answering process. Unit (a question) becomes answered.
       */
  void setAnswered() { m_answered = true; }

      /**
       * Sets @p answered() to @p FALSE
       */
  void unsetAnswered() { m_answered = false; }

      /**
       * @p TRUE when answering process was finished.
       */
  bool answered() const { return m_answered; }

  void toXml(QXmlStreamWriter& xml);
  bool fromXml(QXmlStreamReader& xml);

      /**
       * This method simply rises all unit notes one octave up.
       * The ONLY ONE PURPOSE of this method is to convert/fix
       * bass dropped down clef of old Nootka to ordinary bass clef,
       * so all notes has to transposed octave up
       */
  void riseOctaveUp();

      /**
       * Number of another question (@p e_srcOtherUnit)
       * or an item in a list (@p e_srcLevelSet) that contain a melody.
       */
  int idOfMelody() const { return p_idOfMelody; }

protected:
  quint32                 p_valid;
  quint8                  style;
  int                     p_idOfMelody;
  QList<Tattempt*>       *attemptList; /**< Pointer to a list with attempts or @p nullptr if no attempts */

      /**
       * Deletes this melody if exists and belongs to this unit.
       */
  void deleteMelody();

private:
  Tmelody                *m_melody;
  EmelodySrc              m_srcMelody;
  qreal                   m_effectiveness;
  Texam                  *m_exam;
  bool                    m_answered;
};

NOOTKACORE_EXPORT bool getTQAunitFromStream(QDataStream &in, TQAunit &qaUnit);

#endif // TQAUNIT_H
