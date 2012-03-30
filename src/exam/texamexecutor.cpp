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

#include "texamexecutor.h"
#include "tglobals.h"
#include "tstartexamdlg.h"
#include "tquestionaswdg.h"
#include "tlevelselector.h"
#include "tsound.h"
#include "mainwindow.h"
#include "texam.h"
#include "texamsummary.h"
#include "examsettings.h"
#include "texamhelp.h"
#include "texpertanswerhelp.h"
#include "texamparams.h"
#include "texecutorsupply.h"
#include <QtGui>
#include <QDebug>

#define WAIT_TIME (600) //[ms]

extern Tglobals *gl;


TexamExecutor::TexamExecutor(MainWindow *mainW, QString examFile) :
  m_exam(0),
  mW(mainW),
  m_lockRightButt(false),
  m_goingClosed(false)
{
    QString resultText;
    TstartExamDlg::Eactions userAct;

    mW->sound->wait();
    if (examFile == "") { // start exam dialog
        TstartExamDlg *startDlg = new TstartExamDlg(gl->E->studentName, mW);
        userAct = startDlg->showDialog(resultText, m_level);
        delete startDlg;
    } else { // command line arg with given filename
        resultText = examFile;
        userAct = TstartExamDlg::e_continue;
    }
    m_glStore.tune = gl->Gtune();
    m_glStore.fretsNumber = gl->GfretsNumber;
    if (userAct == TstartExamDlg::e_newLevel) {
        m_exam = new Texam(&m_level, resultText); // resultText is userName
        gl->E->studentName = resultText; // store user name 
        m_exam->setTune(gl->Gtune());
        mW->examResults->startExam();
    } else
      if (userAct == TstartExamDlg::e_continue) {
        m_exam = new Texam(&m_level, "");
        Texam::EerrorType err = m_exam->loadFromFile(resultText);
        if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
          if (err == Texam::e_file_corrupted)
            QMessageBox::warning(mW, "", 
              tr("<b>Exam file seems to be corrupted</b><br>Better start new exam on the same level"));
      //We check are guitar's params suitable for an exam --------------
                  QString changesMessage = "";
                  if (m_exam->tune() != gl->Gtune() ) { //Is tune the same ?
                        gl->setTune(m_exam->tune());
                        changesMessage = tr("Tune of the guitar was changed in this exam !!.<br>Now it is:<br><b>%1</b>").arg(gl->Gtune().name);
                    }
                  if (m_level.hiFret > gl->GfretsNumber) { //Are enought frets ?
                        changesMessage += tr("<br><br>This exam requires more frets,<br>so frets number in the guitar will be changed.");
                        gl->GfretsNumber =  m_level.hiFret;
                    }
                  if (changesMessage != "")
                        QMessageBox::warning(mW, "", changesMessage);
            // ---------- End of checking ----------------------------------
          showExamSummary();
          mW->examResults->startExam(m_exam->totalTime(), m_exam->count(), m_exam->averageReactonTime(),
                          m_exam->mistakes());
        } else {
            if (err == Texam::e_file_not_valid)
                QMessageBox::critical(mW, "", tr("File: %1 \n is not valid exam file !!!")
                                  .arg(resultText));
            mW->clearAfterExam();
            if (m_exam) delete m_exam;
            return;
        }
    } else {
        mW->clearAfterExam();
        if (m_exam) delete m_exam;
        return;
    }

    //We are checking is sound needed in exam and is it available
    if (m_level.questionAs.isSound()) {
        if (!mW->sound->isPlayable()) {
            QMessageBox::warning(mW, "",
                     tr("An exam requires sound but<br>sound output is not available !!!"));
            mW->clearAfterExam();
            if (m_exam) delete m_exam;
            return;
        }
    }
    if ((m_level.questionAs.isNote() && m_level.answersAs[TQAtype::e_asNote].isSound()) ||
     (m_level.questionAs.isName() && m_level.answersAs[TQAtype::e_asName].isSound()) ||
     (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isSound()) ||
     (m_level.questionAs.isSound() && m_level.answersAs[TQAtype::e_asSound].isSound())
    )
    {
      if (!mW->sound->isSniffable()) {
            QMessageBox::warning(mW, "",
                     tr("An exam requires sound input but<br>it is not available !!!"));
            mW->clearAfterExam();
            if (m_exam) delete m_exam;
            return;
      }
    }
    
   // ---------- End of checking ----------------------------------

    m_messageItem = 0;
    m_supp = new TexecutorSupply(&m_level, this);
    prepareToExam();
    if (m_exam->fileName() == "" && gl->E->showHelpOnStart)
      showExamHelp();
    m_supp->createQuestionsList(m_questList);
    
