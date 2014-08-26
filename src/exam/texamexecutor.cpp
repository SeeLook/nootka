/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include "texecutorsupply.h"
#include "tstartexamdlg.h"
#include "texamsummary.h"
#include "tcanvas.h"
#include "tprogresswidget.h"
#include "texamview.h"
#include "tglobalexamstore.h"
#include "texercises.h"
#include "tequalrand.h"
#include "trandmelody.h"
#include "mainwindow.h"
#include "level/tlevelselector.h"
#include <tsound.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
// #include "texamsettings.h"
#include <help/texamhelp.h>
#include <help/texpertanswerhelp.h>
#include <graphics/tgraphicstexttip.h>
#include <score/tmainscore.h>
#include <guitar/tfingerboard.h>
#include <notename/tnotename.h>
#include <widgets/tintonationview.h>
#include <widgets/tpitchview.h>
#include <level/tfixleveldialog.h>
// #include <widgets/tanimedchbox.h>
#include <tglobals.h>
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
#include <music/tmelody.h>
#include <tmainview.h>
#include <QtWidgets>


#define WAIT_TIME (150) //[ms]
#define SOUND_DURATION (1500) //[ms]


void debugStyle(TQAunit &qa) {
    qDebug("styles debugging");
    qDebug() << "Q:" << qa.styleOfQuestion() << "A:" << qa.styleOfAnswer();
}


extern Tglobals *gl;


TexamExecutor::TexamExecutor(MainWindow *mainW, QString examFile, Tlevel *lev) :
  m_exam(0),
  mW(mainW),
  m_lockRightButt(false),
  m_goingClosed(false),
  m_penalStep(65535),
  m_snifferLocked(false),
  m_canvas(0),
  m_supp(0),
  m_exercise(0),
  m_blindCounter(0),
  m_rand(0)
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
		m_glStore->instrument = gl->instrument;
    if (userAct == TstartExamDlg::e_newExam || userAct == TstartExamDlg::e_runExercise) {
        m_exam = new Texam(&m_level, resultText); // resultText is userName
				if (!fixLevelInstrument(m_level, "", gl->instrumentToFix, mainW)) {
							mW->clearAfterExam(e_failed);
							deleteExam();
							return;
          }
        gl->E->studentName = resultText; // store user name
        m_exam->setTune(*gl->Gtune());
        mW->examResults->startExam();
				if (userAct == TstartExamDlg::e_runExercise) {
						m_exercise = new Texercises(m_exam);
						m_exam->setFileName(QDir::toNativeSeparators(QFileInfo(gl->config->fileName()).absolutePath() + "/exercise.noo"));	
				}
// 			//We check are guitar's params suitable for an exam 
// 				TexecutorSupply::checkGuitarParamsChanged(mW, m_exam);
    } else if (userAct == TstartExamDlg::e_contExam) {
        m_exam = new Texam(&m_level, "");
        Texam::EerrorType err = m_exam->loadFromFile(resultText);
        if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
          if (err == Texam::e_file_corrupted)
            QMessageBox::warning(mW, " ", 
              tr("<b>Exam file seems to be corrupted</b><br>Better start new exam on the same level"));
// 				//We check are guitar's params suitable for an exam 
// 					TexecutorSupply::checkGuitarParamsChanged(mW, m_exam);
          if (!fixLevelInstrument(m_level, m_exam->fileName(), gl->instrumentToFix, mainW) || !showExamSummary(true)) {
							mW->clearAfterExam(e_failed);
							deleteExam();
							return;
          }
          mW->examResults->startExam(m_exam->totalTime(), m_exam->count(), m_exam->averageReactonTime(),
                          m_exam->mistakes(), m_exam->halfMistaken());

        } else {
            if (err == Texam::e_file_not_valid)
                QMessageBox::critical(mW, " ", tr("File: %1 \n is not valid exam file!")
                                  .arg(resultText));
            mW->clearAfterExam(e_failed);
            deleteExam();
            return;
        }
    } else {
				if (userAct == TstartExamDlg::e_levelCreator) {
						mW->clearAfterExam(e_openCreator);
				}	else 	
						mW->clearAfterExam(e_failed);
        deleteExam();
        return;
    }

    //We check are guitar's params suitable for an exam 
		TexecutorSupply::checkGuitarParamsChanged(mW, m_exam);
    //We are checking is sound needed in exam and is it available
//     if (m_level.questionAs.isSound()) { // it is too much - just turn sound on...
//         if (!mW->sound->isPlayable()) {
//             QMessageBox::warning(mW, "",
//                      tr("Exercise or exam require sound but<br>sound output is not available!"));
//             mW->clearAfterExam(e_failed);
//             deleteExam();
//             return;
//         }
//     }
//     if (m_level.answerIsSound()) {
//       if (!mW->sound->isSniffable()) {
//             QMessageBox::warning(mW, " ",
//                      tr("An exercises or exam require sound input but<br>it is not available!"));
//             mW->clearAfterExam(e_failed);
//             deleteExam();
//             return;
//       }
//     }
    
   // ---------- End of checking ----------------------------------

    m_supp = new TexecutorSupply(&m_level, this);
    m_supp->createQuestionsList(m_questList);
		if (m_questList.isEmpty()) {
        QMessageBox::critical(mW, " ", tr("Level <b>%1</b><br>makes no sense because there are no questions to ask.<br>It can be re-adjusted.<br>Repair it in Level Creator and try again.").arg(m_level.name));
				delete m_supp;
				mW->clearAfterExam(e_failed);
				deleteExam();
        return;
    }
    prepareToExam();
    if (m_exam->fileName().isEmpty() && gl->E->showHelpOnStart)
				showExamHelp();
    if (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isFret()) {
      if (!m_supp->isGuitarOnlyPossible()) {
          qDebug("Something stupid!\n Level has question and answer as position on guitar but any question is available.");
          mW->clearAfterExam(e_failed);
          deleteExam();
					return;
      }
    }
    
//     qDebug() << "questions number:" << m_questList.size();
		initializeExecuting();
		createActions();
		
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


