/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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
#include "tglobalexamstore.h"
#include "texercises.h"
#include "tequalrand.h"
#include "trandmelody.h"
#include "tpenalty.h"
#include "texammelody.h"
#include "mainwindow.h"
#include <level/tlevelselector.h>
#include <tsound.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include <help/texamhelp.h>
#include <help/texpertanswerhelp.h>
#include <score/tmainscore.h>
#include <guitar/tfingerboard.h>
#include <notename/tnotename.h>
#include <widgets/tintonationview.h>
#include <widgets/tpitchview.h>
#include <level/tfixleveldialog.h>
#include <tglobals.h>
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
#include <music/tmelody.h>
#include <tlayoutparams.h>
#include <graphics/tnotepixmap.h>
#include <gui/ttoolbar.h>
#include <gui/tmainview.h>
#include <QtWidgets>


#define WAIT_TIME (150) //[ms]
#define SOUND_DURATION (1500) //[ms]


void debugStyle(TQAunit &qa) {
    qDebug("styles debugging");
    qDebug() << "Q:" << qa.styleOfQuestion() << "A:" << qa.styleOfAnswer();
}


extern Tglobals *gl;


        /** Returns a file name generated from user name and level,
        * but when such a file exists in current exam directory some time mark is added. */
QString getExamFileName(Texam* e) {
  QString fName = QDir::toNativeSeparators(gl->E->examsDir + "/" + e->userName() + "-" + e->level()->name);
  if (QFileInfo(fName  + ".noo").exists())
    fName += "-" + QDateTime::currentDateTime().toString("(dd-MMM-hhmmss)");
  return fName;
}