//     qDebug() << "questions number:" << m_questList.size();

    m_shouldBeTerminated = false;
    m_incorrectRepeated = false;
    m_isAnswered = true;
    
    m_level.questionAs.randNext(); // Randomize question and answer type
    if (m_level.questionAs.isNote()) m_level.answersAs[TQAtype::e_asNote].randNext();
    if (m_level.questionAs.isName()) m_level.answersAs[TQAtype::e_asName].randNext();
    if (m_level.questionAs.isFret()) m_level.answersAs[TQAtype::e_asFretPos].randNext();
    if (m_level.questionAs.isSound()) m_level.answersAs[TQAtype::e_asSound].randNext();
    
    nextQuestAct = new QAction(tr("next question\n(space %1)").arg(TexamHelp::orRightButtTxt()), this);
    nextQuestAct->setStatusTip(nextQuestAct->text());
    nextQuestAct->setIcon(QIcon(gl->path+"picts/nextQuest.png"));
    nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
    connect(nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
    mW->nootBar->addAction(nextQuestAct);

    prevQuestAct = new QAction(tr("repeat prevoius question (backspace)"), this);
    prevQuestAct->setStatusTip(prevQuestAct->text());
    prevQuestAct->setIcon(QIcon(gl->path+"picts/prevQuest.png"));
    prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));

    checkAct = new QAction(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()), this);
    checkAct->setStatusTip(checkAct->text());
    checkAct->setIcon(QIcon(gl->path+"picts/check.png"));
    checkAct->setShortcut(QKeySequence(Qt::Key_Return));
    connect(checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));

    if (m_level.questionAs.isSound()) {
        repeatSndAct = new QAction(tr("play sound again"), this);
        repeatSndAct->setStatusTip(repeatSndAct->text());
        repeatSndAct->setIcon(QIcon(gl->path+"picts/repeatSound.png"));
        connect(repeatSndAct, SIGNAL(triggered()), this, SLOT(repeatSound()));
    }

    if (m_questList.size() == 0) {
        QMessageBox::critical(mW, "", tr("Level <b>%1<b><br>has no sense till there are no any possible questions to ask.<br>It can be unadjusted to current tune.<br>Repair it in Level Creator and try again.").arg(m_level.name));
        restoreAfterExam();
        return;
    }
}

