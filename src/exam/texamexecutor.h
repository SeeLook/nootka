/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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


#ifndef TEXAMEXECUTOR_H
#define TEXAMEXECUTOR_H

#include "tqaunit.h"
#include "ttune.h"
#include "texamlevel.h"
#include <QList>
#include <QColor>

class Tcanvas;
class QTimer;
class TexecutorSupply;
class Texam;
class MainWindow;
class QAction;

    /** This class manages of an exam executing */
class TexamExecutor : public QObject
{
    friend class MainWindow;

    Q_OBJECT

public:
    explicit TexamExecutor(MainWindow *mainW, QString examFile = "", TexamLevel *lev = 0);

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
      /** Describes state of exam executing */
    enum Estate {
      e_starting,
      e_failed, 
      e_openCreator,
      e_questioning,
      e_answering,
      e_saveing,
      e_finished
    };
    bool closeNootka();
    bool isAnswered() { return m_isAnswered; }

protected:
    MainWindow *mW;
    QAction *nextQuestAct, *prevQuestAct, *checkAct, *repeatSndAct;
    
    bool event(QEvent *event);

protected slots:
    void askQuestion();
        /** If it is called by pressing "check answer" it obviously shows results
        * but if app is closeing it only checks answer and save it without showing.*/
    void checkAnswer(bool showResults = true);
    void stopExamSlot();
    void repeatQuestion();
    void repeatSound();
    void autoRepeatStateChanged(bool enable);
        /** Shows exam summary. If returns false - user don't want to continue an exam */
    bool showExamSummary(bool cont);
    void showExamHelp();
    void expertAnswersSlot();
    void expertAnswersStateChanged(bool enable);
        /** Ivokes Tsound::go() */
    void startSniffing();
        /** Starts sniffing when asked note is finished */
    void sniffAfterPlaying();
    void rightButtonSlot();
    void tipButtonSlot(QString name);

private:
    void prepareToExam();
    void restoreAfterExam();
        /** Disables score, noteName and guitar*/
    void disableWidgets();
    void clearWidgets();
        /** Connects all Nootka widgets signals to expertAnswersSlot() */
    void connectForExpert();
    
private:
    QString saveExamToFile();
    void updatePenalStep();
    
    TexecutorSupply *m_supp; 
    Texam *m_exam;
          /** main instance of TexamLevel, others are poiters or references to it */
    TexamLevel m_level;
    QList<TQAunit::TQAgroup> m_questList;
          /** Invokes startSniffing() and stopPlaying() after delay
           * to avoid feedback betwen played question and listened answer. */
    QTimer *m_soundTimer;
    Tnote::EnameStyle m_prevQuestStyle, m_prevAnswStyle;
    TglStore m_glStore;
    TanswerRequire m_answRequire;
        /** Indicates when sniffing has to be ignored, 
         * because some dilaog window exist over exam. */
    bool m_snifferLocked;
    bool m_shouldBeTerminated, m_isAnswered, m_incorrectRepeated;
          /** If it is sets to TRUE locks invokeing event of right mouse button.
          * It has to be set before singleShot() method caled on askQuestion() 
          * to avoid user click button and call askQuestion() again during time of delay.*/
    bool m_lockRightButt;
          /** It becames true when user wants close Nootka
          * during an exam.*/
    bool m_goingClosed;
          /** stores note if question and answeer are Note Name to restore it if question is repeated
          It is to restorrre buttons state in NoteName widget whitch are uncheced by disableWidget() */
    Tnote m_prevNoteIfName;
    Tcanvas *m_canvas;
        /** -1 if no black, otherwise points question in blackList list. */
    int m_blackQuestNr;
        /** Interval of questions, after it penalty question is asked */
    int m_penalStep;
        /** Counts questions to ask penaltys one. */
    int m_penalCount;


};

#endif // TEXAMEXECUTOR_H
