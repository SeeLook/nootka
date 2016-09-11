/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include "tpenalty.h"
#include "texammelody.h"
#include "texamplugin.h"
#include <qtr.h>
#include <level/tlevelselector.h>
#include <tsound.h>
#include <exam/trandmelody.h>
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
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
#include <music/tmelody.h>
#include <tlayoutparams.h>
#include <graphics/tnotepixmap.h>
#include <gui/ttoolbar.h>
#include <gui/tmainview.h>
#if defined (Q_OS_ANDROID)
  #include <tfiledialog.h>
#else
  #include <level/tfixleveldialog.h>
  #include <gui/tstatuslabel.h>
#endif
#include <QtWidgets/QtWidgets>


#if defined (Q_OS_ANDROID)
  #define WAIT_TIME (500) //[ms]
#else
  #define WAIT_TIME (150) //[ms]
#endif
#define SOUND_DURATION (1500) //[ms]


#if !defined (Q_OS_ANDROID)
void debugStyle(TQAunit &qa) {
    qDebug("styles debugging");
    qDebug() << "Q:" << qa.styleOfQuestion() << "A:" << qa.styleOfAnswer();
}
#endif


        /** Returns a file name generated from user name and level,
        * but when such a file exists in current exam directory some time mark is added. */
QString getExamFileName(Texam* e) {
  QString fName = QDir::toNativeSeparators(Tcore::gl()->E->examsDir + QLatin1String("/") + e->userName() + QLatin1String("-") + e->level()->name);
  if (QFileInfo(fName  + QLatin1String(".noo")).exists())
    fName += QLatin1String("-")+ QDateTime::currentDateTime().toString(QLatin1String("(dd-MMM-hhmmss)"));
  return fName;
}


TexamExecutor::TexamExecutor(QObject* parent) :
  QObject(parent),
  m_exam(0),
  mW(MAINVIEW->mainWindow()),
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
}

void TexamExecutor::init(QString examFile, Tlevel *lev) {
	QString resultText;
	TstartExamDlg::Eactions userAct;

	SOUND->wait();
	if (lev) {
			m_level = *lev;
			if (Tcore::gl()->E->studentName.isEmpty()) {
					resultText = TstartExamDlg::systemUserName();
			} else
					resultText = Tcore::gl()->E->studentName;
			if (examFile == QLatin1String("exercise"))
				userAct = TstartExamDlg::e_runExercise;
			else
				userAct = TstartExamDlg::e_newExam;
	} else {
			if (examFile.isEmpty()) { // start exam dialog
					TstartExamDlg *startDlg = new TstartExamDlg(Tcore::gl()->E->studentName, Tcore::gl()->E, mW);
					userAct = startDlg->showDialog(resultText, m_level);
					delete startDlg;
			} else { // command line arg with given filename
					resultText = examFile;
					userAct = TstartExamDlg::e_contExam;
			}
	}
	m_glStore = new TglobalExamStore(Tcore::gl());
	m_glStore->tune = *Tcore::gl()->Gtune();
	m_glStore->fretsNumber = Tcore::gl()->GfretsNumber;
	m_glStore->instrument = Tcore::gl()->instrument;
	if (userAct == TstartExamDlg::e_newExam || userAct == TstartExamDlg::e_runExercise) {
			m_exam = new Texam(&m_level, resultText); // resultText is userName
#if !defined (Q_OS_ANDROID)
			if (!fixLevelInstrument(m_level, QString(), Tcore::gl()->instrumentToFix, mW)) {
            emit examMessage(Torders::e_examFailed);
						deleteExam();
						return;
				}
#endif
			Tcore::gl()->E->studentName = resultText; // store user name
			m_exam->setTune(*Tcore::gl()->Gtune());
			if (userAct == TstartExamDlg::e_runExercise)
					m_exercise = new Texercises(m_exam);
	} else if (userAct == TstartExamDlg::e_contExam) {
			m_exam = new Texam(&m_level, QString());
			Texam::EerrorType err = m_exam->loadFromFile(resultText);
			if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
				if (err == Texam::e_file_corrupted)
					QMessageBox::warning(mW, QString(),
						tr("<b>Exam file seems to be corrupted</b><br>Better start new exam on the same level"));
#if defined (Q_OS_ANDROID)
        if (!showExamSummary(mW, m_exam, true)) // there is no level fix under android - just display summary window
#else
				if (!fixLevelInstrument(m_level, m_exam->fileName(), Tcore::gl()->instrumentToFix, mW) ||
						!showExamSummary(mW, m_exam, true))
#endif
        {
						emit examMessage(Torders::e_examFailed);
						deleteExam();
						return;
				}
			} else {
					if (err == Texam::e_file_not_valid)
							QMessageBox::critical(mW, QString(), tr("File: %1 \n is not valid exam file!")
																.arg(resultText));
					emit examMessage(Torders::e_examFailed);
					deleteExam();
					return;
			}
	} else {
			if (userAct == TstartExamDlg::e_levelCreator) {
					emit examMessage(Torders::e_examAskCreator);
			}	else
					emit examMessage(Torders::e_examFailed);
			deleteExam();
			return;
	}
	//We check are guitar's params suitable for an exam
#if !defined (Q_OS_ANDROID)
	TexecutorSupply::checkGuitarParamsChanged(m_exam);