void TexamExecutor::askQuestion() {
    m_lockRightButt = false; // release mouse button events
    clearWidgets();
    mW->setStatusMessage("");
    if (!gl->E->autoNextQuest) {
        mW->startExamAct->setDisabled(true);
        clearMessage();//if auto message is cleaned after 1 sec.
    }
    m_isAnswered = false;
    m_incorrectRepeated = false;
    mW->setMessageBg(gl->EquestionColor);
    m_answRequire.octave = true;
    m_answRequire.accid = true;
    m_answRequire.key = false;

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
                curQ.key = TkeySignature((qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) +
                                         m_level.loKey.value());
                if (m_level.onlyCurrKey) { // if note is in current key only
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
            curQ.qa.note = m_supp->determineAccid(curQ.qa.note);
    }

//    qDebug() << QString::fromStdString(curQ.qa.note.getName()) << "Q" << (int)curQ.questionAs
//            << "A" << (int)curQ.answerAs << curQ.key.getMajorName()
//            << (int)curQ.qa.pos.str() << (int)curQ.qa.pos.fret();

  // ASKING QUESIONS
    QString questText = QString("<b>%1. </b>").arg(m_exam->count() + 1); //question number
    if (curQ.questionAs == TQAtype::e_asNote) {
        questText += tr("Given note show ");
        char strNr = 0;
        if ( (curQ.answerAs == TQAtype::e_asFretPos || curQ.answerAs == TQAtype::e_asSound) 
            && !m_level.onlyLowPos && m_level.showStrNr)
            strNr = curQ.qa.pos.str(); //show string nr or not
        if (m_level.useKeySign && curQ.answerAs != TQAtype::e_asNote)
            // when answer is also asNote we determine key in preparing answer part
            mW->score->askQuestion(curQ.qa.note, curQ.key, strNr);
        else mW->score->askQuestion(curQ.qa.note, strNr);
    }

    if (curQ.questionAs == TQAtype::e_asName) {
        if (curQ.answerAs == TQAtype::e_asFretPos && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note);
        questText += tr("Given note name show ");
    }

    if (curQ.questionAs == TQAtype::e_asFretPos) {
        mW->guitar->askQuestion(curQ.qa.pos);
        questText += tr("Given position show ");
        if (!m_level.forceAccids)
            m_answRequire.accid = false;
    }

    if (curQ.questionAs == TQAtype::e_asSound) {
        mW->sound->play(curQ.qa.note);
        questText += tr("Played sound show ");
        if (!m_level.forceAccids)
            m_answRequire.accid = false;
    }

// PREPARING ANSWERS
    if (curQ.answerAs == TQAtype::e_asNote) {
        questText += TquestionAsWdg::asNoteTxt();
//         questText += tr("in the score");
        if (m_level.useKeySign) {
            if (m_level.manualKey) { // user have to manually secect a key
                QString keyTxt;
                if (qrand() % 2) // randomize: ask for minor or major key ?
                    keyTxt = curQ.key.getMajorName();
                else {
                    keyTxt = curQ.key.getMinorName();
                    curQ.key.setMinor(true);
                }
                mW->score->prepareKeyToAnswer(// we randomize some key to cover this expected one
                   (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) +
                                                m_level.loKey.value(), keyTxt);
                questText += tr(" <b>in %1 key.</b>", "in key signature").arg(keyTxt);
                m_answRequire.key = true;
            } else {
                mW->score->setKeySignature(curQ.key);
                mW->score->setKeyViewBg(gl->EquestionColor);
            }
        }
        if (curQ.questionAs == TQAtype::e_asNote) {// note has to be another than question
            curQ.qa_2.note = m_supp->forceEnharmAccid(curQ.qa.note); // curQ.qa_2.note is expected note
            if (curQ.qa_2.note == curQ.qa.note) {
                qDebug() << "Blind question";
                //                    askQuestion();
            }
            questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa_2.note.acidental);
            mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa_2.note.acidental);
        }
        if (curQ.questionAs == TQAtype::e_asFretPos || curQ.questionAs == TQAtype::e_asSound) {
            if (m_level.forceAccids)
                questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa.note.acidental);
            mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa.note.acidental);
        }
        mW->score->unLockScore();
        mW->score->setNoteViewBg(0, gl->EanswerColor);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        Tnote tmpNote = Tnote(0,0,0); // is used to show which accid has to be used (if any)
        questText += TquestionAsWdg::asNameTxt();
        if (curQ.questionAs == TQAtype::e_asName) {
            m_prevStyle = gl->NnameStyleInNoteName; // to keep user prefered style for other issues
            Tnote::EnameStyle tmpStyle = m_supp->randomNameStyle();
            curQ.qa_2.note = m_supp->forceEnharmAccid(curQ.qa.note); // force other name of note - expected note
            tmpNote = curQ.qa_2.note;
            questText = QString("<b>%1. </b>").arg(m_exam->count() + 1) +
                        tr("Give name of") + QString(" <span style=\"color: %1; font-size: %2px;\">").arg(
                                gl->EquestionColor.name()).arg(mW->getFontSize()*2) +
                        TnoteName::noteToRichText(curQ.qa.note) + "</span>. " +
                        getTextHowAccid((Tnote::Eacidentals)curQ.qa_2.note.acidental);
            mW->noteName->setNoteNamesOnButt(tmpStyle);
            gl->NnameStyleInNoteName = tmpStyle;
        }
        if (!m_level.requireOctave) m_answRequire.octave = false;
           /** During an exam Note name style is changed in two cases:
            * 1. If level.requireStyle = true every question or answer with Note Name
            *       switch it (letters/solfege)
            * 2. If Note Name is question and answer this is only way that it has sense    
           */
        if (m_level.requireStyle && curQ.questionAs != TQAtype::e_asName) { // switch style if not switched before
            Tnote::EnameStyle tmpStyle = m_supp->randomNameStyle();
            mW->noteName->setNoteNamesOnButt(tmpStyle);
            gl->NnameStyleInNoteName = tmpStyle;
        }

        if (curQ.questionAs == TQAtype::e_asFretPos) {
            if (m_level.forceAccids) {
                questText += getTextHowAccid((Tnote::Eacidentals)curQ.qa.note.acidental);
                tmpNote = Tnote(1, 0, curQ.qa.note.acidental); // to show which accid on TnoteName
            }
        }
        mW->noteName->prepAnswer(tmpNote);
    }

    if (curQ.answerAs == TQAtype::e_asFretPos) {
        questText += TquestionAsWdg::asFretPosTxt();
        if ( (curQ.questionAs == TQAtype::e_asName && m_level.showStrNr) ||
                curQ.questionAs == TQAtype::e_asSound)
            questText += "<b>" + tr(" on <span style=\"font-family: nootka; font-size:%1px;\">%2</span> string.").arg(qRound(mW->getFontSize()*1.5)).arg((int)curQ.qa.pos.str()) + "</b>";

        mW->guitar->setGuitarDisabled(false);
        mW->guitar->prepareAnswer();
    }
    
    if (curQ.answerAs == TQAtype::e_asSound) {
      questText = QString("<b>%1. </b>").arg(m_exam->count() + 1) +
      tr("Play or sing given note");
      m_answRequire.accid = false;
      m_answRequire.octave = m_level.requireOctave;
      mW->sound->prepareAnswer();
      if (gl->E->expertsAnswerEnable && gl->E->autoNextQuest) {
          if (curQ.questionAs == TQAtype::e_asSound)
              QTimer::singleShot(1600, this, SLOT(startSniffing())); // playing duration
          else
              QTimer::singleShot(WAIT_TIME, this, SLOT(startSniffing()));
          // Give a student some time to prepare for next question in expert mode
          // It avoids capture previous played sound as current answer
      }
      else {
          if (curQ.questionAs == TQAtype::e_asSound)
              QTimer::singleShot(1600, this, SLOT(startSniffing())); // playing duration
          else
              startSniffing();
      }
    }
    m_exam->addQuestion(curQ);
    mW->setStatusMessage(questText);

    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAs == TQAtype::e_asSound)
        mW->nootBar->addAction(repeatSndAct);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
}