void TexamExecutor::initializeExecuting() {
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
        connect(m_canvas, SIGNAL(certificateMagicKeys()), this, SLOT(displayCertificate()));
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
		if (m_level.requireStyle) {
				m_prevQuestStyle = m_supp->randomNameStyle(gl->S->nameStyleInNoteName);
				m_prevAnswStyle = m_supp->randomNameStyle(m_prevQuestStyle);
		} else {
				m_prevQuestStyle = gl->S->nameStyleInNoteName;
				m_prevAnswStyle = gl->S->nameStyleInNoteName;
		}
    
    m_level.questionAs.randNext(); // Randomize question and answer type
    if (m_level.questionAs.isNote()) m_level.answersAs[TQAtype::e_asNote].randNext();
    if (m_level.questionAs.isName()) m_level.answersAs[TQAtype::e_asName].randNext();
    if (m_level.questionAs.isFret()) m_level.answersAs[TQAtype::e_asFretPos].randNext();
    if (m_level.questionAs.isSound()) m_level.answersAs[TQAtype::e_asSound].randNext();
		if (m_rand)
			m_rand->reset();
		else
			m_rand = new TequalRand(m_questList.size());
		m_rand->setTotalRandoms(m_supp->obligQuestions() - m_exam->count());
		qDebug() << "Questions nr: " << m_questList.size() << "Randoms:" << m_supp->obligQuestions() - m_exam->count();
}