#endif
	// ---------- End of checking ----------------------------------

	if (m_exam->melodies())
		emit examMessage(Torders::e_examMultiple);
	else
		emit examMessage(Torders::e_examSingle);
	m_supp = new TexecutorSupply(&m_level, this);
	m_supp->createQuestionsList(m_questList);
  if (m_exam->melodies())
    m_melody = new TexamMelody(this);
	if (m_questList.isEmpty()) {
			QMessageBox::critical(mW, QString(), tr("Level <b>%1</b><br>makes no sense because there are no questions to ask.<br>It can be re-adjusted.<br>Repair it in Level Creator and try again.").arg(m_level.name));
			delete m_supp;
			emit examMessage(Torders::e_examFailed);
			deleteExam();
			return;
	}
	prepareToExam();
	if (Tcore::gl()->E->showHelpOnStart)
			showExamHelp();
	if (m_level.questionAs.isFret() && m_level.answersAs[TQAtype::e_asFretPos].isFret()) {
		if (!m_supp->isGuitarOnlyPossible()) {
				qDebug("Something stupid!\n Level has question and answer as position on guitar but any question is available.");
				emit examMessage(Torders::e_examFailed);
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
	m_penalty = new Tpenalty(m_exam, m_supp);
	connect(m_penalty, SIGNAL(certificate()), this, SLOT(displayCertificate()));
	if (m_exercise) {
    if (Tcore::gl()->E->suggestExam)
      m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
	} else {
			connect(m_canvas, SIGNAL(certificateMagicKeys()), this, SLOT(displayCertificate()));
      if (m_level.answerIsNote())
        SCORE->enableAccidToKeyAnim(false);
	}
	if (m_level.requireStyle) {
			m_prevQuestStyle = m_supp->randomNameStyle(Tcore::gl()->S->nameStyleInNoteName);
			m_prevAnswStyle = m_supp->randomNameStyle(m_prevQuestStyle);
	} else {
			m_prevQuestStyle = Tcore::gl()->S->nameStyleInNoteName;
			m_prevAnswStyle = Tcore::gl()->S->nameStyleInNoteName;
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
	if (m_exercise && !Tcore::gl()->E->showCorrected) // hide correct action button
      TOOLBAR->removeAction(TOOLBAR->correctAct);
	if (m_exam->count() && m_exercise && m_exam->melodies())
    disconnect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody);
	if (m_exam->count() && m_exam->melodies())
		TOOLBAR->removeAction(TOOLBAR->attemptAct);
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
				emit examMessage(Torders::e_examFailed);
				deleteExam();
				return;
		}
    if (!Tcore::gl()->E->autoNextQuest) {
			if (!m_exercise)
					TOOLBAR->startExamAct->setDisabled(true);
			m_canvas->clearCanvas();
    }
//     m_isAnswered = false;
    m_incorrectRepeated = false;
    m_answRequire.octave = m_level.requireOctave;
    m_answRequire.accid = m_level.forceAccids;
    m_answRequire.key = false;
		
		NOTENAME->setStyle(Tcore::gl()->S->nameStyleInNoteName);
    NOTENAME->setNoteNamesOnButt(Tcore::gl()->S->nameStyleInNoteName);
    
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
				SCORE->askQuestion(curQ->melody());
        if (m_level.showStrNr) { // we may be sure that instrument is kind of a guitar
          for (int i = 0; i < curQ->melody()->length(); ++i) {
            if (curQ->melody()->note(i)->g().str() > 1)
              SCORE->noteFromId(i)->setString(curQ->melody()->note(i)->g().str());
          }
          if (GUITAR->isVisible())
            GUITAR->prepareAnswer(); // It just shows range frame
        }
      }
			if (curQ->answerAsSound()) { // in fact, there is no other option yet
				connect(SOUND, &Tsound::noteStartedEntire, this, &TexamExecutor::noteOfMelodyStarted);
        connect(SOUND, &Tsound::noteFinishedEntire, this, &TexamExecutor::noteOfMelodyFinished);
        connect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::noteOfMelodySelected);
				SCORE->selectNote(0); // mark first note
        SCORE->setReadOnlyReacting(true); // allow user to select beginning note to play
			}
		} else {
			char strNr = 0;
			if ((curQ->answerAsFret() || curQ->answerAsSound())
					&& !m_level.onlyLowPos && m_level.showStrNr)
							strNr = curQ->qa.pos.str(); // do show string number or not
			if (m_level.useKeySign && !curQ->answerAsNote())
					// when answer is also asNote we determine key in preparing answer part
					SCORE->askQuestion(curQ->qa.note, curQ->key, strNr);
			else 
					SCORE->askQuestion(curQ->qa.note, strNr);
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
                curQ->setStyle(Tcore::gl()->S->nameStyleInNoteName, Tcore::gl()->S->nameStyleInNoteName);
                m_prevAnswStyle = Tcore::gl()->S->nameStyleInNoteName;
                m_prevQuestStyle = Tcore::gl()->S->nameStyleInNoteName;
            }
          } else // note name only in question
              if (m_level.requireStyle) { // switch previous used style
                curQ->setStyle(m_supp->randomNameStyle(m_prevQuestStyle), Tcore::gl()->S->nameStyleInNoteName);
								m_prevQuestStyle = curQ->styleOfQuestion();
//                 m_prevQuestStyle = m_supp->randomNameStyle(curQ->styleOfQuestion());
              } else {
                  curQ->setStyle(Tcore::gl()->S->nameStyleInNoteName, curQ->styleOfAnswer());
                  m_prevQuestStyle = Tcore::gl()->S->nameStyleInNoteName;
              }
        }
        // Show question on TnoteName widget
        if ((curQ->answerAsFret() || curQ->answerAsSound()) && m_level.showStrNr)
            NOTENAME->askQuestion(curQ->qa.note, curQ->styleOfQuestion(), curQ->qa.pos.str());
        else
            NOTENAME->askQuestion(curQ->qa.note, curQ->styleOfQuestion());
        if (curQ->answerAsSound())
            m_answRequire.accid = false; // reset checking accidentals determined by level
    }

    if (curQ->questionAsFret()) {
        GUITAR->askQuestion(curQ->qa.pos);
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
				SOUND->play(curQ->qa.note);
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
              SCORE->prepareKeyToAnswer(// we randomize some key to cover this expected one
                  (qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value(), curQ->key.getName());
              m_answRequire.key = true;
          } else {
              SCORE->setKeySignature(curQ->key);
          }
      }
      if (curQ->questionAsNote()) {// note has to be another than question
          if (m_penalty->isNot())
              curQ->qa_2.note = m_supp->forceEnharmAccid(curQ->qa.note); // curQ->qa_2.note is expected note
          if (!m_level.manualKey && curQ->qa_2.note == curQ->qa.note) {
              blindQuestion();
              return; // refresh this function scope by calling it outside
          }
          SCORE->forceAccidental((Tnote::Ealter)curQ->qa_2.note.alter);
          m_answRequire.accid = true;
          m_answRequire.octave = true;
      }
      if (curQ->questionAsFret() || curQ->questionAsSound()) {
          if (m_level.forceAccids) {
              SCORE->forceAccidental((Tnote::Ealter)curQ->qa.note.alter);
          }
      }
      if (curQ->questionAsName()) {
          m_answRequire.accid = true;
          m_answRequire.octave = true;
      }
    }
    SCORE->unLockScore();
    if (m_level.useKeySign && !m_level.manualKey) // case for either for single mode and melodies
      SCORE->lockKeySignature(true); // disables key signature enabled above - user cannot change it in this case (fixed key)
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
      NOTENAME->prepAnswer(curQ->styleOfAnswer());
      if (curQ->questionAsFret() || curQ->questionAsSound()) {
          if (m_level.forceAccids) {
              NOTENAME->forceAccidental(answNote.alter);
          }
      } else if (curQ->questionAsName())
                NOTENAME->forceAccidental(answNote.alter);
      NOTENAME->setStyle(curQ->styleOfAnswer());
  }

  if (curQ->answerAsFret()) {
//         GUITAR->setGuitarDisabled(false);
//         GUITAR->prepareAnswer();
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
            GUITAR->setHighlitedString(curQ->qa_2.pos.str());
        }
      } else 
        if (m_level.showStrNr)
          GUITAR->setHighlitedString(curQ->qa.pos.str());
      GUITAR->setGuitarDisabled(false);
      GUITAR->prepareAnswer();
  }
  
  if (curQ->answerAsSound()) {
    SOUND->prepareAnswer();
    if (curQ->questionAsSound()) {
        connect(SOUND, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying())); // sniffing after finished sound
    } else
        QTimer::singleShot(WAIT_TIME, this, SLOT(startSniffing()));
        // Give a student some time to prepare itself for next question in expert mode
        // It avoids capture previous played sound as current answer
  } else
      SOUND->wait(); // stop sniffing if answer is not a played sound

  TOOLBAR->setForQuestion(curQ->questionAsSound(), curQ->questionAsSound() && curQ->answerAsNote());
  m_penalty->startQuestionTime();
  m_canvas->questionTip();
  m_blindCounter = 0; // question successfully asked - reset the counter
}


