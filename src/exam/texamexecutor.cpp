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
#include "taudioout.h"
#include "tanalysdialog.h"
#include "tnotepixmap.h"
#include "tgraphicstexttip.h"
#include "tdialogmessage.h"
#include <QtGui>
#include <QDebug>

#define WAIT_TIME (600) //[ms]
#define SOUND_DURATION (1500) //[ms]

extern Tglobals *gl;


TexamExecutor::TexamExecutor(MainWindow *mainW, QString examFile, TexamLevel *lev) :
  m_exam(0),
  mW(mainW),
  m_lockRightButt(false),
  m_goingClosed(false),
  m_questMessage(0)
{
    QString resultText;
    TstartExamDlg::Eactions userAct;

    mW->sound->wait();
    if (lev) {
        m_level = *lev;
        if (gl->E->studentName == "") {
	    resultText = TstartExamDlg::systemUserName();
        }
        else
            resultText = gl->E->studentName;
        userAct = TstartExamDlg::e_newLevel;
    } else {
        if (examFile == "") { // start exam dialog
            TstartExamDlg *startDlg = new TstartExamDlg(gl->E->studentName, gl->path, mW);
            userAct = startDlg->showDialog(resultText, m_level);
            delete startDlg;
        } else { // command line arg with given filename
            resultText = examFile;
            userAct = TstartExamDlg::e_continue;
        }
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
          showExamSummary(true);
          mW->examResults->startExam(m_exam->totalTime(), m_exam->count(), m_exam->averageReactonTime(),
                          m_exam->mistakes());
//           TfingerPos fp(1, 0);
//           showMessage(QString("START"), fp, 2000);
        } else {
            if (err == Texam::e_file_not_valid)
                QMessageBox::critical(mW, "", tr("File: %1 \n is not valid exam file !!!")
                                  .arg(resultText));
            mW->clearAfterExam();
            if (m_exam) delete m_exam;
            return;
        }
    } else {
	if (userAct == TstartExamDlg::e_levelCreator) {
	    mW->clearAfterExam(e_openCreator);
	}
	else 	
	    mW->clearAfterExam(e_failed);
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
    if (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isFret()) {
      if (!m_supp->isGuitarOnlyPossible()) {
          qDebug("Something stiupid !!!\n Level has question and answer as position on guitar but any question is available.");
          mW->clearAfterExam();
          if (m_exam) delete m_exam;
            return;
      }
    }
    
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
//     mW->setStatusMessage("");
    if (!gl->E->autoNextQuest) {
        mW->startExamAct->setDisabled(true);
        clearMessage();//if auto message is cleaned after 1 sec.
    }
    m_isAnswered = false;
    m_incorrectRepeated = false;
    m_answRequire.octave = m_level.requireOctave;
    m_answRequire.accid = m_level.forceAccids;
    m_answRequire.key = false;

    TQAunit curQ = TQAunit(); // current question
    curQ.qa = m_questList[qrand() % m_questList.size()];
    curQ.questionAs = m_level.questionAs.next();
    curQ.answerAs = m_level.answersAs[curQ.questionAs].next();
    if (curQ.questionAs == TQAtype::e_asFretPos && curQ.answerAs == TQAtype::e_asFretPos) {
      curQ.qa  = m_questList[m_supp->getQAnrForGuitarOnly()];      
    }

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
    if (curQ.questionAs == TQAtype::e_asNote) {
        char strNr = 0;
        if ( (curQ.answerAs == TQAtype::e_asFretPos || curQ.answerAs == TQAtype::e_asSound) 
            && !m_level.onlyLowPos && m_level.showStrNr)
            strNr = curQ.qa.pos.str(); //show string nr or not
        if (m_level.useKeySign && curQ.answerAs != TQAtype::e_asNote)
            // when answer is also asNote we determine key in preparing answer part
            mW->score->askQuestion(curQ.qa.note, curQ.key, strNr);
        else mW->score->askQuestion(curQ.qa.note, strNr);
        if (curQ.answerAs  == TQAtype::e_asName)
            m_answRequire.accid = true; // checking octave determined by level
        if (curQ.answerAs  == TQAtype::e_asSound)
            m_answRequire.accid = false; // checking octave determined by level
    }

    if (curQ.questionAs == TQAtype::e_asName) {
        if (curQ.answerAs == TQAtype::e_asFretPos && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note);
        if (curQ.answerAs  == TQAtype::e_asSound)
            m_answRequire.accid = false; // checking octave determined by level
    }

    if (curQ.questionAs == TQAtype::e_asFretPos) {
        mW->guitar->askQuestion(curQ.qa.pos);
        if (curQ.answerAs  == TQAtype::e_asNote)
            m_answRequire.octave = true; // checking accid determined by level
        if (curQ.answerAs  == TQAtype::e_asSound) {
            m_answRequire.accid = false;
            m_answRequire.octave = true;
        }
        
    }

    if (curQ.questionAs == TQAtype::e_asSound) {
        mW->sound->play(curQ.qa.note);
        if (curQ.answerAs  == TQAtype::e_asSound)
            m_answRequire.accid = false; // checking octave determined by level
    }

// PREPARING ANSWERS
    if (curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.useKeySign) {
            if (m_level.manualKey) { // user have to manually secect a key
//                 QString keyTxt;
//                 if (qrand() % 2) // randomize: ask for minor or major key ?
//                     keyTxt = curQ.key.getMajorName();
//                 else {
//                     keyTxt = curQ.key.getMinorName();
//                     curQ.key.setMinor(true);
//                 }
                curQ.key.setMinor(bool(qrand() % 2));
//                 mW->score->prepareKeyToAnswer(// we randomize some key to cover this expected one
//                    (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) +
//                                                 m_level.loKey.value(), keyTxt);
                mW->score->prepareKeyToAnswer(// we randomize some key to cover this expected one
                   (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value(),
                                              curQ.key.getName());
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
            mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa_2.note.acidental);
            m_answRequire.accid = true;
            m_answRequire.octave = true;
        }
        if (curQ.questionAs == TQAtype::e_asFretPos || curQ.questionAs == TQAtype::e_asSound) {
            if (m_level.forceAccids) {
                mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa.note.acidental);
            }
        }
        if (curQ.questionAs == TQAtype::e_asName) {
            m_answRequire.accid = true;
            m_answRequire.octave = true;
        }
        mW->score->unLockScore();
        mW->score->setNoteViewBg(0, gl->EanswerColor);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
        Tnote tmpNote = Tnote(0,0,0); // is used to show which accid has to be used (if any)
        if (curQ.questionAs == TQAtype::e_asName) {
            m_prevStyle = gl->NnameStyleInNoteName; // to keep user prefered style for other issues
            Tnote::EnameStyle tmpStyle = m_supp->randomNameStyle();
            curQ.qa_2.note = m_supp->forceEnharmAccid(curQ.qa.note); // force other name of note - expected note
            tmpNote = curQ.qa_2.note;
            mW->noteName->setNoteNamesOnButt(tmpStyle);
            gl->NnameStyleInNoteName = tmpStyle;
            m_answRequire.accid = true;
        }
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
        mW->noteName->prepAnswer(tmpNote);
    }

    if (curQ.answerAs == TQAtype::e_asFretPos) {
        mW->guitar->setGuitarDisabled(false);
        mW->guitar->prepareAnswer();
        m_answRequire.accid = false;  // Ignored in checking, positions are comparing
        if (curQ.questionAs == TQAtype::e_asFretPos) {
          QList<TfingerPos> posList;
          m_supp->getTheSamePos(curQ.qa.pos, posList);
          if (posList.isEmpty())
            qDebug() << "Blind question";
          else {
            curQ.qa_2.pos = posList[qrand() % posList.size()];
            mW->guitar->setHighlitedString(curQ.qa_2.pos.str());
          }
        } else 
          if (m_level.showStrNr)
            mW->guitar->setHighlitedString(curQ.qa.pos.str());
    }
    
    if (curQ.answerAs == TQAtype::e_asSound) {
      mW->sound->prepareAnswer();
          if (curQ.questionAs == TQAtype::e_asSound) {
              connect(mW->sound->player, SIGNAL(noteFinished()), this, SLOT(sniffAfterPlaying()));
              // sniffing after finished sound
          } else
              QTimer::singleShot(WAIT_TIME, this, SLOT(startSniffing()));
              // Give a student some time to prepare for next question in expert mode
              // It avoids capture previous played sound as current answer
    }
    
    m_exam->addQuestion(curQ);

    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAs == TQAtype::e_asSound)
        mW->nootBar->addAction(repeatSndAct);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
    m_questMessage = new TdialogMessage(m_exam, mW, m_prevStyle);
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
    disconnect(mW->sound->player, 0, this, 0);
    if (m_questMessage) {
      delete m_questMessage;
      m_questMessage = 0;
    }    
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
    if (curQ.answerAs == TQAtype::e_asFretPos) { // Comparing positions
      if (curQ.questionAs != TQAtype::e_asFretPos) {
        if (curQ.qa.pos != mW->guitar->getfingerPos()) {
          TfingerPos ansPos = mW->guitar->getfingerPos();
          QList <TfingerPos> posList;
          m_supp->getTheSamePos(ansPos, posList, false);
          for (int i = 0; i < posList.size(); i++) {
              if (posList[i] == ansPos) {
                curQ.setMistake(TQAunit::e_wrongString);
                break;
              }
              curQ.setMistake(TQAunit::e_wrongPos);
          }
        }
        
      } else {
        if (curQ.qa_2.pos != mW->guitar->getfingerPos())
            curQ.setMistake(TQAunit::e_wrongPos);
      }
    } else { // we check are the notes the same
//        qDebug() << QString::fromStdString(retN.getName()) << QString::fromStdString(exN.getName());
      if (retN.note) {
        Tnote nE = exN.showAsNatural();
        Tnote nR = retN.showAsNatural();
        if (exN != retN) {
            if (m_answRequire.octave) {
//                qDebug() << "1." << QString::fromStdString(nR.getName()) << QString::fromStdString(nE.getName());
                if (nE.note == nR.note && nE.acidental == nR.acidental) {
                    if (nE.octave != nR.octave) {
                        curQ.setMistake(TQAunit::e_wrongOctave);
                }
            } else {
                    curQ.setMistake(TQAunit::e_wrongNote);
                }
            }
            if (!curQ.wrongNote()) { // There is stil something to check
                if (exN.note != retN.note || exN.acidental != retN.acidental) {// if they are equal it means that only octaves were wrong
                    exN = exN.showAsNatural();
                    retN = retN.showAsNatural();
//                    qDebug() << QString::fromStdString(retN.getName()) << QString::fromStdString(exN.getName());
                    if (m_answRequire.accid) {
                        if (exN.note == retN.note && exN.acidental == retN.acidental) {
                            curQ.setMistake(TQAunit::e_wrongAccid);
                        }
                        else {
                            curQ.setMistake(TQAunit::e_wrongNote);
                        }
                    } else {
                         if (exN.note != retN.note || exN.acidental != retN.acidental) {
                            curQ.setMistake(TQAunit::e_wrongNote);
                         }
                    }
                }
            }
        }
      } else
          curQ.setMistake(TQAunit::e_wrongNote);
    }

    if (showResults) {
        int mesgTime = 0, fc = 1;
        TfingerPos pp = mW->guitar->getfingerPos();
      if (gl->E->autoNextQuest) {
          mesgTime = 1500; // show temporary message
          if (gl->GisRightHanded)
            pp = TfingerPos(1, 14); // show it on the left side of a fingerboard - onnnnnn the right is question dialog
          else
            pp = TfingerPos(1, 11); // left side foor lefthanded
      }
      if (!gl->hintsEnabled || gl->E->autoNextQuest)
          fc = 2; // font size factor to have enought room for text over guitar
      QString answTxt;
      QColor answColor;
      if (curQ.isCorrect()) {
          answTxt = wasAnswerOKtext(&curQ, gl->EanswerColor, mW->getFontSize()*fc);
          answColor = gl->EanswerColor;
      }
      else {
        if (curQ.wrongNote() || curQ.wrongPos()) {
            answTxt = wasAnswerOKtext(&curQ, gl->EquestionColor, mW->getFontSize()*fc);
            answColor = gl->EquestionColor;
        }
        else {
            answTxt = wasAnswerOKtext(&curQ, Qt::darkMagenta, mW->getFontSize()*fc);
            answColor = QColor(124, 0 ,124, 30);
        }
      }
      if (gl->hintsEnabled && !gl->E->autoNextQuest) {
          answTxt += getNextQuestionTxt();
          if (!curQ.isCorrect())
              answTxt += "<br>" + tr("To correct an answer") + " " + 
                  TexamHelp::clickSomeButtonTxt(gl->path+"picts/prev-icon.png") + 
                  " " + TexamHelp::orPressBkSTxt();
//          answTxt += "</span>";
      }
	  showMessage(answTxt, pp, mesgTime, answColor);
    }
    if (!gl->E->autoNextQuest) {
        if (!curQ.isCorrect())
            mW->nootBar->addAction(prevQuestAct);
        mW->nootBar->addAction(nextQuestAct);
    }
    disableWidgets();
    mW->examResults->setAnswer(curQ.isCorrect());
    m_exam->setAnswer(curQ);

    if (gl->E->autoNextQuest) {
        if (curQ.isCorrect()) {
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
//     QString m = mW->statusMessage();
//     m.replace(0, m.indexOf("</b>"), QString("<b>%1.").arg(m_exam->count()+1));
//     mW->setStatusMessage(m);
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
    if (curQ.answerAs == TQAtype::e_asSound && curQ.questionAs != TQAtype::e_asSound)
        startSniffing();
        // *** When question is sound it is playe again (repeatSound()) 
        // and than startSniffing is called

		m_exam->addQuestion(curQ);

    if (!gl->E->autoNextQuest)
        mW->startExamAct->setDisabled(true);
    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAs == TQAtype::e_asSound) { // *** Here :-)
        mW->nootBar->addAction(repeatSndAct);
        repeatSound();
    }
    m_questMessage = new TdialogMessage(m_exam, mW, m_prevStyle);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
}


