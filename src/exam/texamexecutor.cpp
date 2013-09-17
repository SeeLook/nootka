/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include "tlevelselector.h"
#include "tsound.h"
#include <taudioparams.h>
#include "mainwindow.h"
#include "texam.h"
#include "texamsummary.h"
#include "texamsettings.h"
#include "texamhelp.h"
#include "texpertanswerhelp.h"
#include "texamparams.h"
#include "texecutorsupply.h"
#include "tanalysdialog.h"
#include "tgraphicstexttip.h"
#include <ttipchart.h>
#include "tcanvas.h"
#include "tprogresswidget.h"
#include "texamview.h"
#include "tmainscore.h"
#include "tfingerboard.h"
#include "tnotename.h"
#include <QtGui>
#include <QDebug>

#define WAIT_TIME (150) //[ms]
#define SOUND_DURATION (1500) //[ms]


void debugStyle(TQAunit &qa) {
    qDebug("styles debugging");
    qDebug() << "Q:" << qa.styleOfQuestion() << "A:" << qa.styleOfAnswer();
}


extern Tglobals *gl;


TexamExecutor::TexamExecutor(MainWindow *mainW, QString examFile, TexamLevel *lev) :
  m_exam(0),
  mW(mainW),
  m_lockRightButt(false),
  m_goingClosed(false),
  m_penalStep(65535),
  m_snifferLocked(false),
  m_canvas(0)
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
            TstartExamDlg *startDlg = new TstartExamDlg(gl->E->studentName, gl->path, gl->E, mW);
            userAct = startDlg->showDialog(resultText, m_level);
            delete startDlg;
        } else { // command line arg with given filename
            resultText = examFile;
            userAct = TstartExamDlg::e_continue;
        }
    }
    m_glStore.tune = *gl->Gtune();
    m_glStore.fretsNumber = gl->GfretsNumber;
    if (userAct == TstartExamDlg::e_newLevel) {
        m_exam = new Texam(&m_level, resultText); // resultText is userName
        gl->E->studentName = resultText; // store user name
        m_exam->setTune(*gl->Gtune());
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
								if (m_level.instrument != e_noInstrument) {
                  if (m_exam->tune() != *gl->Gtune() ) { //Is tune the same ?
												Ttune tmpTune = m_exam->tune();
                        gl->setTune(tmpTune);
                        changesMessage = tr("Tuning of the guitar was changed in this exam!<br>Now it is:<br><b>%1</b>").
												arg(gl->Gtune()->name);
                    }
                  if (m_level.hiFret > gl->GfretsNumber) { //Are enought frets ?
                        changesMessage += tr("<br><br>This exam requires more frets,<br>so frets number in the guitar will be changed.");
                        gl->GfretsNumber =  m_level.hiFret;
                    }
								}
                  if (changesMessage != "")
                        QMessageBox::warning(mW, "", changesMessage);
            // ---------- End of checking ----------------------------------
          if (!showExamSummary(true)) {
            mW->clearAfterExam();
            if (m_exam) delete m_exam;
            return;
          }
          mW->examResults->startExam(m_exam->totalTime(), m_exam->count(), m_exam->averageReactonTime(),
                          m_exam->mistakes(), m_exam->halfMistaken());

        } else {
            if (err == Texam::e_file_not_valid)
                QMessageBox::critical(mW, "", tr("File: %1 \n is not valid exam file!")
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
                     tr("An exam requires sound but<br>sound output is not available!"));
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
                     tr("An exam requires sound input but<br>it is not available!"));
            mW->clearAfterExam();
            if (m_exam) delete m_exam;
            return;
      }
    }
    
   // ---------- End of checking ----------------------------------

    m_supp = new TexecutorSupply(&m_level, this);
    m_supp->createQuestionsList(m_questList);
    prepareToExam();
    if (m_exam->fileName() == "" && gl->E->showHelpOnStart)
      showExamHelp();
    if (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isFret()) {
      if (!m_supp->isGuitarOnlyPossible()) {
          qDebug("Something stupid!\n Level has question and answer as position on guitar but any question is available.");
          mW->clearAfterExam();
          if (m_exam) delete m_exam;
            return;
      }
    }
    
