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

#ifndef TEXTRANS_H
#define TEXTRANS_H


#include <nootkacoreglobal.h>
#include <QApplication>
#include <QString>

/**
 * This is static texts providing translations of common texts used in exams/exercises
 * It keeps contexts of previous location of the texts for compatibility with already existing translations
 */
class NOOTKACORE_EXPORT TexTrans
{

public:
  TexTrans() {};


   /**
    * Average time taken to answer
    */
static const QString averAnsverTimeTxt() {
    return QApplication::translate("TexamView", "Average time taken to answer");
  }

static const QString inSecondsTxt() { return QApplication::translate("TexamView", "[in seconds]"); } // [in seconds]

    /**
     * Total time
     */
static const QString totalTimetxt() { return QApplication::translate("TexamView", "Total time"); }
    /**
     * Effectiveness
     */
static const QString effectTxt() { return QApplication::translate("TexamView", "Effectiveness"); }

    /**
     * Number of correct answers
     */
static const QString corrAnswersNrTxt() {
    return QApplication::translate("TexamView", "Number of correct answers");
  }

  /**
   * Time for an answer
   */
static const QString reactTimeTxt() { return QApplication::translate("TexamView", "Time for an answer"); }

    /**
     * Number of mistakes
     */
static const QString mistakesNrTxt() { return QApplication::translate("TexamView", "Number of mistakes"); }

    /**
     * Number of not bad answers
     */
static const QString halfMistakenTxt() {
    return QApplication::translate("TexamView", "'Not bad' answers");
  }

  /**
   * (counted as half of a mistake)
   */
static const QString halfMistakenAddTxt() {
    return QApplication::translate("TexamView", "(counted as half of a mistake)");
  }


static const QString examFilterTxt() { return QApplication::translate("TstartExamDlg", "Exam results")  + " (*.noo)" ; }

    /**
     * Load an exam file
     */
static const QString loadExamFileTxt() {
    return QApplication::translate("TstartExamDlg", "Load an exam file");
  }

    /**
     * Get more levels <a>from Nootka home page</a>
     */
static QString moreLevelLinkTxt() {
    return QApplication::translate("LevelsPage" ,"Get more levels <a href=\"%1\">from Nootka home page</a>")
          .arg("https://nootka.sourceforge.io/index.php?C=down#levels");
  }

    /**
     * play melody
     */
static QString playMelodyTxt() { return QApplication::translate("Texam", "play melody"); }

    /**
     * write melody
     */
static QString writeMelodyTxt() { return QApplication::translate("Texam", "write melody"); }

    /**
     * attempt
     */
static QString attemptTxt() { return QApplication::translate("Texam", "attempt"); }

    /**
     * 7 attempts (or other number given as a parameter)
     */
static QString attemptsTxt(int aCount) { return QApplication::translate("Texam", "%n attempt(s)", "like: '1 attempt' or '121 attempts'", aCount); }

    /**
     * Play a melody written in a score
     */
static QString playDescTxt() { return QApplication::translate("Texam", "Play a melody written in a score"); }

    /**
     * Listen to a melody and write it on a score
     */
static QString writeDescTxt() { return QApplication::translate("Texam", "Listen to a melody and write it on a score"); }

    /**
     * question
     */
static QString questionTxt() { return QApplication::translate("QuestionsBox", "question"); }

    /**
     * questions
     */
static QString questionsTxt() { return QApplication::translate("QuestionsBox", "questions"); }

    /**
     * answer
     */
static QString answerTxt() { return QApplication::translate("QuestionsBox", "answer"); }

    /**
     * answers
     */
static QString answersTxt() { return QApplication::translate("QuestionsBox", "answers"); }

};

#endif // TEXTRANS_H