void TexamExecutor::prepareToExam() {
    mW->setWindowTitle(tr("EXAM!!") + " " + m_exam->userName() + " - " + m_level.name);
    mW->setStatusMessage(tr("exam started on level") + ":<br><b>" + m_level.name + "</b>");

    mW->settingsAct->setDisabled(true);
    mW->analyseAct->setVisible(false);
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
//    if (gl->E->expertsAnswerEnable)
//      connectForExpert();
    disableWidgets();

    if (gl->E->expertsAnswerEnable)
      connectForExpert();
    
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
    m_soundTimer = new QTimer(this);
    connect(m_soundTimer, SIGNAL(timeout()), this, SLOT(startSniffing()));

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
    mW->noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);

    mW->settingsAct->setDisabled(false);
    mW->analyseAct->setVisible(true);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setGuitarDisabled(false);
    mW->autoRepeatChB->hide();
    if (gl->E->expertsAnswerEnable) // disconnect check box 
      expertAnswersStateChanged(false);
    mW->expertAnswChB->hide();
    
    qApp->removeEventFilter(m_supp);

    if (m_messageItem)
        delete m_messageItem;
    if (m_questMessage) {
      delete m_questMessage;
      m_questMessage = 0;
    }

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
        QColor c = gl->GfingerColor;
        c.setAlpha(30);
        mW->setMessageBg(c);
        mW->setStatusMessage(tr("Give an answer first!<br>Then the exam'll be finished"), 2000);
        return;
    }
    mW->examResults->stopExam();
    if (m_soundTimer->isActive())
        m_soundTimer->stop();
    mW->sound->stopPlaying();
    mW->sound->wait();
    if (m_exam->fileName() == "" && m_exam->count())
        m_exam->setFileName(saveExamToFile());
    if (m_exam->fileName() != "") {
			m_exam->setTotalTime(mW->examResults->getTotalTime());
			m_exam->setAverageReactonTime(mW->examResults->getAverageTime());
      gl->NnameStyleInNoteName = m_glStore.nameStyleInNoteName; // restore to show in user defined style  
      if (!m_goingClosed) // if Nootka is closeing don't show summary 
          showExamSummary(false);
			if (m_exam->saveToFile() == Texam::e_file_OK) {

          QStringList recentExams = gl->config->value("recentExams").toStringList();
          recentExams.removeAll(m_exam->fileName());
          recentExams.prepend(m_exam->fileName());
          gl->config->setValue("recentExams", recentExams);
			}
    }

    mW->setMessageBg(-1);
    mW->setStatusMessage("");
    mW->setStatusMessage(tr("so a pity"), 5000);

    clearMessage();
    clearWidgets();
    restoreAfterExam();
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
//    qDebug() << m_exam->userName() << m_level.name;
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
    if (m_soundTimer->isActive())
        m_soundTimer->stop();
    if (m_exam->curQ().answerAs == TQAtype::e_asSound) {
        connect(mW->sound->player, SIGNAL(noteFinished()), this, SLOT(sniffAfterPlaying()));
  }
}

