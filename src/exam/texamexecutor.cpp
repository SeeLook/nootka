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
    m_isSolfege = false;

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

    checkAct = new QAction(tr("check answer"), this);
    checkAct->setStatusTip(checkAct->text());
    checkAct->setIcon(QIcon(gl->path+"picts/check.png"));
    // shortcuts
    connect(checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));

}


void TexamExecutor::createQuestionsList() {
    char openStr[6];
    for (int i=0; i<6; i++)
        openStr[i] = gl->Gtune()[i+1].getChromaticNrOfNote();


//    for (int i=0; i<6; i++) qDebug() << i << ": " << (int)openStr[strOrder[i]]
//            << " : " << (int)strOrder[i];
// 1. searching all frets in range, string by string
    for(int s = 0; s < 6; s++) {
        if (m_level.usedStrings[gl->strOrder(s)])// check string by strOrder
            for (int f = m_level.loFret; f <= m_level.hiFret; f++) {
                Tnote n = Tnote(gl->Gtune()[gl->strOrder(s)+1].getChromaticNrOfNote() + f);
            if (n.getChromaticNrOfNote() >= m_level.loNote.getChromaticNrOfNote() &&
                n.getChromaticNrOfNote() <= m_level.hiNote.getChromaticNrOfNote()) {
                bool hope = true; // we stil have hope that note is for an exam
                if (m_level.onlyLowPos) {
                    if (s > 0) {
                       // we have to check when note is on the lowest positions
                       // is it realy lowest pos
                       // when strOrder[s] is 0 - it is the highest sting
                        char diff = openStr[gl->strOrder(s-1)] - openStr[gl->strOrder(s)];
                       if( (f-diff) >= m_level.loFret && (f-diff) <= m_level.hiFret) {
                           hope = false; //There is the same note on highest string
                       }
                       else {
                           hope = true;
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
                    g.note = n; g.pos = TfingerPos(gl->strOrder(s)+1, f);
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
    clearWidgets();
    mW->setStatusMessage("");
    mW->startExamAct->setDisabled(true);
    mW->setMessageBg(gl->EquestionColor);

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
            // when answer is also asNote we determine key in preparing answer part
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
                mW->score->setKeySignature( // we randomize some key to cover this expected one
                        (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1))-7);
		mW->score->setKeyViewBg(gl->EanswerColor);
                QString keyTxt;
                if (qrand() % 2) // we randomize: ask for minor or major key ?
                    keyTxt = curQ.key.getMajorName();
                else
                    keyTxt = curQ.key.getMinorName();
                questText += tr(" <b>in %1 key.</b>", "in key signature").arg(keyTxt);

            } else {
                mW->score->setKeySignature(curQ.key);
		mW->score->setKeyViewBg(gl->EquestionColor);
            }
        }
//        if (m_level.forceAccids && curQ.questionAs != TQAtype::e_asName) {
        if (m_level.forceAccids) {
            Tnote::Eacidentals ac;
            if (curQ.questionAs == TQAtype::e_asNote) {// note has to be another than question
                m_note2 = forceEnharmAccid(curQ.qa.note); // m_note2 is expected note
                if (m_note2 == curQ.qa.note) {
                    qDebug() << "Blind question";
//                    askQuestion();
                }
                ac = (Tnote::Eacidentals)m_note2.acidental;
            } else {
                ac = (Tnote::Eacidentals)curQ.qa.note.acidental;
            }
            questText += getTextHowAccid(ac);
            mW->score->forceAccidental(ac);
        }
        mW->score->unLockScore();
        mW->score->setNoteViewBg(0, gl->EanswerColor);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        questText += TquestionAsWdg::asNameTxt;
        if (curQ.questionAs == TQAtype::e_asName) {
//            qDebug() << "as name as name";
            m_prevStyle = gl->NnameStyleInNoteName;
            Tnote::EnameStyle tmpStyle = m_prevStyle;
            if (m_isSolfege) {
                m_isSolfege = false;
                if (qrand() % 2) { // full name like cis, gisis
                    if (gl->seventhIs_B) tmpStyle = Tnote::e_nederl_Bis;
                    else tmpStyle = Tnote::e_deutsch_His;
                } else { // name and sign like c#, gx
                    if (gl->seventhIs_B) tmpStyle = Tnote::e_english_Bb;
                    else tmpStyle = Tnote::e_norsk_Hb;
                }
            } else {
                m_isSolfege = true;
                tmpStyle = Tnote::e_italiano_Si;
            }
            m_note2 = forceEnharmAccid(curQ.qa.note); // force other name of note
            if (m_note2 == curQ.qa.note) {
                qDebug() << "Blind question";
//                    askQuestion();
            }
            /** @todo change and restore style if needed */
            questText = QString("<b>%1. </b>").arg(m_answList.size()) +
                        tr("Give name of <span style=\"color: %1; font-size: %2px;\">").arg(
                                gl->EquestionColor.name()).arg(20) +
                        TnoteName::noteToRichText(curQ.qa.note) + ". </span>" +
                        getTextHowAccid((Tnote::Eacidentals)m_note2.acidental);
            mW->noteName->setNoteNamesOnButt(tmpStyle);
            gl->NnameStyleInNoteName = tmpStyle;
        }

        if (curQ.questionAs == TQAtype::e_asFretPos) {
            if (m_level.forceAccids) {
                questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa.note.acidental);
            }
        }
        mW->noteName->setNameDisabled(false);
    }

    if (curQ.answerAs == TQAtype::e_asFretPos) {
        questText += TquestionAsWdg::asFretPosTxt;
        if (curQ.questionAs == TQAtype::e_asName)
            questText += "<b>" + tr(" on (%1) string.").arg((int)curQ.qa.pos.str()) + "</b>";

        mW->guitar->setDisabled(false);
    }

    mW->setStatusMessage(questText);

    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
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

Tnote TexamExecutor::forceEnharmAccid(Tnote n) {
    Tnote nX;
    char acc = m_prevAccid;
    int cnt;
    do {
        acc++;
        if (acc > 2) acc = -2;

        if (acc == Tnote::e_DoubleFlat && m_level.withDblAcc)
            nX = n.showWithDoubleFlat();
        if (acc == Tnote::e_Flat && m_level.withFlats)
            nX = n.showWithFlat();
        if (acc == Tnote::e_Natural)
            nX = n.showAsNatural();
        if (acc == Tnote::e_Sharp && m_level.withSharps)
            nX = n.showWithSharp();
        if (acc == Tnote::e_DoubleSharp && m_level.withDblAcc)
            nX = n.showWithDoubleSharp();
        cnt++;
    } while (n == nX || cnt < 6);
    m_prevAccid = (Tnote::Eacidentals)acc;
    return nX;
}

void TexamExecutor::checkAnswer(){
    TQAunit curQ = m_answList[m_answList.size() - 1];
    curQ.time = mW->examResults->questionStop();
    mW->nootBar->removeAction(checkAct);
    mW->nootBar->addAction(nextQuestAct);
    mW->setMessageBg(gl->EanswerColor);
    mW->startExamAct->setDisabled(false);

// Let's check

    if (curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.manualKey) {
            if (mW->score->keySignature().value() != curQ.key.value())
                curQ.setMistake(TQAunit::e_wrongKey);
        }
        Tnote ntc = curQ.qa.note; // note to compare
        if (curQ.questionAs == TQAtype::e_asNote)
            ntc = m_note2;
        if (m_level.forceAccids) {
            if (mW->score->getNote(0) != ntc) {
                if (mW->score->getNote(0).showAsNatural() == ntc.showAsNatural())
                    curQ.setMistake(TQAunit::e_wrongAccid);
                else
                    curQ.setMistake(TQAunit::e_wrongNote);
            }
        } else { // no accid discrimination
            if (ntc.showAsNatural() != mW->score->getNote(0).showAsNatural())
                curQ.setMistake(TQAunit::e_wrongNote);
        }
    }
    /** @todo check octave */
    if (curQ.answerAs == TQAtype::e_asName) {
        Tnote ntc = curQ.qa.note; // note to compare
        if (curQ.questionAs == TQAtype::e_asName)
            ntc = m_note2;
        if (m_level.forceAccids) {
            if (mW->noteName->getNoteName() != ntc) {
                if (mW->noteName->getNoteName().showAsNatural() == ntc.showAsNatural())
                    curQ.setMistake(TQAunit::e_wrongAccid);
                else
                    curQ.setMistake(TQAunit::e_wrongNote);
            }
        } else { // no accid discrimination
            if (ntc.showAsNatural() != mW->noteName->getNoteName().showAsNatural())
                curQ.setMistake(TQAunit::e_wrongNote);
        }
        gl->NnameStyleInNoteName = m_prevStyle;
	mW->noteName->setNoteNamesOnButt(m_prevStyle);
    }

    QString answTxt;
    if (curQ.correct()) { // CORRECT
        answTxt = QString("<span style=\"color: %1;\">").arg(gl->EanswerColor.name());
        answTxt += tr("Exelent !!");
    } else { // WRONG
        answTxt = QString("<span style=\"color: %1;\">").arg(gl->EquestionColor.name());
        if (curQ.wrongNote())
            answTxt += tr("Wrong note.");
        if (curQ.wrongKey())
            answTxt += tr(" Wrong key signature.");
        if (curQ.wrongAccid())
            answTxt += tr(" Wrong accidental.");
    }
    answTxt += "</span>";
    mW->setStatusMessage(answTxt);
    mW->examResults->setAnswer(curQ.correct());

    disableWidgets();
//    mW->setStatusMessage("click next for next questtion<br>or press space bar", 2000);
//     gl->NnameStyleInNoteName = m_prevStyle;
//     mW->noteName->setNoteNamesOnButt(m_prevStyle);

    
}

void TexamExecutor::prepareToExam() {
    mW->setStatusMessage("exam started on level:<br><b>" + m_level.name + "</b>");

    mW->settingsAct->setDisabled(true);
    mW->levelCreatorAct->setDisabled(true);
    mW->startExamAct->setIcon(QIcon(gl->path+"picts/stopExam.png"));
    mW->startExamAct->setStatusTip(tr("stop the exam"));
    mW->startExamAct->setDisabled(true);

    disableWidgets();

    disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    disconnect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    disconnect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    disconnect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    connect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));

    /** @todo store globals

    */
    m_prevStyle = gl->NnameStyleInNoteName;
    m_glStore.nameStyleInNoteName = gl->NnameStyleInNoteName;
    m_glStore.showEnharmNotes = gl->showEnharmNotes;
    m_glStore.showKeySignName = gl->SshowKeySignName;
    m_glStore.showOtherPos = gl->GshowOtherPos;

    gl->showEnharmNotes = false;
    gl->SshowKeySignName = false;
    gl->GshowOtherPos = false;
    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->guitar->acceptSettings();
  // clearing all views/widgets
    clearWidgets();