//     qDebug() << "questions number:" << m_questList.size();
    m_shouldBeTerminated = false;
    m_incorrectRepeated = false;
    m_isAnswered = true;
    m_blackQuestNr = -1;
    m_penalCount = 0;
    if (m_exam->isFinished()) {
      m_supp->setFinished();
      qDebug() << "Exam was finished";
    } else {
        int remained = (m_supp->obligQuestions() + m_exam->penalty()) - m_exam->count();
        remained = qMax(0, remained);
        if (remained < m_exam->blackCount()) {
          m_exam->increasePenaltys(m_exam->blackCount() - remained);
          qDebug() << "penaltys number adjusted:" << m_exam->blackCount() - remained;
          mW->progress->activate(m_exam->count(), m_supp->obligQuestions(), m_exam->penalty(), m_exam->isFinished());
        }
        if (remained == 0 && m_exam->blackCount() == 0) {
          mW->progress->setFinished(true);
          m_supp->setFinished();
          m_exam->setFinished();
          qDebug() << "Finished exam was detected";
        }
        updatePenalStep();
    }
    m_prevQuestStyle = m_supp->randomNameStyle(gl->NnameStyleInNoteName);
    m_prevAnswStyle = m_supp->randomNameStyle(m_prevQuestStyle);
    
    m_level.questionAs.randNext(); // Randomize question and answer type
    if (m_level.questionAs.isNote()) m_level.answersAs[TQAtype::e_asNote].randNext();
    if (m_level.questionAs.isName()) m_level.answersAs[TQAtype::e_asName].randNext();
    if (m_level.questionAs.isFret()) m_level.answersAs[TQAtype::e_asFretPos].randNext();
    if (m_level.questionAs.isSound()) m_level.answersAs[TQAtype::e_asSound].randNext();
    
    nextQuestAct = new QAction(tr("Next", "like next question - tool button text! Please be short"), this);
    nextQuestAct->setStatusTip(tr("next question\n(space %1)").arg(TexamHelp::orRightButtTxt()));
		nextQuestAct->setToolTip(nextQuestAct->statusTip());
    nextQuestAct->setIcon(QIcon(gl->path+"picts/nextQuest.png"));
    nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
    connect(nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
    mW->nootBar->addAction(nextQuestAct);

    prevQuestAct = new QAction(tr("Repeat", "like repeat question - tool button text! Please be short"), this);
    prevQuestAct->setStatusTip(tr("repeat previous question (backspace)"));
		prevQuestAct->setToolTip(prevQuestAct->statusTip());
    prevQuestAct->setIcon(QIcon(gl->path+"picts/prevQuest.png"));
    prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));

    checkAct = new QAction(tr("Check", "like check answer - tool button text! Please be short"), this);
    checkAct->setStatusTip(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()));
		checkAct->setToolTip(checkAct->statusTip());
    checkAct->setIcon(QIcon(gl->path+"picts/check.png"));
    checkAct->setShortcut(QKeySequence(Qt::Key_Return));
    connect(checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));

    if (m_level.questionAs.isSound()) {
        repeatSndAct = new QAction(tr("Play", "tool button text! Please be short"), this);
        repeatSndAct->setStatusTip(tr("play sound again") + "<br>(" + TexamHelp::pressSpaceKey().replace("<b>", " ").replace("</b>", ")"));
				repeatSndAct->setToolTip(repeatSndAct->statusTip().replace("<br>", "\n"));
//         repeatSndAct->setText(repeatSndAct->text() + ",\n" + TexamHelp::pressSpaceKey().remove("<b>").remove("</b>"));
        repeatSndAct->setShortcut(QKeySequence(Qt::Key_Space));
        repeatSndAct->setIcon(QIcon(gl->path+"picts/repeatSound.png"));
        connect(repeatSndAct, SIGNAL(triggered()), this, SLOT(repeatSound()));
    }

    if (m_questList.size() == 0) {
        QMessageBox::critical(mW, "", tr("Level <b>%1<b><br>has no sense till there are no any possible questions to ask.<br>It can be unadjusted to current tune.<br>Repair it in Level Creator and try again.").arg(m_level.name));
        restoreAfterExam();
        return;
    }
    /*
       for (int i = 0; i < m_exam->blacList()->size(); i++)
          if (m_exam->blacList()->operator[](i).questionAs == m_exam->blacList()->operator[](i).answerAs)
            if (m_exam->blacList()->operator[](i).questionAs != TQAtype::e_asFretPos &&
              m_exam->blacList()->operator[](i).questionAs != TQAtype::e_asSound )
              qDebug() << m_exam->blacList()->operator[](i).qa.note.toText() << 
              m_exam->blacList()->operator[](i).qa_2.note.toText();*/
}