void TexamExecutor::askQuestion() {
		m_askingTimer->stop();
    m_lockRightButt = false; // release mouse button events
    clearWidgets();
		if (m_blindCounter > 20) {
				QMessageBox::critical(mW, "Level error!", QString("Nootka attempted to create proper question-answer pair 20 times<br>Send this message and a level file to developers and we will try to fix it in further releases."));
				mW->clearAfterExam(e_failed);
				deleteExam();
				return;
		}
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
		
		mW->noteName->setStyle(gl->S->nameStyleInNoteName);
    mW->noteName->setNoteNamesOnButt(gl->S->nameStyleInNoteName);

		TQAunit Q; // current question
		m_exam->addQuestion(Q);
    TQAunit& curQ = m_exam->answList()->last();
    
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
// 				curQ.qa = m_questList[qrand() % m_questList.size()];
				curQ.qa = m_questList[m_rand->get()];
				curQ.questionAs = m_level.questionAs.next();
				curQ.answerAs = m_level.answersAs[curQ.questionAs].next();
		}
    
    if (m_blackQuestNr == -1 && curQ.questionAsFret() && curQ.answerAsFret()) {
      curQ.qa  = m_questList[m_supp->getQAnrForGuitarOnly()];      
    }

    if (m_blackQuestNr == -1 && (curQ.questionAsNote() || curQ.answerAsNote())) {
        if (m_level.useKeySign) {
            Tnote tmpNote = curQ.qa.note;
            if (m_level.isSingleKey) { //for single key
                curQ.key = m_level.loKey;
                if (m_level.onlyCurrKey) {
                    tmpNote = m_level.loKey.inKey(curQ.qa.note);
                    if (tmpNote.note == 0)
                      qDebug() << "No note from questions list in single key. It should never happened!" << tmpNote.toText();
                }
            } else { // for many key signatures
                curQ.key = TkeySignature((qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) +
                                         m_level.loKey.value());
                if (m_level.onlyCurrKey) { // if note is in current key only
                    int keyRangeWidth = m_level.hiKey.value() - m_level.loKey.value();
                    int patience = 0; // we are looking for suitable key
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
        if (!m_level.onlyCurrKey) { // if key doesn't determine accidentals, we do this
            curQ.qa.note = m_supp->determineAccid(curQ.qa.note);
        }
    }
    if (m_level.melodyLen > 1 && 
			((curQ.questionAsNote() && curQ.answerAsSound()) ||
			 (curQ.questionAsSound() && curQ.answerAsNote()) ||
			 (curQ.questionAsSound() && curQ.answerAsSound())) ) {
					curQ.addMelody(QString("Melody %1").arg(m_exam->count() + 1));
					curQ.melody()->setKey(curQ.key);
					getRandomMelody(m_questList, curQ.melody(), m_level.melodyLen, m_level.onlyCurrKey, m_level.endsOnTonic);
		}
		if (curQ.melody())
			mW->setSingleNoteMode(false);
		else
			mW->setSingleNoteMode(true);
//    qDebug() << curQ.qa.note.toText() << "Q" << (int)curQ.questionAs
//            << "A" << (int)curQ.answerAs << curQ.key.getName()
//            << (int)curQ.qa.pos.str() << (int)curQ.qa.pos.fret();

  // ASKING QUESIONS
    if (curQ.questionAsNote()) {
			if (curQ.melody()) {
					mW->score->askQuestion(curQ.melody());
					connect(mW->sound, SIGNAL(newNoteStarted(Tnote&)), this, SLOT(noteOfMelodySlot(Tnote&)));
					m_melodyNoteIndex = 1; // when first note will be played and detected the second one is marked
					mW->score->selectNote(0); // mark first note
					mW->sound->notes().clear();
					// all above is correct only when answer is a sound (no other cases are supported now)
					curQ.newAttempt();
			} else {
        char strNr = 0;
        if ( (curQ.answerAsFret() || curQ.answerAsSound()) 
            && !m_level.onlyLowPos && m_level.showStrNr)
								strNr = curQ.qa.pos.str(); // do show string number or not
        if (m_level.useKeySign && !curQ.answerAsNote())
            // when answer is also asNote we determine key in preparing answer part
            mW->score->askQuestion(curQ.qa.note, curQ.key, strNr);
        else 
						mW->score->askQuestion(curQ.qa.note, strNr);
        if (curQ.answerAsName())
            m_answRequire.accid = true;
        else if (curQ.answerAsSound())
            m_answRequire.accid = false;
			}
    }

    if (curQ.questionAsName()) {
        if (m_blackQuestNr == -1) { // regular question
          if (curQ.answerAsName()) { // prepare answer
            curQ.qa_2.note = m_supp->forceEnharmAccid(curQ.qa.note); // force other enharm name of note - expected note
            m_answRequire.accid = true;
            if (m_level.requireStyle || curQ.qa.note == curQ.qa_2.note) {
              // when user (level) wants different names or only way to have different answer and question is
              // to change the style (note are the same)
                curQ.setStyle(m_prevQuestStyle, m_supp->randomNameStyle(m_prevQuestStyle)); // randomize style
                m_prevAnswStyle= curQ.styleOfAnswer(); 
            } else { // enharmonic notes in the same style
                curQ.setStyle(gl->S->nameStyleInNoteName, gl->S->nameStyleInNoteName);
                m_prevAnswStyle = gl->S->nameStyleInNoteName;
                m_prevQuestStyle = gl->S->nameStyleInNoteName;
            }
          } else // note name only in question
              if (m_level.requireStyle) { // switch previous used style
                curQ.setStyle(m_supp->randomNameStyle(m_prevQuestStyle), gl->S->nameStyleInNoteName);
								m_prevQuestStyle = curQ.styleOfQuestion();
//                 m_prevQuestStyle = m_supp->randomNameStyle(curQ.styleOfQuestion());
              } else {
                  curQ.setStyle(gl->S->nameStyleInNoteName, curQ.styleOfAnswer());
                  m_prevQuestStyle = gl->S->nameStyleInNoteName;
              }
        }
        // Show question on TnoteName widget
        if ((curQ.answerAsFret() || curQ.answerAsSound()) && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion(), curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion());
        if (curQ.answerAsSound())
            m_answRequire.accid = false; // reset checking accidentals determined by level
    }

    if (curQ.questionAsFret()) {
        mW->guitar->askQuestion(curQ.qa.pos);
        if (curQ.answerAsNote())
            m_answRequire.octave = true; // checking accid determined by level
        if (curQ.answerAsSound()) {
            m_answRequire.accid = false;
            m_answRequire.octave = true;
        }
        
    }

    if (curQ.questionAsSound()) {
			if (curQ.melody()) {
					curQ.newAttempt();
					mW->sound->playMelody(curQ.melody());
			} else {
					mW->sound->play(curQ.qa.note);
					if (curQ.answerAsSound())
							m_answRequire.accid = false;
			}
    }

// PREPARING ANSWERS
    if (curQ.answerAsNote()) {
			if (curQ.melody()) {
// 				mW->score->setMelody(curQ.melody());
			} else {
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
        if (curQ.questionAsNote()) {// note has to be another than question
            if (m_blackQuestNr == -1)
                curQ.qa_2.note = m_supp->forceEnharmAccid(curQ.qa.note); // curQ.qa_2.note is expected note
            if (!m_level.manualKey && curQ.qa_2.note == curQ.qa.note) {
								blindQuestion();
								return; // refresh this function scope by calling it outside
						}
            mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa_2.note.acidental);
            m_answRequire.accid = true;
            m_answRequire.octave = true;
        }
        if (curQ.questionAsFret() || curQ.questionAsSound()) {
            if (m_level.forceAccids) {
                mW->score->forceAccidental((Tnote::Eacidentals)curQ.qa.note.acidental);
            }
        }
        if (curQ.questionAsName()) {
            m_answRequire.accid = true;
            m_answRequire.octave = true;
        }
			}
        mW->score->unLockScore();
    }

    if (curQ.answerAsName()) {
           /** During an exam Note name style is changed in two cases:
            * 1. If level.requireStyle = true every question or answer with Note Name
            *       switch it (letters/solfege)
            * 2. If Note Name is question and answer and are the same - this is only way that it has sense    
           */
				Tnote answNote = Tnote(0, 0, 0);
        if (curQ.questionAsName())
						answNote = curQ.qa_2.note;
				else {
					answNote = curQ.qa.note;
          if (m_level.requireStyle)
              m_prevAnswStyle = m_supp->randomNameStyle(m_prevAnswStyle);
          curQ.setStyle(curQ.styleOfQuestion(), m_prevAnswStyle);
        }
        mW->noteName->prepAnswer(curQ.styleOfAnswer());
        if (curQ.questionAsFret() || curQ.questionAsSound()) {
            if (m_level.forceAccids) {
								mW->noteName->forceAccidental(answNote.acidental);
						}
				} else if (curQ.questionAsName())
									mW->noteName->forceAccidental(answNote.acidental);
        mW->noteName->setStyle(curQ.styleOfAnswer());
    }

    if (curQ.answerAsFret()) {
//         mW->guitar->setGuitarDisabled(false);
//         mW->guitar->prepareAnswer();
        m_answRequire.accid = false;  // Ignored in checking, positions are comparing
        if (curQ.questionAsFret()) {
          QList<TfingerPos> posList;
          m_supp->getTheSamePosNoOrder(curQ.qa.pos, posList);
          if (posList.isEmpty()) {
							blindQuestion();
							return; // refresh this function scope by calling it outside
					} else {
							if (m_blackQuestNr == -1)
									curQ.qa_2.pos = posList[qrand() % posList.size()];
							mW->guitar->setHighlitedString(curQ.qa_2.pos.str());
          }
        } else 
          if (m_level.showStrNr)
            mW->guitar->setHighlitedString(curQ.qa.pos.str());
				mW->guitar->setGuitarDisabled(false);
        mW->guitar->prepareAnswer();
    }
    
    if (curQ.answerAsSound()) {
      mW->sound->prepareAnswer();
			if (curQ.melody())
				mW->sound->notes().clear();
			if (curQ.questionAsSound()) {
					connect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
					// sniffing after finished sound
			} else
					QTimer::singleShot(WAIT_TIME, this, SLOT(startSniffing()));
					// Give a student some time to prepare for next question in expert mode
					// It avoids capture previous played sound as current answer
    }

    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAsSound())
        mW->nootBar->addAction(repeatSndAct);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
    m_canvas->questionTip(m_exam);
    m_blindCounter = 0; // question successfully asked - reset the counter
}


