/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TEXAMEXECUTOR_H
#define TEXAMEXECUTOR_H

#include "tqaunit.h"
#include "ttune.h"
#include "texamlevel.h"
#include <QList>

class Texam;
class MainWindow;
class QAction;
class QGraphicsTextItem;

    /** This class manages of an exam executing */
class TexamExecutor : public QObject
{
    friend class MainWindow;

    Q_OBJECT

public:
    explicit TexamExecutor(MainWindow *mainW, QString examFile = "");

    struct TglStore {
        bool showEnharmNotes;
        bool showKeySignName;
        bool showOtherPos;
        Tnote::EnameStyle nameStyleInNoteName;
        bool useDblAccids;
        bool useKeySign;
        Ttune tune;
        bool octaveInName;
        char fretsNumber;
    };
    struct TanswerRequire {
        bool octave;
        bool accid;
        bool key;
    };

    QString getNextQuestionTxt();
    bool closeNootka();
    bool isAnswered() { return m_isAnswered; }

protected:
    MainWindow *mW;
    QAction *nextQuestAct, *prevQuestAct, *checkAct, *repeatSndAct;

protected slots:
    void askQuestion();
        /** If it is called by pressing "check answer" it obviously shows results
        * but if app is closeing it only checks answer and save it without showing.*/
    void checkAnswer(bool showResults = true);
    void stopExamSlot();
    void repeatQuestion();
    void repeatSound();
        /** Clears message over the guitar.*/
    void clearMessage();
    void autoRepeatStateChanged(bool enable);
    void showExamSummary();
    void showExamHelp();
    void expertAnswersSlot();
    void expertAnswersStateChanged(bool enable);

private:
    void createQuestionsList();
    Tnote determineAccid(Tnote n);
        /** */
    Tnote forceEnharmAccid(Tnote n);

    void prepareToExam();
    void restoreAfterExam();
        /** Disables score, noteName and guitar*/
    void disableWidgets();
    void clearWidgets();
    QString getTextHowAccid(Tnote::Eacidentals accid);
    QString saveExamToFile();
    Tnote::EnameStyle randomNameStyle();
        /** Displays message with given @param htmlText over the fingerboard widget
        * for given @param time in [ms] or forever if 0.
        * To calculate position it has to have curent selected positions
        * by @param curPos. */
    void showMessage(QString htmlText, TfingerPos &curPos, int time = 0);

    Texam *m_exam;
      /** main instance of TexamLevel, others are poiters or references to it */
    TexamLevel m_level;
    QList<TQAunit::TQAgroup> m_questList;
        /** Previous accidentals used*/
    Tnote::Eacidentals m_prevAccid;
        /** intervals between asking about double accidentals.
        * By default every forth question is with double adccid.*/
    int m_dblAccidsCntr;
        /** to switch nameing style between "do re mi" and "c d e"
        * when question and answer are note name.*/
    bool m_isSolfege;
    Tnote::EnameStyle m_prevStyle;
    TglStore m_glStore;
    TanswerRequire m_answRequire;
    bool m_shouldBeTerminated, m_isAnswered, m_incorrectRepeated;
    QGraphicsTextItem *m_messageItem;

};

#endif // TEXAMEXECUTOR_H
