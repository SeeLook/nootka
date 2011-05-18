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


#include "texamexecutor.h"
#include "tglobals.h"
#include "tstartexamdlg.h"
#include "tquestionaswdg.h"
#include "mainwindow.h"
#include <QDebug>

extern Tglobals *gl;

TexamExecutor::TexamExecutor(MainWindow *mainW)
{
    mW = mainW;

    TstartExamDlg *startDlg = new TstartExamDlg(mW);
    QString actTxt;
    TstartExamDlg::Eactions userAct = startDlg->showDialog(actTxt, m_level);
    if (userAct == TstartExamDlg::e_newLevel) {
//        qDebug() << "Level: " << m_level.name;

    } else 
      return;

    prepareToExam();
    createQuestionsList();

    m_prevAccid = Tnote::e_Natural;
    m_dblAccidsCntr = 0;
    m_level.questionAs.randNext(); // Randomize question and answer type
    for (int i = 0; i < 4; i++)
        m_level.answersAs[i].randNext();

    nextQuestAct = new QAction(tr("next question"), this);
    nextQuestAct->setStatusTip(nextQuestAct->text());
    nextQuestAct->setIcon(QIcon(gl->path+"picts/nextQuest.png"));
    connect(nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
    mW->nootBar->addAction(nextQuestAct);

}


void TexamExecutor::createQuestionsList() {
  // 1. creating array with guitar strings in order of their height
    char strOrder[6] = { 0,1,2,3,4,5};
    char openStr[6];
    for (int i=0; i<6; i++)
        openStr[i] = gl->Gtune[i+1].getChromaticNrOfNote();

    int i = 4;
    while (i > -1) {
        for (int j=i; j < 5 && openStr[strOrder[j]] < openStr[strOrder[j+1]]; j++) {
            char tmp = strOrder[j];
            strOrder[j] = strOrder[j+1];
            strOrder[j+1] = tmp;
        }
        i--;
    }

  // array done, stored in strOrder[6]

//    for (int i=0; i<6; i++) qDebug() << i << ": " << (int)openStr[strOrder[i]]
//            << " : " << (int)strOrder[i];

  // 2. searching all frets in range, string by string
    for(int s = 0; s < 6; s++) {
        if (m_level.usedStrings[strOrder[s]])// check string by strOrder
            for (int f = m_level.loFret; f <= m_level.hiFret; f++) {
            Tnote n = Tnote(gl->Gtune[strOrder[s]+1].getChromaticNrOfNote() + f);
            if (n.getChromaticNrOfNote() >= m_level.loNote.getChromaticNrOfNote() &&
                n.getChromaticNrOfNote() <= m_level.hiNote.getChromaticNrOfNote()) {
                bool hope = true; // we stil have hope that note is for an exam
                if (m_level.onlyLowPos) {
                    if (s > 0) {
//                    if (strOrder[s] > 0) {
                       // we have to check when note is on the lowest positions
                       // is it realy lowest pos
                       // when strOrder[s] is 0 - it is the highest sting
                       char diff = openStr[strOrder[s-1]] - openStr[strOrder[s]];
                       if( (f-diff) >= m_level.loFret && (f-diff) <= m_level.hiFret) {
                           hope = false; //There is the same note on highest string
//                           qDebug() << (int)s << (int)diff << (int)f-diff << "bad";
                       }
                       else {
                           hope = true;
//                           qDebug() << (int)strOrder[s] << (int)diff << (int)f-diff << "ok";
                       }
                    }
                }
                if (hope && m_level.useKeySign && m_level.onlyCurrKey) {
                  hope = false;
                  if (m_level.isSingleKey) {
                    if(m_level.loKey.inKey(n).note != 0)
                        hope = true;
                    } else {
                        for (int k = m_level.loKey.value(); k <= m_level.hiKey.value(); k++) {
                          if (TkeySignature::inKey(TkeySignature(k), n).note != 0) {
                            hope = true;
                            break;
                          }
                        }
                    }
                }
                if (hope) {
                    TQAunit::TQAgroup g;
                    g.note = n; g.pos = TfingerPos(strOrder[s]+1, f);
                    m_questList << g;
                }
            }
        }
    }

//    if (m_questList.size() == 0)
//    for (int i = 0; i < m_questList.size(); i++)
//        qDebug() << i << (int)m_questList[i].pos.str() << "f"
//                << (int)m_questList[i].pos.fret() << " note: "
//                << QString::fromStdString(m_questList[i].note.getName());
}

void TexamExecutor::askQuestion() {
    TQAunit curQ = TQAunit(); // current question
    curQ.qa = m_questList[qrand() % m_questList.size()];
    curQ.questionAs = m_level.questionAs.next();
    curQ.answerAs = m_level.answersAs[curQ.questionAs].next();

    if (curQ.questionAs == TQAtype::e_asNote || curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.useKeySign) {
            Tnote tmpNote = curQ.qa.note;
            if (m_level.isSingleKey) { //for single key
                curQ.key = m_level.loKey;
                tmpNote = m_level.loKey.inKey(curQ.qa.note);
            } else { // for multi keys
                curQ.key = TkeySignature((qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1))-7);
                if (m_level.onlyCurrKey) { // if hote is in current key only
                    int keyRangeWidth = m_level.hiKey.value() - m_level.loKey.value();
                    int patience = 0; // we are lookimg for suitable key
                    char keyOff = curQ.key.value() - m_level.loKey.value();
                    tmpNote = curQ.key.inKey(curQ.qa.note);
                    while(tmpNote.note == 0 && patience < keyRangeWidth) {
                        keyOff++;
                        if (keyOff > keyRangeWidth) keyOff = 0;
                        curQ.key = TkeySignature(m_level.loKey.value() + keyOff);
                        patience++;
                        tmpNote = curQ.key.inKey(curQ.qa.note);
                        if (patience >= keyRangeWidth) {
                            qDebug() << "Oops!! It should never happend. I can not find key signature!!";
                            break;
                        }
                    }
                }
            }
            curQ.qa.note = tmpNote;
        }
        if ( !m_level.onlyCurrKey) // if key dosen't determine accidentals, we do this
            curQ.qa.note = determineAccid(curQ.qa.note);
    }
