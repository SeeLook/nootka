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
#include <tintonationview.h>
#include <tpitchview.h>
#include "mainwindow.h"
#include "texam.h"
#include "texamsummary.h"
#include "texamsettings.h"
#include "texamhelp.h"
#include "texpertanswerhelp.h"
#include "texamparams.h"
#include "texecutorsupply.h"
#include "tgraphicstexttip.h"
#include <ttipchart.h>
#include "tcanvas.h"
#include "tprogresswidget.h"
#include "texamview.h"
#include "tglobalexamstore.h"
#include "texercises.h"
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
  m_canvas(0),
  m_supp(0),
  m_exercise(0)
{
    QString resultText;
    TstartExamDlg::Eactions userAct;

    mW->sound->wait();
    if (lev) {
        m_level = *lev;
        if (gl->E->studentName == "") {
						resultText = TstartExamDlg::systemUserName();
        } else
            resultText = gl->E->studentName;
        userAct = TstartExamDlg::e_newExam;
    } else {
        if (examFile == "") { // start exam dialog
            TstartExamDlg *startDlg = new TstartExamDlg(gl->E->studentName, gl->path, gl->E, mW);
            userAct = startDlg->showDialog(resultText, m_level);
            delete startDlg;
        } else { // command line arg with given filename
            resultText = examFile;
            userAct = TstartExamDlg::e_contExam;
        }
    }
    m_glStore = new TglobalExamStore(gl);
    m_glStore->tune = *gl->Gtune();
    m_glStore->fretsNumber = gl->GfretsNumber;
    if (userAct == TstartExamDlg::e_newExam || userAct == TstartExamDlg::e_newExercise) {
        m_exam = new Texam(&m_level, resultText); // resultText is userName
        gl->E->studentName = resultText; // store user name
        m_exam->setTune(*gl->Gtune());
        mW->examResults->startExam();
				if (userAct == TstartExamDlg::e_newExercise) {
						m_exercise = new Texercises(m_exam);
						m_exam->setFileName(QDir::toNativeSeparators(QFileInfo(gl->config->fileName()).absolutePath() + "/exercise.noo"));
				}
    } else if (userAct == TstartExamDlg::e_contExam) {
        m_exam = new Texam(&m_level, "");
        Texam::EerrorType err = m_exam->loadFromFile(resultText);
        if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
          if (err == Texam::e_file_corrupted)
            QMessageBox::warning(mW, "", 
              tr("<b>Exam file seems to be corrupted</b><br>Better start new exam on the same level"));
				//We check are guitar's params suitable for an exam 
						TexecutorSupply::checkGuitarParamsChanged(mW, m_exam);
          if (!showExamSummary(true)) {
            mW->clearAfterExam();
            deleteExam();
            return;
          }
          mW->examResults->startExam(m_exam->totalTime(), m_exam->count(), m_exam->averageReactonTime(),
                          m_exam->mistakes(), m_exam->halfMistaken());

        } else {
            if (err == Texam::e_file_not_valid)
                QMessageBox::critical(mW, "", tr("File: %1 \n is not valid exam file!")
                                  .arg(resultText));
            mW->clearAfterExam();
            deleteExam();
            return;
        }
    } else if (userAct == TstartExamDlg::e_contExercise) {
				m_exam = new Texam(&m_level, ""); // We are sure: exercise file is OK - it was checked in TstartExamDlg before
				QString exerciseFile = QDir::toNativeSeparators(QFileInfo(gl->config->fileName()).absolutePath() + "/exercise.noo");
        Texam::EerrorType err = m_exam->loadFromFile(exerciseFile);
				TexecutorSupply::checkGuitarParamsChanged(mW, m_exam);
				mW->examResults->startExam(m_exam->totalTime(), m_exam->count(), m_exam->averageReactonTime(),
                          m_exam->mistakes(), m_exam->halfMistaken());
				m_exercise = new Texercises(m_exam);
    } else {
				if (userAct == TstartExamDlg::e_levelCreator) {
						mW->clearAfterExam(e_openCreator);
				}	else 	
						mW->clearAfterExam(e_failed);
        deleteExam();
        return;
    }

    //We are checking is sound needed in exam and is it available
    if (m_level.questionAs.isSound()) {
        if (!mW->sound->isPlayable()) {
            QMessageBox::warning(mW, "",
                     tr("Exercise or exam require sound but<br>sound output is not available!"));
            mW->clearAfterExam();
            deleteExam();
            return;
        }
    }
    if ((m_level.questionAs.isNote() && m_level.answersAs[TQAtype::e_asNote].isSound()) ||
     (m_level.questionAs.isName() && m_level.answersAs[TQAtype::e_asName].isSound()) ||
     (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isSound()) ||
     (m_level.questionAs.isSound() && m_level.answersAs[TQAtype::e_asSound].isSound()) )
    {
      if (!mW->sound->isSniffable()) {
            QMessageBox::warning(mW, "",
                     tr("An exercises or exam require sound input but<br>it is not available!"));
            mW->clearAfterExam();
            deleteExam();
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
          deleteExam();
					return;
      }
    }
    