void TexamExecutor::checkAnswer(bool showResults) {
	TQAunit* curQ = m_exam->curQ();
	m_penalty->stopQuestionTime();
	TOOLBAR->setAfterAnswer();
	if (curQ->answerAsSound()) {
			if (m_exam->melodies())
				SOUND->wait(); // flush buffers after captured melody
			else
				SOUND->pauseSinffing(); // but only skip detected for single sound
			SCORE->selectNote(-1);
			disconnect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying);
			disconnect(SOUND, &Tsound::noteStartedEntire, this, &TexamExecutor::noteOfMelodyStarted);
      disconnect(SOUND, &Tsound::noteFinishedEntire, this, &TexamExecutor::noteOfMelodyFinished);
      disconnect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::noteOfMelodySelected);
	}
	if (m_exam->melodies() && SOUND->melodyIsPlaying())
		SOUND->stopPlaying();
		
	if (!Tcore::gl()->E->autoNextQuest || m_exercise)
			TOOLBAR->startExamAct->setDisabled(false);
	m_isAnswered = true;
// Let's check
	Tnote questNote, answNote, userNote; // example note & returned note
// At first we determine what has to be checked
	if (!curQ->melody()) {
    questNote = curQ->qa.note;
    if (curQ->answerAsNote()) {
        if (m_level.manualKey) {
            if (SCORE->keySignature().value() != curQ->key.value())
                curQ->setMistake(TQAunit::e_wrongKey);
        }
        if (curQ->questionAsNote())
            questNote = curQ->qa_2.note;
        answNote = SCORE->getNote(0);
    }
    if (curQ->answerAsName()) {
        if (curQ->questionAsName())
            questNote = curQ->qa_2.note;
        m_prevNoteIfName = NOTENAME->getNoteName(); // store note to restore it if will be repeated
        answNote = NOTENAME->getNoteName();
    }
    userNote = answNote;
    if (curQ->answerAsSound()) {
				answNote = SOUND->note();
				if ((TintonationView::Eaccuracy)m_level.intonation != TintonationView::e_noCheck) {
						if (TnoteStruct(Tnote(), SOUND->pitch()).inTune(TintonationView::getThreshold(m_level.intonation)))
								curQ->setMistake(TQAunit::e_wrongIntonation);
				}
    }
	}