TexamExecutor::TexamExecutor(MainWindow *mainW, QString examFile, Tlevel *lev) :
  QObject(mainW),
  m_exam(0),
  mW(mainW),
  m_lockRightButt(false),
  m_goingClosed(false),
  m_snifferLocked(false),
  m_canvas(0),
  m_supp(0),
  m_penalty(0),
  m_exercise(0),
  m_blindCounter(0),
  m_rand(0)
{
	QString resultText;
	TstartExamDlg::Eactions userAct;

	mW->sound->wait();
	if (lev) {
			m_level = *lev;
			if (gl->E->studentName.isEmpty()) {
					resultText = TstartExamDlg::systemUserName();
			} else
					resultText = gl->E->studentName;
			if (examFile == "exercise")
				userAct = TstartExamDlg::e_runExercise;
			else
				userAct = TstartExamDlg::e_newExam;
	} else {
			if (examFile.isEmpty()) { // start exam dialog
					TstartExamDlg *startDlg = new TstartExamDlg(gl->E->studentName, gl->E, mW);
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
			if (!fixLevelInstrument(m_level, QString(), gl->instrumentToFix, mainW)) {
						mW->clearAfterExam(e_failed);
						deleteExam();
						return;
				}
			gl->E->studentName = resultText; // store user name
			m_exam->setTune(*gl->Gtune());
//         mW->examResults->startExam(m_exam);
			if (userAct == TstartExamDlg::e_runExercise) {
					m_exercise = new Texercises(m_exam);
			}
	} else if (userAct == TstartExamDlg::e_contExam) {
			m_exam = new Texam(&m_level, QString());
			Texam::EerrorType err = m_exam->loadFromFile(resultText);
			if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
				if (err == Texam::e_file_corrupted)
					QMessageBox::warning(mW, " ", 
						tr("<b>Exam file seems to be corrupted</b><br>Better start new exam on the same level"));
				if (!fixLevelInstrument(m_level, m_exam->fileName(), gl->instrumentToFix, mainW) ||
						!showExamSummary(mW, m_exam, true)) {
              mW->clearAfterExam(e_failed);
              deleteExam();
              return;
				}
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
	// ---------- End of checking ----------------------------------

  mW->setSingleNoteMode(!m_exam->melodies());
	m_supp = new TexecutorSupply(&m_level, this);
	m_supp->createQuestionsList(m_questList);
  if (m_exam->melodies())
    m_melody = new TexamMelody(this);
	if (m_questList.isEmpty()) {
			QMessageBox::critical(mW, " ", tr("Level <b>%1</b><br>makes no sense because there are no questions to ask.<br>It can be re-adjusted.<br>Repair it in Level Creator and try again.").arg(m_level.name));
			delete m_supp;
			mW->clearAfterExam(e_failed);
			deleteExam();
			return;
	}
	prepareToExam();
	if (gl->E->showHelpOnStart)
			showExamHelp();
	if (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isFret()) {
		if (!m_supp->isGuitarOnlyPossible()) {
				qDebug("Something stupid!\n Level has question and answer as position on guitar but any question is available.");
				mW->clearAfterExam(e_failed);
				deleteExam();
				return;
		}
	}
	
	initializeExecuting();
	createActions();
}


TexamExecutor::~TexamExecutor() {
	if (m_penalty)
		delete m_penalty;
	if (m_supp)
			delete m_supp;
	delete m_glStore;
	if (m_rand)
		delete m_rand;
	deleteExam();
}


void TexamExecutor::initializeExecuting() {
	m_shouldBeTerminated = false;
	m_incorrectRepeated = false;
	m_isAnswered = true;
	m_penalty = new Tpenalty(m_exam, m_supp, mW->examResults, mW->progress);
	connect(m_penalty, SIGNAL(certificate()), this, SLOT(displayCertificate()));
	if (m_exercise) {
    if (gl->E->suggestExam)
      m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
	} else {
			connect(m_canvas, SIGNAL(certificateMagicKeys()), this, SLOT(displayCertificate()));
      if (m_level.answerIsNote())
        mW->score->enableAccidToKeyAnim(false);
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
// 	qDebug() << "Questions nr: " << m_questList.size() << "Randoms:" << m_supp->obligQuestions() - m_exam->count();
}


void TexamExecutor::askQuestion(bool isAttempt) {
	m_askingTimer->stop();
	m_lockRightButt = false; // release mouse button events
	if (m_exercise && !gl->E->showCorrected) // hide correct action button
      mW->bar->removeAction(mW->bar->correctAct);
	if (m_exam->count() && m_exercise && m_exam->melodies())
    disconnect(mW->score, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody);
	if (m_exam->count() && m_exam->melodies())
		mW->bar->removeAction(mW->bar->attemptAct);
	if (!isAttempt) { // add new question to the list
		m_penalty->setMelodyPenalties();
		if (m_exam->count() && m_exercise) // Check answer only after summarize
				m_exercise->checkAnswer();
		TQAunit Q(m_exam);
		m_exam->addQuestion(Q);
	}
	TQAunit* curQ = m_exam->curQ();
  m_isAnswered = false;
	if (!isAttempt) {
		clearWidgets();
		if (m_blindCounter > 20) {
				QMessageBox::critical(mW, "Level error!", QString("Nootka attempted to create proper question-answer pair 20 times<br>Send this message and a level file to developers and we will try to fix it in further releases."));
				mW->clearAfterExam(e_failed);
				deleteExam();
				return;
		}
    if (!gl->E->autoNextQuest) {
			if (!m_exercise)
					mW->bar->startExamAct->setDisabled(true);
			m_canvas->clearCanvas();
    }
//     m_isAnswered = false;
    m_incorrectRepeated = false;
    m_answRequire.octave = m_level.requireOctave;
    m_answRequire.accid = m_level.forceAccids;
    m_answRequire.key = false;
		
		mW->noteName->setStyle(gl->S->nameStyleInNoteName);
    mW->noteName->setNoteNamesOnButt(gl->S->nameStyleInNoteName);
    
		m_penalty->nextQuestion();
		if (!m_exercise && m_penalty->ask()) {

		} else {
				if (!m_exam->melodies()) // leave them empty for melody - there are all notes and positions
						curQ->qa = m_questList[m_rand->get()];
				curQ->questionAs = m_level.questionAs.next();
				curQ->answerAs = m_level.answersAs[curQ->questionAs].next();
		}
    
    if (m_penalty->isNot() && curQ->questionAsFret() && curQ->answerAsFret())
      curQ->qa  = m_questList[m_supp->getQAnrForGuitarOnly()];

    if (m_penalty->isNot() && (curQ->questionAsNote() || curQ->answerAsNote())) {
        if (m_level.useKeySign)
					curQ->key = m_supp->getKey(curQ->qa.note);
        if (!m_level.onlyCurrKey) // if key doesn't determine accidentals, we do this
            curQ->qa.note = m_supp->determineAccid(curQ->qa.note);
    }
    if (m_exam->melodies()) {
      int melodyLength = qBound(qMax(2, qRound(m_level.melodyLen * 0.7)), //at least 70% of length but not less than 2
                                      qRound(((6.0 + (qrand() % 5)) / 10.0) * (qreal)m_level.melodyLen), (int)m_level.melodyLen);
      if (m_penalty->isNot()) {
        curQ->addMelody(QString("%1").arg(m_exam->count()));
        curQ->melody()->setKey(curQ->key);
        getRandomMelody(m_questList, curQ->melody(), melodyLength, m_level.onlyCurrKey, m_level.endsOnTonic);
      }
      m_melody->newMelody(curQ->answerAsSound() ? curQ->melody()->length() : 0); // prepare list to store notes played by user or clear it
      m_exam->newAttempt();
			curQ->lastAttempt()->melodyWasPlayed(); // it was played once for sure 
      if (m_exercise) 
        m_melody->clearToFix(melodyLength);
		}
//    qDebug() << curQ->qa.note.toText() << "Q" << (int)curQ->questionAs
//            << "A" << (int)curQ->answerAs << curQ->key.getName()
//            << (int)curQ->qa.pos.str() << (int)curQ->qa.pos.fret();
	}

  // ASKING QUESTIONS
	if (curQ->questionAsNote()) {
		if (curQ->melody()) {
			if (!isAttempt) {
				mW->score->askQuestion(curQ->melody());
        if (m_level.showStrNr) { // we may be sure that instrument is kind of a guitar
          for (int i = 0; i < curQ->melody()->length(); ++i) {
            if (curQ->melody()->note(i)->g().str() > 1)
              mW->score->noteFromId(i)->setString(curQ->melody()->note(i)->g().str());
          }
          if (mW->guitar->isVisible())
            mW->guitar->prepareAnswer(); // It just shows range frame
        }
      }
			if (curQ->answerAsSound()) { // in fact, there is no other option yet
				connect(mW->sound, &Tsound::noteStartedEntire, this, &TexamExecutor::noteOfMelodyStarted);
        connect(mW->sound, &Tsound::noteFinishedEntire, this, &TexamExecutor::noteOfMelodyFinished);
        connect(mW->score, &TmainScore::lockedNoteClicked, this, &TexamExecutor::noteOfMelodySelected);
				mW->score->selectNote(0); // mark first note
        mW->score->setReadOnlyReacting(true); // allow user to select beginning note to play
			}
		} else {
			char strNr = 0;
			if ((curQ->answerAsFret() || curQ->answerAsSound())
					&& !m_level.onlyLowPos && m_level.showStrNr)
							strNr = curQ->qa.pos.str(); // do show string number or not
			if (m_level.useKeySign && !curQ->answerAsNote())
					// when answer is also asNote we determine key in preparing answer part
					mW->score->askQuestion(curQ->qa.note, curQ->key, strNr);
			else 
					mW->score->askQuestion(curQ->qa.note, strNr);
			if (curQ->answerAsName())
					m_answRequire.accid = true;
			else if (curQ->answerAsSound())
					m_answRequire.accid = false;
		}
	}

    if (curQ->questionAsName()) {
        if (m_penalty->isNot()) { // regular question
          if (curQ->answerAsName()) { // prepare answer
            curQ->qa_2.note = m_supp->forceEnharmAccid(curQ->qa.note); // force other enharm name of note - expected note
            m_answRequire.accid = true;
            if (m_level.requireStyle || curQ->qa.note == curQ->qa_2.note) {
              // when user (level) wants different names or only way to have different answer and question is
              // to change the style (note are the same)
                curQ->setStyle(m_prevQuestStyle, m_supp->randomNameStyle(m_prevQuestStyle)); // randomize style
                m_prevAnswStyle= curQ->styleOfAnswer(); 
            } else { // enharmonic notes in the same style
                curQ->setStyle(gl->S->nameStyleInNoteName, gl->S->nameStyleInNoteName);
                m_prevAnswStyle = gl->S->nameStyleInNoteName;
                m_prevQuestStyle = gl->S->nameStyleInNoteName;
            }
          } else // note name only in question
              if (m_level.requireStyle) { // switch previous used style
                curQ->setStyle(m_supp->randomNameStyle(m_prevQuestStyle), gl->S->nameStyleInNoteName);
								m_prevQuestStyle = curQ->styleOfQuestion();
//                 m_prevQuestStyle = m_supp->randomNameStyle(curQ->styleOfQuestion());
              } else {
                  curQ->setStyle(gl->S->nameStyleInNoteName, curQ->styleOfAnswer());
                  m_prevQuestStyle = gl->S->nameStyleInNoteName;
              }
        }
        // Show question on TnoteName widget
        if ((curQ->answerAsFret() || curQ->answerAsSound()) && m_level.showStrNr)
            mW->noteName->askQuestion(curQ->qa.note, curQ->styleOfQuestion(), curQ->qa.pos.str());
        else
            mW->noteName->askQuestion(curQ->qa.note, curQ->styleOfQuestion());
        if (curQ->answerAsSound())
            m_answRequire.accid = false; // reset checking accidentals determined by level
    }

    if (curQ->questionAsFret()) {
        mW->guitar->askQuestion(curQ->qa.pos);
        if (curQ->answerAsNote())
            m_answRequire.octave = true; // checking accidental determined by level
        if (curQ->answerAsSound()) {
            m_answRequire.accid = false;
            m_answRequire.octave = true;
        }
        
    }

	if (curQ->questionAsSound()) {
		if (curQ->melody()) {
			if (!isAttempt) // play melody but not when user tries again
				repeatSound();
		} else {
				mW->sound->play(curQ->qa.note);
				if (curQ->answerAsSound())
						m_answRequire.accid = false;
		}
	}

// PREPARING ANSWERS
  if (curQ->answerAsNote()) {
    if (!curQ->melody()) {
      if (m_level.useKeySign) {
          if (m_level.manualKey) { // user have to manually select a key
//               if (m_penalty->isNot()) // if black question key mode is defined
//                   curQ->key.setMinor(bool(qrand() % 2));
              mW->score->prepareKeyToAnswer(// we randomize some key to cover this expected one
                  (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value(), curQ->key.getName());
              m_answRequire.key = true;
          } else {
              mW->score->setKeySignature(curQ->key);
          }
      }
      if (curQ->questionAsNote()) {// note has to be another than question
          if (m_penalty->isNot())
              curQ->qa_2.note = m_supp->forceEnharmAccid(curQ->qa.note); // curQ->qa_2.note is expected note
          if (!m_level.manualKey && curQ->qa_2.note == curQ->qa.note) {
              blindQuestion();
              return; // refresh this function scope by calling it outside
          }
          mW->score->forceAccidental((Tnote::Ealter)curQ->qa_2.note.alter);
          m_answRequire.accid = true;
          m_answRequire.octave = true;
      }
      if (curQ->questionAsFret() || curQ->questionAsSound()) {
          if (m_level.forceAccids) {
              mW->score->forceAccidental((Tnote::Ealter)curQ->qa.note.alter);
          }
      }
      if (curQ->questionAsName()) {
          m_answRequire.accid = true;
          m_answRequire.octave = true;
      }
    }
    mW->score->unLockScore();
    if (m_level.useKeySign && !m_level.manualKey) // case for either for single mode and melodies
      mW->score->lockKeySignature(true); // disables key signature enabled above - user cannot change it in this case (fixed key)
  }

  if (curQ->answerAsName()) {
          /** During an exam Note name style is changed in two cases:
          * 1. If level.requireStyle = true every question or answer with Note Name
          *       switch it (letters/solfege)
          * 2. If Note Name is question and answer and are the same - this is only way that it has sense    
          */
      Tnote answNote = Tnote(0, 0, 0);
      if (curQ->questionAsName())
          answNote = curQ->qa_2.note;
      else {
        answNote = curQ->qa.note;
        if (m_level.requireStyle)
            m_prevAnswStyle = m_supp->randomNameStyle(m_prevAnswStyle);
        curQ->setStyle(curQ->styleOfQuestion(), m_prevAnswStyle);
      }
      mW->noteName->prepAnswer(curQ->styleOfAnswer());
      if (curQ->questionAsFret() || curQ->questionAsSound()) {
          if (m_level.forceAccids) {
              mW->noteName->forceAccidental(answNote.alter);
          }
      } else if (curQ->questionAsName())
                mW->noteName->forceAccidental(answNote.alter);
      mW->noteName->setStyle(curQ->styleOfAnswer());
  }

  if (curQ->answerAsFret()) {
//         mW->guitar->setGuitarDisabled(false);
//         mW->guitar->prepareAnswer();
      m_answRequire.accid = false;  // Ignored in checking, positions are comparing
      if (curQ->questionAsFret()) {
        QList<TfingerPos> posList;
        m_supp->getTheSamePosNoOrder(curQ->qa.pos, posList);
        if (posList.isEmpty()) {
            blindQuestion();
            return; // refresh this function scope by calling it outside
        } else {
            if (m_penalty->isNot())
                curQ->qa_2.pos = posList[qrand() % posList.size()];
            mW->guitar->setHighlitedString(curQ->qa_2.pos.str());
        }
      } else 
        if (m_level.showStrNr)
          mW->guitar->setHighlitedString(curQ->qa.pos.str());
      mW->guitar->setGuitarDisabled(false);
      mW->guitar->prepareAnswer();
  }
  
  if (curQ->answerAsSound()) {
    mW->sound->prepareAnswer();
    if (curQ->questionAsSound()) {
        connect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying())); // sniffing after finished sound
    } else
        QTimer::singleShot(WAIT_TIME, this, SLOT(startSniffing()));
        // Give a student some time to prepare itself for next question in expert mode
        // It avoids capture previous played sound as current answer
  } else
      mW->sound->wait(); // stop sniffing if answer is not a played sound
  
  mW->bar->setForQuestion(curQ->questionAsSound(), curQ->questionAsSound() && curQ->answerAsNote());
  m_penalty->startQuestionTime();
  m_canvas->questionTip();
  m_blindCounter = 0; // question successfully asked - reset the counter
}


void TexamExecutor::checkAnswer(bool showResults) {
	TQAunit* curQ = m_exam->curQ();
	m_penalty->stopQuestionTime();
	mW->bar->setAfterAnswer();
	if (curQ->answerAsSound()) {
			if (m_exam->melodies())
				mW->sound->wait(); // flush buffers after captured melody
			else
				mW->sound->pauseSinffing(); // but only skip detected for single sound
			mW->score->selectNote(-1);
			disconnect(mW->sound, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying);
			disconnect(mW->sound, &Tsound::noteStartedEntire, this, &TexamExecutor::noteOfMelodyStarted);
      disconnect(mW->sound, &Tsound::noteFinishedEntire, this, &TexamExecutor::noteOfMelodyFinished);
      disconnect(mW->score, &TmainScore::lockedNoteClicked, this, &TexamExecutor::noteOfMelodySelected);
	}
	if (m_exam->melodies() && mW->sound->melodyIsPlaying())
		mW->sound->stopPlaying();
		
	if (!gl->E->autoNextQuest || m_exercise)
			mW->bar->startExamAct->setDisabled(false);
	m_isAnswered = true;
// Let's check
	Tnote questNote, answNote, userNote; // example note & returned note
// At first we determine what has to be checked
	if (!curQ->melody()) {
    questNote = curQ->qa.note;
    if (curQ->answerAsNote()) {
        if (m_level.manualKey) {
            if (mW->score->keySignature().value() != curQ->key.value())
                curQ->setMistake(TQAunit::e_wrongKey);
        }
        if (curQ->questionAsNote())
            questNote = curQ->qa_2.note;
        answNote = mW->score->getNote(0);
    }
    if (curQ->answerAsName()) {
        if (curQ->questionAsName())
            questNote = curQ->qa_2.note;
        m_prevNoteIfName = mW->noteName->getNoteName(); // store note to restore it if will be repeated
        answNote = mW->noteName->getNoteName();
    }
    userNote = answNote;
    if (curQ->answerAsSound()) {
				answNote = mW->sound->note();
				if ((TintonationView::Eaccuracy)m_level.intonation != TintonationView::e_noCheck) {
						if (TnoteStruct(Tnote(), mW->sound->pitch()).inTune(TintonationView::getThreshold(m_level.intonation)))
								curQ->setMistake(TQAunit::e_wrongIntonation);
				}
    }
	}
// Now we can check
	if (curQ->answerAsFret()) { // 1. Comparing positions
			TfingerPos answPos, questPos;
			answPos = mW->guitar->getfingerPos();
			if (curQ->questionAsFret()) { 
				if (answPos == curQ->qa.pos) { // check has not user got answer the same as question position
					curQ->setMistake(TQAunit::e_wrongPos);
					qDebug("Cheater!");
				} else 
					questPos = curQ->qa_2.pos;
			} else
				questPos = curQ->qa.pos;
			if (questPos != answPos && curQ->isCorrect()) { // if no cheater give him a chance
				QList <TfingerPos> tmpPosList; // Maybe hi gave correct note but on incorrect string only
				m_supp->getTheSamePosNoOrder(answPos, tmpPosList); // get other positions
        bool otherPosFound = false;
				for (int i = 0; i < tmpPosList.size(); i++) {
            if (tmpPosList[i] == questPos) { // and compare it with expected
              otherPosFound = true;
              break;
            }
        }
        if (otherPosFound) {
          if (m_level.showStrNr || answPos.fret() < m_level.loFret || answPos.fret() > m_level.hiFret)
            curQ->setMistake(TQAunit::e_wrongString); // check the level settings and mark as wrong string when deserve
        } else
            curQ->setMistake(TQAunit::e_wrongPos);
			}
	} else {
			if (curQ->melody()) { // 2. or checking melodies
					curQ->setMistake(TQAunit::e_correct); // reset an answer
					if (curQ->answerAsNote()) { // dictation
						Tmelody answMelody;
						mW->score->getMelody(&answMelody);
						m_supp->compareMelodies(curQ->melody(), &answMelody, curQ->lastAttempt());
					} else { // playing a score
						m_supp->compareMelodies(curQ->melody(), m_melody->listened(), curQ->lastAttempt());
					}
					int goodAllready = 0, notBadAlready = 0, wrongAlready = 0;
					for (int i = 0; i < curQ->lastAttempt()->mistakes.size(); ++i) { // setting mistake type in TQAunit
						if (curQ->lastAttempt()->mistakes[i] == TQAunit::e_correct) {
							goodAllready++;
							continue; // it was correct - skip
						}
						if (curQ->lastAttempt()->mistakes[i] & TQAunit::e_wrongNote) {
							wrongAlready++;
						} else // or 'not bad'
								notBadAlready++;
					}
					if (goodAllready == curQ->melody()->length()) { // all required notes are correct
							curQ->setMistake(TQAunit::e_correct); // even if user put them more and effect. is poor
// 							qDebug() << "Melody is correct";
					} else if (goodAllready + notBadAlready == curQ->melody()->length()) { // add committed mistakes of last attempt
							curQ->setMistake(curQ->lastAttempt()->summary()); // or 'not bad'
// 							qDebug() << "Melody is not bad" << curQ->mistake();
					} else if (goodAllready + notBadAlready >= curQ->melody()->length() * 0.7) { // at least 70% notes answered properly
// 						qDebug() << "Melody has little notes";
						if (curQ->lastAttempt()->effectiveness() >= 50.0) { // and effectiveness is sufficient
                curQ->setMistake(curQ->lastAttempt()->summary());
								curQ->setMistake(TQAunit::e_littleNotes); // but less notes than required
// 								qDebug() << "... and sufficient effectiveness";
						} else { // or effectiveness is too poor
								curQ->setMistake(TQAunit::e_veryPoor);
// 								qDebug() << "... but very poor effectiveness" << curQ->lastAttempt()->effectiveness();
						}
					} else {
							curQ->setMistake(TQAunit::e_wrongNote);
// 							qDebug() << "Simply wrong answer";
					}
					if (m_level.manualKey && !curQ->isWrong()) {
            if (mW->score->keySignature().value() != curQ->key.value())
                curQ->setMistake(TQAunit::e_wrongKey);
					}
					// Another case is poor or very poor effectiveness but it is obtained after effect. update in sumarizeAnswer()
			} else { // 3. or checking are the notes the same
					m_supp->checkNotes(curQ, questNote, answNote, m_answRequire.octave, m_answRequire.accid);
					if (!m_answRequire.accid && curQ->isCorrect() && (curQ->answerAsNote() || curQ->answerAsName())) {
			// Save user given note when it is correct and accidental was not forced to respect kind of accidental
						if (curQ->questionAs == curQ->answerAs)
							curQ->qa_2.note = userNote;
						else
							curQ->qa.note = userNote;
					}
			}
	}
	m_penalty->checkAnswer();
	
	disableWidgets();
	bool autoNext = gl->E->autoNextQuest;
	if (gl->E->afterMistake == TexamParams::e_stop && !curQ->isCorrect())
			autoNext = false; // when mistake and e_stop - the same like autoNext = false;
		
	if (showResults) {
      m_canvas->resultTip(curQ); // tip duration is calculated by itself (inside resultTip() method)
			if ((!m_exercise || (m_exercise && curQ->isCorrect())) && !autoNext)
        m_canvas->whatNextTip(curQ->isCorrect());
      if (!autoNext) {
        if (!curQ->isCorrect() && !m_exercise && !curQ->melody())
            mW->bar->addAction(mW->bar->prevQuestAct);
        if (!curQ->isCorrect() && curQ->melody())
          mW->bar->addAction(mW->bar->attemptAct);
        mW->bar->addAction(mW->bar->nextQuestAct);
      }
	}

	markAnswer(curQ);
	int waitTime = gl->E->questionDelay;
	if (m_exercise) {
    if ((gl->E->autoNextQuest && gl->E->afterMistake != TexamParams::e_continue) || !gl->E->autoNextQuest || gl->E->showCorrected)
      waitTime = gl->E->correctPreview; // user has to have time to see his mistake and correct answer
    m_exercise->checkAnswer();
    if (!curQ->isCorrect()) { // correcting wrong answer
        if (gl->E->showCorrected) // TODO for dictation it should always stop and show mistakes
          correctAnswer();
        else {
          if (!gl->E->autoNextQuest || (gl->E->autoNextQuest && gl->E->afterMistake == TexamParams::e_stop))
              mW->bar->addAction(mW->bar->correctAct); // show too button only when exam stops after mistake
          if (!autoNext) {
              m_canvas->whatNextTip(true, true);
              m_lockRightButt = false;
              return; // wait for user
          }
        }
    }
	}
	if (showResults && autoNext) {
      m_lockRightButt = true; // to avoid nervous users clicking mouse during wait time
      if (m_shouldBeTerminated)
          stopExamSlot();
      else {
				if (curQ->isCorrect()) {
          m_askingTimer->start(gl->E->questionDelay);
      } else {
					if (gl->E->repeatIncorrect && !m_incorrectRepeated) {
						if (curQ->melody())
							QTimer::singleShot(waitTime, this, SLOT(newAttempt()));
						else {
							if (!m_exercise) // repeat only once if any
								QTimer::singleShot(waitTime, this, SLOT(repeatQuestion()));
							else
								m_askingTimer->start(waitTime);
						}
					} else {
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
    mW->bar->removeAction(mW->bar->correctAct);
  bool correctAnimStarted = false;
	if (m_askingTimer->isActive())
			m_askingTimer->stop();
	m_canvas->clearWhatNextTip();
	TQAunit* curQ = m_exam->answList()->last();
	QColor markColor = m_supp->answerColor(curQ);
	if (curQ->melody() && (curQ->answerAsNote() || curQ->questionAsNote())) {
		mW->score->setReadOnlyReacting(true); // It is undone whenever unLockScore() is called
	}
	if (curQ->answerAsNote()) {
		if (curQ->melody()) {
			
		} else {
			Tnote goodNote = curQ->qa.note;
			if (curQ->questionAsNote())
				goodNote = curQ->qa_2.note;
			if (curQ->wrongAccid() || curQ->wrongOctave()) // it corrects wrong octave as well
					mW->score->correctAccidental(goodNote);
			else if (curQ->wrongNote()) {
					if (m_level.manualKey && curQ->key.value() != mW->score->keySignature().value())
						mW->score->correctKeySignature(curQ->key);
          m_exercise->setCorrectedNoteId(0);
					mW->score->correctNote(goodNote, markColor);
			}
			if (curQ->wrongKey())
					mW->score->correctKeySignature(curQ->key);
      correctAnimStarted = true;
		}
	} else if (curQ->answerAsFret()) {
			TfingerPos goodPos = curQ->qa.pos;
			if (curQ->questionAsFret())
					goodPos = curQ->qa_2.pos;
			mW->guitar->correctPosition(goodPos, markColor);
      correctAnimStarted = true;
	} else if (curQ->answerAsName()) {
			Tnote goodNote = curQ->qa.note;
			if (curQ->questionAsName())
					goodNote = curQ->qa_2.note;
			if (!m_answRequire.accid && curQ->isNotSoBad()) { // respect accidental selected by user 
				switch (mW->noteName->getNoteName().alter) {
					case -2 : goodNote = goodNote.showWithDoubleFlat(); break;
					case -1 : goodNote = goodNote.showWithFlat(); break;
					case  0 : goodNote = goodNote.showAsNatural(); break;
					case  1 : goodNote = goodNote.showWithSharp(); break;
					case  2 : goodNote = goodNote.showWithDoubleSharp(); break;
				}
			}
			mW->noteName->correctName(goodNote, markColor, curQ->isWrong());
      correctAnimStarted = true;
	} else { // answer as played sound
		if (curQ->melody()) {
			
		} else {
			if (curQ->wrongIntonation()) {
					float outTune = mW->sound->pitch() - (float)qRound(mW->sound->pitch());
					mW->pitchView->outOfTuneAnim(outTune, 1200);
					m_canvas->outOfTuneTip(outTune); // we are sure that it is beyond the accuracy threshold
          correctAnimStarted = true;
			}
			if (m_supp->isCorrectedPlayable())
					repeatSound();
			else {
				if (mW->guitar->isVisible()) {
				// Animation towards guitar when instrument is guitar and answer was wrong or octave was wrong
					if (curQ->questionAsFret())
						mW->guitar->correctPosition(curQ->qa.pos, markColor);
					else
						m_canvas->correctToGuitar(curQ->questionAs, gl->E->mistakePreview, curQ->qa.pos);
          correctAnimStarted = true;
				}
			}
		}
	}
	if (correctAnimStarted) { // disable space bar and right mouse button when animation is performed
    mW->bar->nextQuestAct->setDisabled(true);
    m_lockRightButt = true;
  } else
    correctionFinished();
}


void TexamExecutor::newAttempt() {
	m_canvas->tryAgainTip(3000);
	QTimer::singleShot(2000, m_canvas, SLOT(clearResultTip())); 
	if (m_exam->curQ()->answerAsNote() || m_exam->curQ()->questionAsNote()) // remove names and marks from score notes
		for (int i = 0; i < mW->score->notesCount(); ++i) {
			if (m_exercise) {
					mW->score->deleteNoteName(i);
					if (m_exam->curQ()->lastAttempt()->mistakes[i] != TQAunit::e_correct)
						mW->score->markQuestion(-1, i); // leave note marks (colored borders) only for correct answers
			} else // although clear all marked notes in exams
					mW->score->markQuestion(-1, i);
		}
	// prepare list to store notes played by user or clear it
	m_melody->newMelody(m_exam->curQ()->answerAsSound() ? m_exam->curQ()->melody()->length() : 0);
// 	m_melody->clearToFix(m_exam->curQ()->melody()->length());
	m_penalty->newAttempt();
	if (m_exam->curQ()->answerAsSound())
				m_exam->curQ()->lastAttempt()->melodyWasPlayed(); // we can suppose that user will play an answer for sure 
	askQuestion(true);
}


void TexamExecutor::markAnswer(TQAunit* curQ) {
	QColor markColor = m_supp->answerColor(curQ);
	if (curQ->melody()) {
		for (int i = 0; i < curQ->lastAttempt()->mistakes.size(); ++i) {
			mW->score->markAnswered(m_supp->answerColor(curQ->lastAttempt()->mistakes[i]), i);
		}
	} else {
		switch (curQ->answerAs) {
			case TQAtype::e_asNote:
				mW->score->markAnswered(markColor);
				break;
			case TQAtype::e_asFretPos:
				mW->guitar->markAnswer(markColor);
				break;
			case TQAtype::e_asName:
				mW->noteName->markNameLabel(markColor);      
				break;
			case TQAtype::e_asSound:
				mW->pitchView->markAnswer(markColor);
				break;
		}
		switch (curQ->questionAs) {
			case TQAtype::e_asNote:
				mW->score->markQuestion(markColor);
				break;
			case TQAtype::e_asFretPos:
				mW->guitar->markQuestion(markColor);
				break;
			case TQAtype::e_asName:
				mW->noteName->markNameLabel(markColor);      
				break;
			case TQAtype::e_asSound:
				break;
		}
	}                                                       // TODO
  if (m_exercise && gl->E->showNameOfAnswered /*&& (!gl->E->autoNextQuest || (gl->E->autoNextQuest && gl->E->afterMistake != TexamParams::e_continue))*/) {
		if (!curQ->questionAsName() && !curQ->answerAsName()) {
      Tnote emptyNote; // to force guitar to find note name set by user
			if (curQ->answerAsNote() || (curQ->answerAsSound() && curQ->questionAsNote()))
				mW->score->showNames(gl->S->nameStyleInNoteName);
			else if (curQ->answerAsFret()) // for q/a fret-fret this will be the first case
				mW->guitar->showName(gl->S->nameStyleInNoteName, emptyNote, markColor); // Take it from user answer
			else if (curQ->answerAsSound() && curQ->questionAsFret())
					mW->guitar->showName(gl->S->nameStyleInNoteName, emptyNote, markColor);
		} else { // cases when name was an question
			if (curQ->questionAsName()) {
				if (curQ->answerAsNote())
					mW->score->showNames(curQ->styleOfQuestion());
				else if (curQ->answerAsFret())
					mW->guitar->showName(curQ->styleOfQuestion(), markColor);
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
// for melodies it never comes here - questions are newer repeated - copying of TQAunit is safe 
  TQAunit curQ(*m_exam->curQ()); // copy last unit as a new one

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
              mW->noteName->forceAccidental(answNote.alter);
          }
      } else if (curQ.questionAsName())
                mW->noteName->forceAccidental(answNote.alter);
  }
  if (curQ.answerAsFret())
      mW->guitar->setGuitarDisabled(false);
  if (curQ.answerAsSound() && !curQ.questionAsSound())
      startSniffing();
      // *** When question is sound it is played again (repeatSound()) 
      // and than startSniffing is called

  m_exam->addQuestion(curQ); // curQ is copied here - it becomes differ than this in exam list
  m_penalty->setBlackQuestion();
    
  if (!gl->E->autoNextQuest)
      mW->bar->startExamAct->setDisabled(true);
  mW->bar->setForQuestion(m_exam->curQ()->questionAsSound(), m_exam->curQ()->questionAsSound() && m_exam->curQ()->answerAsNote());
  if (m_exam->curQ()->questionAsSound())
      repeatSound();
  m_canvas->questionTip();
  m_penalty->startQuestionTime();
}


void TexamExecutor::displayCertificate() {
  m_snifferLocked = true;
	mW->sound->wait();
	m_penalty->pauseTime();
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
  mW->bar->actionsToExam();
  
  disableWidgets();
// connect all events to check an answer or display tip how to check
  connect(mW->score, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
  connect(mW->noteName, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
  connect(mW->guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
  if (m_level.instrument != e_noInstrument)
    connect(mW->sound, &Tsound::noteStarted, this, &TexamExecutor::expertAnswersSlot);
  else
    connect(mW->sound, &Tsound::noteFinished, this, &TexamExecutor::expertAnswersSlot);
  qApp->installEventFilter(m_supp);
  connect(m_supp, SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));

  disconnect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
  disconnect(mW->guitar, &TfingerBoard::guitarClicked, mW, &MainWindow::guitarWasClicked);
  disconnect(mW->sound, &Tsound::noteStarted, mW, &MainWindow::soundWasStarted);
  disconnect(mW->sound, &Tsound::noteFinished, mW, &MainWindow::soundWasFinished);
  disconnect(mW->bar->levelCreatorAct, SIGNAL(triggered()), mW, SLOT(openLevelCreator()));
  disconnect(mW->bar->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
  if (m_exercise) {
    connect(mW->bar->startExamAct, SIGNAL(triggered()), this, SLOT(stopExerciseSlot()));
    connect(m_exercise, SIGNAL(messageDisplayed()), this, SLOT(stopSound()));
    connect(m_exercise, SIGNAL(messageClosed(bool)), this, SLOT(suggestDialogClosed(bool)));
  } else
    connect(mW->bar->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
  connect(mW->bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));

  m_glStore->storeSettings();
  m_glStore->prepareGlobalsToExam(m_level);

  mW->pitchView->setVisible(gl->L->soundViewEnabled);
  mW->guitar->setVisible(gl->L->guitarEnabled);
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
    if (m_level.requireOctave)
      mW->sound->prepareToExam(m_level.loNote, m_level.hiNote);
    // when octave are not required do not change ambitus - it is already set to instrument scale
    mW->pitchView->setIntonationAccuracy(m_level.intonation);
    mW->pitchView->enableAccuracyChange(false);
  }
  TnotePixmap::setDefaultClef(m_level.clef);
  mW->updateSize(mW->centralWidget()->size());
  clearWidgets();
  if (gl->instrument != e_noInstrument && !m_supp->isCorrectedPlayable())
      mW->guitar->createRangeBox(m_supp->loFret(), m_supp->hiFret());
  m_soundTimer = new QTimer(this);
  connect(m_soundTimer, SIGNAL(timeout()), this, SLOT(startSniffing()));
  m_askingTimer = new QTimer(this);
  connect(m_askingTimer, SIGNAL(timeout()), this, SLOT(askQuestion()));

  if (!m_exercise) {
    if (mW->guitar->isVisible() && !m_level.canBeMelody())
        mW->innerWidget->moveExamToName();
  }
  m_snifferLocked = false;
  m_canvas = new Tcanvas(mW->innerWidget, m_exam, mW);
  connect(m_canvas, &Tcanvas::buttonClicked, this, &TexamExecutor::tipButtonSlot);
  m_canvas->startTip();
  if (m_exercise && !m_exam->melodies()) {
    if (m_level.answerIsNote())
      connect(mW->score, &TmainScore::correctingFinished, this, &TexamExecutor::correctionFinished);
    if (m_level.answerIsName())
      connect(mW->noteName, &TnoteName::correctingFinished, this, &TexamExecutor::correctionFinished);
    if (m_level.answerIsGuitar())
      connect(mW->guitar, &TfingerBoard::correctingFinished, this, &TexamExecutor::correctionFinished);
    if (m_level.answerIsSound()) {
      connect(mW->pitchView, &TpitchView::correctingFinished, this, &TexamExecutor::correctionFinished);
      connect(m_canvas, &Tcanvas::correctingFinished, this, &TexamExecutor::correctionFinished);
    }
  }
}


void TexamExecutor::restoreAfterExam() {
  mW->setWindowTitle(qApp->applicationName());
  mW->bar->removeAction(mW->bar->nextQuestAct);
  mW->score->isExamExecuting(false);

  m_glStore->restoreSettings();
  if (m_exercise) {
    gl->E->suggestExam = m_exercise->suggestInFuture();
  }
  
  TnotePixmap::setDefaultClef(gl->S->clef);
  mW->pitchView->setVisible(gl->L->soundViewEnabled);
  mW->guitar->setVisible(gl->L->guitarEnabled);
  mW->setSingleNoteMode(gl->S->isSingleNoteMode);
  mW->score->acceptSettings();
  mW->score->enableAccidToKeyAnim(true);
  mW->noteName->setEnabledEnharmNotes(false);
  mW->noteName->setEnabledDblAccid(gl->S->doubleAccidentalsEnabled);
  mW->guitar->acceptSettings();
  mW->noteName->setNoteNamesOnButt(gl->S->nameStyleInNoteName);
  mW->sound->acceptSettings();
  mW->pitchView->setIntonationAccuracy(gl->A->intonation);
  mW->pitchView->enableAccuracyChange(true);

  mW->noteName->setNameDisabled(false);
  mW->guitar->setGuitarDisabled(false);

  if (m_canvas)
      m_canvas->deleteLater();

  connect(mW->score, SIGNAL(noteChanged(int,Tnote)), mW, SLOT(noteWasClicked(int,Tnote)));
  connect(mW->guitar, &TfingerBoard::guitarClicked, mW, &MainWindow::guitarWasClicked);
  connect(mW->sound, &Tsound::noteStarted, mW, &MainWindow::soundWasStarted);
  connect(mW->sound, &Tsound::noteFinished, mW, &MainWindow::soundWasFinished);
  disconnect(mW->bar->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
  disconnect(mW->bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));
  connect(mW->bar->startExamAct, SIGNAL(triggered()), mW, SLOT(startExamSlot()));
  connect(mW->bar->levelCreatorAct, SIGNAL(triggered()), mW, SLOT(openLevelCreator()));
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
	connect(mW->bar->nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
	connect(mW->bar->prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));
	connect(mW->bar->checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));
	if (m_level.questionAs.isSound()) {
		mW->bar->createRepeatSoundAction();
		connect(mW->bar->repeatSndAct, SIGNAL(triggered()), this, SLOT(repeatSound()));
	}
	if (m_level.questionAs.isSound() && m_level.answersAs[TQAtype::e_asSound].isNote()) {
		mW->bar->createTuneForkAction();
		connect(mW->bar->tuneForkAct, SIGNAL(triggered()), this, SLOT(playMiddleA()));
	}
	if (m_exercise) {
		mW->bar->createCorrectAction();
		connect(mW->bar->correctAct, SIGNAL(triggered()), this, SLOT(correctAnswer()));
	}
	if (m_level.canBeMelody()) {
		mW->bar->createAttemptAction();
		connect(mW->bar->attemptAct, SIGNAL(triggered()), this, SLOT(newAttempt()));
	}
}


void TexamExecutor::exerciseToExam() {
	m_isAnswered = true;
	qApp->installEventFilter(m_supp);
	m_exam->saveToFile();
	QString userName = m_exam->userName();
	delete m_penalty;
	delete m_exam;
  delete mW->bar->correctAct;
	m_exam = new Texam(&m_level, userName);
  m_exam->setTune(*gl->Gtune());
	delete m_exercise;
	m_exercise = 0;
  m_canvas->changeExam(m_exam);
	setTitleAndTexts();
	m_canvas->levelStatusMessage();
	m_supp->setFinished(false); // exercise had it set to true
  m_supp->resetKeyRandom(); // new set of randomized key signatures when exam requires them
	initializeExecuting();
	disconnect(mW->bar->startExamAct, SIGNAL(triggered()), this, SLOT(stopExerciseSlot()));
	connect(mW->bar->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
	clearWidgets();
	m_canvas->clearCanvas();
	m_canvas->startTip();
  if (mW->guitar->isVisible() && !m_level.canBeMelody())
    mW->innerWidget->moveExamToName();
}


void TexamExecutor::stopExerciseSlot() {
	bool askAfter = m_askingTimer->isActive();
	m_askingTimer->stop(); // stop questioning, if any
	bool continuePractice = false;
	stopSound();
	if (m_exam->count()) {
		if (!m_isAnswered) {
			m_penalty->pauseTime();
      m_exam->skipLast(true);
		}
		if (m_isAnswered && m_exam->curQ()->melody() && m_exam->curQ()->answerAsNote() && !m_exam->curQ()->isCorrect()) 
      m_exam->curQ()->melody()->setTitle(m_exam->curQ()->melody()->title() + ";skip"); // user still can take new attempt to correct a melody, so hide it
		m_penalty->updateExamTimes();
		Tnote::EnameStyle tmpStyle = gl->S->nameStyleInNoteName;
		gl->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show charts in user defined style  
			
		bool startExam = false;
		if (!m_goingClosed)
				continuePractice = showExamSummary(mW, m_exam, true, &startExam);
    if (m_isAnswered && m_exam->curQ()->melody() && m_exam->curQ()->answerAsNote() && !m_exam->curQ()->isCorrect()) // revert melody title
      m_exam->curQ()->melody()->setTitle(m_exam->curQ()->melody()->title().remove(";skip"));
    if (m_isAnswered)
        m_exam->curQ()->setAnswered();
		gl->S->nameStyleInNoteName = tmpStyle;
		if (startExam) {
				exerciseToExam();
				return;
		}
		if (!m_isAnswered && continuePractice) {
      m_exam->skipLast(false);
      m_penalty->continueTime();
		}
	}
	if (continuePractice) {
		if (askAfter) // ask next question if questioning was stopped
			askQuestion();
		else // restore sniffing if necessary
			if (m_exam->curQ()->answerAsSound())
				startSniffing();
		qApp->installEventFilter(m_supp);
		return;
	} else {
		if ((m_exam->count() == 1 && m_exam->curQ()->answered()) || m_exam->count() > 1)
			m_exam->saveToFile();
	}
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
  if (!m_isAnswered)
    checkAnswer(false);
  m_penalty->stopTimeView();
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
        m_exam->setFileName(getExamFileName(m_exam) + ".noo");
      } else {
        m_exam->setFileName(saveExamToFile());
        if (m_exam->fileName() != "")
          gl->E->examsDir = QFileInfo(m_exam->fileName()).absoluteDir().absolutePath();
      }
    }
    if (m_exam->fileName() != "") {
      if (m_exam->melodies()) // summarize answer if not summarized yet (melodies can have such cases)
        m_penalty->setMelodyPenalties();
      m_penalty->updateExamTimes();
      gl->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show in user defined style  
      if (m_exam->saveToFile() == Texam::e_file_OK) {
          QStringList recentExams = gl->config->value("recentExams").toStringList();
          recentExams.removeAll(m_exam->fileName());
          recentExams.prepend(m_exam->fileName());
          gl->config->setValue("recentExams", recentExams);
      }
      if (!m_goingClosed) // if Nootka is closing don't show summary 
          showExamSummary(mW, m_exam, false);
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


void TexamExecutor::prepareToSettings() {
	stopSound();
}


void TexamExecutor::settingsAccepted() {
// set new colors in exam view - so far it is not allowed during exams
// 			examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor),
// 															Tcolor::bgTag(gl->EnotBadColor));
	if (m_exercise) {
		if (gl->E->suggestExam)
			m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
		else
			m_exercise->setSuggestionEnabled(0);
	}
	if (m_exam->count() && m_exam->curQ()->answerAsSound() && !mW->pitchView->isPaused())
		startSniffing();
	qApp->installEventFilter(m_supp);
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
		if (m_exam->curQ()->answerAsSound())
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
    msg->setWindowTitle(" ");
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
  QString fileName = QFileDialog::getSaveFileName(mW, tr("Save exam results as:"), getExamFileName(m_exam), TexTrans::examFilterTxt());
  if (fileName.isEmpty()) {
      QMessageBox *msg = new QMessageBox(mW);
      msg->setText(tr("If you don't save to file<br>you lost all results!"));
      QPushButton *saveButt = msg->addButton(tr("Save"), QMessageBox::ApplyRole);
      msg->addButton(tr("Discard"), QMessageBox::RejectRole);
      msg->exec();
      if (msg->clickedButton() == saveButt)
          fileName = saveExamToFile();
      delete msg;
  }
  if (!fileName.isEmpty() && fileName.right(4) != ".noo")
      fileName += ".noo";
  return fileName;
}


void TexamExecutor::repeatSound() {
  if (m_exam->curQ()->melody()) {
    mW->sound->playMelody(m_exam->curQ()->melody());
    if (mW->sound->melodyIsPlaying()) // the same methods stops a melody
      m_exam->curQ()->lastAttempt()->melodyWasPlayed(); // increase only when playing was started
  } else
    mW->sound->play(m_exam->curQ()->qa.note);
  connectPlayingFinished();
}


void TexamExecutor::playMiddleA() {
  Tnote a1(6, 1, 0);
  mW->sound->stopPlaying();
  mW->sound->play(a1);
  connectPlayingFinished();
}


void TexamExecutor::connectPlayingFinished() {
	if (m_soundTimer->isActive())
			m_soundTimer->stop();
	if (m_exam->curQ()->answerAsSound())
			connect(mW->sound, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
}


void TexamExecutor::noteOfMelodyStarted(const TnoteStruct& n) {
  if (m_melody->wasIndexChanged())
    m_exam->curQ()->lastAttempt()->melodyWasPlayed();
  m_melody->noteStarted();
	if (m_melody->currentIndex() == 0) // first played note was detected
		m_exam->curQ()->lastAttempt()->setPrepareTime(m_penalty->elapsedTime() - n.duration);
  if (m_melody->currentIndex() + 1 < m_exam->curQ()->melody()->length()) // highlight next note
    mW->score->selectNote(m_melody->currentIndex() + 1);
}


void TexamExecutor::noteOfMelodyFinished(const TnoteStruct& n) {
  m_melody->setNote(n);
  if (m_melody->currentIndex() == m_exam->curQ()->melody()->length() - 1) {
    if (gl->E->expertsAnswerEnable)
      checkAnswer();
    else {
      m_canvas->playMelodyAgainMessage();
      m_canvas->confirmTip(800);
      mW->sound->wait();
    }
  }
}


void TexamExecutor::noteOfMelodySelected(int nr) {
  m_melody->setCurrentIndex(nr);
  mW->score->selectNote(nr);
  mW->sound->go();
  m_canvas->clearConfirmTip();
  if (isExercise() && mW->guitar->isVisible() && m_exam->curQ()->melody()) // in exercises, display guitar position of clicked note for a hint
    mW->guitar->setFinger(m_exam->curQ()->melody()->note(nr)->g());
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
	if (!gl->E->expertsAnswerEnable && !m_exam->melodies()) { // no expert and no melodies
			m_canvas->confirmTip(1500);
      return;
	}
	// ignore slot when some dialog window appears or answer for melody
	if (m_snifferLocked || (m_exam->count() && m_exam->curQ()->melody())) 
			return;

	if (m_exam->curQ()->answerAsSound())
			mW->sound->pauseSinffing();
	QTimer::singleShot(0, this, SLOT(checkAnswer()));
	/** expertAnswersSlot() is invoked also by TaudioIN/TpitchFinder.
		* Calling checkAnswer() from here invokes stopping and deleting TaudioIN.
		* It finishes with crash. To avoid this checkAnswer() has to be called from outside - by timer event. */
}

/** This slot is invoked  during correction of melody on the score. 
 * Each note can be clicked and: 
 * - corrected if score is an answer
 * - shows position on the guitar
 * - plays its sound
 * - displays message with detected pitch if note was played wrong  */
void TexamExecutor::correctNoteOfMelody(int noteNr) {
	if (m_exam->curQ()->melody()) {
		mW->score->selectNote(noteNr);
		if (noteNr < m_exam->curQ()->lastAttempt()->mistakes.size()) {
			quint32 &m = m_exam->curQ()->lastAttempt()->mistakes[noteNr];
			if (m_exam->curQ()->answerAsNote() && m_exam->curQ()->melody()->length() > noteNr) { // only dictations can be corrected
				if (m && !m_melody->fixed(noteNr) && !mW->score->isCorrectAnimPending()) { // fix if it has not been fixed yet
          m_exercise->setCorrectedNoteId(noteNr);
					mW->score->correctNote(m_exam->curQ()->melody()->note(noteNr)->p(), m_supp->answerColor(m), noteNr);
					m_melody->setFixed(noteNr);
          if (m_melody->numberOfFixed() > m_exam->curQ()->melody()->length() / 2) { // to much fixed - block new attempt
            mW->bar->removeAction(mW->bar->attemptAct);
            m_canvas->whatNextTip(true); // it will cheat m_canvas that question is correct and 'new attempt' will be blocked as well
          }
				}
			}
			if (mW->sound->isPlayable() && m_exam->curQ()->melody()->length() > noteNr)
					mW->sound->play(m_exam->curQ()->melody()->note(noteNr)->p());
			if (mW->guitar->isVisible() && m_exam->curQ()->melody()->length() > noteNr)
				mW->guitar->setFinger(m_exam->curQ()->melody()->note(noteNr)->p());
			if (m && m_exam->curQ()->answerAsSound()) {
        if (m_melody->listened()[noteNr].pitch.isValid())
          m_canvas->detectedNoteTip(m_melody->listened()[noteNr].pitch);
        else 
          mW->setStatusMessage(m_canvas->detectedText(tr("This note was not played!")), 3000);
      }
		}
	}
}


void TexamExecutor::rightButtonSlot() {
  if (m_lockRightButt)
      return;
	if (m_isAnswered)
			askQuestion();
	else {
			checkAnswer();
	}
}


void TexamExecutor::tipButtonSlot(const QString& name) {
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
	else if (name == "newAttempt")
			newAttempt();
} 


void TexamExecutor::deleteExam() {
		if (m_exam) {
			delete m_exam;
			m_exam = 0;
		}
}


void TexamExecutor::delayerTip() {
	m_lockRightButt = false;
	m_canvas->whatNextTip(!(!m_exercise && gl->E->repeatIncorrect && !m_incorrectRepeated));
	/** When exam mode and mistake occurred it will be true,
	 * so whatNextTip(false) is invoked - whatNextTip displays repeat question hint
	 * otherwise (exercise and/or correct answer) @p whatNextTip(true) goes. */
}


void TexamExecutor::setTitleAndTexts() {
	if (m_exercise) {
			mW->setWindowTitle(tr("Exercises with Nootka"));
			mW->bar->startExamAct->setStatusTip(tr("finish exercising"));
		} else {
			mW->setWindowTitle(tr("EXAM!") + " " + m_exam->userName() + " - " + m_level.name);
			mW->bar->startExamAct->setStatusTip(tr("stop the exam"));
	}
}


void TexamExecutor::unlockAnswerCapturing() {
	if (m_exam->curQ()->answerAsSound())
		mW->sound->go();
	m_penalty->continueTime();
  qApp->installEventFilter(m_supp); // restore grabbing right mouse button
  m_snifferLocked = false;
}


void TexamExecutor::blindQuestion() {
	qDebug() << "Blind question - asking again";
	m_blindCounter++;
	QTimer::singleShot(10, this, SLOT(askQuestion()));
}


void TexamExecutor::correctionFinished() {
  if (sender() == mW->score) { // show name on score only when it is enabled and corrected
    if (gl->E->showNameOfAnswered && m_exercise->idOfCorrectedNote() > -1) {
      Tnote::EnameStyle tmpStyle = Tnote::defaultStyle; // store current name style
      Tnote::defaultStyle = m_exam->curQ()->styleOfQuestion(); // set style of question
      mW->score->noteFromId(m_exercise->idOfCorrectedNote())->showNoteName(QColor(gl->EanswerColor.lighter().name())); // show note name
      Tnote::defaultStyle = tmpStyle; // restore style
    }
  }
  mW->bar->nextQuestAct->setDisabled(false);
  if (gl->E->autoNextQuest && gl->E->afterMistake != TexamParams::e_stop && !m_exam->curQ()->melody()) {
    m_askingTimer->start(gl->E->correctPreview); // new question will be started after preview time
  }
  if (m_exam->curQ()->melody()) { // despite of 'auto' settings when melody - auto next question will not work
    m_canvas->whatNextTip(false, false);
    connect(mW->score, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody); // only once per answer
  } else if (!gl->E->autoNextQuest || gl->E->afterMistake == TexamParams::e_stop)
      m_canvas->whatNextTip(!(!m_exercise && gl->E->repeatIncorrect && !m_incorrectRepeated));
  if (m_exam->curQ()->melody() && (m_exam->curQ()->questionAsNote() || m_exam->curQ()->answerAsNote()))
      m_canvas->melodyCorrectMessage();
  if (!gl->E->autoNextQuest || !gl->E->showCorrected || gl->E->afterMistake == TexamParams::e_stop)
      QTimer::singleShot(4000, m_canvas, SLOT(clearResultTip())); // exam will stop so clear result tip after correction
  m_lockRightButt = false;
}