void TexamExecutor::checkAnswer(bool showResults) {
	TQAunit& curQ = m_exam->answList()->last();
	curQ.time = mW->examResults->questionStop();
	mW->nootBar->removeAction(checkAct);
	if (curQ.questionAsSound())
			mW->nootBar->removeAction(repeatSndAct);
	if (curQ.answerAsSound()) {
			mW->sound->pauseSinffing();
			disconnect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
			disconnect(mW->sound, SIGNAL(newNoteStarted(Tnote&)), this, SLOT(noteOfMelodySlot(Tnote&)));
	}
	if (!gl->E->autoNextQuest || m_exercise)
			mW->startExamAct->setDisabled(false);
	m_isAnswered = true;
// Let's check
	Tnote questNote, answNote, userNote; // example note & returned note
// At first we determine what has to be checked
	if (!curQ.melody()) {
    questNote = curQ.qa.note;
    if (curQ.answerAsNote()) {
        if (m_level.manualKey) {
            if (mW->score->keySignature().value() != curQ.key.value())
                curQ.setMistake(TQAunit::e_wrongKey);
        }
        if (curQ.questionAsNote())
            questNote = curQ.qa_2.note;
        answNote = mW->score->getNote(0);
    }
    if (curQ.answerAsName()) {
        if (curQ.questionAsName())
            questNote = curQ.qa_2.note;
        m_prevNoteIfName = mW->noteName->getNoteName(); // store note to restore it if will be repeated
        answNote = mW->noteName->getNoteName();
    }
    userNote = answNote;
    if (curQ.answerAsSound()) {
				answNote = mW->sound->note();
				if ((TintonationView::Eaccuracy)m_level.intonation != TintonationView::e_noCheck) {
						if (TnoteStruct(Tnote(), mW->sound->pitch()).inTune(TintonationView::getThreshold(m_level.intonation)))
// 						float diff = qAbs(mW->sound->pitch() - (float)qRound(mW->sound->pitch()));
// 						if (diff >= TintonationView::getThreshold(m_level.intonation))
								curQ.setMistake(TQAunit::e_wrongIntonation);
				}
    }
	}
// Now we can check
	if (curQ.answerAsFret()) { // 1. Comparing positions
			TfingerPos answPos, questPos;
			answPos = mW->guitar->getfingerPos();
			if (curQ.questionAsFret()) { 
				if (answPos == curQ.qa.pos) { // check has not user got answer the same as question position
					curQ.setMistake(TQAunit::e_wrongPos);
					qDebug("Cheater!");
				} else 
					questPos = curQ.qa_2.pos;
			} else
				questPos = curQ.qa.pos;
			if (questPos != answPos && curQ.isCorrect()) { // if no cheater give him a chance
				QList <TfingerPos> tmpPosList; // Maybe hi gave correct note but on incorrect string only
				m_supp->getTheSamePosNoOrder(answPos, tmpPosList); // get other positions
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
	} else {
			if (curQ.melody()) { // 2. or checking melodies
					Tmelody answMelody;
					if (curQ.answerAsNote())
						mW->score->getMelody(&answMelody);
					else // if melody is not in score it was played for sure
						answMelody.fromNoteStruct(mW->sound->notes());
					m_supp->compareMelodies(curQ.melody(), &answMelody, curQ.lastAttepmt());
// 				TODO check intonation if required
// 				TODO it should be overloaded compareMelodies with list of TnoteStruct as an arg
					for (int i = 0; i < curQ.lastAttepmt()->mistakes.size(); ++i) { // setting mistake type in TQAunit
						if (curQ.answerAsSound()) // and put note times to attempt if played
							curQ.lastAttepmt()->times << mW->sound->notes()[i].duration * 1000;
							// it is clumsy - TODO see previous TODO
						if (curQ.lastAttepmt()->mistakes[i] == 0)
							continue; // it was correct - skip
						if (curQ.lastAttepmt()->mistakes[i] & TQAunit::e_wrongNote) {
							curQ.setMistake(TQAunit::e_wrongNote); // so far answer is not accepted - some note is wrong
// 							break; // don't check further
						} else // or collect all other "smaller" mistakes
							curQ.setMistake(curQ.mistake() | curQ.lastAttepmt()->mistakes[i]);
					}
					qDebug() << "\nsummary" << curQ.lastAttepmt()->mistakes.size() << curQ.lastAttepmt()->times.size()
						<< curQ.melody()->length() << answMelody.length();
					for (int i = 0; i < curQ.melody()->length(); ++i) {
						qDebug() << i << curQ.melody()->notes()[i].p().toText() << answMelody.notes()[i].p().toText() <<
						curQ.lastAttepmt()->mistakes[i] << curQ.lastAttepmt()->times[i] / 1000.0;
					}
			} else // 3. or checking are the notes the same
					m_supp->checkNotes(curQ, questNote, answNote, m_answRequire.octave, m_answRequire.accid);
	}
	if (curQ.melody()) {
// 		for (int i = 0; i < curQ.melody()->length(); ++i) {
// 			qDebug() << i << curQ.melody()->notes()[i].p().toText() << 
// 			curQ.lastAttepmt()->mistakes[i] << curQ.lastAttepmt()->times[i] / 1000.0;
// 		}
	} else {
		if (!m_answRequire.accid && curQ.isCorrect() && (curQ.answerAsNote() || curQ.answerAsName())) {
			// Save user given note when it is correct and accidental was not forced to respect kind of accidental
				if (curQ.questionAs == curQ.answerAs)
					curQ.qa_2.note = userNote;
				else
					curQ.qa.note = userNote;
		}
	}
    disableWidgets();
		bool autoNext = gl->E->autoNextQuest;
		if (gl->E->afterMistake == TexamParams::e_stop && !curQ.isCorrect())
				autoNext = false; // when mistake and e_stop - the same like autoNext = false;
    if (showResults) {
			int mesgTime = 0;
      if (autoNext) { // determine time of displaying
				if (curQ.isCorrect() || gl->E->afterMistake == TexamParams::e_continue)
						mesgTime = 2500; // hard-coded 
				else
						mesgTime = gl->E->mistakePreview; // user defined wait time
			}
      m_canvas->resultTip(&curQ, mesgTime);
			if ((!m_exercise || (m_exercise && curQ.isCorrect())) && gl->hintsEnabled && !autoNext)
						m_canvas->whatNextTip(curQ.isCorrect());
      if (!autoNext) {
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

//     mW->noteName->setStyle(gl->S->nameStyleInNoteName);
//     mW->noteName->setNoteNamesOnButt(gl->S->nameStyleInNoteName);

		markAnswer(curQ);
    int waitTime = gl->E->questionDelay;
		if (m_exercise) {
			if (gl->E->afterMistake != TexamParams::e_continue || gl->E->showCorrected)
				waitTime = gl->E->correctPreview; // user has to have time to see his mistake and correct answer
			m_exercise->checkAnswer();
			if (!curQ.isCorrect()) { // correcting wrong answer
					if (gl->E->showCorrected)
						correctAnswer();
					else {
						mW->nootBar->addAction(correctAct);
						if (!autoNext) {
								m_canvas->whatNextTip(true, true);
								m_lockRightButt = false;
								return; // wait for user
						}
					}
			}
		} else {
				if (!m_supp->wasFinished() && m_exam->count() >= (m_supp->obligQuestions() + m_exam->penalty()) ) { // maybe enough
					if (m_exam->blackCount()) {
							m_exam->increasePenaltys(m_exam->blackCount());
							qDebug() << "penalties increased. Can't finish this exam yet.";
					} else {
              mW->progress->setFinished(true);
              m_exam->setFinished();
							displayCertificate();
							m_supp->setFinished();
					}
				}
		}
    if (showResults && autoNext) {
      m_lockRightButt = true; // to avoid nervous users clicking mouse during wait time
      if (m_shouldBeTerminated)
          stopExamSlot();
      else {
				if (curQ.isCorrect()) {
          m_askingTimer->start(gl->E->questionDelay);
      } else {
          if (!m_exercise && gl->E->repeatIncorrect && !m_incorrectRepeated) // repeat only once if any
              QTimer::singleShot(waitTime, this, SLOT(repeatQuestion()));
          else {
							if (gl->E->afterMistake == TexamParams::e_wait && (!m_exercise || !gl->E->showCorrected))
									waitTime = gl->E->mistakePreview; // for exercises time was set above
							m_askingTimer->start(waitTime);
					}
        }
      }
    }
}

/**
 * %%%%%%%%%% Time flow in Nootka %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * @p correctPreview @p mistakePreview and @p questionDelay  are user configurable vars determining corresponding times
 * Correction animation takes 1500 ms - rest time of @p correctPreview is for user
 * 'result tip (good, not bad, wrong)' takes 2500 ms
 * 'Try again tip is displayed for 3000 ms'
 * 'how to confirm an answer tip' is displayed after 1500 ms
 * When exam/exercise stops after mistake - 'what next tip' appears after 1000 ms
 * Detected note if wrong appears for result tip time or 5000 ms when no auto next question
 */
void TexamExecutor::correctAnswer() {
	if (!gl->E->showCorrected)
			mW->nootBar->removeAction(correctAct);
	if (m_askingTimer->isActive())
			m_askingTimer->stop();
	m_canvas->clearWhatNextTip();
	TQAunit& curQ = m_exam->answList()->last();
	QColor markColor;
	if (curQ.isNotSoBad())
			markColor = gl->EnotBadColor;
	else
			markColor = gl->EquestionColor;
	if (curQ.answerAsNote()) {
			Tnote goodNote = curQ.qa.note;
			if (curQ.questionAsNote())
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
	} else if (curQ.answerAsFret()) {
			TfingerPos goodPos = curQ.qa.pos;
			if (curQ.questionAsFret())
					goodPos = curQ.qa_2.pos;
			mW->guitar->correctPosition(goodPos, markColor);
	} else if (curQ.answerAsName()) {
			Tnote goodNote = curQ.qa.note;
			if (curQ.questionAsName())
					goodNote = curQ.qa_2.note;
			if (!m_answRequire.accid && curQ.isNotSoBad()) { // respect accidental selected by user 
				switch (mW->noteName->getNoteName().acidental) {
					case -2 : goodNote = goodNote.showWithDoubleFlat(); break;
					case -1 : goodNote = goodNote.showWithFlat(); break;
					case  0 : goodNote = goodNote.showAsNatural(); break;
					case  1 : goodNote = goodNote.showWithSharp(); break;
					case  2 : goodNote = goodNote.showWithDoubleSharp(); break;
				}
			}
			mW->noteName->correctName(goodNote, markColor, curQ.isWrong());
	} else { // answer as played sound
			if (curQ.wrongIntonation()) {
					float outTune = mW->sound->pitch() - (float)qRound(mW->sound->pitch());
					mW->pitchView->outOfTuneAnim(outTune, 1200);
					m_canvas->outOfTuneTip(outTune); // we are sure that it is beyond the accuracy threshold
			}
			if (m_supp->isCorrectedPlayable())
					repeatSound();
			else { 
				// Animation towards guitar when instrument is guitar and answer was wrong or octave was wrong
					if (curQ.questionAsFret())
						mW->guitar->correctPosition(curQ.qa.pos, markColor);
					else
						m_canvas->correctToGuitar(curQ.questionAs, gl->E->mistakePreview, curQ.qa.pos);
			}
				
	}
	m_lockRightButt = true; // to avoid nervous users click mouse during correctViewDuration
	if (gl->E->autoNextQuest && gl->E->afterMistake != TexamParams::e_stop) {
			m_askingTimer->start(gl->E->correctPreview);
  }
  if (!gl->E->autoNextQuest || gl->E->afterMistake == TexamParams::e_stop) 
			QTimer::singleShot(2000, this, SLOT(delayerTip())); // 2000 ms - fastest preview time - longer than animation duration
	if (!gl->E->autoNextQuest || !gl->E->showCorrected || gl->E->afterMistake == TexamParams::e_stop)
			QTimer::singleShot(2000, m_canvas, SLOT(clearResultTip())); // exam will stop so clear result tip after correction
}


void TexamExecutor::markAnswer(TQAunit& curQ) {
	QColor markColor = m_supp->answerColor(curQ);
	if (curQ.melody()) {
		for (int i = 0; i < curQ.lastAttepmt()->mistakes.size(); ++i) {
			mW->score->markAnswered(m_supp->answerColor(curQ.lastAttepmt()->mistakes[i]), i);
		}
	} else {
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
			case TQAtype::e_asSound:
				break;
		}
	}
  if (m_exercise && gl->E->showNameOfAnswered) {
		if (!curQ.questionAsName() && !curQ.answerAsName()) {
			if (curQ.answerAsNote() || (curQ.answerAsSound() && curQ.questionAsNote()))
				mW->score->showNames(gl->S->nameStyleInNoteName, true);
			else if (curQ.answerAsFret()) // for q/a fret-fret this will be the first case
				mW->guitar->showName(gl->S->nameStyleInNoteName, markColor); // Take it from user answer
			else if (curQ.answerAsSound() && curQ.questionAsFret())
					mW->guitar->showName(gl->S->nameStyleInNoteName, curQ.qa.note, markColor);
		} else { // cases when name was an question
			if (curQ.questionAsName()) {
				if (curQ.answerAsNote())
					mW->score->showNames(curQ.styleOfQuestion());
				else if (curQ.answerAsFret())
					mW->guitar->showName(curQ.styleOfQuestion(), markColor);
			}
		}
  }
}


void TexamExecutor::repeatQuestion() {
    m_canvas->tryAgainTip(3000);
    m_lockRightButt = false;
    m_incorrectRepeated = true;
    m_isAnswered = false;
		if (gl->E->showNameOfAnswered) {
			for (int i = 0; i < 2; i++)
				mW->score->deleteNoteName(i);
			mW->guitar->deleteNoteName();
		}
	// for melodies questions are newer repeated - copying of TQAunit is safe 
		TQAunit curQ = m_exam->curQ();

    if (!gl->E->autoNextQuest) {
        m_canvas->clearCanvas();
    }
    curQ.setMistake(TQAunit::e_correct);
    
    if (curQ.answerAsNote())
        mW->score->unLockScore();
    if (curQ.questionAsName()) { // refresh question on NoteName
      if (curQ.answerAsFret() && m_level.showStrNr)
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion(), curQ.qa.pos.str());
        else
            mW->noteName->askQuestion(curQ.qa.note, curQ.styleOfQuestion());
    } else
      mW->noteName->clearNoteName();
    if (curQ.answerAsName()) {
			Tnote answNote = Tnote(0, 0, 0);
      mW->noteName->setNameDisabled(false);
      if (curQ.questionAsName())
						answNote = curQ.qa_2.note;
			else if (!curQ.answerAsNote())
							answNote = curQ.qa.note;
			mW->noteName->prepAnswer(curQ.styleOfAnswer());
      mW->noteName->setStyle(curQ.styleOfAnswer());
			if (curQ.questionAsFret() || curQ.questionAsSound()) {
            if (m_level.forceAccids) {
								mW->noteName->forceAccidental(answNote.acidental);
						}
				} else if (curQ.questionAsName())
									mW->noteName->forceAccidental(answNote.acidental);
    }
    if (curQ.answerAsFret())
        mW->guitar->setGuitarDisabled(false);
    if (curQ.answerAsSound() && !curQ.questionAsSound())
        startSniffing();
        // *** When question is sound it is played again (repeatSound()) 
        // and than startSniffing is called

    m_exam->addQuestion(curQ);
    m_blackQuestNr = m_exam->blacList()->count() - 1;
        // Previous answer was wrong or not so bad and it was added at the end of blacList
        // When an answer will be correct the list will be decreased

    if (!gl->E->autoNextQuest)
        mW->startExamAct->setDisabled(true);
    mW->nootBar->removeAction(nextQuestAct);
    mW->nootBar->removeAction(prevQuestAct);
    if (curQ.questionAsSound()) { // *** Here :-)
        mW->nootBar->addAction(repeatSndAct);
        repeatSound();
    }
    m_canvas->questionTip(m_exam);
    mW->nootBar->addAction(checkAct);
    mW->examResults->questionStart();
}


void TexamExecutor::displayCertificate() {
  m_snifferLocked = true;
	mW->sound->wait();
	mW->examResults->pause();
  qApp->removeEventFilter(m_supp); // stop grabbing right button and calling checkAnswer()
  m_canvas->certificateTip();
}

/**
 * Instrument selection in exams/exercises:
 * - Build-in levels force instruments type,
 * - instrument type in saved levels is copied from user preferences when level has guitar or sound 
 * - or it is set to none when level uses only name and note on the staff
 * - during loading levels from older files above rules are used
 * - exam/exercise respects level instrument when it is kind of guitar
 * - but when level has no instrument it lefts user preferred instrument
 * 
 *   Corrected answers appears on guitar when it is visible, and level scale matches to guitar scale
 *   so question list has to be created fret by fret 
 */
void TexamExecutor::prepareToExam() {
		setTitleAndTexts();
		int levelMessageDelay = 1;
		if (TexecutorSupply::paramsChangedMessage())
				levelMessageDelay = 7000;
		QTimer::singleShot(levelMessageDelay, this, SLOT(levelStatusMessage()));
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
		if (m_exercise) {
// 				gl->E->showCorrected = gl->E->showCorrected;
		}	else {
				mW->progress->activate(m_exam->count(), m_supp->obligQuestions(), m_exam->penalty(), m_exam->isFinished());
				mW->examResults->displayTime();
		}
		if (gl->instrument != e_noInstrument && !m_level.answerIsSound())
					mW->pitchView->hide(); // hide pitchView when it is no necessary
    disableWidgets();
// connect all events to check an answer or display tip how to check
    connect(mW->score, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
    connect(mW->noteName, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
    connect(mW->sound, SIGNAL(detectedNote(Tnote)), this, SLOT(expertAnswersSlot()));
    
    qApp->installEventFilter(m_supp);
    connect(m_supp, SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));

    disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
//     disconnect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
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
//     connect(mW->autoRepeatChB, SIGNAL(clicked(bool)), this,
//             SLOT(autoRepeatStateChanged(bool)));
//     connect(mW->expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersStateChanged(bool)));

		m_glStore->storeSettings();
		m_glStore->prepareGlobalsToExam(m_level);

		mW->setSingleNoteMode(gl->S->isSingleNoteMode);
    mW->score->acceptSettings();
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(m_level.withDblAcc);
    mW->guitar->acceptSettings();
    mW->score->isExamExecuting(true);
		mW->score->enableAccidToKeyAnim(!gl->E->expertsAnswerEnable); // no key animation for experts (no time for it)
		if (m_level.canBeSound()) {
				mW->sound->acceptSettings();
			if (mW->sound->isSniffable())
					mW->sound->wait();
			mW->sound->prepareToExam(m_level.loNote, m_level.hiNote);
		}
// 		TtipChart::defaultClef = m_level.clef; // TODO
		mW->updateSize(mW->centralWidget()->size());
    clearWidgets();
		if (gl->instrument != e_noInstrument && !m_supp->isCorrectedPlayable())
				mW->guitar->createRangeBox(m_supp->loFret(), m_supp->hiFret());
    m_soundTimer = new QTimer(this);
    connect(m_soundTimer, SIGNAL(timeout()), this, SLOT(startSniffing()));
		m_askingTimer = new QTimer(this);
		connect(m_askingTimer, SIGNAL(timeout()), this, SLOT(askQuestion()));

		if (m_exercise) {
			mW->progress->hide();
			mW->examResults->hide();
		} else if (mW->guitar->isVisible() && !m_level.canBeMelody()) {
			mW->innerWidget->moveExamToName();
			mW->score->resizeSlot();
		}
    m_snifferLocked = false;
    m_canvas = new Tcanvas(mW->innerWidget, mW);
    connect(m_canvas, SIGNAL(buttonClicked(QString)), this, SLOT(tipButtonSlot(QString)));
		if (m_level.canBeMelody() && m_level.answerIsSound())
				mW->sound->enableStoringNotes(true);
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
// 			gl->E->showCorrected = gl->E->showCorrected;
			gl->E->suggestExam = m_exercise->suggestInFuture();
		}
		
// 		TtipChart::defaultClef = gl->S->clef;
    mW->score->acceptSettings();
		mW->score->enableAccidToKeyAnim(true);
    mW->noteName->setEnabledEnharmNotes(false);
    mW->noteName->setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
    mW->guitar->acceptSettings();
    mW->noteName->setNoteNamesOnButt(gl->S->nameStyleInNoteName);
    mW->progress->terminate();
		mW->sound->acceptSettings();
		mW->sound->enableStoringNotes(false);

		mW->settingsAct->setVisible(true);
		mW->aboutAct->setVisible(true);
    mW->analyseAct->setVisible(true);
    mW->startExamAct->setDisabled(false);
    mW->noteName->setNameDisabled(false);
    mW->guitar->setGuitarDisabled(false);
		mW->pitchView->show();

    if (m_canvas)
        m_canvas->deleteLater();

    connect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
//     connect(mW->noteName, SIGNAL(noteNameWasChanged(Tnote)), mW, SLOT(noteNameWasChanged(Tnote)));
    connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), mW, SLOT(guitarWasClicked(Tnote)));
    connect(mW->sound, SIGNAL(detectedNote(Tnote)), mW, SLOT(soundWasPlayed(Tnote)));
    disconnect(mW->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
    disconnect(mW->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));