void TexamExecutor::showMessage(QString htmlText, TfingerPos& curPos, int time, QColor bgColor) {
    if (!m_messageItem) {
        m_messageItem = new TgraphicsTextTip();
        m_messageItem->hide();
        mW->guitar->scene()->addItem(m_messageItem);
        m_messageItem->setZValue(100);
        if (!gl->GisRightHanded) {
            m_messageItem->scale(-1, 1);
        }
    }
    QString txt = QString("<p>") + htmlText + "</p>";

    m_messageItem->setBgColor(bgColor);
    m_messageItem->setHtml(txt);

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
                TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt();
}

void TexamExecutor::showExamSummary(bool cont) {
  TexamSummary *ES = new TexamSummary(m_exam, gl->path, cont, mW);
  if (ES->exec() == QDialog::Accepted) {
     TanalysDialog *AD = new TanalysDialog(m_exam, mW);
     AD->exec();
     delete AD;
  }  
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


void TexamExecutor::sniffAfterPlaying() {
//     mW->sound->stopPlaying();
    disconnect(mW->sound->player, 0, this, 0);
    if (m_soundTimer->isActive())
      m_soundTimer->stop();
    m_soundTimer->start(100);
}

void TexamExecutor::startSniffing() {
//    mW->sound->stopPlaying();
    if (m_soundTimer->isActive())
      m_soundTimer->stop();
    mW->sound->go();
}


void TexamExecutor::expertAnswersSlot() {
    /** expertAnswersSlot() is invoked also by TaudioIN/TpitchFinder.
     * Calling checkAnswer() from here invokes stoping and deleting TaudioIN.
     * It finishs with crash. To avoid this checkAnswer() has to be called
     * from outside - by timer event. */
  QTimer::singleShot(10, this, SLOT(checkAnswer()));
}

void TexamExecutor::rightButtonSlot() {
  if (m_lockRightButt)
      return;
    if (m_isAnswered)
        askQuestion();
    else
        checkAnswer();
}
