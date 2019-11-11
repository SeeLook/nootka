/***************************************************************************
 *   Copyright (C) 2015-2019 by Tomasz Bojczuk                             *
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

#include "tresulttext.h"
#include "tqaunit.h"
#include "tattempt.h"
#include <QtGui/qguiapplication.h>


    /**
     * Adds comma and space ', ' (',<br>' on Android) to not empty string or returns the same.
     */
void addSpaceToNotEmpty(QString& txt) {
  if (!txt.isEmpty()) {
#if defined (Q_OS_ANDROID)
      txt += QLatin1String(",<br>"); // new line for any mistake entry under Android (big letters)
#else
      txt += QLatin1String(", ");
#endif
  }
}


/** Checks the length of string @p txt and adds new line tag if necessary */
void newLineText(QString& txt, const QString& newText) {
#if !defined (Q_OS_ANDROID) // ignore it under mobile
  if (txt.length() > 20 && !txt.contains(QLatin1String("<br>")))
      txt += QLatin1String("<br>");
#endif
  txt += newText;
}


QString wasAnswerOKtext(TQAunit* answer, int attempt) {
  QString txt;
  TQAunit curQ;
  if (answer->melody() && attempt > 0 && attempt <= answer->attemptsCount())
    curQ.setMistake(answer->attempt(attempt - 1)->summary());
  else
    curQ.setMistake(answer->mistake());
  if (curQ.isCorrect()) {
    txt += QGuiApplication::translate("AnswerText", "Good answer!", "or 'Good!' or 'Correct!' would be somewhat more specific than merely 'It was good!' (previous version) 'It' in this case certainly does refer to a specific thing, which is in this case the answer, but it might be momentarily confused with some other specific thing, such as a shoe or a crocodile, or the wind on one's back. I know that's probably confusing, but the implied subject of 'Correct! is in a certain sense much more specific than a mere 'It' and is more certain to refer to the answer.");
  } else
      if (curQ.wrongNote() || curQ.wrongPos() || curQ.veryPoor())
          txt += QGuiApplication::translate("AnswerText", "Wrong answer!");
      else {
          txt += QGuiApplication::translate("AnswerText", "Not bad, but:", "'Not so bad, but:' is perfectly clear, but a little less common in US English. To be a bit shorter, it might just as well be, 'Not bad, but:'") + QLatin1String("<br>");
          QString misMes; // Message with mistakes
          if (curQ.wrongString())
            misMes = QGuiApplication::translate("AnswerText", "wrong string");
          if (answer->melody() && curQ.littleNotes())
            misMes = QGuiApplication::translate("AnswerText", "little valid notes", "the amount of correct notes in an answer is little");
          if (answer->melody() && curQ.wrongRhythm()) {
            addSpaceToNotEmpty(misMes);
            newLineText(misMes, QGuiApplication::translate("AnswerText", "incorrect rhythm"));
          }
          if (curQ.poorEffect()) {
            addSpaceToNotEmpty(misMes);
#if !defined (Q_OS_ANDROID) // Under mobile - above method does it
              if (!misMes.isEmpty())
                misMes += QLatin1String("<br>");
#endif
              misMes += QGuiApplication::translate("AnswerText", "poor effectiveness");
          }

          if (curQ.wrongAccid()) {
              misMes = QGuiApplication::translate("AnswerText", "wrong accidental");
          }
          if (curQ.wrongKey()) {
              addSpaceToNotEmpty(misMes);
              newLineText(misMes, QGuiApplication::translate("AnswerText", "wrong key signature"));
          }
          if (curQ.wrongOctave()) {
              addSpaceToNotEmpty(misMes);
              newLineText(misMes, QGuiApplication::translate("AnswerText", "wrong octave"));
          }
          if (curQ.wrongIntonation()) {
              addSpaceToNotEmpty(misMes);
              newLineText(misMes, QGuiApplication::translate("AnswerText", "out of tune"));
          }
          txt += misMes;
      }
  return txt;

}