//     disconnect(mW->autoRepeatChB, SIGNAL(clicked(bool)), this,
//             SLOT(autoRepeatStateChanged(bool)));
    connect(mW->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
    connect(mW->levelCreatorAct, SIGNAL(triggered()), mW, SLOT(openLevelCreator()));
    mW->score->unLockScore();
		// unfortunately, unLockScore locks clef again
		mW->score->setClefDisabled(false);
    mW->guitar->deleteRangeBox();
    mW->sound->restoreAfterExam();
    mW->clearAfterExam(e_finished);
}


void TexamExecutor::disableWidgets() {
    mW->noteName->setNameDisabled(true);
    mW->score->setScoreDisabled(true);
    mW->guitar->setGuitarDisabled(true);
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
//		nextQuestAct->setToolTip(nextQuestAct->statusTip());
    nextQuestAct->setIcon(QIcon(gl->path + "picts/nextQuest.png"));
    nextQuestAct->setShortcut(QKeySequence(Qt::Key_Space));
    connect(nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
    mW->nootBar->addAction(nextQuestAct);

    prevQuestAct = new QAction(tr("Repeat", "like a repeat question"), this);
    prevQuestAct->setStatusTip(tr("repeat previous question (backspace)"));
//		prevQuestAct->setToolTip(prevQuestAct->statusTip());
    prevQuestAct->setIcon(QIcon(gl->path + "picts/prevQuest.png"));
    prevQuestAct->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));

    checkAct = new QAction(tr("Check", "like a check answer"), this);
    checkAct->setStatusTip(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()));