// Now we can check
	if (curQ->answerAsFret()) { // 1. Comparing positions
			TfingerPos answPos, questPos;
			answPos = GUITAR->getfingerPos();
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
						SCORE->getMelody(&answMelody);
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
            if (SCORE->keySignature().value() != curQ->key.value())
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
	bool autoNext = Tcore::gl()->E->autoNextQuest;
	if (Tcore::gl()->E->afterMistake == TexamParams::e_stop && !curQ->isCorrect())
			autoNext = false; // when mistake and e_stop - the same like autoNext = false;
		
	if (showResults) {
      m_canvas->resultTip(curQ); // tip duration is calculated by itself (inside resultTip() method)
			if ((!m_exercise || (m_exercise && curQ->isCorrect())) && !autoNext)
        m_canvas->whatNextTip(curQ->isCorrect());
      if (!autoNext) {
        if (!curQ->isCorrect() && !m_exercise && !curQ->melody())
            TOOLBAR->addAction(TOOLBAR->prevQuestAct);
        if (!curQ->isCorrect() && curQ->melody())
          TOOLBAR->addAction(TOOLBAR->attemptAct);
        TOOLBAR->addAction(TOOLBAR->nextQuestAct);
      }
	}

	markAnswer(curQ);
	int waitTime = Tcore::gl()->E->questionDelay;
	if (m_exercise) {
    if ((Tcore::gl()->E->autoNextQuest && Tcore::gl()->E->afterMistake != TexamParams::e_continue) || !Tcore::gl()->E->autoNextQuest || Tcore::gl()->E->showCorrected)
      waitTime = Tcore::gl()->E->correctPreview; // user has to have time to see his mistake and correct answer
    m_exercise->checkAnswer();
    if (!curQ->isCorrect()) { // correcting wrong answer
        if (Tcore::gl()->E->showCorrected) // TODO for dictation it should always stop and show mistakes
          correctAnswer();
        else {
          if (!Tcore::gl()->E->autoNextQuest || (Tcore::gl()->E->autoNextQuest && Tcore::gl()->E->afterMistake == TexamParams::e_stop))
              TOOLBAR->addAction(TOOLBAR->correctAct); // show too button only when exam stops after mistake
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
          m_askingTimer->start(Tcore::gl()->E->questionDelay);
      } else {
					if (Tcore::gl()->E->repeatIncorrect && !m_incorrectRepeated) {
						if (curQ->melody())
							QTimer::singleShot(waitTime, this, SLOT(newAttempt()));
						else {
							if (!m_exercise) // repeat only once if any
								QTimer::singleShot(waitTime, this, SLOT(repeatQuestion()));
							else
								m_askingTimer->start(waitTime);
						}
					} else {
							if (Tcore::gl()->E->afterMistake == TexamParams::e_wait && (!m_exercise || !Tcore::gl()->E->showCorrected))
									waitTime = Tcore::gl()->E->mistakePreview; // for exercises time was set above
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
	if (!Tcore::gl()->E->showCorrected)
    TOOLBAR->removeAction(TOOLBAR->correctAct);
  bool correctAnimStarted = false;
	if (m_askingTimer->isActive())
			m_askingTimer->stop();
	m_canvas->clearWhatNextTip();
	TQAunit* curQ = m_exam->answList()->last();
	QColor markColor = m_supp->answerColor(curQ);
	if (curQ->melody() && (curQ->answerAsNote() || curQ->questionAsNote())) {
		SCORE->setReadOnlyReacting(true); // It is undone whenever unLockScore() is called
	}
	if (curQ->answerAsNote()) {
		if (curQ->melody()) {
			
		} else {
			Tnote goodNote = curQ->qa.note;
			if (curQ->questionAsNote())
				goodNote = curQ->qa_2.note;
			if (curQ->wrongAccid() || curQ->wrongOctave()) // it corrects wrong octave as well
					SCORE->correctAccidental(goodNote);
			else if (curQ->wrongNote()) {
					if (m_level.manualKey && curQ->key.value() != SCORE->keySignature().value())
						SCORE->correctKeySignature(curQ->key);
          m_exercise->setCorrectedNoteId(0);
					SCORE->correctNote(goodNote, markColor);
			}
			if (curQ->wrongKey())
					SCORE->correctKeySignature(curQ->key);
      correctAnimStarted = true;
		}
	} else if (curQ->answerAsFret()) {
			TfingerPos goodPos = curQ->qa.pos;
			if (curQ->questionAsFret())
					goodPos = curQ->qa_2.pos;
			GUITAR->correctPosition(goodPos, markColor);
      correctAnimStarted = true;
	} else if (curQ->answerAsName()) {
			Tnote goodNote = curQ->qa.note;
			if (curQ->questionAsName())
					goodNote = curQ->qa_2.note;
			if (!m_answRequire.accid && curQ->isNotSoBad()) { // respect accidental selected by user 
				switch (NOTENAME->getNoteName().alter) {
					case -2 : goodNote = goodNote.showWithDoubleFlat(); break;
					case -1 : goodNote = goodNote.showWithFlat(); break;
					case  0 : goodNote = goodNote.showAsNatural(); break;
					case  1 : goodNote = goodNote.showWithSharp(); break;
					case  2 : goodNote = goodNote.showWithDoubleSharp(); break;
				}
			}
			NOTENAME->correctName(goodNote, markColor, curQ->isWrong());
      correctAnimStarted = true;
	} else { // answer as played sound
		if (curQ->melody()) {
			
		} else {
			if (curQ->wrongIntonation()) {
					float outTune = SOUND->pitch() - (float)qRound(SOUND->pitch());
					SOUND->pitchView()->outOfTuneAnim(outTune, 1200);
					m_canvas->outOfTuneTip(outTune); // we are sure that it is beyond the accuracy threshold
          correctAnimStarted = true;
			}
			if (m_supp->isCorrectedPlayable())
					repeatSound();
			else {
				if (GUITAR->isVisible()) {
				// Animation towards guitar when instrument is guitar and answer was wrong or octave was wrong
					if (curQ->questionAsFret())
						GUITAR->correctPosition(curQ->qa.pos, markColor);
					else
						m_canvas->correctToGuitar(curQ->questionAs, Tcore::gl()->E->mistakePreview, curQ->qa.pos);
          correctAnimStarted = true;
				}
			}
		}
	}
	if (correctAnimStarted) { // disable space bar and right mouse button when animation is performed
    TOOLBAR->nextQuestAct->setDisabled(true);
    m_lockRightButt = true;
  } else
    correctionFinished();
}


void TexamExecutor::newAttempt() {
	m_canvas->tryAgainTip(3000);
	QTimer::singleShot(2000, m_canvas, SLOT(clearResultTip())); 
	if (m_exam->curQ()->answerAsNote() || m_exam->curQ()->questionAsNote()) // remove names and marks from score notes
		for (int i = 0; i < SCORE->notesCount(); ++i) {
			if (m_exercise) {
					SCORE->deleteNoteName(i);
					if (m_exam->curQ()->lastAttempt()->mistakes[i] != TQAunit::e_correct)
						SCORE->markQuestion(-1, i); // leave note marks (colored borders) only for correct answers
			} else // although clear all marked notes in exams
					SCORE->markQuestion(-1, i);
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
			SCORE->markAnswered(m_supp->answerColor(curQ->lastAttempt()->mistakes[i]), i);
		}
	} else {
		switch (curQ->answerAs) {
			case TQAtype::e_asNote:
				SCORE->markAnswered(markColor);
				break;
			case TQAtype::e_asFretPos:
				GUITAR->markAnswer(markColor);
				break;
			case TQAtype::e_asName:
				NOTENAME->markNameLabel(markColor);
				break;
			case TQAtype::e_asSound:
				SOUND->pitchView()->markAnswer(markColor);
				break;
		}
		switch (curQ->questionAs) {
			case TQAtype::e_asNote:
				SCORE->markQuestion(markColor);
				break;
			case TQAtype::e_asFretPos:
				GUITAR->markQuestion(markColor);
				break;
			case TQAtype::e_asName:
				NOTENAME->markNameLabel(markColor);
				break;
			case TQAtype::e_asSound:
				break;
		}
	}                                                       // TODO
  if (m_exercise && Tcore::gl()->E->showNameOfAnswered /*&& (!Tcore::gl()->E->autoNextQuest || (Tcore::gl()->E->autoNextQuest && Tcore::gl()->E->afterMistake != TexamParams::e_continue))*/) {
		if (!curQ->questionAsName() && !curQ->answerAsName()) {
			if (curQ->answerAsNote() || (curQ->answerAsSound() && curQ->questionAsNote()))
				SCORE->showNames(Tcore::gl()->S->nameStyleInNoteName);
			else if (curQ->answerAsFret()) // for q/a fret-fret this will be the first case
        GUITAR->showName(Tcore::gl()->S->nameStyleInNoteName, markColor); // Take it from user answer
			else if (curQ->answerAsSound() && curQ->questionAsFret())
          GUITAR->showName(Tcore::gl()->S->nameStyleInNoteName, markColor);
		} else { // cases when name was an question
			if (curQ->questionAsName()) {
				if (curQ->answerAsNote())
					SCORE->showNames(curQ->styleOfQuestion());
				else if (curQ->answerAsFret())
					GUITAR->showName(curQ->styleOfQuestion(), markColor);
			}
		}
  }
}


void TexamExecutor::repeatQuestion() {
  m_canvas->tryAgainTip(3000);
  m_lockRightButt = false;
  m_incorrectRepeated = true;
  m_isAnswered = false;
  if (Tcore::gl()->E->showNameOfAnswered) {
    for (int i = 0; i < 2; i++)
      SCORE->deleteNoteName(i);
    GUITAR->deleteNoteName();
  }
// for melodies it never comes here - questions are newer repeated - copying of TQAunit is safe 
  TQAunit curQ(*m_exam->curQ()); // copy last unit as a new one

  if (!Tcore::gl()->E->autoNextQuest) {
      m_canvas->clearCanvas();
  }
  curQ.setMistake(TQAunit::e_correct);
  
  if (curQ.answerAsNote())
      SCORE->unLockScore();
  if (curQ.questionAsName()) { // refresh question on NoteName
    if (curQ.answerAsFret() && m_level.showStrNr)
          NOTENAME->askQuestion(curQ.qa.note, curQ.styleOfQuestion(), curQ.qa.pos.str());
      else
          NOTENAME->askQuestion(curQ.qa.note, curQ.styleOfQuestion());
  } else
    NOTENAME->clearNoteName();
  if (curQ.answerAsName()) {
    Tnote answNote = Tnote(0, 0, 0);
    NOTENAME->setNameDisabled(false);
    if (curQ.questionAsName())
          answNote = curQ.qa_2.note;
    else if (!curQ.answerAsNote())
            answNote = curQ.qa.note;
    NOTENAME->prepAnswer(curQ.styleOfAnswer());
    NOTENAME->setStyle(curQ.styleOfAnswer());
    if (curQ.questionAsFret() || curQ.questionAsSound()) {
          if (m_level.forceAccids) {
              NOTENAME->forceAccidental(answNote.alter);
          }
      } else if (curQ.questionAsName())
                NOTENAME->forceAccidental(answNote.alter);
  }
  if (curQ.answerAsFret())
      GUITAR->setGuitarDisabled(false);
  if (curQ.answerAsSound() && !curQ.questionAsSound())
      startSniffing();
      // *** When question is sound it is played again (repeatSound()) 
      // and than startSniffing is called

  m_exam->addQuestion(curQ); // curQ is copied here - it becomes differ than this in exam list
  m_penalty->setBlackQuestion();
    
  if (!Tcore::gl()->E->autoNextQuest)
      TOOLBAR->startExamAct->setDisabled(true);
  TOOLBAR->setForQuestion(m_exam->curQ()->questionAsSound(), m_exam->curQ()->questionAsSound() && m_exam->curQ()->answerAsNote());
  if (m_exam->curQ()->questionAsSound())
      repeatSound();
  m_canvas->questionTip();
  m_penalty->startQuestionTime();
}


void TexamExecutor::displayCertificate() {
  m_snifferLocked = true;
	SOUND->wait();
	m_penalty->pauseTime();
#if !defined (Q_OS_ANDROID)
  qApp->removeEventFilter(m_supp); // stop grabbing right button and calling checkAnswer()
#endif
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
  TOOLBAR->actionsToExam();

  disableWidgets();
// connect all events to check an answer or display tip how to check
  connect(SCORE, SIGNAL(noteClicked()), this, SLOT(expertAnswersSlot()));
  connect(NOTENAME, SIGNAL(noteButtonClicked()), this, SLOT(expertAnswersSlot()));
  connect(GUITAR, SIGNAL(guitarClicked(Tnote)), this, SLOT(expertAnswersSlot()));
  if (m_level.instrument != e_noInstrument)
    connect(SOUND, &Tsound::noteStarted, this, &TexamExecutor::expertAnswersSlot);
  else
    connect(SOUND, &Tsound::noteFinished, this, &TexamExecutor::expertAnswersSlot);
#if !defined (Q_OS_ANDROID)
  qApp->installEventFilter(m_supp);
#endif
  connect(m_supp, SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));

  emit examMessage(Torders::e_examDisconnect); // disconnect main window widgets
  if (m_exercise) {
    connect(TOOLBAR->startExamAct, SIGNAL(triggered()), this, SLOT(stopExerciseSlot()));
    connect(m_exercise, SIGNAL(messageDisplayed()), this, SLOT(stopSound()));
    connect(m_exercise, SIGNAL(messageClosed(bool)), this, SLOT(suggestDialogClosed(bool)));
  } else
    connect(TOOLBAR->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
  connect(TOOLBAR->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));

  m_glStore->storeSettings();
  m_glStore->prepareGlobalsToExam(m_level);

  if (Tcore::gl()->S->isSingleNoteMode)
    emit examMessage(Torders::e_examSingle);
  else
    emit examMessage(Torders::e_examMultiple);
#if defined (Q_OS_ANDROID) // remove/hide actions from main and score menus
  if (!Tcore::gl()->S->isSingleNoteMode) {
    TOOLBAR->playMelody()->setVisible(false);
    TOOLBAR->recordMelody()->setVisible(false);
    TOOLBAR->generateMelody()->setVisible(false);
  }
#endif
#if !defined (Q_OS_ANDROID) // Do not show it user Android - it sucks there
  SOUND->pitchView()->setVisible(Tcore::gl()->L->soundViewEnabled);
#endif
  GUITAR->setVisible(Tcore::gl()->L->guitarEnabled);
  SCORE->acceptSettings();
  NOTENAME->setEnabledEnharmNotes(false);
  NOTENAME->setEnabledDblAccid(m_level.withDblAcc);
  GUITAR->acceptSettings();
  SCORE->isExamExecuting(true);
  SCORE->enableAccidToKeyAnim(!Tcore::gl()->E->expertsAnswerEnable); // no key animation for experts (no time for it)
  if (m_level.canBeSound()) {
      SOUND->acceptSettings();
    if (SOUND->isSniffable())
        SOUND->wait();
    if (m_level.requireOctave)
      SOUND->prepareToExam(m_level.loNote, m_level.hiNote);
    // when octave are not required do not change ambitus - it is already set to instrument scale
    SOUND->pitchView()->setIntonationAccuracy(m_level.intonation);
    SOUND->pitchView()->enableAccuracyChange(false);
  }
  TnotePixmap::setDefaultClef(m_level.clef);
  emit examMessage(Torders::e_examResize);
  clearWidgets();
  if (Tcore::gl()->instrument != e_noInstrument && !m_supp->isCorrectedPlayable())
      GUITAR->createRangeBox(m_supp->loFret(), m_supp->hiFret());
  m_soundTimer = new QTimer(this);
  connect(m_soundTimer, SIGNAL(timeout()), this, SLOT(startSniffing()));
  m_askingTimer = new QTimer(this);
  connect(m_askingTimer, SIGNAL(timeout()), this, SLOT(askQuestion()));

  if (!m_exercise) {
    if (GUITAR->isVisible() && !m_level.canBeMelody())
        MAINVIEW->moveExamToName();
  }
  m_snifferLocked = false;
  m_canvas = new Tcanvas(MAINVIEW, m_exam);
  connect(m_canvas, &Tcanvas::buttonClicked, this, &TexamExecutor::tipButtonSlot);
  m_canvas->startTip();
  if (m_exercise && !m_exam->melodies()) {
    if (m_level.answerIsNote())
      connect(SCORE, &TmainScore::correctingFinished, this, &TexamExecutor::correctionFinished);
    if (m_level.answerIsName())
      connect(NOTENAME, &TnoteName::correctingFinished, this, &TexamExecutor::correctionFinished);
    if (m_level.answerIsGuitar())
      connect(GUITAR, &TfingerBoard::correctingFinished, this, &TexamExecutor::correctionFinished);
    if (m_level.answerIsSound()) {
      connect(SOUND->pitchView(), &TpitchView::correctingFinished, this, &TexamExecutor::correctionFinished);
      connect(m_canvas, &Tcanvas::correctingFinished, this, &TexamExecutor::correctionFinished);
    }
  }
}


void TexamExecutor::restoreAfterExam() {
  mW->setWindowTitle(qApp->applicationName());
  TOOLBAR->removeAction(TOOLBAR->nextQuestAct);
  SCORE->isExamExecuting(false);
#if !defined (Q_OS_ANDROID)
  if (!Tcore::gl()->A->dumpPath.isEmpty())
    SOUND->setDumpFileName(QLatin1String("nootka_dump"));
#endif

  m_glStore->restoreSettings();
  if (m_exercise) {
    Tcore::gl()->E->suggestExam = m_exercise->suggestInFuture();
  }

  TnotePixmap::setDefaultClef(Tcore::gl()->S->clef);
  SOUND->pitchView()->setVisible(Tcore::gl()->L->soundViewEnabled);
  GUITAR->setVisible(Tcore::gl()->L->guitarEnabled);
  if (Tcore::gl()->S->isSingleNoteMode)
    emit examMessage(Torders::e_examSingle);
  else
    emit examMessage(Torders::e_examMultiple);
#if defined (Q_OS_ANDROID) // revert actions
  if (!m_level.answerIsSound()) {
    SOUND->pitchView()->pauseAction()->setVisible(true);
    MAINVIEW->flyActions()->append(SOUND->pitchView()->pauseAction());
  }
#endif
  SCORE->acceptSettings();
  SCORE->enableAccidToKeyAnim(true);
  NOTENAME->setEnabledEnharmNotes(false);
  NOTENAME->setEnabledDblAccid(Tcore::gl()->S->doubleAccidentalsEnabled);
  GUITAR->acceptSettings();
  NOTENAME->setNoteNamesOnButt(Tcore::gl()->S->nameStyleInNoteName);
  SOUND->acceptSettings();
  SOUND->pitchView()->setIntonationAccuracy(Tcore::gl()->A->intonation);
  SOUND->pitchView()->enableAccuracyChange(true);

  NOTENAME->setNameDisabled(false);
  GUITAR->setGuitarDisabled(false);

  if (m_canvas)
      m_canvas->deleteLater();

  disconnect(TOOLBAR->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
  disconnect(TOOLBAR->levelCreatorAct, SIGNAL(triggered()), this, SLOT(showExamHelp()));
  emit examMessage(Torders::e_examConnect);
  SCORE->unLockScore();
  // unfortunately, unLockScore locks clef again
  SCORE->setClefDisabled(false);
  GUITAR->deleteRangeBox();
  SOUND->restoreAfterExam();
  emit examMessage(Torders::e_examFinished);
}


void TexamExecutor::disableWidgets() {
  NOTENAME->setNameDisabled(true);
  SCORE->setScoreDisabled(true);
  GUITAR->setGuitarDisabled(true);
}


void TexamExecutor::clearWidgets() {
  SCORE->clearScore();
  NOTENAME->clearNoteName();
  GUITAR->clearFingerBoard();
  SOUND->restoreAfterAnswer();
}


void TexamExecutor::createActions() {
#if defined (Q_OS_ANDROID)
  if (!m_level.answerIsSound()) {
    SOUND->pitchView()->pauseAction()->setVisible(false);
    MAINVIEW->flyActions()->removeOne(SOUND->pitchView()->pauseAction());
  }
#endif
	connect(TOOLBAR->nextQuestAct, SIGNAL(triggered()), this, SLOT(askQuestion()));
	connect(TOOLBAR->prevQuestAct, SIGNAL(triggered()), this, SLOT(repeatQuestion()));
	connect(TOOLBAR->checkAct, SIGNAL(triggered()), this, SLOT(checkAnswer()));
	if (m_level.questionAs.isSound()) {
		TOOLBAR->createRepeatSoundAction();
		connect(TOOLBAR->repeatSndAct, SIGNAL(triggered()), this, SLOT(repeatSound()));
	}
	if (m_level.questionAs.isSound() && m_level.answersAs[TQAtype::e_asSound].isNote()) {
		TOOLBAR->createTuneForkAction();
		connect(TOOLBAR->tuneForkAct, SIGNAL(triggered()), this, SLOT(playMiddleA()));
	}
	if (m_exercise) {
		TOOLBAR->createCorrectAction();
		connect(TOOLBAR->correctAct, SIGNAL(triggered()), this, SLOT(correctAnswer()));
	}
	if (m_level.canBeMelody()) {
		TOOLBAR->createAttemptAction();
		connect(TOOLBAR->attemptAct, SIGNAL(triggered()), this, SLOT(newAttempt()));
	}
}


void TexamExecutor::exerciseToExam() {
	m_isAnswered = true;
#if !defined (Q_OS_ANDROID)
	qApp->installEventFilter(m_supp);
#endif
	m_exam->saveToFile();
	QString userName = m_exam->userName();
	delete m_penalty;
	delete m_exam;
  delete TOOLBAR->correctAct;
	m_exam = new Texam(&m_level, userName);
  m_exam->setTune(*Tcore::gl()->Gtune());
	delete m_exercise;
	m_exercise = 0;
  m_canvas->changeExam(m_exam);
	setTitleAndTexts();
#if !defined (Q_OS_ANDROID) // TODO: Some hint under Android
	m_canvas->levelStatusMessage();
#endif
	m_supp->setFinished(false); // exercise had it set to true
  m_supp->resetKeyRandom(); // new set of randomized key signatures when exam requires them
	initializeExecuting();
	disconnect(TOOLBAR->startExamAct, SIGNAL(triggered()), this, SLOT(stopExerciseSlot()));
	connect(TOOLBAR->startExamAct, SIGNAL(triggered()), this, SLOT(stopExamSlot()));
	clearWidgets();
	m_canvas->clearCanvas();
	m_canvas->startTip();
  if (GUITAR->isVisible() && !m_level.canBeMelody())
    MAINVIEW->moveExamToName();
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
		Tnote::EnameStyle tmpStyle = Tcore::gl()->S->nameStyleInNoteName;
		Tcore::gl()->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show charts in user defined style
			
		bool startExam = false;
		if (!m_goingClosed)
				continuePractice = showExamSummary(mW, m_exam, true, &startExam);
    if (m_isAnswered && m_exam->curQ()->melody() && m_exam->curQ()->answerAsNote() && !m_exam->curQ()->isCorrect()) // revert melody title
      m_exam->curQ()->melody()->setTitle(m_exam->curQ()->melody()->title().remove(";skip"));
    if (m_isAnswered)
        m_exam->curQ()->setAnswered();
		Tcore::gl()->S->nameStyleInNoteName = tmpStyle;
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
#if !defined (Q_OS_ANDROID)
		qApp->installEventFilter(m_supp);
#endif
		return;
	} else {
		if ((m_exam->count() == 1 && m_exam->curQ()->answered()) || m_exam->count() > 1)
			m_exam->saveToFile();
	}
	closeExecutor();
}


void TexamExecutor::stopExamSlot() {
  if (!m_isAnswered && !Tcore::gl()->E->closeWithoutConfirm) {
    m_shouldBeTerminated = true;
  int messageDuration = 2000;
#if defined (Q_OS_ANDROID)
  messageDuration = 5000;
#else
    QColor c = Tcore::gl()->GfingerColor;
    c.setAlpha(30);
    STATUS->setBackground(c);
#endif
    m_canvas->setStatusMessage(tr("Give an answer first!<br>Then the exam will end."), messageDuration);
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
      if (Tcore::gl()->E->closeWithoutConfirm) {
        m_exam->setFileName(getExamFileName(m_exam) + ".noo");
      } else {
        m_exam->setFileName(saveExamToFile());
        if (m_exam->fileName() != "")
          Tcore::gl()->E->examsDir = QFileInfo(m_exam->fileName()).absoluteDir().absolutePath();
      }
    }
    if (m_exam->fileName() != "") {
      if (m_exam->melodies()) // summarize answer if not summarized yet (melodies can have such cases)
        m_penalty->setMelodyPenalties();
      m_penalty->updateExamTimes();
      Tcore::gl()->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show in user defined style
      if (m_exam->saveToFile() == Texam::e_file_OK) {
          QStringList recentExams = Tcore::gl()->config->value("recentExams").toStringList();
          recentExams.removeAll(m_exam->fileName());
          recentExams.prepend(m_exam->fileName());
          Tcore::gl()->config->setValue("recentExams", recentExams);
      }
      if (!m_goingClosed) // if Nootka is closing don't show summary
          showExamSummary(mW, m_exam, false);
    }
  }
  closeExecutor();
}


void TexamExecutor::closeExecutor() {
#if !defined (Q_OS_ANDROID)
	STATUS->setBackground(-1);
	STATUS->setMessage(QString());
#endif
  m_canvas->setStatusMessage(tr("Such a pity."), 5000);

	m_canvas->clearCanvas();
	clearWidgets();
	restoreAfterExam();
}


void TexamExecutor::prepareToSettings() {
	stopSound();
}


void TexamExecutor::settingsAccepted() {
// set new colors in exam view - so far it is not allowed during exams
// 			examResults->setStyleBg(Tcolor::bgTag(Tcore::gl()->EanswerColor), Tcolor::bgTag(Tcore::gl()->EquestionColor),
// 															Tcolor::bgTag(Tcore::gl()->EnotBadColor));
	if (m_exercise) {
		if (Tcore::gl()->E->suggestExam)
			m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
		else
			m_exercise->setSuggestionEnabled(0);
	}
	if (m_exam->count() && m_exam->curQ()->answerAsSound() && !SOUND->pitchView()->isPaused())
		startSniffing();
#if !defined (Q_OS_ANDROID)
	qApp->installEventFilter(m_supp);
#endif
}


void TexamExecutor::stopSound() {
	if (m_soundTimer->isActive())
			m_soundTimer->stop();
	SOUND->stopPlaying();
	SOUND->wait();
#if !defined (Q_OS_ANDROID)
	qApp->removeEventFilter(m_supp);
#endif
}


void TexamExecutor::suggestDialogClosed(bool startExam) {
	if (startExam) {
        exerciseToExam();
	} else {
#if !defined (Q_OS_ANDROID)
      qApp->installEventFilter(m_supp);
#endif
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
#if !defined (Q_OS_ANDROID)
    qApp->removeEventFilter(m_supp);
#endif
    auto msg = new QMessageBox(mW);
		msg->setText(tr("Psssst... Exam is going.<br><br>"
                    "Select <b>%1</b> to check, save and exit<br>"
                    "or <b>%2</b> to continue.<br>")
                .arg(qTR("QShortcut", "Save"))
                .arg(qTR("QPlatformTheme", "Retry")));
    msg->setStandardButtons(QMessageBox::Retry | QMessageBox::Save);
    msg->setWindowTitle(QStringLiteral("Psssst..."));
		if (!Tcore::gl()->E->closeWithoutConfirm)
				msg->exec();
    if (!Tcore::gl()->E->closeWithoutConfirm && msg->clickedButton() == msg->button(QMessageBox::Retry)) {
        m_snifferLocked = false;
#if !defined (Q_OS_ANDROID)
        qApp->installEventFilter(m_supp);
#endif
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
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getSaveFileName(mW, getExamFileName(m_exam), QStringLiteral("noo"));
#else
  QString fileName = QFileDialog::getSaveFileName(mW, tr("Save exam results as:"), getExamFileName(m_exam), TexTrans::examFilterTxt());
#endif
  QLatin1String noo(".noo");
  if (fileName.isEmpty()) {
      auto msg = new QMessageBox(mW);
      msg->setText(tr("If you don't save to file<br>you lost all results!"));
      msg->setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
      if (msg->exec() == QMessageBox::Save)
          fileName = saveExamToFile();
      delete msg;
  }
  if (!fileName.isEmpty() && fileName.right(4) != noo)
      fileName += noo;
  return fileName;
}


void TexamExecutor::repeatSound() {
  if (m_exam->curQ()->melody()) {
    SOUND->playMelody(m_exam->curQ()->melody());
    if (SOUND->melodyIsPlaying()) // the same methods stops a melody
      m_exam->curQ()->lastAttempt()->melodyWasPlayed(); // increase only when playing was started
  } else
    SOUND->play(m_exam->curQ()->qa.note);
  connectPlayingFinished();
}


void TexamExecutor::playMiddleA() {
  Tnote a1(6, 1, 0);
  SOUND->stopPlaying();
  SOUND->play(a1);
  connectPlayingFinished();
}


void TexamExecutor::connectPlayingFinished() {
	if (m_soundTimer->isActive())
			m_soundTimer->stop();
	if (m_exam->curQ()->answerAsSound())
			connect(SOUND, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
}


void TexamExecutor::noteOfMelodyStarted(const TnoteStruct& n) {
  if (m_melody->wasIndexChanged())
    m_exam->curQ()->lastAttempt()->melodyWasPlayed();
  m_melody->noteStarted();
	if (m_melody->currentIndex() == 0) // first played note was detected
    m_exam->curQ()->lastAttempt()->setPrepareTime(m_penalty->elapsedTime() - quint32(n.duration));
  if (m_melody->currentIndex() + 1 < m_exam->curQ()->melody()->length()) // highlight next note
    SCORE->selectNote(m_melody->currentIndex() + 1);
}


void TexamExecutor::noteOfMelodyFinished(const TnoteStruct& n) {
  m_melody->setNote(n);
  if (m_melody->currentIndex() == m_exam->curQ()->melody()->length() - 1) {
    if (Tcore::gl()->E->expertsAnswerEnable)
      checkAnswer();
    else {
      m_canvas->playMelodyAgainMessage();
      m_canvas->confirmTip(800);
      SOUND->wait();
    }
  }
}


void TexamExecutor::noteOfMelodySelected(int nr) {
  m_melody->setCurrentIndex(nr);
  SCORE->selectNote(nr);
  SOUND->go();
  m_canvas->clearConfirmTip();
  if (isExercise() && GUITAR->isVisible() && m_exam->curQ()->melody()) // in exercises, display guitar position of clicked note for a hint
      GUITAR->setFinger(m_exam->curQ()->melody()->note(nr)->g());
}


void TexamExecutor::showExamHelp() {
  m_snifferLocked = true;
#if !defined (Q_OS_ANDROID)
  qApp->removeEventFilter(m_supp);
#endif
  TexamHelp *hlp = new TexamHelp(Tcolor::bgTag(Tcore::gl()->EquestionColor), Tcolor::bgTag(Tcore::gl()->EanswerColor),
																 &Tcore::gl()->E->showHelpOnStart, mW);
  hlp->exec();
  delete hlp;
#if !defined (Q_OS_ANDROID)
  qApp->installEventFilter(m_supp);
#endif
  m_snifferLocked = false;
}


void TexamExecutor::sniffAfterPlaying() {
  disconnect(SOUND, SIGNAL(plaingFinished()), this, SLOT(sniffAfterPlaying()));
  if (m_soundTimer->isActive())
    m_soundTimer->stop();
  m_soundTimer->start(100);
}


void TexamExecutor::startSniffing() {
  if (m_soundTimer->isActive())
    m_soundTimer->stop();
#if !defined (Q_OS_ANDROID)
      if (m_exam->curQ()->answerAsSound() && !Tcore::gl()->A->dumpPath.isEmpty()) {
        QString dumpFileName = QString("Question-%1").arg(m_exam->count(), 3, 'i', 0, '0');
        if (m_melody)
          dumpFileName += QString("-attempt%1").arg(m_exam->curQ()->attemptsCount());
        SOUND->setDumpFileName(dumpFileName);
      }
#endif
  if (SOUND->isSnifferPaused())
    SOUND->unPauseSniffing();
	else
    SOUND->go();
}


void TexamExecutor::expertAnswersSlot() {
	if (!Tcore::gl()->E->expertsAnswerEnable && !m_exam->melodies()) { // no expert and no melodies
			m_canvas->confirmTip(1500);
      return;
	}
	// ignore slot when some dialog window appears or answer for melody
	if (m_snifferLocked || (m_exam->count() && m_exam->curQ()->melody())) 
			return;

	if (m_exam->curQ()->answerAsSound())
			SOUND->pauseSinffing();
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
		SCORE->selectNote(noteNr);
		if (noteNr < m_exam->curQ()->lastAttempt()->mistakes.size()) {
			quint32 &m = m_exam->curQ()->lastAttempt()->mistakes[noteNr];
			if (m_exam->curQ()->answerAsNote() && m_exam->curQ()->melody()->length() > noteNr) { // only dictations can be corrected
				if (m && !m_melody->fixed(noteNr) && !SCORE->isCorrectAnimPending()) { // fix if it has not been fixed yet
          m_exercise->setCorrectedNoteId(noteNr);
					SCORE->correctNote(m_exam->curQ()->melody()->note(noteNr)->p(), m_supp->answerColor(m), noteNr);
					m_melody->setFixed(noteNr);
          if (m_melody->numberOfFixed() > m_exam->curQ()->melody()->length() / 2) { // to much fixed - block new attempt
            TOOLBAR->removeAction(TOOLBAR->attemptAct);
            m_canvas->whatNextTip(true); // it will cheat m_canvas that question is correct and 'new attempt' will be blocked as well
          }
				}
			}
			if (SOUND->isPlayable() && m_exam->curQ()->melody()->length() > noteNr)
					SOUND->play(m_exam->curQ()->melody()->note(noteNr)->p());
			if (GUITAR->isVisible() && m_exam->curQ()->melody()->length() > noteNr)
				GUITAR->setFinger(m_exam->curQ()->melody()->note(noteNr)->p());
			if (m && m_exam->curQ()->answerAsSound()) {
        if (m_melody->listened()[noteNr].pitch.isValid())
          m_canvas->detectedNoteTip(m_melody->listened()[noteNr].pitch);
        else
          m_canvas->setStatusMessage(m_canvas->detectedText(tr("This note was not played!")), 3000);
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
	if (name == QLatin1String("nextQuest"))
			askQuestion();
	else if (name == QLatin1String("stopExam")) {
		if (m_exercise)
			stopExerciseSlot();
		else
			stopExamSlot();
	}
	else if (name == QLatin1String("prevQuest"))
			repeatQuestion();
	else if (name == QLatin1String("checkAnswer"))
			checkAnswer();
	else if (name == QLatin1String("examHelp"))
			showExamHelp();
	else if (name == QLatin1String("correct"))
			correctAnswer();
	else if (name == QLatin1String("certClosing"))
			unlockAnswerCapturing();
	else if (name == QLatin1String("newAttempt"))
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
	m_canvas->whatNextTip(!(!m_exercise && Tcore::gl()->E->repeatIncorrect && !m_incorrectRepeated));
	/** When exam mode and mistake occurred it will be true,
	 * so whatNextTip(false) is invoked - whatNextTip displays repeat question hint
	 * otherwise (exercise and/or correct answer) @p whatNextTip(true) goes. */
}


void TexamExecutor::setTitleAndTexts() {
#if !defined (Q_OS_ANDROID)
	if (m_exercise) {
			mW->setWindowTitle(tr("Exercises with Nootka"));
			TOOLBAR->startExamAct->setStatusTip(tr("finish exercising"));
		} else {
			mW->setWindowTitle(tr("EXAM!") + " " + m_exam->userName() + " - " + m_level.name);
			TOOLBAR->startExamAct->setStatusTip(tr("stop the exam"));
	}
#endif
}


void TexamExecutor::unlockAnswerCapturing() {
	if (m_exam->curQ()->answerAsSound())
		SOUND->go();
	m_penalty->continueTime();
#if !defined (Q_OS_ANDROID)
  qApp->installEventFilter(m_supp); // restore grabbing right mouse button
#endif
  m_snifferLocked = false;
}


void TexamExecutor::blindQuestion() {
	qDebug() << "Blind question - asking again";
	m_blindCounter++;
	QTimer::singleShot(10, this, SLOT(askQuestion()));
}


void TexamExecutor::correctionFinished() {
  if (sender() == SCORE) { // show name on score only when it is enabled and corrected
    if (Tcore::gl()->E->showNameOfAnswered && m_exercise->idOfCorrectedNote() > -1) {
      Tnote::EnameStyle tmpStyle = Tnote::defaultStyle; // store current name style
      Tnote::defaultStyle = m_exam->curQ()->styleOfQuestion(); // set style of question
      SCORE->noteFromId(m_exercise->idOfCorrectedNote())->showNoteName(QColor(Tcore::gl()->EanswerColor.lighter().name())); // show note name
      Tnote::defaultStyle = tmpStyle; // restore style
    }
  }
  TOOLBAR->nextQuestAct->setDisabled(false);
  if (Tcore::gl()->E->autoNextQuest && Tcore::gl()->E->afterMistake != TexamParams::e_stop && !m_exam->curQ()->melody()) {
    m_askingTimer->start(Tcore::gl()->E->correctPreview); // new question will be started after preview time
  }
  if (m_exam->curQ()->melody()) { // despite of 'auto' settings when melody - auto next question will not work
    m_canvas->whatNextTip(false, false);
    connect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody); // only once per answer
  } else if (!Tcore::gl()->E->autoNextQuest || Tcore::gl()->E->afterMistake == TexamParams::e_stop)
      m_canvas->whatNextTip(!(!m_exercise && Tcore::gl()->E->repeatIncorrect && !m_incorrectRepeated));
  if (m_exam->curQ()->melody() && (m_exam->curQ()->questionAsNote() || m_exam->curQ()->answerAsNote()))
      m_canvas->melodyCorrectMessage();
  if (!Tcore::gl()->E->autoNextQuest || !Tcore::gl()->E->showCorrected || Tcore::gl()->E->afterMistake == TexamParams::e_stop)
      QTimer::singleShot(4000, m_canvas, SLOT(clearResultTip())); // exam will stop so clear result tip after correction
  m_lockRightButt = false;
}