void TexamExecutor::checkAnswer(bool showResults) {
    TQAunit curQ = m_exam->curQ();
    curQ.time = mW->examResults->questionStop();
    mW->nootBar->removeAction(checkAct);
    if (curQ.questionAs == TQAtype::e_asSound)
        mW->nootBar->removeAction(repeatSndAct);
    if (!gl->E->autoNextQuest)
        mW->startExamAct->setDisabled(false);
    m_isAnswered = true;
// Let's check
    Tnote exN, retN; // example note & returned note
    // First we determine what have to be checked
    exN = curQ.qa.note;
    if (curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.manualKey) {
            if (mW->score->keySignature().value() != curQ.key.value())
                curQ.setMistake(TQAunit::e_wrongKey);
        }
        if (curQ.questionAs == TQAtype::e_asNote)
            exN = curQ.qa_2.note;
        retN = mW->score->getNote(0);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        if (curQ.questionAs == TQAtype::e_asName)
            exN = curQ.qa_2.note;
        m_prevNoteIfName = mW->noteName->getNoteName();
        retN = mW->noteName->getNoteName();
    }
    if (curQ.answerAs == TQAtype::e_asSound) {
      retN = mW->sound->note();
    }
    if (curQ.answerAs == TQAtype::e_asFretPos) {
        if (curQ.qa.pos != mW->guitar->getfingerPos())
            curQ.setMistake(TQAunit::e_wrongPos);
    } else { // we check are the notes the same
//       qDebug() << QString::fromStdString(retN.getName()) << QString::fromStdString(exN.getName());
      if (retN.note) {
        if (exN != retN) {
            if (m_answRequire.octave) {
                Tnote nE = exN.showAsNatural();
                Tnote nR = retN.showAsNatural();
                qDebug() << QString::fromStdString(nE.getName()) << QString::fromStdString(nR.getName());
                if (nE.note == nR.note && nE.acidental == nR.acidental) {
                    if (nE.octave != nR.octave)
                        curQ.setMistake(TQAunit::e_wrongOctave);
                } else
                    curQ.setMistake(TQAunit::e_wrongNote);
            }
            if (!curQ.wrongNote()) { // There is stil something to check
//                 exN.octave = 1;
//                 retN.octave = 1;//octaves are checed so we are reseting them
                if (exN != retN) {// if they are equal it means that only octaves were wrong
                    if (m_answRequire.accid) {
                        exN = exN.showAsNatural();
                        retN = retN.showAsNatural();
//                         if(exN.showAsNatural() == retN.showAsNatural())
                        if (exN.note == retN.note && exN.acidental == retN.acidental)
                            curQ.setMistake(TQAunit::e_wrongAccid);
                        else
                            curQ.setMistake(TQAunit::e_wrongNote);
                    } else
//                         if (exN.note != retN.note || exN.acidental != retN.acidental)
                        if (exN.showAsNatural() != retN.showAsNatural())
                            curQ.setMistake(TQAunit::e_wrongNote);
                }
            }
        }
      } else
          curQ.setMistake(TQAunit::e_wrongNote);
    }

    if (showResults) {
        int mesgTime = 0, fc = 1;
      if (gl->E->autoNextQuest)
          mesgTime = 1500; // show temporary message
      if (!gl->hintsEnabled || gl->E->autoNextQuest)
          fc = 2; // font size factor to have enought room for text over guitar
      QString answTxt;
      if (curQ.correct()) { // CORRECT
          answTxt = QString("<span style=\"color: %1; font-size:%2px; %3\">").arg(gl->EanswerColor.name()).arg(mW->getFontSize()*fc).arg(gl->getBGcolorText(gl->EanswerColor));
          answTxt += tr("Exelent !!");
      } else { // WRONG
          answTxt = QString("<span style=\"color: %1; font-size:%2px; %3\">").arg(gl->EquestionColor.name()).arg(mW->getFontSize()*fc).arg(gl->getBGcolorText(gl->EquestionColor));
          if (curQ.wrongNote())
              answTxt += tr("Wrong note.");
          if (curQ.wrongKey())
              answTxt += tr(" Wrong key signature.");
          if (curQ.wrongAccid())
              answTxt += tr(" Wrong accidental.");
          if (curQ.wrongPos())
              answTxt += tr(" Wrong position.");
          if (curQ.wrongOctave())
              answTxt += tr(" Wrong octave.");
          if (gl->E->autoNextQuest && gl->E->repeatIncorrect && !m_incorrectRepeated)
              answTxt += tr("<br>Try again !");
      }
      answTxt += "</span><br>";
      if (gl->hintsEnabled && !gl->E->autoNextQuest) {
          answTxt += getNextQuestionTxt();
          if (!curQ.correct())
              answTxt += "<br>" + tr("To correct an answer") + " " + 
                  TexamHelp::clickSomeButtonTxt(gl->path+"picts/prev-icon.png") + 
                  " " + TexamHelp::orPressBkSTxt();
//          answTxt += "</span>";
      }
//       showMessage(answTxt, curQ.qa.pos, mesgTime);
	  TfingerPos pp = mW->guitar->getfingerPos();
	  showMessage(answTxt, pp, mesgTime);
    }
    if (!gl->E->autoNextQuest) {
        if (!curQ.correct())
            mW->nootBar->addAction(prevQuestAct);
        mW->nootBar->addAction(nextQuestAct);
    }
    disableWidgets();
    mW->examResults->setAnswer(curQ.correct());
    m_exam->setAnswer(curQ);

    if (gl->E->autoNextQuest) {
        if (curQ.correct()) {
            if (m_shouldBeTerminated)
                stopExamSlot();
            else {
                m_lockRightButt = true; // to avoid nervous users click mouse during WAIT_TIME
                QTimer::singleShot(WAIT_TIME, this, SLOT(askQuestion()));
            }
        } else {
            if (m_shouldBeTerminated)
                stopExamSlot();
            else {
                if (gl->E->repeatIncorrect && !m_incorrectRepeated) { // repeat only once if any
                    m_lockRightButt = true;
                    QTimer::singleShot(WAIT_TIME, this, SLOT(repeatQuestion()));
                }
                else {
                    m_lockRightButt = true;
                    QTimer::singleShot(WAIT_TIME, this, SLOT(askQuestion()));
                }
            }
        }
    }
}