//		checkAct->setToolTip(checkAct->statusTip());
    checkAct->setIcon(QIcon(gl->path + "picts/check.png"));
    checkAct->setShortcut(QKeySequence(Qt::Key_Return));
    connect(checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));

    if (m_level.questionAs.isSound()) {
        repeatSndAct = new QAction(tr("Play"), this);
        repeatSndAct->setStatusTip(tr("play sound again") + "<br>(" + TexamHelp::pressSpaceKey().replace("<b>", " ").replace("</b>", ")"));
//				repeatSndAct->setToolTip(repeatSndAct->statusTip().replace("<br>", "\n"));
        repeatSndAct->setShortcut(QKeySequence(Qt::Key_Space));
        repeatSndAct->setIcon(QIcon(gl->path + "picts/repeatSound.png"));
        connect(repeatSndAct, SIGNAL(triggered()), this, SLOT(repeatSound()));
    }
    if (m_exercise) {
			correctAct = new QAction(tr("Correct", "like a correct answer with mistake"), this);
			correctAct->setStatusTip(tr("correct answer\n(enter)"));
//			correctAct->setToolTip(correctAct->statusTip());
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
  m_exam->setTune(*gl->Gtune());
	m_penalCount = 0;
	updatePenalStep();
	delete m_exercise;
	m_exercise = 0;
	setTitleAndTexts();
	levelStatusMessage();
	m_supp->setFinished(false); // exercise had it set to true
	initializeExecuting();
	mW->examResults->startExam();
	mW->progress->activate(m_exam->count(), m_supp->obligQuestions(), m_exam->penalty(), m_exam->isFinished());
	mW->examResults->displayTime();
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
			Tnote::EnameStyle tmpStyle = gl->S->nameStyleInNoteName;
      gl->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show charts in user defined style  
      
      bool startExam = false;
      if (!m_goingClosed)
					continuePractice = showExamSummary(true, &startExam);
			gl->S->nameStyleInNoteName = tmpStyle;
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
				if (m_exam->curQ().answerAsSound())
					startSniffing();
			qApp->installEventFilter(m_supp);
			return;
		}
		if (m_exam->count())
				m_exam->saveToFile();
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
						fName += "-" + QDateTime::currentDateTime().toString("(dd-MMM-hhmmss)");
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
				gl->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show in user defined style  
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
		if (m_exam->curQ().answerAsSound())
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
                         TexTrans::examFilterTxt(), 0 , QFileDialog::DontUseNativeDialog);
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
	if (m_exam->curQ().melody())
		mW->sound->playMelody(m_exam->curQ().melody());
	else
		mW->sound->play(m_exam->curQ().qa.note);
	if (m_soundTimer->isActive())
			m_soundTimer->stop();
	if (m_exam->curQ().answerAsSound())
			connect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
}