void TexamExecutor::askQuestion() {
    m_lockRightButt = false; // release mouse button events
//     m_canvas->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    clearWidgets();
    if (!gl->E->autoNextQuest) {
        mW->startExamAct->setDisabled(true);
        m_canvas->clearCanvas();
    }
    m_isAnswered = false;
    m_incorrectRepeated = false;
    m_answRequire.octave = m_level.requireOctave;
    m_answRequire.accid = m_level.forceAccids;
    m_answRequire.key = false;

    TQAunit curQ = TQAunit(); // current question
    
    m_penalCount++;
    if (m_exam->blackCount() && m_penalCount > m_penalStep) {
      qDebug("penalty");
      m_penalCount = 0;
      m_blackQuestNr = qrand() % m_exam->blacList()->size();
      curQ = m_exam->blacList()->operator[](m_blackQuestNr);
      curQ.time = 0;
      curQ.setMistake(TQAunit::e_correct);
    } else {
        m_blackQuestNr = -1; // reset
        curQ.qa = m_questList[qrand() % m_questList.size()];
        curQ.questionAs = m_level.questionAs.next();
        curQ.answerAs = m_level.answersAs[curQ.questionAs].next();
    }
    
    if (m_blackQuestNr == -1 && curQ.questionAs == TQAtype::e_asFretPos && curQ.answerAs == TQAtype::e_asFretPos) {
      curQ.qa  = m_questList[m_supp->getQAnrForGuitarOnly()];      
    }

    if (m_blackQuestNr == -1 && curQ.questionAs == TQAtype::e_asNote || curQ.answerAs == TQAtype::e_asNote) {
        if (m_level.useKeySign) {
            Tnote tmpNote = curQ.qa.note;
            if (m_level.isSingleKey) { //for single key
                curQ.key = m_level.loKey;
                if (m_level.onlyCurrKey) {
                    tmpNote = m_level.loKey.inKey(curQ.qa.note);
                    if (tmpNote == Tnote(0, 0, 0))
                      qDebug() << "No note from questions list in single key. It should never happened!" << tmpNote.toText();
                }
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
                            qDebug() << "Oops! It should never happened. I can not find key signature!";
                            break;
                        }
                    }
                }
            }
            curQ.qa.note = tmpNote;
        }
        if (!m_level.onlyCurrKey) { // if key dosen't determine accidentals, we do this
            curQ.qa.note = m_supp->determineAccid(curQ.qa.note);
        }
    }