void TexamExecutor::repeatQuestion() {
    m_lockRightButt = false;
    m_incorrectRepeated = true;
    m_isAnswered = false;
		TQAunit curQ = m_exam->curQ();
    QString m = mW->statusMessage();
    m.replace(0, m.indexOf("</b>"), QString("<b>%1.").arg(m_exam->count()+1));
    mW->setStatusMessage(m);
    if (!gl->E->autoNextQuest)
        clearMessage();
    curQ.setMistake(TQAunit::e_correct);
    if (curQ.answerAs == TQAtype::e_asNote)
        mW->score->unLockScore();
    if (curQ.answerAs == TQAtype::e_asName) {
        mW->noteName->setNameDisabled(false);
        mW->noteName->setNoteName(m_prevNoteIfName); // restore previous answered name (and button state)
    }
    if (curQ.answerAs == TQAtype::e_asFretPos)
        mW->guitar->setGuitarDisabled(false);
    if (curQ.answerAs == TQAtype::e_asSound)
        mW->sound->go();

		m_exam->addQuestion(curQ);

    if (!gl->E->autoNextQuest)
        mW->startExamAct->setDisabled(true);
    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAs == TQAtype::e_asSound) {
        mW->nootBar->addAction(repeatSndAct);
        repeatSound();
    }
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
}