//     qDebug() << "questions number:" << m_questList.size();
    m_shouldBeTerminated = false;
    m_incorrectRepeated = false;
    m_isAnswered = true;
    m_blackQuestNr = -1;
		if (m_exercise) { // Do not count penalties in exercising mode
				m_exam->setFinished(); // to avoid adding penalties in exercising
				m_supp->setFinished();
				if (gl->E->suggestExam)
					m_exercise->setSuggestionEnabled(m_supp->qaPossibilities());
		} else {
				m_penalCount = 0;
				if (m_exam->isFinished()) {
					m_supp->setFinished();
					qDebug() << "Exam was finished";
				} else {
						int remained = (m_supp->obligQuestions() + m_exam->penalty()) - m_exam->count();
						remained = qMax(0, remained);
						if (remained < m_exam->blackCount()) {
							m_exam->increasePenaltys(m_exam->blackCount() - remained);
							qDebug() << "penalties number adjusted:" << m_exam->blackCount() - remained;
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
		}
    m_prevQuestStyle = m_supp->randomNameStyle(gl->NnameStyleInNoteName);
    m_prevAnswStyle = m_supp->randomNameStyle(m_prevQuestStyle);
    
    m_level.questionAs.randNext(); // Randomize question and answer type
    if (m_level.questionAs.isNote()) m_level.answersAs[TQAtype::e_asNote].randNext();
    if (m_level.questionAs.isName()) m_level.answersAs[TQAtype::e_asName].randNext();
    if (m_level.questionAs.isFret()) m_level.answersAs[TQAtype::e_asFretPos].randNext();
    if (m_level.questionAs.isSound()) m_level.answersAs[TQAtype::e_asSound].randNext();
    
		createActions();
		
    if (m_questList.size() == 0) {
        QMessageBox::critical(mW, "", tr("Level <b>%1<b><br>makes no sense because there are no questions to ask.<br>It can be re-adjusted.<br>Repair it in Level Creator and try again.").arg(m_level.name));
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


TexamExecutor::~TexamExecutor() {
		if (m_supp)
				delete m_supp;
		delete m_glStore;
		deleteExam();
}



void TexamExecutor::askQuestion() {
		m_askingTimer->stop();
    m_lockRightButt = false; // release mouse button events
    clearWidgets();
    if (!gl->E->autoNextQuest) {
			if (!m_exercise)
					mW->startExamAct->setDisabled(true);
			m_canvas->clearCanvas();
    }
    if (m_exercise)
			mW->nootBar->removeAction(correctAct);
    m_isAnswered = false;
    m_incorrectRepeated = false;
    m_answRequire.octave = m_level.requireOctave;
    m_answRequire.accid = m_level.forceAccids;
    m_answRequire.key = false;

    TQAunit curQ = TQAunit(); // current question
    
		m_penalCount++;
		if (!m_exercise && m_exam->blackCount() && m_penalCount > m_penalStep) {
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
        if (m_level.useKeySign) {
            if (m_level.manualKey) { // user have to manually select a key
                if (m_blackQuestNr == -1) // if black question key mode is defined
                    curQ.key.setMinor(bool(qrand() % 2));
                mW->score->prepareKeyToAnswer(// we randomize some key to cover this expected one
                   (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value(), curQ.key.getName());
                m_answRequire.key = true;
            } else {
                mW->score->setKeySignature(curQ.key);
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
    }

    if (curQ.answerAs == TQAtype::e_asName) {
           /** During an exam Note name style is changed in two cases:
            * 1. If level.requireStyle = true every question or answer with Note Name
            *       switch it (letters/solfege)
            * 2. If Note Name is question and answer and are the same - this is only way that it has sense    
           */
				Tnote answNote = Tnote(0, 0, 0);
        if (curQ.questionAs == TQAtype::e_asName)
						answNote = curQ.qa_2.note;
				else {
					answNote = curQ.qa.note;
          if (m_level.requireStyle)
              m_prevAnswStyle = m_supp->randomNameStyle(m_prevAnswStyle);
          curQ.setStyle(curQ.styleOfQuestion(), m_prevAnswStyle);
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
    if (!gl->E->autoNextQuest || m_exercise)
        mW->startExamAct->setDisabled(false);
    m_isAnswered = true;
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
			if ((TintonationView::Eaccuracy)m_level.intonation != TintonationView::e_noCheck) {
					float diff = qAbs(mW->sound->pitch() - (float)qRound(mW->sound->pitch()));
					if (diff >= TintonationView::getThreshold(m_level.intonation))
							curQ.setMistake(TQAunit::e_wrongIntonation);
			}
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
					mesgTime = 2500;
      m_canvas->resultTip(&curQ, mesgTime);
			if ((!m_exercise || (m_exercise && curQ.isCorrect())) && gl->hintsEnabled && !gl->E->autoNextQuest)
						m_canvas->whatNextTip(curQ.isCorrect());
      if (!gl->E->autoNextQuest) {
          if (!curQ.isCorrect() && !m_exercise)
              mW->nootBar->addAction(prevQuestAct);
          mW->nootBar->addAction(nextQuestAct);
      }
    }
    mW->examResults->setAnswer(&curQ);
    if (!m_exercise && m_blackQuestNr != -1) { // decrease black list
      if (m_exam->blacList()->operator[](m_blackQuestNr).time == 65502)
        m_exam->blacList()->operator[](m_blackQuestNr).time--; // remains one penalty
      else
        m_exam->blacList()->removeAt(m_blackQuestNr); // delete - penalties cleared
    }
    m_exam->setAnswer(curQ);
		if (!m_exercise) {
				mW->progress->progress(m_exam->penalty());
				if (!curQ.isCorrect())
						updatePenalStep();
		}

    mW->noteName->setStyle(gl->NnameStyleInNoteName);
    mW->noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);

		markAnswer(curQ);
// 		m_canvas->finishTip();
    int waitTime = WAIT_TIME;
		if (m_exercise) {
// 			if (curQ.answerAs != TQAtype::e_asSound) // there is no correction after played answer 
			waitTime = gl->E->correctViewDuration; // user has to have time to see his mistake and correct answer
			m_exercise->checkAnswer();
			if (!curQ.isCorrect()) { // correcting wrong answer
					if (mW->correctChB->isChecked())
						correctAnswer();
					else {
						m_canvas->whatNextTip(true, true);
						mW->nootBar->addAction(correctAct);
						m_lockRightButt = false;
						return; // wait for user
					}
			}
		} else {
				if (!m_supp->wasFinished() && m_exam->count() >= (m_supp->obligQuestions() + m_exam->penalty()) ) { // maybe enough
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
		}
    if (showResults && (gl->E->autoNextQuest /*|| (m_exercise && curQ.answerAs == TQAtype::e_asSound)*/)) {
      m_lockRightButt = true; // to avoid nervous users click mouse during wait time
      if (m_shouldBeTerminated)
          stopExamSlot();
      else {
      if (curQ.isCorrect()) {
          m_askingTimer->start(WAIT_TIME);
      } else {
          if ((!m_exercise /*|| (m_exercise && curQ.answerAs == TQAtype::e_asSound)*/) 
						&& gl->E->repeatIncorrect && !m_incorrectRepeated) // repeat only once if any
              QTimer::singleShot(WAIT_TIME, this, SLOT(repeatQuestion()));
          else
              m_askingTimer->start(waitTime);
        }
      }
    }
}


void TexamExecutor::correctAnswer() {
	if (!mW->correctChB->isChecked())
			mW->nootBar->removeAction(correctAct);
	TQAunit curQ = m_exam->curQ();
	QColor markColor;
	if (curQ.isNotSoBad())
			markColor = gl->EnotBadColor;
	else
			markColor = gl->EquestionColor;
	if (curQ.answerAs == TQAtype::e_asNote) {
			Tnote goodNote = curQ.qa.note;
			if (curQ.questionAs == TQAtype::e_asNote)
				goodNote = curQ.qa_2.note;
			if (curQ.wrongAccid() || curQ.wrongOctave()) // it corrects wrong octave as well
					mW->score->correctAccidental(goodNote);
			else if (curQ.wrongNote()) {
					if (m_level.manualKey && curQ.key.value() != mW->score->keySignature().value())
						mW->score->correctKeySignature(curQ.key);
					mW->score->correctNote(goodNote, markColor);
			}
			if (curQ.wrongKey())
					mW->score->correctKeySignature(curQ.key);
	} else if (curQ.answerAs == TQAtype::e_asFretPos) {
			TfingerPos goodPos = curQ.qa.pos;
			if (curQ.questionAs == TQAtype::e_asFretPos)
					goodPos = curQ.qa_2.pos;
			mW->guitar->correctPosition(goodPos, markColor);
	} else if (curQ.answerAs == TQAtype::e_asName) {
			Tnote goodNote = curQ.qa.note;
			if (curQ.questionAs == TQAtype::e_asName)
					goodNote = curQ.qa_2.note;
			mW->noteName->correctName(goodNote, markColor, curQ.isWrong());
	} else { // answer as played sound
			if (curQ.questionAs == TQAtype::e_asNote)
				m_canvas->correctFromScore(gl->E->correctViewDuration, curQ.qa.pos);
	}
	m_lockRightButt = true; // to avoid nervous users click mouse during correctViewDuration
	if (!mW->correctChB->isChecked() && gl->E->autoNextQuest) {
			// !mW->correctChB->isChecked() means that correctAnswer() was called by clicking correctAct
			m_askingTimer->start(gl->E->correctViewDuration);
  }
  if (!gl->E->autoNextQuest)
			QTimer::singleShot(gl->E->correctViewDuration, this, SLOT(delayerTip()));
}


void TexamExecutor::markAnswer(TQAunit& curQ) {
  QColor markColor;
  if (curQ.isCorrect())
    markColor = gl->EanswerColor;
  else if (curQ.isNotSoBad())
    markColor = gl->EnotBadColor;
  else
    markColor = gl->EquestionColor;
  switch (curQ.answerAs) {
    case TQAtype::e_asNote:
      mW->score->markAnswered(QColor(markColor));
      break;
    case TQAtype::e_asFretPos:
      mW->guitar->markAnswer(QColor(markColor));
      break;
    case TQAtype::e_asName:
      mW->noteName->markNameLabel(markColor);      
      break;
		case TQAtype::e_asSound:
			mW->pitchView->markAnswer(QColor(markColor.name()));
			break;
  }
  switch (curQ.questionAs) {
    case TQAtype::e_asNote:
      mW->score->markQuestion(QColor(markColor));
      break;
    case TQAtype::e_asFretPos:
      mW->guitar->markQuestion(QColor(markColor));
      break;
    case TQAtype::e_asName:
      mW->noteName->markNameLabel(markColor);      
      break;
// 		case TQAtype::e_asSound:
// 			mW->pitchView->markAnswer(QColor(markColor.name()));
// 			break;
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
		setTitleAndTexts();
		mW->settingsAct->setVisible(false);
		mW->aboutAct->setVisible(false);
    mW->analyseAct->setVisible(false);
    mW->levelCreatorAct->setIcon(QIcon(gl->path + "picts/help.png"));
    mW->levelCreatorAct->setText(tr("Help"));
    mW->levelCreatorAct->setStatusTip(mW->levelCreatorAct->text());
		mW->levelCreatorAct->setToolTip(mW->levelCreatorAct->statusTip());
    mW->startExamAct->setIcon(QIcon(gl->path + "picts/stopExam.png"));
    mW->startExamAct->setText(tr("Stop"));
		mW->startExamAct->setToolTip(mW->startExamAct->statusTip());
    mW->autoRepeatChB->show();
    mW->autoRepeatChB->setChecked(gl->E->autoNextQuest);
    mW->expertAnswChB->show();
    mW->expertAnswChB->setChecked(gl->E->expertsAnswerEnable);
		if (m_exercise) {
				mW->correctChB->show();
				mW->correctChB->setChecked(gl->E->showCorrected);
		}	else
				mW->progress->activate(m_exam->count(), m_supp->obligQuestions(), m_exam->penalty(), m_exam->isFinished());
		if (gl->instrument != e_noInstrument && 
			((/*m_level.questionAs.isNote() && */!m_level.answersAs[TQAtype::e_asNote].isSound()) &&
			(/*m_level.questionAs.isName() && */!m_level.answersAs[TQAtype::e_asName].isSound()) &&
			(/*m_level.questionAs.isFret() && */!m_level.answersAs[TQAtype::e_asFretPos].isSound()) &&
			(/*m_level.questionAs.isSound() && */!m_level.answersAs[TQAtype::e_asSound].isSound())))
					mW->pitchView->hide();
    disableWidgets();
// connect all events to check an answer or display tip how to check
    connect(mW->score, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
    connect(mW->noteName, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
    connect(mW->sound, SIGNAL(detectedNote(Tnote)), this, SLOT(expertAnswersSlot()));
    
    qApp->installEventFilter(m_supp);
    connect(m_supp, SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));

    disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
    disconnect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    disconnect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    disconnect(mW->sound, SIGNAL(detectedNote(Tnote)), mW, SLOT(soundWasPlayed(Tnote)));
    disconnect(mW->levelCreatorAct, SIGNAL(triggered()), mW, SLOT(openLevelCreator()));
    disconnect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
		if (m_exercise) {
			connect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExerciseSlot()));
			connect(m_exercise, SIGNAL(messageDisplayed()), this, SLOT(stopSound()));
			connect(m_exercise, SIGNAL(messageClosed(bool)), this, SLOT(suggestDialogClosed(bool)));
		} else
			connect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
    connect(mW->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));
    connect(mW->autoRepeatChB, SIGNAL(clicked(bool)), this,
            SLOT(autoRepeatStateChanged(bool)));
    connect(mW->expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersStateChanged(bool)));

		m_glStore->storeSettings();
		m_glStore->prepareGlobalsToExam(m_level);

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
		mW->updsateSize();
    clearWidgets();
		if (m_level.instrument != e_noInstrument)
				mW->guitar->createRangeBox(m_level.loFret, m_level.hiFret);
    m_soundTimer = new QTimer(this);
    connect(m_soundTimer, SIGNAL(timeout()), this, SLOT(startSniffing()));
		m_askingTimer = new QTimer(this);
		connect(m_askingTimer, SIGNAL(timeout()), this, SLOT(askQuestion()));

		if (!m_exercise) {
			mW->progress->show();
			mW->examResults->show();
		}
    m_snifferLocked = false;
    m_canvas = new Tcanvas(mW);
    m_canvas->show();
    connect(m_canvas, SIGNAL(buttonClicked(QString)), this, SLOT(tipButtonSlot(QString)));
    if(gl->hintsEnabled)
        m_canvas->startTip();
}


void TexamExecutor::restoreAfterExam() {
    mW->setWindowTitle(qApp->applicationName());
    mW->nootBar->removeAction(nextQuestAct);
    mW->examResults->clearResults();
    mW->score->isExamExecuting(false);

		m_glStore->restoreSettings();
		if (m_exercise) {
			gl->E->showCorrected = mW->correctChB->isChecked();
			gl->E->suggestExam = m_exercise->suggestInFuture();
		}
		
		TtipChart::defaultClef = gl->Sclef;
    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
    mW->guitar->acceptSettings();
    mW->noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
    mW->progress->terminate();
		mW->sound->acceptSettings();

		mW->settingsAct->setVisible(true);
		mW->aboutAct->setVisible(true);
    mW->analyseAct->setVisible(true);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setGuitarDisabled(false);
    mW->autoRepeatChB->hide();
    mW->expertAnswChB->hide();
		mW->correctChB->hide();
		mW->pitchView->show();

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
    mW->score->unLockScore();
		// unfortunately, unLockScore locks clef again
		mW->score->setClefDisabled(false);
    mW->guitar->deleteRangeBox();
    mW->sound->restoreAfterExam();
    mW->clearAfterExam();
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


void TexamExecutor::createActions() {
		nextQuestAct = new QAction(tr("Next", "like a next question"), this);
    nextQuestAct->setStatusTip(tr("next question\n(space %1)").arg(TexamHelp::orRightButtTxt()));
		nextQuestAct->setToolTip(nextQuestAct->statusTip());
    nextQuestAct->setIcon(QIcon(gl->path + "picts/nextQuest.png"));
    nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
    connect(nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
    mW->nootBar->addAction(nextQuestAct);

    prevQuestAct = new QAction(tr("Repeat", "like a repeat question"), this);
    prevQuestAct->setStatusTip(tr("repeat previous question (backspace)"));
		prevQuestAct->setToolTip(prevQuestAct->statusTip());
    prevQuestAct->setIcon(QIcon(gl->path + "picts/prevQuest.png"));
    prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));

    checkAct = new QAction(tr("Check", "like a check answer"), this);
    checkAct->setStatusTip(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()));
		checkAct->setToolTip(checkAct->statusTip());
    checkAct->setIcon(QIcon(gl->path + "picts/check.png"));
    checkAct->setShortcut(QKeySequence(Qt::Key_Return));
    connect(checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));

    if (m_level.questionAs.isSound()) {
        repeatSndAct = new QAction(tr("Play"), this);
        repeatSndAct->setStatusTip(tr("play sound again") + "<br>(" + TexamHelp::pressSpaceKey().replace("<b>", " ").replace("</b>", ")"));
				repeatSndAct->setToolTip(repeatSndAct->statusTip().replace("<br>", "\n"));
        repeatSndAct->setShortcut(QKeySequence(Qt::Key_Space));
        repeatSndAct->setIcon(QIcon(gl->path + "picts/repeatSound.png"));
        connect(repeatSndAct, SIGNAL(triggered()), this, SLOT(repeatSound()));
    }
    if (m_exercise) {
			correctAct = new QAction(tr("Correct", "like a correct answer with mistake"), this);
			correctAct->setStatusTip(tr("correct answer\n(enter)"));
			correctAct->setToolTip(correctAct->statusTip());
			correctAct->setIcon(QIcon(gl->path + "picts/correct.png"));
			correctAct->setShortcut(QKeySequence(Qt::Key_Return));
			connect(correctAct, SIGNAL(triggered()), this, SLOT(correctAnswer()));
    }
}


void TexamExecutor::exerciseToExam() {
	m_isAnswered = true;
	qApp->installEventFilter(m_supp);
	m_exam->saveToFile();
	QString userName = m_exam->userName();
	delete m_exam;
	m_exam = new Texam(&m_level, userName);
	m_penalCount = 0;				
	updatePenalStep();
	delete m_exercise;
	m_exercise = 0;
	setTitleAndTexts();
	mW->correctChB->hide();
	mW->examResults->startExam();
	mW->progress->activate(m_exam->count(), m_supp->obligQuestions(), m_exam->penalty(), m_exam->isFinished());
	disconnect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExerciseSlot()));
	connect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
	mW->progress->show();
	mW->examResults->show();
	clearWidgets();
	m_canvas->clearCanvas();
	if(gl->hintsEnabled)
        m_canvas->startTip();
}



void TexamExecutor::stopExerciseSlot() {
		bool askAfter = m_askingTimer->isActive();
		m_askingTimer->stop(); // stop questioning, if any
		bool continuePractice = false;
		stopSound();
    if (m_exam->count()) {
			TQAunit lastQuestion = m_exam->curQ();
			if (!m_isAnswered) {
					mW->examResults->pause();
					m_exam->removeLastQuestion();
			}
			m_exam->setTotalTime(mW->examResults->getTotalTime());
			m_exam->setAverageReactonTime(mW->examResults->getAverageTime());
			Tnote::EnameStyle tmpStyle = gl->NnameStyleInNoteName;
      gl->NnameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show charts in user defined style  
      
      bool startExam = false;
      if (!m_goingClosed)
					continuePractice = showExamSummary(true, &startExam);
			gl->NnameStyleInNoteName = tmpStyle;
			if (startExam) {
					exerciseToExam();
					return;
			}
			if (!m_isAnswered && continuePractice) {
					m_exam->addQuestion(lastQuestion); // add previously deleted
					mW->examResults->go();
			}
    }
    if (continuePractice) {
			if (askAfter) // ask next question if questioning was stopped
				askQuestion();
			else // restore sniffing if necessary
				if (m_exam->curQ().answerAs == TQAtype::e_asSound)
					startSniffing();
			qApp->installEventFilter(m_supp);
			return;
		}
		if (m_exam->count())
				m_exam->saveToFile();
		gl->E->prevExerciseLevel = m_level.name; // TODO easy to remove - see in TstartExamDlg
		closeExecutor();
}


void TexamExecutor::stopExamSlot() {
    if (!m_isAnswered && !gl->E->closeWithoutConfirm) {
        m_shouldBeTerminated = true;
        QColor c = gl->GfingerColor;
        c.setAlpha(30);
        mW->setMessageBg(c);
        mW->setStatusMessage(tr("Give an answer first!<br>Then the exam will end."), 2000);
        return;
    }
    mW->examResults->stopExam();
    stopSound();
		if (m_exam->count()) {
			if (m_exam->fileName() != "") {
				if(!QFileInfo(m_exam->fileName()).isWritable()) {
					qDebug() << "Can't write to file. Another name is needed";
					m_exam->setFileName("");
				}
			}
			if (m_exam->fileName() == "") {
				if (gl->E->closeWithoutConfirm) {
					QString fName = QDir::toNativeSeparators(gl->E->examsDir + "/" + m_exam->userName() + "-" + m_level.name);
					if (QFileInfo(fName  + ".noo").exists())
						fName += "-" + QDateTime::currentDateTime().toString("dd MMM hh:mm:ss");
					m_exam->setFileName(fName + ".noo");
				} else {
					m_exam->setFileName(saveExamToFile());
					if (m_exam->fileName() != "")
						gl->E->examsDir = QFileInfo(m_exam->fileName()).absoluteDir().absolutePath();
				}
			}
			if (m_exam->fileName() != "") {
				m_exam->setTotalTime(mW->examResults->getTotalTime());
				m_exam->setAverageReactonTime(mW->examResults->getAverageTime());
				gl->NnameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show in user defined style  
				if (m_exam->saveToFile() == Texam::e_file_OK) {
						QStringList recentExams = gl->config->value("recentExams").toStringList();
						recentExams.removeAll(m_exam->fileName());
						recentExams.prepend(m_exam->fileName());
						gl->config->setValue("recentExams", recentExams);
				}
				if (!m_goingClosed) // if Nootka is closing don't show summary 
						showExamSummary(false);
			}
		}
    closeExecutor();
}


void TexamExecutor::closeExecutor() {
	mW->setMessageBg(-1);
	mW->setStatusMessage("");
	mW->setStatusMessage(tr("Such a pity."), 5000);

	m_canvas->clearCanvas();
	clearWidgets();
	restoreAfterExam();
}


void TexamExecutor::stopSound() {
	if (m_soundTimer->isActive())
			m_soundTimer->stop();
	mW->sound->stopPlaying();
	mW->sound->wait();
	qApp->removeEventFilter(m_supp);
}


void TexamExecutor::suggestDialogClosed(bool startExam) {
	if (startExam) {
			exerciseToExam();
	} else {
		qApp->installEventFilter(m_supp);
		if (m_exam->curQ().answerAs == TQAtype::e_asSound)
					startSniffing();
	}
}


bool TexamExecutor::closeNootka() {
	bool result;
	if (m_exercise) {
		m_goingClosed = true;
		stopExerciseSlot();
		result = true;
	} else {
    m_snifferLocked = true;
    qApp->removeEventFilter(m_supp);
    QMessageBox *msg = new QMessageBox(mW);
		msg->setText(tr("Psssst... Exam is going.<br><br><b>Continue</b> it<br>or<br><b>Terminate</b> to check, save and exit<br>"));
		QAbstractButton *contBut = msg->addButton(tr("Continue"), QMessageBox::ApplyRole);
		msg->addButton(tr("Terminate"), QMessageBox::RejectRole);
		if (!gl->E->closeWithoutConfirm)
				msg->exec();
    if (!gl->E->closeWithoutConfirm && msg->clickedButton() == contBut) {
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
	}
    return result;
}


QString TexamExecutor::saveExamToFile() {
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


bool TexamExecutor::showExamSummary(bool cont, bool* startExam) {
  TexamSummary *ES = new TexamSummary(m_exam, gl->path, cont, mW);
	if (m_exercise)
			ES->setForExercise();
  TexamSummary::Eactions respond = ES->exec();
	if (startExam) {
		if (respond == TexamSummary::e_startExam)
			*startExam = true;
		else
			*startExam = false;
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
  TexamHelp *hlp = new TexamHelp(gl->getBGcolorText(gl->EquestionColor), gl->getBGcolorText(gl->EanswerColor), 
																 &gl->E->showHelpOnStart, mW);
  hlp->exec();
  delete hlp;
  qApp->installEventFilter(m_supp);
  m_snifferLocked = false;
}


void TexamExecutor::expertAnswersStateChanged(bool enable) {
  if (enable) {
      if (!gl->E->askAboutExpert || showExpertAnswersHelpDlg(mW, &gl->E->askAboutExpert, true))
      {}
      else
          mW->expertAnswChB->setChecked(false); // ignore it, user resigned
  }
  gl->E->expertsAnswerEnable = mW->expertAnswChB->isChecked();
}


void TexamExecutor::sniffAfterPlaying() {
    disconnect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
    if (m_soundTimer->isActive())
      m_soundTimer->stop();
    m_soundTimer->start(100);
}


void TexamExecutor::startSniffing() {
    if (m_soundTimer->isActive())
      m_soundTimer->stop();
    if (mW->sound->isSnifferPaused()) {
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
    if (m_snifferLocked) // ignore slot when some dialog window appears
        return;

    /** expertAnswersSlot() is invoked also by TaudioIN/TpitchFinder.
     * Calling checkAnswer() from here invokes stoping and deleting TaudioIN.
     * It finishes with crash. To avoid this checkAnswer() has to be called
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
    else if (name == "stopExam") {
			if (m_exercise)
				stopExerciseSlot();
			else
        stopExamSlot();
		}
    else if (name == "prevQuest")
        repeatQuestion();
    else if (name == "checkAnswer")
        checkAnswer();
    else if (name == "examHelp")
        showExamHelp();
		else if (name == "correct")
        correctAnswer();
}


bool TexamExecutor::event(QEvent* event) {
    if (m_canvas)
      if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
        m_canvas->event(event);
    return QObject::event(event);
}


void TexamExecutor::deleteExam() {
		if (m_exam) {
			delete m_exam;
			m_exam = 0;
		}
}


void TexamExecutor::delayerTip() {
	m_lockRightButt = false;
	m_canvas->whatNextTip(true); 
}



void TexamExecutor::setTitleAndTexts() {
	if (m_exercise) {
			mW->setWindowTitle(tr("Exercises with Nootka"));
			mW->setStatusMessage(tr("You are exercising on level") + ":<br><b>" + m_level.name + "</b>");
			mW->startExamAct->setStatusTip(tr("finish exercising"));
		} else {
			mW->setWindowTitle(tr("EXAM!") + " " + m_exam->userName() + " - " + m_level.name);
			mW->setStatusMessage(tr("Exam started on level") + ":<br><b>" + m_level.name + "</b>");
			mW->startExamAct->setStatusTip(tr("stop the exam"));
	}
}