//    qDebug() << curQ.qa.note.toText() << "Q" << (int)curQ.questionAs
//            << "A" << (int)curQ.answerAs << curQ.key.getName()
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
        if (m_blackQuestNr == -1) { // regular question
          if (curQ.answerAs == TQAtype::e_asName) { // prepare answer
            curQ.qa_2.note = m_supp->forceEnharmAccid(curQ.qa.note); // force other enharm name of note - expected note
            m_answRequire.accid = true;
            if (m_level.requireStyle || curQ.qa.note == curQ.qa_2.note) {
              // when user (level) wants different names or only way to have different answer and question is
              // to change the style (note are the same)
                curQ.setStyle(m_prevQuestStyle, m_supp->randomNameStyle(m_prevQuestStyle)); // randomize style
                m_prevAnswStyle= curQ.styleOfAnswer(); 
            } else { // enharmonic notes in the same style
                curQ.setStyle(gl->NnameStyleInNoteName, gl->NnameStyleInNoteName);
                m_prevAnswStyle = gl->NnameStyleInNoteName;
                m_prevQuestStyle = gl->NnameStyleInNoteName;
            }
          } else // note name only in question
              if (m_level.requireStyle) { // switch previous used style
                curQ.setStyle(m_supp->randomNameStyle(m_prevQuestStyle), gl->NnameStyleInNoteName);
                m_prevQuestStyle = m_supp->randomNameStyle(curQ.styleOfQuestion());
              } else {
                  curQ.setStyle(gl->NnameStyleInNoteName, curQ.styleOfAnswer());
                  m_prevQuestStyle = gl->NnameStyleInNoteName;
              }
        }
        // Show question on TnoteName widget
        if (curQ.answerAs == TQAtype::e_asFretPos && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion(), curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion());
        if (curQ.answerAs  == TQAtype::e_asSound)
            m_answRequire.accid = false; // reset checking accidentals determined by level
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
//         bool showExclamOnKEy = false;
        if (m_level.useKeySign) {
            if (m_level.manualKey) { // user have to manually secect a key
                if (m_blackQuestNr == -1) // if black question key mode is defined
                    curQ.key.setMinor(bool(qrand() % 2));
                mW->score->prepareKeyToAnswer(// we randomize some key to cover this expected one
                   (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value(), curQ.key.getName());
                m_answRequire.key = true;
//                 showExclamOnKEy = true;
            } else {
                mW->score->setKeySignature(curQ.key);
//                 mW->score->setKeyViewBg(gl->EquestionColor);
            }
        }
        if (curQ.questionAs == TQAtype::e_asNote) {// note has to be another than question
            if (m_blackQuestNr == -1)
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
//         mW->score->setNoteViewBg(0, gl->EanswerColor);
//         mW->score->showExclamationMarks(true, showExclamOnKEy);
    }

    if (curQ.answerAs == TQAtype::e_asName) {
           /** During an exam Note name style is changed in two cases:
            * 1. If level.requireStyle = true every question or answer with Note Name
            *       switch it (letters/solfege)
            * 2. If Note Name is question and answer and are the same - this is only way that it has sense    
           */
				Tnote answNote = Tnote(0, 0, 0);
        if (curQ.questionAs == TQAtype::e_asName) {
//             mW->noteName->prepAnswer(curQ.styleOfAnswer());
						answNote = curQ.qa_2.note;
				} else {
// 					if (curQ.questionAs != TQAtype::e_asNote)
						answNote = curQ.qa.note;
          if (m_level.requireStyle)
              m_prevAnswStyle = m_supp->randomNameStyle(m_prevAnswStyle);
          curQ.setStyle(curQ.styleOfQuestion(), m_prevAnswStyle);
//           mW->noteName->prepAnswer(curQ.styleOfAnswer());
        }
        mW->noteName->prepAnswer(curQ.styleOfAnswer());
        if (curQ.questionAs == TQAtype::e_asFretPos || curQ.questionAs == TQAtype::e_asSound) {
            if (m_level.forceAccids) {
								mW->noteName->forceAccidental(answNote.acidental);
						}
				} else if (curQ.questionAs == TQAtype::e_asName)
									mW->noteName->forceAccidental(answNote.acidental);
        mW->noteName->setStyle(curQ.styleOfAnswer());
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
            if (m_blackQuestNr == -1)
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
              connect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
              // sniffing after finished sound
          } else
              QTimer::singleShot( WAIT_TIME, this, SLOT(startSniffing()));
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
    m_canvas->questionTip(m_exam);
    
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
//     disconnect(mW->sound->audioPlayer, 0, this, 0);
    disconnect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
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
        m_prevNoteIfName = mW->noteName->getNoteName(); // store note to restore it if will be repeated
        retN = mW->noteName->getNoteName();
    }
    if (curQ.answerAs == TQAtype::e_asSound) {
      retN = mW->sound->note();
    }
    if (curQ.answerAs == TQAtype::e_asFretPos) { // Comparing positions
      TfingerPos answPos, questPos;
      answPos = mW->guitar->getfingerPos();
      if (curQ.questionAs == TQAtype::e_asFretPos) { 
        if (answPos == curQ.qa.pos) { // check has not user got answer the same as question position
          curQ.setMistake(TQAunit::e_wrongPos);
          qDebug("Cheater!");
        } else 
          questPos = curQ.qa_2.pos;
      } else
        questPos = curQ.qa.pos;
      if (questPos != answPos && curQ.isCorrect()) { // if no cheater give him a chance
        QList <TfingerPos> tmpPosList; // Maybe hi gave correct note but on incorrect string only
        m_supp->getTheSamePos(answPos, tmpPosList); // get other positions
        for (int i = 0; i < tmpPosList.size(); i++) {
              if (tmpPosList[i] == questPos) { // and compare it with expected
                curQ.setMistake(TQAunit::e_wrongString);
                break;
              }
          }
          if (!curQ.wrongString()) { 
            curQ.setMistake(TQAunit::e_wrongPos);
          }
      }
    } else { // we check are the notes the same
//        qDebug() << QString::fromStdString(retN.getName()) << QString::fromStdString(exN.getName());
      if (retN.note) {
        Tnote nE = exN.showAsNatural();
        Tnote nR = retN.showAsNatural();
//         qDebug() << "Q:" << exN.toText(curQ.styleOfQuestion()) << "A:" << retN.toText(curQ.styleOfAnswer());
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

    disableWidgets();
    if (showResults) {
        int mesgTime = 0;
      if (gl->E->autoNextQuest)
          mesgTime = 2000; // show temporary message
      m_canvas->resultTip(&curQ, mesgTime);
      if (gl->hintsEnabled && !gl->E->autoNextQuest) {
        m_canvas->whatNextTip(curQ.isCorrect());
      }
      if (!gl->E->autoNextQuest) {
          if (!curQ.isCorrect())
              mW->nootBar->addAction(prevQuestAct);
          mW->nootBar->addAction(nextQuestAct);
      }
    }
    mW->examResults->setAnswer(&curQ);
    if (m_blackQuestNr != -1) { // decrease black list
      if (m_exam->blacList()->operator[](m_blackQuestNr).time == 65502)
        m_exam->blacList()->operator[](m_blackQuestNr).time--; // remains one penalty
      else
        m_exam->blacList()->removeAt(m_blackQuestNr); // delete - penaltys cleared
    }
    m_exam->setAnswer(curQ);
    mW->progress->progress(m_exam->penalty());
    if (!curQ.isCorrect())
      updatePenalStep();

    mW->noteName->setStyle(gl->NnameStyleInNoteName);
    mW->noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);

    if (!m_supp->wasFinished() && m_exam->count() >= (m_supp->obligQuestions() + m_exam->penalty()) ) { // maybe enought 
      if (m_exam->blackCount()) {
        m_exam->increasePenaltys(m_exam->blackCount());
        qDebug() << "penalties increased. Can't finish this exam yet.";
      } else {
        m_snifferLocked = true;
        mW->progress->setFinished(true);
        qApp->removeEventFilter(m_supp); // stop grabbing right button and calling checkAnswer()
        m_supp->examFinished();
        qApp->installEventFilter(m_supp); // restore grabbing right mouse button
        m_exam->setFinished();
        m_snifferLocked = false;
      }
    }
    
    if (showResults && gl->E->autoNextQuest) {
      m_lockRightButt = true; // to avoid nervous users click mouse during WAIT_TIME
      markAnswer(curQ);
      if (m_shouldBeTerminated)
          stopExamSlot();
      else {
      if (curQ.isCorrect()) {
          QTimer::singleShot(WAIT_TIME, this, SLOT(askQuestion()));
      } else {
          if (gl->E->repeatIncorrect && !m_incorrectRepeated) // repeat only once if any
              QTimer::singleShot(WAIT_TIME, this, SLOT(repeatQuestion()));
          else
              QTimer::singleShot(WAIT_TIME, this, SLOT(askQuestion()));
        }
      }
    }
}