//    mW->setStatusMessage("click next for next", 2000);

}

void TexamExecutor::restoreAfterExam() {
    gl->NnameStyleInNoteName = m_glStore.nameStyleInNoteName;
    gl->showEnharmNotes = m_glStore.showEnharmNotes;
    gl->SshowKeySignName = m_glStore.showKeySignName;
    gl->GshowOtherPos = m_glStore.showOtherPos;

    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->guitar->acceptSettings();

    mW->settingsAct->setDisabled(false);
    mW->levelCreatorAct->setDisabled(false);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setDisabled(false);

    connect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    connect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    disconnect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
    connect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    mW->score->isExamExecuting(false);
    mW->score->unLockScore();
    

}

void TexamExecutor::disableWidgets() {
    mW->noteName->setNameDisabled(true);
    mW->score->isExamExecuting(true);
    mW->score->setScoreDisabled(true);
    mW->guitar->setDisabled(true);
}

void TexamExecutor::clearWidgets() {
    mW->score->clearScore();
    mW->noteName->clearNoteName();
    mW->guitar->setFinger(Tnote(0,0,0));
}

void TexamExecutor::stopExamSlot() {
//    mW->setMessageBg();
    mW->setStatusMessage("so a pity");

    restoreAfterExam();
}

QString TexamExecutor::getTextHowAccid(Tnote::Eacidentals accid) {
    QString S = QString(" <span style=\"color: %1\">").arg(gl->GfingerColor.name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  "</span>";
    return S;

}