void TexamExecutor::prepareToExam() {
    mW->setWindowTitle(tr("EXAM!!") + " " + m_exam->userName() + " - " + m_level.name);
    mW->setStatusMessage(tr("exam started on level") + ":<br><b>" + m_level.name + "</b>");

    mW->settingsAct->setDisabled(true);
    mW->levelCreatorAct->setIcon(QIcon(gl->path+"picts/help.png"));
    mW->levelCreatorAct->setText(tr("help"));
    mW->levelCreatorAct->setStatusTip(mW->levelCreatorAct->text());
    mW->startExamAct->setIcon(QIcon(gl->path+"picts/stopExam.png"));
    mW->startExamAct->setText(tr("stop the exam"));
    mW->startExamAct->setStatusTip(mW->startExamAct->text());
    mW->autoRepeatChB->show();
    mW->autoRepeatChB->setChecked(gl->E->autoNextQuest);
    mW->expertAnswChB->show();
    mW->expertAnswChB->setChecked(gl->E->expertsAnswerEnable);
    if (gl->E->expertsAnswerEnable)
      connectForExpert();

    disableWidgets();
    
    qApp->installEventFilter(m_supp);
    connect(m_supp, SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));

//     mW->score->isExamExecuting(true);
    disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    disconnect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    disconnect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    disconnect(mW->sound, SIGNAL(detectedNote(Tnote)), mW, SLOT(soundWasPlayed(Tnote)));
    disconnect(mW->levelCreatorAct, SIGNAL(triggered()), mW, SLOT(openLevelCreator()));
    disconnect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    connect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
    connect(mW->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));
    connect(mW->autoRepeatChB, SIGNAL(clicked(bool)), this,
            SLOT(autoRepeatStateChanged(bool)));
    connect(mW->expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersStateChanged(bool)));

    m_prevStyle = gl->NnameStyleInNoteName;
    m_glStore.nameStyleInNoteName = gl->NnameStyleInNoteName;
    m_glStore.showEnharmNotes = gl->showEnharmNotes;
    m_glStore.showKeySignName = gl->SshowKeySignName;
    m_glStore.showOtherPos = gl->GshowOtherPos;
    m_glStore.useDblAccids = gl->doubleAccidentalsEnabled;
    m_glStore.useKeySign = gl->SkeySignatureEnabled;
    m_glStore.octaveInName = gl->NoctaveInNoteNameFormat;

    gl->showEnharmNotes = false;
    gl->SshowKeySignName = false;
    gl->GshowOtherPos = false;
    gl->doubleAccidentalsEnabled = m_level.withDblAcc;
    gl->SkeySignatureEnabled = m_level.useKeySign;
    gl->NoctaveInNoteNameFormat = true;

    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(m_level.withDblAcc);
    mW->guitar->acceptSettings();
    mW->score->isExamExecuting(true);
    mW->sound->prepareToExam();
  // clearing all views/widgets
    clearWidgets();
    mW->guitar->createRangeBox(m_level.loFret, m_level.hiFret);

    if(gl->hintsEnabled) {
        TfingerPos pos(1, 0);
        showMessage(getNextQuestionTxt(), pos, 5000);
    }
}

void TexamExecutor::restoreAfterExam() {
    mW->setWindowTitle(qApp->applicationName());
    mW->nootBar->removeAction(nextQuestAct);
    mW->examResults->clearResults();
    mW->score->isExamExecuting(false);

// It has to be restored before the exam summary is shown
    //     gl->NnameStyleInNoteName = m_glStore.nameStyleInNoteName;
    gl->showEnharmNotes = m_glStore.showEnharmNotes;
    gl->SshowKeySignName = m_glStore.showKeySignName;
    gl->GshowOtherPos = m_glStore.showOtherPos;
    gl->doubleAccidentalsEnabled  = m_glStore.useDblAccids;
    gl->SkeySignatureEnabled = m_glStore.useKeySign;
    gl->setTune(m_glStore.tune);
    gl->NoctaveInNoteNameFormat = m_glStore.octaveInName;
    gl->GfretsNumber = m_glStore.fretsNumber;

    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    mW->guitar->acceptSettings();

    mW->settingsAct->setDisabled(false);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setGuitarDisabled(false);
    mW->autoRepeatChB->hide();
    if (gl->E->expertsAnswerEnable) // disconnect check box 
      expertAnswersStateChanged(false);
    mW->expertAnswChB->hide();
    
    qApp->removeEventFilter(m_supp);

    connect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    connect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    connect(mW->sound, SIGNAL(detectedNote(Tnote)), mW, SLOT(soundWasPlayed(Tnote)));
    disconnect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
    disconnect(mW->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));
    disconnect(mW->autoRepeatChB, SIGNAL(clicked(bool)), this,
            SLOT(autoRepeatStateChanged(bool)));
    connect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    connect(mW->levelCreatorAct, SIGNAL(triggered()), mW, SLOT(openLevelCreator()));