//    qDebug() << QString::fromStdString(curQ.qa.note.getName()) << "Q" << (int)curQ.questionAs
//            << "A" << (int)curQ.answerAs << curQ.key.getMajorName()
//            << (int)curQ.qa.pos.str() << (int)curQ.qa.pos.fret();
    m_answList << curQ;

	    
  // ASKING QUESIONS
    QString questText = QString("<b>%1. </b>").arg(m_answList.size()); //question number
    if (curQ.questionAs == TQAtype::e_asNote) {
        questText += tr("Point given note ");
        char strNr = 0;
        if ( curQ.answerAs == TQAtype::e_asFretPos && !m_level.onlyLowPos )
            strNr = curQ.qa.pos.str(); //show string nr or not
        if (m_level.useKeySign && curQ.answerAs != TQAtype::e_asNote)
            // when answer is also asNote we determine key in preparing answer
            mW->score->askQuestion(curQ.qa.note, curQ.key, strNr);
        else mW->score->askQuestion(curQ.qa.note, strNr);

    }

    if (curQ.questionAs == TQAtype::e_asName) {
        mW->noteName->askQuestion(curQ.qa.note);
        questText += tr("Point given note name ");
    }

    if (curQ.questionAs == TQAtype::e_asFretPos) {
        mW->guitar->setFinger(curQ.qa.pos);
        questText += tr("Point given position ");
    }

// PREPARING ANSWERS
    if (curQ.answerAs == TQAtype::e_asNote) {
        questText += TquestionAsWdg::asNoteTxt;
        if (m_level.useKeySign) {
            if (m_level.manualKey) { // user have to manually secect a key
                mW->score->setKeySignature( // we randomize some key to cover this expected
                        (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1))-7);
                QString keyTxt;
                if (qrand() % 2) // we randomize: ask for minor or major key
                    keyTxt = curQ.key.getMajorName();
                else
                    keyTxt = curQ.key.getMinorName();
                questText += tr(" <u>in %1 key</u>", "in key signature").arg(keyTxt);

            } else {
                mW->score->setKeySignature(curQ.key);
            }
        }
        if (m_level.forceAccids && curQ.questionAs != TQAtype::e_asName) {
            if (curQ.questionAs != TQAtype::e_asNote)
                m_answList[m_answList.size()-1].note2 = determineAccid(curQ.qa.note);
            // force accidental in TscoreWidgetSimple
        }
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        questText += TquestionAsWdg::asNameTxt;
    }

    if (curQ.answerAs == TQAtype::e_asFretPos) {
        questText += TquestionAsWdg::asFretPosTxt;
    }

    mW->setStatusMessage(questText);

}

Tnote TexamExecutor::determineAccid(Tnote n) {
    Tnote nA = n;
    if (m_level.withSharps || m_level.withFlats || m_level.withDblAcc) {
        if (m_level.withDblAcc) {
            m_dblAccidsCntr++;
            if (m_dblAccidsCntr == 4) { //double accid note occurs every 4-th question
                if ( (qrand() % 2) ) // randomize dblSharp or dblFlat
                    nA = n.showWithDoubleSharp();
                else
                    nA = n.showWithDoubleFlat();
                if (nA == n) // dbl accids are not possible
                    m_dblAccidsCntr--;
                else
                    m_dblAccidsCntr = 0;
            }
        } else
            if (m_prevAccid != Tnote::e_Flat && m_level.withFlats) {
                nA = n.showWithFlat();
        } else
            if (m_prevAccid != Tnote::e_Sharp && m_level.withSharps) {
            nA = n.showWithSharp();
        }
    }
    m_prevAccid = (Tnote::Eacidentals)nA.acidental;
    return nA;
}

void TexamExecutor::CheckAnswer() {

}

void TexamExecutor::prepareToExam() {
    mW->setStatusMessage("exam started on level:<br><b>" + m_level.name + "</b>");
    mW->settingsAct->setDisabled(true);
    mW->levelCreatorAct->setDisabled(true);
    mW->startExamAct->setIcon(QIcon(gl->path+"picts/stopExam.png"));
    mW->startExamAct->setStatusTip(tr("stop the exam"));
    mW->startExamAct->setDisabled(true);
    mW->noteName->setNameDisabled(true);

    disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    disconnect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    disconnect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    mW->score->isExamExecuting(true);

  // clearing all views/widgets
    /** @todo hide key signature name */
    mW->score->clearScore();
    mW->noteName->setNoteName(Tnote(0,0,0));
    mW->guitar->setFinger(Tnote(0,0,0));
    


}

void TexamExecutor::restoreAfterExam() {
    mW->settingsAct->setDisabled(false);
    mW->levelCreatorAct->setDisabled(false);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);

    connect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    connect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    mW->score->isExamExecuting(false);
    
    mW->score->setEnableEnharmNotes(gl->showEnharmNotes);

}