void TexamExecutor::markAnswer(TQAunit& curQ) {
  QColor markColor;
  if (curQ.isCorrect())
    markColor = gl->EanswerColor;
  else if (curQ.isNotSoBad())
    markColor = gl->EnotBadColor;
  else
    markColor = gl->EquestionColor.lighter();
  switch (curQ.answerAs) {
    case TQAtype::e_asNote:
      mW->score->markAnswered(QColor(markColor.name()));
      break;
    case TQAtype::e_asFretPos:
      mW->guitar->markAnswer(QColor(markColor.name()));
      break;
    case TQAtype::e_asName:
      mW->noteName->markNameLabel(markColor.name());      
      break;
  }
  switch (curQ.questionAs) {
    case TQAtype::e_asNote:
      mW->score->markQuestion(QColor(markColor.name()));
      break;
    case TQAtype::e_asFretPos:
      mW->guitar->markQuestion(QColor(markColor.name()));
      break;
    case TQAtype::e_asName:
      mW->noteName->markNameLabel(markColor.name());      
      break;
  }
  
}


void TexamExecutor::repeatQuestion() {
    m_canvas->tryAgainTip(3000);
    m_lockRightButt = false;
    m_incorrectRepeated = true;
    m_isAnswered = false;
		TQAunit curQ = m_exam->curQ();

    if (!gl->E->autoNextQuest) {
        m_canvas->clearCanvas();
    }
    curQ.setMistake(TQAunit::e_correct);
    
    if (curQ.answerAs == TQAtype::e_asNote)
        mW->score->unLockScore();
    if (curQ.questionAs == TQAtype::e_asName) { // refresh question on NoteName
      if (curQ.answerAs == TQAtype::e_asFretPos && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion(), curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion());
    } else
      mW->noteName->clearNoteName();
    if (curQ.answerAs == TQAtype::e_asName) {
			Tnote answNote = Tnote(0, 0, 0);
      mW->noteName->setNameDisabled(false);
      if (curQ.questionAs == TQAtype::e_asName)
// 						mW->noteName->prepAnswer(curQ.styleOfAnswer());
						answNote = curQ.qa_2.note;
			else if (curQ.questionAs != TQAtype::e_asNote)
								answNote = curQ.qa.note;
//           mW->noteName->prepAnswer(curQ.styleOfAnswer(), curQ.qa.note);
// 			else // do not highlight accid button when question is on the score - accid is marked on the score
// 					mW->noteName->prepAnswer(curQ.styleOfAnswer(), Tnote(0, 0, 0));
			mW->noteName->prepAnswer(curQ.styleOfAnswer());
      mW->noteName->setStyle(curQ.styleOfAnswer());
			if (curQ.questionAs == TQAtype::e_asFretPos || curQ.questionAs == TQAtype::e_asSound) {
            if (m_level.forceAccids) {
								mW->noteName->forceAccidental(answNote.acidental);
						}
				} else if (curQ.questionAs == TQAtype::e_asName)
									mW->noteName->forceAccidental(answNote.acidental);
//         tmpStyle = curQ.styleOfQuestion();
            
    }
    if (curQ.answerAs == TQAtype::e_asFretPos)
        mW->guitar->setGuitarDisabled(false);
    if (curQ.answerAs == TQAtype::e_asSound && curQ.questionAs != TQAtype::e_asSound)
        startSniffing();
        // *** When question is sound it is played again (repeatSound()) 
        // and than startSniffing is called

    m_exam->addQuestion(curQ);
    m_blackQuestNr = m_exam->blacList()->count() - 1;
        // Previus answer was wroong or not so bad and it was added at the end of blacList
        // When an answer will be correct the list will be decresed

    if (!gl->E->autoNextQuest)
        mW->startExamAct->setDisabled(true);
    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAs == TQAtype::e_asSound) { // *** Here :-)
        mW->nootBar->addAction(repeatSndAct);
        repeatSound();
    }
    m_canvas->questionTip(m_exam);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
}