void TexamExecutor::noteOfMelodySlot(Tnote& n) {
// 	if (mW->sound->notes().size() < m_exam->curQ().melody()->length()) {
		mW->score->selectNote(m_melodyNoteIndex);
		m_melodyNoteIndex++;
// 	} else {
		if ((mW->sound->notes().size() == m_exam->curQ().melody()->length()) && gl->E->expertsAnswerEnable)
			checkAnswer();
		// TODO add a timer to wait for some extra played notes if some captured sounds was invalid. Comparing routines can exclude them then. It will improve detection accuracy.
// 	}
}

/*
void TexamExecutor::autoRepeatStateChanged(bool enable) {
    gl->E->autoNextQuest = enable;
    if (enable) {
        mW->startExamAct->setDisabled(false);
        m_canvas->clearResultTip();
    }
}*/


bool TexamExecutor::showExamSummary(bool cont, bool* startExam) {
  TexamSummary *ES = new TexamSummary(m_exam, gl->path, cont, mW);
	if (m_exercise)
			ES->setForExercise();
  TexamSummary::Eactions respond = ES->doExec();
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
  TexamHelp *hlp = new TexamHelp(Tcolor::bgTag(gl->EquestionColor), Tcolor::bgTag(gl->EanswerColor), 
																 &gl->E->showHelpOnStart, mW);
  hlp->exec();
  delete hlp;
  qApp->installEventFilter(m_supp);
  m_snifferLocked = false;
}