//     mW->score->isExamExecuting(false);
    delete m_supp;
    mW->score->unLockScore();
    mW->guitar->deleteRangeBox();
    mW->sound->restoreAfterExam();
    mW->clearAfterExam();
//     if (m_exam) delete m_exam;
    
}

void TexamExecutor::disableWidgets() {
    mW->noteName->setNameDisabled(true);
    mW->score->setScoreDisabled(true);
    mW->guitar->setGuitarDisabled(true);
    mW->sound->wait();
}

void TexamExecutor::clearWidgets() {
    mW->score->clearScore();
    mW->noteName->clearNoteName();
    mW->guitar->clearFingerBoard();
    mW->sound->restoreAfterAnswer();
}

void TexamExecutor::stopExamSlot() {
    if (!m_isAnswered) {
        m_shouldBeTerminated = true;
        mW->setStatusMessage(tr("Give an answer first!<br>Then the exam'll be finished"), 2000);
        return;
    }
    mW->examResults->stopExam();
    if (m_exam->fileName() == "" && m_exam->count())
        m_exam->setFileName(saveExamToFile());
    if (m_exam->fileName() != "") {
			m_exam->setTotalTime(mW->examResults->getTotalTime());
			m_exam->setAverageReactonTime(mW->examResults->getAverageTime());
      gl->NnameStyleInNoteName = m_glStore.nameStyleInNoteName; // restore to show in user defined style  
      if (!m_goingClosed) // if Nootka is closeing don't show summary 
          showExamSummary();
			if (m_exam->saveToFile() == Texam::e_file_OK) {
#if defined(Q_OS_WIN32)
				QSettings sett(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
				QSettings sett;
#endif
				QStringList recentExams = sett.value("recentExams").toStringList();
				recentExams.removeAll(m_exam->fileName());
				recentExams.prepend(m_exam->fileName());
				sett.setValue("recentExams", recentExams);
			}
    }

    mW->setMessageBg(-1);
    mW->setStatusMessage("");
    mW->setStatusMessage(tr("so a pity"), 5000);

    clearMessage();
    clearWidgets();
    restoreAfterExam();
}

QString TexamExecutor::getTextHowAccid(Tnote::Eacidentals accid) {
    QString S = QString("<br><span style=\"color: %1\">").arg(gl->GfingerColor.name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  "</span>";
    return S;
}

bool TexamExecutor::closeNootka() {
    QMessageBox *msg = new QMessageBox(mW);
    msg->setText(tr("Psssst... Exam is going.<br><br><b>Continue</b> it<br>or<br><b>Terminate</b> to check, save and exit<br>"));
    QAbstractButton *contBut = msg->addButton(tr("Continue"), QMessageBox::ApplyRole);
    msg->addButton(tr("Terminate"), QMessageBox::RejectRole);
    msg->exec();
    if (msg->clickedButton() == contBut) {
        return false;
    } else {
        m_goingClosed = true;
        if (m_isAnswered) {
            stopExamSlot();
            return true;
        } else {
            checkAnswer(false);
            m_isAnswered = true;
            stopExamSlot();
            return true;
        }
    }
}

QString TexamExecutor::saveExamToFile() {
    QString fileName = QFileDialog::getSaveFileName(mW, tr("Save exam's results as:"),
                         QDir::toNativeSeparators(QDir::homePath()+ "/" +
                         m_exam->userName() + "-" + m_level.name),
                         TstartExamDlg::examFilterTxt());
    if (fileName == "") {
		QMessageBox *msg = new QMessageBox(mW);
		msg->setText(tr("If You don't save to file<br>You lost all results !!"));
		QAbstractButton *saveButt = msg->addButton(tr("Save"), QMessageBox::ApplyRole);
		msg->addButton(tr("Discard"), QMessageBox::RejectRole);
		msg->exec();
        if (msg->clickedButton() == saveButt)
            fileName = saveExamToFile();
	}
    return fileName;
}

void TexamExecutor::repeatSound() {
	mW->sound->play(m_exam->curQ().qa.note);
  if (m_exam->curQ().answerAs == TQAtype::e_asSound)
    QTimer::singleShot(1600, this, SLOT(startSniffing()));
  // Tsound in exam doesn't call go() after playing.
  // When answer is asSound we do this. 1600ms is played sound duration
}

void TexamExecutor::showMessage(QString htmlText, TfingerPos &curPos, int time) {
    if (!m_messageItem) {
        m_messageItem = new QGraphicsTextItem();
        m_messageItem->hide();
        mW->guitar->scene()->addItem(m_messageItem);
        if (!gl->GisRightHanded)
            m_messageItem->scale(-1, 1);
    }
    QString txt = QString("<p align=\"center\" style=\"%1 color: #000;\">")
            .arg(gl->getBGcolorText(QColor(255, 255, 255, 220)))
            + htmlText + "</p>";
    m_messageItem->setZValue(30);
//    m_messageItem->setHtml(txt); // to make possible calculating text width
//    m_messageItem->document()->setTextWidth(m_messageItem->document()->size().width());// calc.
    m_messageItem->document()->setTextWidth(mW->guitar->width() / 3);
    m_messageItem->setHtml(txt); // now text is able to be centered
    bool onRightSide;
    if (curPos.fret() > 0 && curPos.fret() < 11) { // on which widget side
        onRightSide = gl->GisRightHanded;
    } else
        onRightSide = !gl->GisRightHanded;
    int xPos = 0;
    if (onRightSide)
        xPos = mW->guitar->width() / 2;
    xPos += (mW->guitar->width() / 2 - m_messageItem->document()->size().width()) / 2;
    m_messageItem->setPos(xPos,
       (mW->guitar->height() - m_messageItem->document()->size().height()) / 2 );
    m_messageItem->show();
    if (time)
        QTimer::singleShot(time, this, SLOT(clearMessage()));
}

void TexamExecutor::clearMessage() {
    if (m_messageItem) {
        if (m_messageItem->isVisible()) {
            m_messageItem->hide();
            m_messageItem->setHtml("");
        }
    }
}

void TexamExecutor::autoRepeatStateChanged(bool enable) {
    gl->E->autoNextQuest = enable;
    if (enable) {
        mW->startExamAct->setDisabled(false);
    }
}

QString TexamExecutor::getNextQuestionTxt() {
    return TexamHelp::toGetQuestTxt() + ":<br>" + TexamHelp::clickSomeButtonTxt(gl->path+"picts/next-icon.png") + ",<br>" +
                TexamHelp::pressSpaceKey() + "<br>" + TexamHelp::orRightButtTxt();
}

void TexamExecutor::showExamSummary() {
  TexamSummary *ES = new TexamSummary(m_exam, mW);
  ES->exec(); 
  delete ES;
}

void TexamExecutor::showExamHelp() {
      TexamHelp *hlp = new TexamHelp(gl->getBGcolorText(gl->EquestionColor), gl->getBGcolorText(gl->EanswerColor), 
        gl->path, gl->E->showHelpOnStart, mW);
      hlp->exec();
      delete hlp;
}

void TexamExecutor::connectForExpert() {
  connect(mW->score, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
  connect(mW->noteName, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
  connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
  connect(mW->sound, SIGNAL(detectedNote(Tnote)), this, SLOT(expertAnswersSlot()));
}

void TexamExecutor::expertAnswersStateChanged(bool enable) {
  if (enable) {
      if (!gl->E->askAboutExpert || showExpertAnswersHelpDlg(gl->E->askAboutExpert, mW))
          connectForExpert();
      else
          mW->expertAnswChB->setChecked(false);
  } else {
    disconnect(mW->score, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
    disconnect(mW->noteName, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
    disconnect(mW->guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
    disconnect(mW->sound, SIGNAL(detectedNote(Tnote)), this, SLOT(expertAnswersSlot()));
  }
  gl->E->expertsAnswerEnable = mW->expertAnswChB->isChecked();
}

void TexamExecutor::startSniffing() {
    mW->sound->go();
}

void TexamExecutor::expertAnswersSlot() {
    /** expertAnswersSlot() is invoked also by TaudioIN/TpitchFinder.
     * Calling checkAnswer() from here invokes stoping and deleting TaudioIN.
     * It finishs with crash. To avoid this checkAnswer() has to be called
     * from outside - by timer event. */
  QTimer::singleShot(10, this, SLOT(checkAnswer()));
//  if (m_exam->curQ().questionAs == TQAtype::e_asSound)
//      mW->sound->stopPlaying();
}

void TexamExecutor::rightButtonSlot() {
  if (m_lockRightButt)
      return;
    if (m_isAnswered)
        askQuestion();
    else
        checkAnswer();
}