void TexamExecutor::prepareToExam() {
    mW->setWindowTitle(tr("EXAM!") + " " + m_exam->userName() + " - " + m_level.name);
    mW->setStatusMessage(tr("exam started on level") + ":<br><b>" + m_level.name + "</b>");

//     mW->settingsAct->setDisabled(true);
		mW->settingsAct->setVisible(false);
		mW->aboutAct->setVisible(false);
    mW->analyseAct->setVisible(false);
    mW->levelCreatorAct->setIcon(QIcon(gl->path+"picts/help.png"));
    mW->levelCreatorAct->setText(tr("Help"));
    mW->levelCreatorAct->setStatusTip(mW->levelCreatorAct->text());
		mW->levelCreatorAct->setToolTip(mW->levelCreatorAct->statusTip());
    mW->startExamAct->setIcon(QIcon(gl->path+"picts/stopExam.png"));
    mW->startExamAct->setText(tr("Stop"));
    mW->startExamAct->setStatusTip(tr("stop the exam"));
		mW->startExamAct->setToolTip(mW->startExamAct->statusTip());
    mW->autoRepeatChB->show();
    mW->autoRepeatChB->setChecked(gl->E->autoNextQuest);
    mW->expertAnswChB->show();
    mW->expertAnswChB->setChecked(gl->E->expertsAnswerEnable);
    mW->progress->activate(m_exam->count(), m_supp->obligQuestions(), m_exam->penalty(), m_exam->isFinished());

    disableWidgets();
// connect all events to check an answer or display tip how to check
    connect(mW->score, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
    connect(mW->noteName, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
    connect(mW->sound, SIGNAL(detectedNote(Tnote)), this, SLOT(expertAnswersSlot()));
    
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

    m_glStore.nameStyleInNoteName = mW->noteName->style();
    m_glStore.showEnharmNotes = gl->showEnharmNotes;
    m_glStore.showKeySignName = gl->SshowKeySignName;
    m_glStore.showOtherPos = gl->GshowOtherPos;
    m_glStore.useDblAccids = gl->doubleAccidentalsEnabled;
    m_glStore.useKeySign = gl->SkeySignatureEnabled;
    m_glStore.octaveInName = gl->NoctaveInNoteNameFormat;
		m_glStore.clef = Tclef(gl->Sclef);
		m_glStore.instrument = gl->instrument;
		m_glStore.detectRange = (int)gl->A->range;

    gl->showEnharmNotes = false;
    gl->SshowKeySignName = false;
    gl->GshowOtherPos = false;
    gl->doubleAccidentalsEnabled = m_level.withDblAcc;
    gl->SkeySignatureEnabled = m_level.useKeySign;
    gl->NoctaveInNoteNameFormat = true;
		gl->Sclef = m_level.clef.type();
		gl->instrument = m_level.instrument;
		if (m_level.instrument == e_bassGuitar)
				gl->A->range = TaudioParams::e_low;
		else
				gl->A->range = TaudioParams::e_middle;

    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(m_level.withDblAcc);
    mW->guitar->acceptSettings();
    mW->score->isExamExecuting(true);
		mW->sound->acceptSettings();
		if (mW->sound->isSniffable())
			mW->sound->wait();
    mW->sound->prepareToExam(m_level.loNote, m_level.hiNote);
		TtipChart::defaultClef = m_level.clef;
  // clearing all views/widgets
    clearWidgets();
		if (m_level.instrument != e_noInstrument)
				mW->guitar->createRangeBox(m_level.loFret, m_level.hiFret);
    m_soundTimer = new QTimer(this);
    connect(m_soundTimer, SIGNAL(timeout()), this, SLOT(startSniffing()));

    m_snifferLocked = false;
    m_canvas = new Tcanvas(mW);
    m_canvas->show();
    m_canvas->setQApossibilities(m_supp->qaPossibilitys());
    connect(m_canvas, SIGNAL(buttonClicked(QString)), this, SLOT(tipButtonSlot(QString)));
    if(gl->hintsEnabled)
        m_canvas->startTip();
}


void TexamExecutor::restoreAfterExam() {
    mW->setWindowTitle(qApp->applicationName());
    mW->nootBar->removeAction(nextQuestAct);
    mW->examResults->clearResults();
    mW->score->isExamExecuting(false);

    gl->showEnharmNotes = m_glStore.showEnharmNotes;
    gl->SshowKeySignName = m_glStore.showKeySignName;
    gl->GshowOtherPos = m_glStore.showOtherPos;
    gl->doubleAccidentalsEnabled  = m_glStore.useDblAccids;
    gl->SkeySignatureEnabled = m_glStore.useKeySign;
    gl->setTune(m_glStore.tune);
    gl->NoctaveInNoteNameFormat = m_glStore.octaveInName;
    gl->GfretsNumber = m_glStore.fretsNumber;
		gl->Sclef = m_glStore.clef.type();
		gl->instrument = m_glStore.instrument;
		gl->A->range = (TaudioParams::Erange)m_glStore.detectRange;

		TtipChart::defaultClef = gl->Sclef;
    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    mW->guitar->acceptSettings();
    mW->noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
    mW->progress->terminate();
		mW->sound->acceptSettings();

//     mW->settingsAct->setDisabled(false);
		mW->settingsAct->setVisible(true);
		mW->aboutAct->setVisible(true);
    mW->analyseAct->setVisible(true);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setGuitarDisabled(false);
    mW->autoRepeatChB->hide();
//     if (gl->E->expertsAnswerEnable) // disconnect check box 
//       expertAnswersStateChanged(false);
      
    mW->expertAnswChB->hide();

    if (m_canvas)
        m_canvas->deleteLater();

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
//    if (!mW->sound->isSnifferPaused()) // stop sniffing only when is not paused
//        mW->sound->wait();
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
        mW->setStatusMessage(tr("Give an answer first!<br>Then the exam will end."), 2000);
        return;
    }
    mW->examResults->stopExam();
    if (m_soundTimer->isActive())
        m_soundTimer->stop();
    mW->sound->stopPlaying();
    mW->sound->wait();
    qApp->removeEventFilter(m_supp);
    if (m_exam->fileName() == "" && m_exam->count()) {
        m_exam->setFileName(saveExamToFile());
        if (m_exam->fileName() != "")
          gl->E->examsDir = QFileInfo(m_exam->fileName()).absoluteDir().absolutePath();
    }
    if (m_exam->fileName() != "") {
			m_exam->setTotalTime(mW->examResults->getTotalTime());
			m_exam->setAverageReactonTime(mW->examResults->getAverageTime());
      gl->NnameStyleInNoteName = m_glStore.nameStyleInNoteName; // restore to show in user defined style  
      if (m_exam->saveToFile() == Texam::e_file_OK) {
          QStringList recentExams = gl->config->value("recentExams").toStringList();
          recentExams.removeAll(m_exam->fileName());
          recentExams.prepend(m_exam->fileName());
          gl->config->setValue("recentExams", recentExams);
      }
      if (!m_goingClosed) // if Nootka is closeing don't show summary 
          showExamSummary(false);
    }

    mW->setMessageBg(-1);
    mW->setStatusMessage("");
    mW->setStatusMessage(tr("Such a pity."), 5000);

    m_canvas->clearCanvas();
    clearWidgets();
    restoreAfterExam();
}


bool TexamExecutor::closeNootka() {
    m_snifferLocked = true;
    qApp->removeEventFilter(m_supp);
    QMessageBox *msg = new QMessageBox(mW);
    msg->setText(tr("Psssst... Exam is going.<br><br><b>Continue</b> it<br>or<br><b>Terminate</b> to check, save and exit<br>"));
    QAbstractButton *contBut = msg->addButton(tr("Continue"), QMessageBox::ApplyRole);
    msg->addButton(tr("Terminate"), QMessageBox::RejectRole);
    msg->exec();
    bool result;
    if (msg->clickedButton() == contBut) {
        m_snifferLocked = false;
        qApp->installEventFilter(m_supp);
        result = false;
    } else {
        m_goingClosed = true;
        if (!m_isAnswered)
          checkAnswer(false);
        stopExamSlot();
        result = true;
    }
    delete msg;
    return result;
}

QString TexamExecutor::saveExamToFile() {
      //     
    QString fileName = QFileDialog::getSaveFileName(mW, tr("Save exam results as:"),
                         QDir::toNativeSeparators(gl->E->examsDir + "/" +
                                                  m_exam->userName() + "-" + m_level.name + ".noo"),
                         TstartExamDlg::examFilterTxt(), 0 , QFileDialog::DontUseNativeDialog);
    if (fileName == "") {
		QMessageBox *msg = new QMessageBox(mW);
		msg->setText(tr("If you don't save to file<br>you lost all results!"));
		QAbstractButton *saveButt = msg->addButton(tr("Save"), QMessageBox::ApplyRole);
		msg->addButton(tr("Discard"), QMessageBox::RejectRole);
		msg->exec();
        if (msg->clickedButton() == saveButt)
            fileName = saveExamToFile();
    }
    if (!fileName.isEmpty() && fileName.right(4) != ".noo")
        fileName += ".noo";
    return fileName;
}

void TexamExecutor::repeatSound() {
    mW->sound->play(m_exam->curQ().qa.note);
    if (m_soundTimer->isActive())
        m_soundTimer->stop();
    if (m_exam->curQ().answerAs == TQAtype::e_asSound) {
        connect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
  }
}


void TexamExecutor::autoRepeatStateChanged(bool enable) {
    gl->E->autoNextQuest = enable;
    if (enable) {
        mW->startExamAct->setDisabled(false);
        m_canvas->clearResultTip();
    }
}


bool TexamExecutor::showExamSummary(bool cont) {
  TexamSummary *ES = new TexamSummary(m_exam, gl->path, cont, mW);
  TexamSummary::Eactions respond = ES->exec();
  if (respond == TexamSummary::e_analyse) {
     TanalysDialog *AD = new TanalysDialog(m_exam, mW);
     AD->exec();
     delete AD;
  }
  delete ES;
  if (respond == TexamSummary::e_discard)
    return false;
  else
    return true;
}

void TexamExecutor::showExamHelp() {
  m_snifferLocked = true;
  qApp->removeEventFilter(m_supp);
  TexamHelp *hlp = new TexamHelp(gl->getBGcolorText(gl->EquestionColor), gl->getBGcolorText(gl->EanswerColor), gl->path, gl->E->showHelpOnStart, mW);
  hlp->exec();
  delete hlp;
  qApp->installEventFilter(m_supp);
  m_snifferLocked = false;
}


void TexamExecutor::expertAnswersStateChanged(bool enable) {
  if (enable) {
      if (!gl->E->askAboutExpert || showExpertAnswersHelpDlg(gl->E->askAboutExpert, mW))
      {}
      else
          mW->expertAnswChB->setChecked(false); // ignore it, user resigned
  }
  gl->E->expertsAnswerEnable = mW->expertAnswChB->isChecked();
}


void TexamExecutor::sniffAfterPlaying() {
//     disconnect(mW->sound->audioPlayer, 0, this, 0);
    disconnect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
    if (m_soundTimer->isActive())
      m_soundTimer->stop();
    m_soundTimer->start(100);
}

void TexamExecutor::startSniffing() {
    if (m_soundTimer->isActive())
      m_soundTimer->stop();
    if (mW->sound->isSnifferPaused()) {
//        qDebug("unPaused");
        mW->sound->unPauseSniffing();
    } else
        mW->sound->go();
}


void TexamExecutor::expertAnswersSlot() {
    if (!mW->expertAnswChB->isChecked()) { // no expert
			if (gl->hintsEnabled) // show hint how to confirm an answer
				m_canvas->confirmTip(1500);
      return;
    }
    if (m_snifferLocked) // ignore slot when some dialog window apears
        return;
//    if (mW->examResults->questionTime() <= 2) { // answer time less than 0.1 s (not human...)
//         qDebug("answer time too short!");
//        return;
//    }
    /** expertAnswersSlot() is invoked also by TaudioIN/TpitchFinder.
     * Calling checkAnswer() from here invokes stoping and deleting TaudioIN.
     * It finishs with crash. To avoid this checkAnswer() has to be called
     * from outside - by timer event. */
//    if (m_exam->curQ().answerAs == TQAtype::e_asSound)
//      m_canvas->noteTip(600);
    if (m_exam->curQ().answerAs == TQAtype::e_asSound) {
//        qDebug("paused");
        mW->sound->pauseSinffing();
    }
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


void TexamExecutor::updatePenalStep() {
    if (m_supp->wasFinished())
        return;
    if (m_exam->blacList()->isEmpty())
      m_penalStep = 65535;
    else
    if ((m_supp->obligQuestions() + m_exam->penalty() - m_exam->count()) > 0)
          m_penalStep = (m_supp->obligQuestions() + m_exam->penalty() - m_exam->count()) / m_exam->blackCount();
    else
          m_penalStep = 0; // only penaltys questions
//     qDebug() << "m_penalStep" << m_penalStep;
}


void TexamExecutor::tipButtonSlot(QString name) {
    if (name == "nextQuest")
        askQuestion();
    else if (name == "stopExam")
        stopExamSlot();
    else if (name == "prevQuest")
        repeatQuestion();
    else if (name == "checkAnswer")
        checkAnswer();
    else if (name == "examHelp")
        showExamHelp();
}


bool TexamExecutor::event(QEvent* event) {
    if (m_canvas)
      if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
        m_canvas->event(event);
    return QObject::event(event);
}