// void TexamExecutor::expertAnswersStateChanged(bool enable) {
//   if (enable) {
//       if (!gl->E->askAboutExpert || showExpertAnswersHelpDlg(mW, &gl->E->askAboutExpert, true))
//       {}
//       else
//           mW->expertAnswChB->setChecked(false); // ignore it, user resigned
//   }
//   gl->E->expertsAnswerEnable = mW->expertAnswChB->isChecked();
// 	mW->score->enableAccidToKeyAnim(!gl->E->expertsAnswerEnable); // no accid to key animation when experts enabled
// }


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
	if (!gl->E->expertsAnswerEnable) { // no expert
		if (gl->hintsEnabled) // show hint how to confirm an answer
			m_canvas->confirmTip(1500);
		return;
	}
	if (m_snifferLocked || m_exam->curQ().melody()) // ignore slot when some dialog window appears or answer for melody
			return;

	/** expertAnswersSlot() is invoked also by TaudioIN/TpitchFinder.
		* Calling checkAnswer() from here invokes stopping and deleting TaudioIN.
		* It finishes with crash. To avoid this checkAnswer() has to be called
		* from outside - by timer event. */
	if (m_exam->curQ().answerAsSound())
			mW->sound->pauseSinffing();
	QTimer::singleShot(1, this, SLOT(checkAnswer()));
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
    else if (name == "certClosing")
        unlockAnswerCapturing();
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
	if (!gl->hintsEnabled)
		return;
	m_canvas->whatNextTip(!(!m_exercise && gl->E->repeatIncorrect && !m_incorrectRepeated));
	/** When exam mode and mistake occurred it will be true,
	 * so whatNextTip(false) is invoked - whatNextTip displays repeat question hint
	 * otherwise (exercise and/or correct answer) @p whatNextTip(true) goes. */
}


void TexamExecutor::setTitleAndTexts() {
	if (m_exercise) {
			mW->setWindowTitle(tr("Exercises with Nootka"));
			mW->startExamAct->setStatusTip(tr("finish exercising"));
		} else {
			mW->setWindowTitle(tr("EXAM!") + " " + m_exam->userName() + " - " + m_level.name);
			mW->startExamAct->setStatusTip(tr("stop the exam"));
	}
}


void TexamExecutor::levelStatusMessage() {
	mW->setMessageBg(-1); // reset background
	if (m_exercise)
			mW->setStatusMessage(tr("You are exercising on level") + ":<br><b>" + m_level.name + "</b>");
	else
			mW->setStatusMessage(tr("Exam started on level") + ":<br><b>" + m_level.name + "</b>");
}


void TexamExecutor::unlockAnswerCapturing() {
	if (m_exam->curQ().answerAsSound())
		mW->sound->go();
	mW->examResults->go();
  qApp->installEventFilter(m_supp); // restore grabbing right mouse button
  m_snifferLocked = false;
}


void TexamExecutor::blindQuestion() {
	qDebug() << "Blind question - asking again";
	m_blindCounter++;
	QTimer::singleShot(10, this, SLOT(askQuestion()));
}




