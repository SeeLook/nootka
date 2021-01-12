/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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
#include "tglobalexamstore.h"
#include "texercises.h"
#include "tequalrand.h"
#include "tpenalty.h"
#include "texammelody.h"
#include "trandmelody.h"
#include "ttiphandler.h"
#include <qtr.h>
#include <tcolor.h>
#include <tsound.h>
#include <tglobals.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include "help/texamhelp.h"
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
// #include <tlayoutparams.h>
#include <music/tmelody.h>
#include <taction.h>
#include <tnootkaqml.h>
#include <instruments/tcommoninstrument.h>
#include "tnameitem.h"
#include "tmainscoreobject.h"
#include <Android/tfiledialog.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qtimer.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qsettings.h>
#include <QtQml/qqmlengine.h>

#include <QtCore/qdebug.h>


#if defined (Q_OS_ANDROID)
  #define WAIT_TIME (500) //[ms]
#else
  #define WAIT_TIME (150) //[ms]
#endif
#define SOUND_DURATION (1500) //[ms]

#define INSTRUMENT NOO->instrument()
#define CURR_Q (m_exam->curQ())


#if !defined (Q_OS_ANDROID)
void debugStyle(TQAunit &qa) {
  qDebug("styles debugging");
  qDebug() << "Q:" << qa.styleOfQuestion() << "A:" << qa.styleOfAnswer();
}
#endif


/**
 * Returns a file name generated from user name and level,
 * but when such a file exists in current exam directory some time mark is added.
 */
QString getExamFileName(Texam* e) {
  QString fName = QDir::toNativeSeparators(GLOB->E->examsDir + QLatin1String("/") + e->userName() + QLatin1String("-") + e->level()->name);
  if (QFileInfo::exists(fName  + QLatin1String(".noo")))
    fName += QLatin1String("-") + QDateTime::currentDateTime().toString(QLatin1String("(dd-MMM-hhmmss)"));
  return fName;
}


TexamExecutor* TexamExecutor::m_instance = nullptr;


TexamExecutor::TexamExecutor(QQuickItem* parent) :
  QQuickItem(parent),
  m_exam(nullptr),
  m_snifferLocked(false),
  m_lockRightButt(false),
  m_goingClosed(false),
  m_penalty(nullptr),
  m_exercise(nullptr),
  m_blindCounter(0),
  m_rand(nullptr)
{
  if (m_instance) {
    qDebug() << "TexamExecutor instance already exists!";
    return;
  }
  m_instance = this;
}


bool TexamExecutor::init(TexamExecutor::EexecOrigin whatToDo, const QVariant& arg) {
  if (whatToDo == NewExam || whatToDo == StartExercise) {
    if (!castLevelFromQVariant(arg))
      return false;
  }

  if (whatToDo == NewExam || whatToDo == StartExercise) {
      m_exam = new Texam(&m_level, GLOB->E->studentName);
      m_exam->setTune(*GLOB->Gtune());
      if (whatToDo == StartExercise)
        m_exercise = new Texercises(m_exam);
  } else if (whatToDo == ContinueExam) {
      m_exam = new Texam(&m_level, QString());
      Texam::EerrorType err = m_exam->loadFromFile(arg.toString());
      if (err == Texam::e_file_OK || err == Texam::e_file_corrupted) {
          if (err == Texam::e_file_corrupted)
            QMessageBox::warning(nullptr, QString(), tr("<b>Exam file seems to be corrupted</b><br>Better start new exam on the same level"));
          m_summaryReason = SumContExam;
          QTimer::singleShot(50, [=]{ emit examSummary(); }); // Allow 'Start Exam dialog' to be closed
          return true;
      } else {
          if (err == Texam::e_file_not_valid) {
            QMessageBox::critical(nullptr, QString(), tr("File: %1 \n is not valid exam file!").arg(arg.toString()));
            return false;
          }
      }
  }
  return continueInit();
}


TexamExecutor::~TexamExecutor() {
  m_examActions.clear();
  emit examActionsChanged();
  if (m_penalty)
    delete m_penalty;
  if (m_supp)
    delete m_supp;
  if (m_glStore)
    delete m_glStore;
  if (m_rand)
    delete m_rand;
  deleteExam();
  m_instance = nullptr;
//   qDebug() << "[TexamExecutor] destroyed";
}


bool TexamExecutor::continueInit() {
  m_summaryReason = NoReason;
  if (GLOB->E->studentName.isEmpty())
    GLOB->E->studentName = GLOB->systemUserName();
  m_glStore = new TglobalExamStore(GLOB);
  m_glStore->tune = *GLOB->Gtune();
  m_glStore->fretsNumber = GLOB->GfretsNumber;
  m_glStore->instrument = GLOB->instrument().type();
  m_glStore->isSingleNoteMode = GLOB->isSingleNote();
  //We check are guitar's params suitable for an exam
#if !defined (Q_OS_ANDROID)
  TexecutorSupply::checkGuitarParamsChanged(m_exam);
#endif
  GLOB->setSingleNote(!m_exam->melodies());
  m_supp = new TexecutorSupply(&m_level, this);
  // TODO: when level has its own list of notes for melodies - it is wasting energy!
  m_supp->createQuestionsList(m_questList);

  if (m_exam->melodies()) {
    m_melody = new TexamMelody(this);
    if (m_level.randMelody == Tlevel::e_melodyFromSet && !m_level.randOrderInSet) {
      // Determine id of melody in the set and initialize appropriate variables
      auto lastQ = m_exam->count() ? CURR_Q : nullptr;
      if (lastQ) { // exam is continued, so find latest melody id and how many times it was asked already
        int q = m_exam->count() - 1;
        int repeatNr = 1;
        while (q >= 0 && lastQ->idOfMelody() == m_exam->question(q)->idOfMelody()) {
          q--;
          repeatNr++;
        }
        m_melody->setLastMelodyId(lastQ->idOfMelody() + (repeatNr >= m_level.repeatNrInSet ? 1 : 0));
        if (m_melody->lastMelodyId() >= m_level.melodySet.count()) {
          qDebug() << "[TexamExecutor] FIXME! Melody ID in the set is greater than melodies number there!";
          m_melody->setLastMelodyId(m_level.melodySet.count() - 1);
        }
        m_melody->setRepeat(repeatNr);
      }
      // for newly started exam, above variables are initialized by default
    }
  }
  if (m_questList.isEmpty() && !m_level.isMelodySet()) {
    QMessageBox::critical(nullptr, QString(), tr("Level <b>%1</b><br>makes no sense because there are no questions to ask.<br>It can be re-adjusted.<br>Repair it in Level Creator and try again.").arg(m_level.name));
    return false;
  }
  if (m_level.questionAs.isOnInstr() && m_level.answersAs[TQAtype::e_onInstr].isOnInstr()) {
    if (!m_supp->isGuitarOnlyPossible()) {
      qDebug("Something stupid!\n Level has question and answer as position on guitar but any question is available.");
      return false;
    }
  }
  if (GLOB->E->showHelpOnStart)
    showExamHelp();
  prepareToExam();
  initializeExecuting();
  createActions();
  return true;
}


void TexamExecutor::initializeExecuting() {
  m_shouldBeTerminated = false;
  m_incorrectRepeated = false;
  m_isAnswered = true;
  m_penalty = new Tpenalty(m_exam, m_supp);
  connect(m_penalty, &Tpenalty::certificate, this, &TexamExecutor::displayCertificate, Qt::UniqueConnection);
  if (m_exercise) {
    if (GLOB->E->suggestExam)
      m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
  } else {
      connect(m_tipHandler, &TtipHandler::certificateMagicKeys, this, &TexamExecutor::displayCertificate, Qt::UniqueConnection);
//       if (m_level.answerIsNote())
//         SCORE->enableAccidToKeyAnim(false);
  }
  if (m_level.requireStyle) {
      m_prevQuestStyle = m_supp->randomNameStyle(GLOB->S->nameStyleInNoteName);
      m_prevAnswStyle = m_supp->randomNameStyle(m_prevQuestStyle);
  } else {
      m_prevQuestStyle = GLOB->S->nameStyleInNoteName;
      m_prevAnswStyle = GLOB->S->nameStyleInNoteName;
  }

  m_level.questionAs.randNext(); // Randomize question and answer type
  if (m_level.questionAs.isOnScore()) m_level.answersAs[TQAtype::e_onScore].randNext();
  if (m_level.questionAs.isName()) m_level.answersAs[TQAtype::e_asName].randNext();
  if (m_level.questionAs.isOnInstr()) m_level.answersAs[TQAtype::e_onInstr].randNext();
  if (m_level.questionAs.isSound()) m_level.answersAs[TQAtype::e_asSound].randNext();
  if (m_rand)
    m_rand->reset();
  else
    m_rand = new TequalRand(m_level.isMelodySet() ? m_level.melodySet.count() : m_questList.size());
  m_rand->setTotalRandoms(m_supp->obligQuestions() - m_exam->count());

//   qDebug() << "Questions nr: " << m_questList.size() << "Randoms:" << m_supp->obligQuestions() - m_exam->count();
}


void TexamExecutor::askQuestion(bool isAttempt) {
  m_askingTimer->stop();
//   if (m_tipHandler->hasCertificate()) // in auto mode new question can be asked "under" certificate
//     return;

//   m_lockRightButt = false; // release mouse button events
  if (m_exercise && !GLOB->E->showCorrected && m_correctAct) // hide correct action button
    m_correctAct->setEnabled(false);
//   if (m_exam->count() && m_exercise && m_exam->melodies())
//     disconnect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody);
  if (m_exam->count() && m_exam->melodies())
    m_newAtemptAct->setEnabled(false);
  if (!isAttempt) { // add new question to the list
    m_penalty->setMelodyPenalties();
    if (m_exam->count() && m_exercise) // Check answer only after summarize
      m_exercise->checkAnswer();
    TQAunit Q(m_exam);
    m_exam->addQuestion(Q);
  }
  auto curQ = CURR_Q;
  m_isAnswered = false;
  if (!isAttempt) {
    clearWidgets();
    if (m_blindCounter > 20) {
        QMessageBox::critical(nullptr, QStringLiteral("Level error!"), QString(
          "Nootka attempted to create proper question-answer pair 20 times<br>"
          " Send this message and a level file to developers and we will try to fix it in further releases."));
        deleteExam();
        return;
    }
    if (!GLOB->E->autoNextQuest) {
      if (!m_exercise)
        m_stopExamAct->setEnabled(false);
      m_tipHandler->clearTips();
    }
    m_incorrectRepeated = false;
    m_answRequire.octave = m_level.requireOctave;
    m_answRequire.accid = m_level.forceAccids;
    m_answRequire.key = false;

    if (NOTENAME) {
      NOTENAME->setNameStyle(GLOB->S->nameStyleInNoteName);
      NOTENAME->setButtonNameStyle(GLOB->S->nameStyleInNoteName);
    }

    m_penalty->nextQuestion();
    if (!m_exercise && m_penalty->ask()) {

    } else {
        if (!m_exam->melodies()) // leave them empty for melody - there are all notes and positions
          curQ->qa = m_questList[m_rand->get()];
        curQ->questionAs = m_level.questionAs.next();
        curQ->answerAs = m_level.answersAs[curQ->questionAs].next();
    }

    if (m_penalty->isNot() && curQ->questionOnInstr() && curQ->answerOnInstr())
      curQ->qa = m_questList[m_supp->getQAnrForGuitarOnly()];

    if (m_penalty->isNot() && (curQ->questionOnScore() || curQ->answerOnScore())) {
      if (m_level.useKeySign)
        curQ->key = m_supp->getKey(curQ->qa.note);
      if (!m_level.onlyCurrKey) // if key doesn't determine accidentals, we do this
        curQ->qa.note = m_supp->determineAccid(curQ->qa.note);
    }
    if (m_exam->melodies()) {
      disconnect(MAIN_SCORE, &TmainScoreObject::readOnlyNoteClicked, this, &TexamExecutor::correctNoteOfMelody);
      int melodyLength = qBound(qMax(2, qRound(m_level.melodyLen * 0.7)), //at least 70% of length but not less than 2
                                      qRound(((6.0 + (qrand() % 5)) / 10.0) * (qreal)m_level.melodyLen), (int)m_level.melodyLen);
//       qDebug() << "========== Asking melody" << m_exam->count();
      if (m_penalty->isNot()) {
          TrhythmList rhythms;
          if (!m_level.isMelodySet()) {
            curQ->addMelody(QString("%1").arg(m_exam->count()));
            curQ->melody()->setKey(curQ->key);
            if (m_level.useRhythms()) {
              curQ->melody()->setMeter(m_supp->randomMeter());
              int barsNr = m_supp->getBarNumber(m_exam->count(), m_exam->penalty());
              rhythms = getRandomRhythm(curQ->melody()->meter()->meter(), barsNr, m_level.basicRhythms, m_level.dotsRhythms, m_level.rhythmDiversity);
              melodyLength = rhythms.count();
            }
          }
          if (m_level.randMelody == Tlevel::e_randFromList) {
              QList<TQAgroup> qaList;
              m_supp->listForRandomNotes(curQ->key, qaList);
              // ignore in key (4th param) of level, notes from list are already in key (if required)
              getRandomMelodyNG(qaList, curQ->melody(), melodyLength, false, false);
          } else if (m_level.randMelody == Tlevel::e_melodyFromSet) {
            // Randomize melody number or get it in order of the melody set
              int melodyId = m_melody->lastMelodyId();
              if (m_level.randOrderInSet) {
                  melodyId = m_rand->get();
              } else {
                // Set the next melody ID:
                // It may be the same ID when repetition number is not fulfilled
                // or the next melody when repeated enough or this is exercise
//                   qDebug() << "[MELODY SET] ID" << m_melody->lastMelodyId() << "repeat" << m_melody->repeatCounter() << "of" << m_level.repeatNrInSet;
                  m_melody->nextRepeat();
                  if (m_exercise || m_melody->repeatCounter() > m_level.repeatNrInSet) {
                    m_melody->setRepeat(1);
                    m_melody->setLastMelodyId(melodyId + 1);
                    if (m_melody->lastMelodyId() >= m_level.melodySet.count()) {
                      if (m_exercise) {
                          m_melody->setLastMelodyId(0); // start from the first melody
                          qDebug() << "[TexamExecutor] All melodies were exercised, starting again.";
                      } else { // TODO It should never happen, delete when tested
                          if (m_exam->count() < m_supp->obligQuestions() + m_exam->penalty())
                            qDebug() << "[TexamExecutor] FIXME! Trying to get melody out of the list!";
                          m_melody->setLastMelodyId(m_level.melodySet.count() - 1);
                      }
                    }
                  }
              }
              curQ->addMelody(&m_level.melodySet[melodyId], TQAunit::e_srcLevelSet, melodyId);
              if (!m_level.useKeySign) // respect melody key to avoid transposition
                curQ->key = curQ->melody()->key();
              melodyLength = curQ->melody()->length();
          } else { // Tlevel::e_melodyFromRange
              getRandomMelodyNG(m_questList, curQ->melody(), melodyLength, m_level.onlyCurrKey, m_level.endsOnTonic);
          }
          if (!m_level.isMelodySet() && m_level.useRhythms()) {
            mergeRhythmAndMelody(rhythms, curQ->melody());
          }
      } else {
          melodyLength = curQ->melody()->length();
      }
      if (curQ->answerAsSound()) // prepare list to store notes played by user
        m_melody->newMelody(curQ->melody(), curQ->melody()->key().difference(curQ->key));
      else // or clear it
        m_melody->newMelody(nullptr);
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
  if (curQ->questionOnScore()) {
    if (curQ->melody()) {
        if (!isAttempt) {
          bool ignoreTechnical = GLOB->instrument().type() != Tinstrument::Bandoneon;
          MAIN_SCORE->askQuestion(curQ->melody(), ignoreTechnical, curQ->key);
          if (m_level.showStrNr) { // we may be sure that instrument is kind of a guitar
            for (int i = 0; i < curQ->melody()->length(); ++i) {
              if (curQ->melody()->note(i)->g().str() > 1)
                MAIN_SCORE->setTechnical(i, curQ->melody()->note(i)->technical());
            }
  //           if (INSTRUMENT->isVisible())
  //             INSTRUMENT->prepareAnswer(); // It just shows range frame
          }
        }
        if (curQ->answerAsSound()) { // in fact, there is no other option yet
          connect(SOUND, &Tsound::noteStartedEntire, this, &TexamExecutor::noteOfMelodyStarted);
          connect(SOUND, &Tsound::noteFinishedEntire, this, &TexamExecutor::noteOfMelodyFinished);
          connect(MAIN_SCORE, &TmainScoreObject::readOnlyNoteClicked, this, &TexamExecutor::noteOfMelodySelected);
          if (curQ->melody()->note(0)->p().isRest()) { // skip first rest(s) and mark first note after it/them
              m_melody->noteStarted();
              m_melody->setNote(TnoteStruct());
              int noteId = 1;
              while (noteId < curQ->melody()->length() && curQ->melody()->note(noteId)->p().isRest())
                noteId++;
              if (noteId >= curQ->melody()->length())
                  qDebug() << "[TexamExecutor] Melody has only rests. Someone is mocking us. Troubles on the way...";
              else {
                  MAIN_SCORE->setSelectedItem(noteId);
                  m_melodySelectionIndex = noteId + 1;
              }
          } else // just mark first note
              MAIN_SCORE->setSelectedItem(0);
          MAIN_SCORE->setSelectInReadOnly(true); // allow user to select beginning note to play
        }
    } else {
        quint32 technData = 255; // empty
        if ((curQ->answerOnInstr() || curQ->answerAsSound()) && !m_level.onlyLowPos && m_level.showStrNr && curQ->qa.pos().str() > 1)
          technData = curQ->qa.technical.data(); // do show string number or not
        if (m_level.instrument == Tinstrument::Bandoneon)
          technData = curQ->qa.technical.data();
        if (m_level.useKeySign && !curQ->answerOnScore())
          MAIN_SCORE->askQuestion(curQ->qa.note, curQ->key, technData); // when answer is also on the score we determine key in preparing answer part
        else
          MAIN_SCORE->askQuestion(curQ->qa.note, technData);
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
                curQ->setStyle(GLOB->S->nameStyleInNoteName, GLOB->S->nameStyleInNoteName);
                m_prevAnswStyle = GLOB->S->nameStyleInNoteName;
                m_prevQuestStyle = GLOB->S->nameStyleInNoteName;
            }
        } else { // note name only in question
            if (m_level.requireStyle) { // switch previous used style
              curQ->setStyle(m_supp->randomNameStyle(m_prevQuestStyle), GLOB->S->nameStyleInNoteName);
              m_prevQuestStyle = curQ->styleOfQuestion();
            } else {
                curQ->setStyle(GLOB->S->nameStyleInNoteName, curQ->styleOfAnswer());
                m_prevQuestStyle = GLOB->S->nameStyleInNoteName;
            }
        }
      }
      NOTENAME->askQuestion(curQ->qa.note, curQ->styleOfQuestion());
      if (curQ->answerAsSound())
          m_answRequire.accid = false; // reset checking accidentals determined by level
  }

  if (curQ->questionOnInstr()) {
    INSTRUMENT->askQuestion(curQ->qa.note, curQ->qa.technical.data());
    if (curQ->answerOnScore())
        m_answRequire.octave = true; // checking accidental determined by level
    if (curQ->answerAsSound()) {
        m_answRequire.accid = false;
        m_answRequire.octave = true;
    }
  }

  if (curQ->questionAsSound()) {
    if (curQ->melody()) {
      if (!isAttempt) {
        // Meter was not set yet on score, set sound meter then to properly calculate metronome tempo & beat
        SOUND->setCurrentMeter(static_cast<int>(curQ->melody()->meter()->meter()));
        SOUND->setTempo(curQ->melody()->tempo());
        repeatSound(); // play melody but not when user tries again
      }
    } else {
        SOUND->play(curQ->qa.note);
        if (curQ->answerAsSound())
          m_answRequire.accid = false;
    }
  }

// PREPARING ANSWERS
  if (curQ->answerOnScore()) {
    if (!curQ->melody()) {
      if (m_level.useKeySign) {
          if (m_level.manualKey) { // user have to manually select a key
//               if (m_penalty->isNot()) // if black question key mode is defined (FIXME: What is this dead code?)
//                   curQ->key.setMinor(bool(qrand() % 2));
              // we randomize some key to cover this expected one
              TkeySignature fakeKey((qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value());
              MAIN_SCORE->prepareKeyToAnswer(fakeKey, curQ->key.getName());
              m_answRequire.key = true;
          } else
              MAIN_SCORE->setKeySignature(curQ->key);
      }
      if (curQ->questionOnScore()) { // note has to be another than question
          if (m_penalty->isNot())
              curQ->qa_2.note = m_supp->forceEnharmAccid(curQ->qa.note); // curQ->qa_2.note is expected note
          if (!m_level.manualKey && curQ->qa_2.note == curQ->qa.note) {
              blindQuestion();
              return; // refresh this function scope by calling it outside
          }
          MAIN_SCORE->forceAccidental(static_cast<int>(curQ->qa_2.note.alter()));
          m_answRequire.accid = true;
          m_answRequire.octave = true;
      }
      if (curQ->questionOnInstr() || curQ->questionAsSound()) {
          if (m_level.forceAccids)
            MAIN_SCORE->forceAccidental(static_cast<int>(curQ->qa.note.alter()));
      }
      if (curQ->questionAsName()) {
          m_answRequire.accid = true;
          m_answRequire.octave = true;
      }
    }
    MAIN_SCORE->unLockScore();
    if (curQ->melody()) {
      if (curQ->key.value() || m_level.manualKey) {
          MAIN_SCORE->setKeySignatureEnabled(true);
          if (m_level.useKeySign && m_level.manualKey) // set fake key
            MAIN_SCORE->setKeySignature(TkeySignature((qrand() % (m_level.hiKey.value() - m_level.loKey.value() + 1)) + m_level.loKey.value()));
          else
            MAIN_SCORE->setKeySignature(curQ->key);
      } else {
          MAIN_SCORE->setKeySignatureEnabled(false);
      }
      if (m_level.isMelodySet()) // only melody set can change clef during exam
        MAIN_SCORE->setClef(curQ->melody()->clef());
      MAIN_SCORE->setMeter(curQ->melody()->meter()->meter());
    }
    if (m_level.useKeySign && !m_level.manualKey) // case either for single mode and melodies
      MAIN_SCORE->lockKeySignature(true); // disables key signature enabled above - user cannot change it when fixed key sign.
  }

  if (curQ->answerAsName()) {
          /** During an exam Note name style is changed in two cases:
           * 1. If level.requireStyle = true every question or answer with Note Name switch it (letters/solfege)
           * 2. If Note Name is question and answer and are the same - this is only way that it makes sense
           */
      char answerAlter = 0;
      if (curQ->questionAsName())
          answerAlter = curQ->qa_2.note.alter();
      else {
          answerAlter = curQ->qa.note.alter();
          if (m_level.requireStyle)
              m_prevAnswStyle = m_supp->randomNameStyle(m_prevAnswStyle);
          curQ->setStyle(curQ->styleOfQuestion(), m_prevAnswStyle);
      }
      NOTENAME->prepareAnswer(curQ->styleOfAnswer());
      // force accidental when question and answer are note name or question as sound or instrument and level forces accidentals
      if (curQ->questionAsName() || ((curQ->questionOnInstr() || curQ->questionAsSound()) && m_level.forceAccids))
        NOTENAME->forceAccidental(answerAlter);
  }

  if (curQ->answerOnInstr()) {
      m_answRequire.accid = false;  // Ignored in checking, positions are comparing
      if (curQ->questionOnInstr()) { // such combination is possible only for guitar
        if (GLOB->instrument().isGuitar()) {
            QList<TfingerPos> posList;
            m_supp->getTheSamePosNoOrder(curQ->qa.pos(), posList);
            if (posList.isEmpty()) {
                blindQuestion();
                return; // refresh this function scope by calling it outside
            } else {
                if (m_penalty->isNot())
                  curQ->qa_2.pos() = posList[qrand() % posList.size()];
                INSTRUMENT->highlightAnswer(Tnote(), curQ->qa_2.technical.data());
            }
          }
      } else {
          if ((GLOB->instrument().isGuitar() && m_level.showStrNr) || (GLOB->instrument().type() == Tinstrument::Bandoneon && !curQ->questionOnScore()))
            INSTRUMENT->highlightAnswer(curQ->qa.note, curQ->qa.technical.data());
      }
      INSTRUMENT->setEnabled(true);
  }

  emit questionChanged();
  if (curQ->answerAsSound()) {
      if (curQ->melody() && curQ->melody()->note(0)->p().rhythm() > Trhythm::NoRhythm)
        SOUND->setQuantization(m_supp->findQuantization(curQ->melody()));
//       SOUND->prepareAnswer();
      if (curQ->questionAsSound())
          connect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying); // sniffing after finished sound
      else
          QTimer::singleShot(WAIT_TIME, [=]{ startSniffing(); });
          // Give a student some time to prepare itself for next question in expert mode
          // It avoids capture previous played sound as current answer
  } //else
      //SOUND->stopListen(); // stop sniffing if answer is not a played sound

  m_checkQuestAct->setEnabled(true);
  m_repeatQuestAct->setEnabled(false);
  m_nextQuestAct->setEnabled(false);
  if (curQ->questionAsSound() && m_playAgainAct)
    m_playAgainAct->setEnabled(true);
  if (curQ->questionAsSound() && curQ->answerOnScore() && m_tuningForkAct)
    m_tuningForkAct->setEnabled(true);
  m_penalty->startQuestionTime();
  if (!curQ->answerAsSound() && SOUND->stoppedByUser())
    m_tipHandler->showQuestionTip();
  else // As long as starting sound is expensive (PulseAudio) invoking tip animation at the same time sucks.
    QTimer::singleShot(WAIT_TIME + 100, [=]{ m_tipHandler->showQuestionTip(); }); // So call it with delay
  m_blindCounter = 0; // question successfully asked - reset the counter
}


void TexamExecutor::checkAnswer(bool showResults) {
  auto curQ = CURR_Q;
  m_penalty->stopQuestionTime();
  m_checkQuestAct->setEnabled(false);
  if (m_playAgainAct)
    m_playAgainAct->setEnabled(false);
  if (m_tuningForkAct)
    m_tuningForkAct->setEnabled(false);
  if (curQ->answerAsSound()) {
    if (curQ->melody())
      SOUND->stopListen();
    else
      SOUND->pauseSinffing(); // but only skip detected for single sound
    MAIN_SCORE->setSelectedItem(-1);
    disconnect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying);
    disconnect(SOUND, &Tsound::noteStartedEntire, this, &TexamExecutor::noteOfMelodyStarted);
    disconnect(SOUND, &Tsound::noteFinishedEntire, this, &TexamExecutor::noteOfMelodyFinished);
    disconnect(MAIN_SCORE, &TmainScoreObject::readOnlyNoteClicked, this, &TexamExecutor::noteOfMelodySelected);
  }
  if (m_exam->melodies() && SOUND->melodyIsPlaying())
    SOUND->stopPlaying();

  if (!GLOB->E->autoNextQuest || m_exercise)
    m_stopExamAct->setEnabled(true);
  m_isAnswered = true;
// Let's check
  Tnote questNote, answNote, userNote; // example note & returned note
// At first we determine what has to be checked
  if (!curQ->melody()) {
    questNote = curQ->qa.note;
    if (curQ->answerOnScore()) {
        if (m_level.manualKey) {
            if (MAIN_SCORE->keySignatureValue() != curQ->key.value())
              curQ->setMistake(TQAunit::e_wrongKey);
        }
        if (curQ->questionOnScore())
            questNote = curQ->qa_2.note;
        answNote = MAIN_SCORE->getNote(0);
    }
    if (curQ->answerAsName()) {
        if (curQ->questionAsName())
            questNote = curQ->qa_2.note;
        m_prevNoteIfName = NOTENAME->note(); // store note to restore it if will be repeated
        answNote = NOTENAME->note();
    }
    userNote = answNote;
    if (curQ->answerAsSound()) {
        answNote = SOUND->note();
//         if ((TintonationView::Eaccuracy)m_level.intonation != TintonationView::e_noCheck) {
//             if (TnoteStruct(Tnote(), SOUND->pitch()).inTune(TintonationView::getThreshold(m_level.intonation)))
//                 curQ->setMistake(TQAunit::e_wrongIntonation);
//         }
    }
  }
// Now we can check
  if (curQ->answerOnInstr()) { // 1. Comparing positions on the instrument
      if (GLOB->instrument().isGuitar()) {
          TfingerPos answPos(INSTRUMENT->technical()), questPos;
          if (curQ->questionOnInstr()) {
              if (answPos == curQ->qa.pos()) { // check has not user got answer the same as question position
                  curQ->setMistake(TQAunit::e_wrongPos);
                  qDebug("Cheater!");
              } else
                  questPos = curQ->qa_2.pos();
          } else
              questPos = curQ->qa.pos();
          if (questPos != answPos && curQ->isCorrect()) { // if no cheater give him a chance
            QList <TfingerPos> tmpPosList; // Maybe he gave correct note but on incorrect string only
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
          answNote = INSTRUMENT->note();
          m_supp->checkNotes(curQ, questNote, answNote, m_answRequire.octave, m_answRequire.accid);
          if (curQ->questionOnScore() && m_level.instrument == Tinstrument::Bandoneon) { // check bowing (bellow direction)
            Ttechnical bowFromInstr(INSTRUMENT->technical());
            if (bowFromInstr.bowing() != curQ->qa.technical.bowing())
              qDebug() << "[TexamExecutor check answer] mistake: wrong bellow direction";
          }
      }
  } else {
      if (curQ->melody()) { // 2. or checking melodies
          curQ->setMistake(TQAunit::e_correct); // reset an answer
          if (curQ->answerOnScore()) { // dictation
              Tmelody answMelody;
              MAIN_SCORE->getMelody(&answMelody);
              m_supp->compareWrittenFromPlayed(curQ->melody(), &answMelody, curQ->lastAttempt(), curQ->melody()->key().difference(curQ->key));
          } else { // playing a score
              m_supp->comparePlayedFromScore(curQ->melody(), m_melody->toPlay(), m_melody->played(),
                                             curQ->lastAttempt(), curQ->melody()->key().difference(curQ->key));
          }
          int goodAllready = 0, notBadAlready = 0, wrongAlready = 0;
          for (int i = 0; i < curQ->lastAttempt()->mistakes.size(); ++i) { // setting mistake type in TQAunit
            if (curQ->lastAttempt()->mistakes[i] == TQAunit::e_correct) {
              goodAllready++;
              continue; // it was correct - skip
            }
            if (curQ->lastAttempt()->mistakes[i] & TQAunit::e_wrongNote)
              wrongAlready++;
            else // or 'not bad'
              notBadAlready++;
          }
          if (goodAllready == curQ->melody()->length()) { // all required notes are correct
              curQ->setMistake(TQAunit::e_correct); // even if user put them more and effect. is poor
          } else if (goodAllready + notBadAlready == curQ->melody()->length()) { // add committed mistakes of last attempt
              curQ->setMistake(curQ->lastAttempt()->summary()); // or 'not bad'
          } else if (goodAllready + notBadAlready >= curQ->melody()->length() * 0.7) { // at least 70% notes answered properly
              if (curQ->lastAttempt()->effectiveness() >= 50.0) { // and effectiveness is sufficient
                  curQ->setMistake(curQ->lastAttempt()->summary());
                  curQ->setMistake(TQAunit::e_littleNotes); // but less notes than required
              } else { // or effectiveness is too poor
                  curQ->setMistake(TQAunit::e_veryPoor);
              }
          } else {
              curQ->setMistake(TQAunit::e_wrongNote);
          }
          if (m_level.manualKey && !curQ->isWrong()) {
            if (MAIN_SCORE->keySignatureValue() != curQ->key.value())
              curQ->setMistake(TQAunit::e_wrongKey);
          }
          // Another case is poor or very poor effectiveness but it is obtained after effect. update in sumarizeAnswer()
      } else { // 3. or checking are the notes the same
          m_supp->checkNotes(curQ, questNote, answNote, m_answRequire.octave, m_answRequire.accid);
          if (!m_answRequire.accid && curQ->isCorrect() && (curQ->answerOnScore() || curQ->answerAsName())) {
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
  bool autoNext = GLOB->E->autoNextQuest;
  if (GLOB->E->afterMistake == TexamParams::e_stop && !curQ->isCorrect())
      autoNext = false; // when mistake and e_stop - the same like autoNext = false;

  if (showResults) {
//     if (!(GLOB->waitForCorrect() && curQ->answerAsSound() && m_exercise && m_melody))
      m_tipHandler->showResultTip(curQ); // tip duration is calculated by itself (inside resultTip() method)
    if ((!m_exercise || (m_exercise && curQ->isCorrect())) && !autoNext)
      m_tipHandler->showWhatNextTip(curQ->isCorrect());
    if (!autoNext) {
      if (!curQ->isCorrect() && !m_exercise && !curQ->melody())
        m_repeatQuestAct->setEnabled(true);
      if (!curQ->isCorrect() && curQ->melody())
        m_newAtemptAct->setEnabled(true);
      m_nextQuestAct->setEnabled(true);
    }
  }

  markAnswer(curQ);
  int waitTime = GLOB->E->questionDelay;
  if (m_melody) // increase minimal delay before next question for melodies to 500ms
    waitTime = qMax(waitTime, 500);
  if (m_exercise) {
    if ((GLOB->E->autoNextQuest && GLOB->E->afterMistake != TexamParams::e_continue) || !GLOB->E->autoNextQuest || GLOB->E->showCorrected)
      waitTime = GLOB->E->correctPreview; // user has to have time to see his mistake and correct answer
    m_exercise->checkAnswer();
    if (!curQ->isCorrect()) { // correcting wrong answer
        if (GLOB->correctAnswers()) // TODO for dictation it should always stop and show mistakes
            correctAnswer();
        else {
          if (!GLOB->autoNextQuestion() /*|| (GLOB->autoNextQuestion() && GLOB->E->afterMistake == TexamParams::e_stop)*/)
            m_correctAct->setEnabled(true); // show too button only when exam stops after mistake
          if (!autoNext) {
            m_tipHandler->showWhatNextTip(true, true);
            m_lockRightButt = false;
            return; // wait for user
          }
        }
    }
  }
  if (showResults && autoNext) {
    waitTime = 1500;
//       m_lockRightButt = true; // to avoid nervous users clicking mouse during wait time
      if (m_shouldBeTerminated)
          stopExamSlot();
      else {
        if (curQ->isCorrect()) {
          m_askingTimer->start(m_melody ? qMax(GLOB->E->questionDelay, 500) : waitTime /*GLOB->E->questionDelay*/);
      } else {
            if (GLOB->E->repeatIncorrect && !m_incorrectRepeated) {
              if (curQ->melody())
                  QTimer::singleShot(waitTime, [=]{ newAttempt(); });
              else {
                  if (!m_exercise) // repeat only once if any
                    QTimer::singleShot(waitTime, [=] { repeatQuestion(); });
                  else
                    m_askingTimer->start(waitTime);
              }
          } else {
//               if (GLOB->E->afterMistake == TexamParams::e_wait && (!m_exercise || !GLOB->E->showCorrected))
//                 waitTime = GLOB->E->mistakePreview; // for exercises time was set above
              m_askingTimer->start(waitTime);
          }
        }
      }
  }
}


/**
 * @p correctAnswer() invokes @p correct() method of appropriate control
 * and connects to @p correctingFinished() signal of the control.
 * When the control will finish correction routines it will emit this signal,
 * so @p correctionFinishedSlot continues executor action
 *
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
  if (m_correctAct && !GLOB->correctAnswers())
    m_correctAct->setEnabled(false);
  QObject* correctAnimObject = nullptr;
  if (m_askingTimer->isActive())
    m_askingTimer->stop();
  m_tipHandler->clearTips(false);
  auto curQ = m_exam->answList()->last();
//   QColor markColor = m_supp->answerColor(curQ);
  if (curQ->melody()) {
    MAIN_SCORE->setSelectInReadOnly(true);
    connect(MAIN_SCORE, &TmainScoreObject::readOnlyNoteClicked, this, &TexamExecutor::correctNoteOfMelody);
    m_tipHandler->melodyCorrectMessage();
  }
  if (curQ->answerOnScore()) {
    if (curQ->melody()) {
      if (m_level.manualKey && curQ->key.value() != MAIN_SCORE->keySignatureValue())
        MAIN_SCORE->correctKeySignature(curQ->key);
    } else {
        Tnote goodNote = curQ->questionOnScore() ? curQ->qa_2.note : curQ->qa.note;
//         bool correctAccid = curQ->wrongAccid() || curQ->wrongOctave();
        if (curQ->wrongAccid() || curQ->wrongOctave()) // it corrects wrong octave as well
          MAIN_SCORE->correctNote(goodNote, true);
        else if (curQ->wrongNote()) {
//             if (m_level.manualKey && curQ->key.value() != MAIN_SCORE->keySignatureValue())
//               MAIN_SCORE->correctKeySignature(curQ->key);
            m_exercise->setCorrectedNoteId(0);
            MAIN_SCORE->correctNote(goodNote, false);
        }
        if (curQ->wrongKey())
          MAIN_SCORE->correctKeySignature(curQ->key);
//         MAIN_SCORE->correctNote(goodNote, correctAccid);
        correctAnimObject = MAIN_SCORE;
    }
  } else if (curQ->answerOnInstr()) {
      auto goodPos = curQ->questionOnInstr() ? curQ->qa_2.technical.data() : curQ->qa.technical.data();
      INSTRUMENT->correct(curQ->questionOnInstr() ? curQ->qa_2.note : curQ->qa.note, goodPos);
      correctAnimObject = INSTRUMENT;
  } else if (curQ->answerAsName()) {
      Tnote goodNote = curQ->questionAsName() ? curQ->qa_2.note : curQ->qa.note;
      if (!m_answRequire.accid && curQ->isNotSoBad()) { // respect accidental selected by user 
        switch (NOTENAME->note().alter()) {
          case -2 : goodNote = goodNote.showWithDoubleFlat(); break;
          case -1 : goodNote = goodNote.showWithFlat(); break;
          case  0 : goodNote = goodNote.showAsNatural(); break;
          case  1 : goodNote = goodNote.showWithSharp(); break;
          case  2 : goodNote = goodNote.showWithDoubleSharp(); break;
        }
      }
      NOTENAME->correct(goodNote);
//       NOTENAME->correctName(goodNote, markColor, curQ->isWrong());
      correctAnimObject = NOTENAME;
  } else { // answer as played sound
      if (curQ->melody()) {

      } else {
          if (curQ->wrongIntonation()) { // TODO
//               float outTune = SOUND->pitch() - (float)qRound(SOUND->pitch());
//               SOUND->pitchView()->outOfTuneAnim(outTune, 1200);
//               m_tipHandler->outOfTuneTip(outTune); // we are sure that it is beyond the accuracy threshold
//               correctAnimStarted = true;
          }
          if (m_supp->isCorrectedPlayable())
              repeatSound();
          else {
//             if (INSTRUMENT->isVisible()) {
            // Animation towards guitar when instrument is guitar and answer was wrong or octave was wrong
//               if (curQ->questionOnInstr())
//                 INSTRUMENT->correctPosition(curQ->qa.pos, markColor);
//               else
//                 m_tipHandler->correctToGuitar(curQ->questionAs, GLOB->E->mistakePreview, curQ->qa.pos);
//               correctAnimStarted = true;
//             }
          }
    }
  }
  if (correctAnimObject) { // disable space bar and right mouse button when animation is performed
      m_nextQuestAct->setEnabled(false);
      connect(correctAnimObject, SIGNAL(correctionFinished()), this, SLOT(correctionFinishedSlot()));
      m_disconnectAfterAnim = true;
//     m_lockRightButt = true;
  } else
      correctionFinishedSlot();
}


void TexamExecutor::newAttempt() {
  m_tipHandler->showTryAgainTip(3000);
//   QTimer::singleShot(2000, m_tipHandler, SLOT(clearResultTip())); // TODO remove when not used
//   MAIN_SCORE->showNoteNames(false);
  if (CURR_Q->answerOnScore() || CURR_Q->questionOnScore()) { // remove names and marks from score notes
    int scoreNoteId = 0;
    QColor mc;
    for (int i = 0; i < CURR_Q->lastAttempt()->mistakes.size(); ++i) {
      MAIN_SCORE->showNoteName(i, false);
      if (!m_exercise || CURR_Q->lastAttempt()->mistakes[i] == TQAunit::e_correct)
        mc.setAlpha(0);
      else
        mc = m_supp->answerColor(CURR_Q->lastAttempt()->mistakes[i]);
      scoreNoteId += MAIN_SCORE->markNoteHead(mc, scoreNoteId); // remove all note head marks in exams and those for correct notes in exercises
    }
  }
  if (CURR_Q->answerAsSound()) // prepare list to store notes played by user
    m_melody->newMelody(CURR_Q->melody(), CURR_Q->melody()->key().difference(CURR_Q->key));
  else // or clear it
    m_melody->newMelody(nullptr);
  m_melody->newMelody(CURR_Q->answerAsSound() ? CURR_Q->melody() : nullptr);

  m_penalty->newAttempt();
  if (CURR_Q->answerAsSound())
    CURR_Q->lastAttempt()->melodyWasPlayed(); // we can suppose that user will play an answer for sure
  askQuestion(true);
}


void TexamExecutor::markAnswer(TQAunit* curQ) {
  QColor markColor = m_supp->answerColor(curQ);
  if (curQ->melody()) {
      int scoreNoteId = 0;
      for (int i = 0; i < curQ->lastAttempt()->mistakes.size(); ++i) {
        scoreNoteId += MAIN_SCORE->markNoteHead(m_supp->answerColor(curQ->lastAttempt()->mistakes[i]), scoreNoteId);
      }
  } else {
    switch (curQ->answerAs) {
      case TQAtype::e_onScore:
        MAIN_SCORE->markNoteHead(markColor, 0);
        break;
      case TQAtype::e_onInstr:
        INSTRUMENT->markSelected(markColor);
        break;
      case TQAtype::e_asName:
        NOTENAME->setMarkColor(markColor);
        break;
      case TQAtype::e_asSound:
//         SOUND->pitchView()->markAnswer(markColor);
        break;
    }
    switch (curQ->questionAs) {
      case TQAtype::e_onScore:
        MAIN_SCORE->markNoteHead(markColor, 1);
        break;
      case TQAtype::e_onInstr:
        INSTRUMENT->markSelected(markColor);
        break;
      case TQAtype::e_asName:
        NOTENAME->setMarkColor(markColor);
        break;
      case TQAtype::e_asSound:
        break;
    }
  }
  if (m_exercise && GLOB->extraNames()) {
    if (!curQ->questionAsName() && !curQ->answerAsName()) {
        if (curQ->answerOnScore() || (curQ->answerAsSound() && curQ->questionOnScore())) {
          if (!m_melody || !GLOB->waitForCorrect())
            MAIN_SCORE->showNoteNames(true);
//       else if (curQ->answerAsFret()) // for q/a fret-fret this will be the first case
//         INSTRUMENT->showName(GLOB->S->nameStyleInNoteName, curQ->qa.note, markColor); // Take it from user answer
//       else if (curQ->answerAsSound() && curQ->questionAsFret())
//           INSTRUMENT->showName(GLOB->S->nameStyleInNoteName, curQ->qa.note, markColor);
        }
    } else { // cases when name was an question
        if (curQ->questionAsName()) {
          if (curQ->answerOnScore())
            MAIN_SCORE->showNoteNames(true); // TODO name style
//             SCORE->showNames(curQ->styleOfQuestion());
//           else if (curQ->answerAsFret())
//             INSTRUMENT->showName(curQ->styleOfQuestion(), curQ->qa.note, markColor);
        }
    }
  }
}


void TexamExecutor::repeatQuestion() {
  m_tipHandler->showTryAgainTip(3000);
//   m_lockRightButt = false;
  m_incorrectRepeated = true;
  m_isAnswered = false;
//   if (GLOB->E->showNameOfAnswered) {
//     for (int i = 0; i < 2; i++)
//       SCORE->deleteNoteName(i);
//     INSTRUMENT->deleteNoteName();
//   }
// for melodies it never comes here - questions are newer repeated - copying of TQAunit is safe 
  TQAunit curQ(*CURR_Q); // copy last unit as a new one

  if (!GLOB->E->autoNextQuest)
      m_tipHandler->clearTips();
  curQ.setMistake(TQAunit::e_correct);

  if (curQ.answerOnScore())
      MAIN_SCORE->unLockScore();
  if (curQ.questionAsName()) { // refresh question on NoteName
      NOTENAME->askQuestion(curQ.qa.note, curQ.styleOfQuestion());
  } else
        NOTENAME->setNote(Tnote());
  if (curQ.answerAsName()) {
    Tnote answNote = Tnote(0, 0, 0);
    NOTENAME->setDisabled(false);
    if (curQ.questionAsName())
      answNote = curQ.qa_2.note;
    else if (!curQ.answerOnScore())
        answNote = curQ.qa.note;
    NOTENAME->prepareAnswer(curQ.styleOfAnswer());
//     NOTENAME->setStyle(curQ.styleOfAnswer());
    if (curQ.questionOnInstr() || curQ.questionAsSound()) {
        if (m_level.forceAccids)
          NOTENAME->forceAccidental(answNote.alter());
    } else if (curQ.questionAsName())
        NOTENAME->forceAccidental(answNote.alter());
  }
  if (curQ.answerOnInstr())
    INSTRUMENT->setEnabled(true);
  if (curQ.answerAsSound() && !curQ.questionAsSound())
      startSniffing();
      // *** When question is sound it is played again (repeatSound()) 
      // and than startSniffing is called

  m_exam->addQuestion(curQ); // curQ is copied here - it becomes differ than this in exam list
  m_penalty->setBlackQuestion();

  if (!GLOB->autoNextQuestion())
    m_stopExamAct->setEnabled(false);
  m_repeatQuestAct->setEnabled(false);
  m_nextQuestAct->setEnabled(false);
  m_checkQuestAct->setEnabled(true);
//       TOOLBAR->startExamAct->setDisabled(true);
//   TOOLBAR->setForQuestion(CURR_Q->questionAsSound(), CURR_Q->questionAsSound() && CURR_Q->answerAsNote());
  if (CURR_Q->questionAsSound())
    repeatSound();
  m_tipHandler->showQuestionTip();
  m_penalty->startQuestionTime();
}


void TexamExecutor::displayCertificate() {
  m_snifferLocked = true;
  SOUND->stopListen();
  m_penalty->pauseTime();
// #if !defined (Q_OS_ANDROID)
//   qApp->removeEventFilter(m_supp); // stop grabbing right button and calling checkAnswer()
// #endif
  m_tipHandler->certificateTip();
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
  if (!NOTENAME && !m_exam->melodies()) { // TODO It should never happened, delete it when checked
    qDebug() << "[TexamExecutor prepareToExam ] Single note mode required but note name was not created. THERE IS A BUG!!!\n\n";
    return;
  }

  disableWidgets();
// connect all events to check an answer or display tip how to check
  connect(MAIN_SCORE, &TmainScoreObject::clicked, this, &TexamExecutor::expertAnswersSlot);
  if (NOTENAME)
    connect(NOTENAME, &TnameItem::noteButtonClicked, this, &TexamExecutor::expertAnswersSlot);
  if (INSTRUMENT)
  connect(INSTRUMENT, &TcommonInstrument::noteChanged, this, &TexamExecutor::expertAnswersSlot);
  if (m_level.instrument != Tinstrument::NoInstrument)
    connect(SOUND, &Tsound::noteStarted, this, &TexamExecutor::expertAnswersSlot);
  else
    connect(SOUND, &Tsound::noteFinished, this, &TexamExecutor::expertAnswersSlot);
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp);
// #endif
//   connect(m_supp, SIGNAL(rightButtonClicked()), this, SLOT(rightButtonSlot()));

  if (m_exercise) {
    connect(m_exercise, &Texercises::messageDisplayed, this, &TexamExecutor::stopSound);
    connect(m_exercise, &Texercises::messageClosed, this, &TexamExecutor::suggestDialogClosed);
  }

  m_glStore->storeSettings();
  m_glStore->prepareGlobalsToExam(m_level);
  GLOB->setRhythmsEnabled(m_level.useRhythms());

//   INSTRUMENT->setVisible(GLOB->L->guitarEnabled);
  if (m_level.canBeSound()) {
    SOUND->acceptSettings();
    if (SOUND->isSniffable())
        SOUND->stopListen();
    SOUND->prepareToExam(m_level.requireOctave ? m_level.loNote : GLOB->loNote(), m_level.requireOctave ? m_level.hiNote : GLOB->hiNote());
//     SOUND->pitchView()->setIntonationAccuracy(m_level.intonation);
//     SOUND->pitchView()->enableAccuracyChange(false);
  }
  clearWidgets();
//   if (GLOB->instrument().isGuitar() && !m_supp->isCorrectedPlayable())
//       INSTRUMENT->createRangeBox(m_supp->loFret(), m_supp->hiFret());
  m_soundTimer = new QTimer(this);
  connect(m_soundTimer, &QTimer::timeout, this, &TexamExecutor::startSniffing);
  m_askingTimer = new QTimer(this);
  connect(m_askingTimer, &QTimer::timeout, this, &TexamExecutor::askQuestionSlot);

  m_snifferLocked = false;
  m_tipHandler = new TtipHandler(m_exam, this);
  emit tipHandlerCreated();
  m_tipHandler->showStartTip();
  emit titleChanged();
  if (m_exercise && !m_exam->melodies()) {
//     if (m_level.answerIsNote())
//       connect(SCORE, &TmainScore::correctingFinished, this, &TexamExecutor::correctionFinished);
//     if (m_level.answerIsName())
//       connect(NOTENAME, &TnoteName::correctingFinished, this, &TexamExecutor::correctionFinished);
//     if (m_level.answerIsGuitar())
//       connect(GUITAR, &TfingerBoard::correctingFinished, this, &TexamExecutor::correctionFinished);
//     if (m_level.answerIsSound()) {
//       connect(SOUND->pitchView(), &TpitchView::correctingFinished, this, &TexamExecutor::correctionFinished);
//       connect(m_tipHandler, &Tcanvas::correctingFinished, this, &TexamExecutor::correctionFinished);
//     }
  }
}


void TexamExecutor::restoreAfterExam() {
#if !defined (Q_OS_ANDROID)
  if (!GLOB->A->dumpPath.isEmpty())
    SOUND->setDumpFileName(QLatin1String("nootka_dump"));
#endif

  m_glStore->restoreSettings();
  if (m_exercise) {
    GLOB->E->suggestExam = m_exercise->suggestInFuture();
  }


  SOUND->acceptSettings();
//   SOUND->pitchView()->setIntonationAccuracy(GLOB->A->intonation);
//   SOUND->pitchView()->enableAccuracyChange(true);

  if (NOTENAME) {
    NOTENAME->setDisabled(false);
    NOTENAME->setButtonNameStyle(GLOB->S->nameStyleInNoteName);
  }
  if (INSTRUMENT) {
    INSTRUMENT->restoreAfterExam();
    INSTRUMENT->setEnabled(true);
//   INSTRUMENT->deleteRangeBox();
  }

  if (m_tipHandler)
    m_tipHandler->deleteLater();
  m_tipHandler = nullptr;
  emit titleChanged();

  MAIN_SCORE->setReadOnly(false);
  SOUND->restoreAfterExam();
}


void TexamExecutor::disableWidgets() {
  MAIN_SCORE->setReadOnly(true);
  if (!GLOB->isSingleNote())
    MAIN_SCORE->setSelectedItem(-1);
  if (INSTRUMENT)
    INSTRUMENT->setEnabled(false);
  if (NOTENAME)
    NOTENAME->setDisabled(true);
}


void TexamExecutor::clearWidgets() {
  MAIN_SCORE->clearScore();
  if (NOTENAME) {
    NOTENAME->setNote(Tnote());
    NOTENAME->setMarkColor(Qt::transparent);
  }
  if (INSTRUMENT) {
    INSTRUMENT->setNote(Tnote());
    INSTRUMENT->markSelected(Qt::transparent);
  }
}


void TexamExecutor::createActions() {
  QQmlComponent actionsComp(NOO->qmlEngine(), this);
  m_settAct = new Taction(QApplication::translate("TtoolBar", "Settings"), QStringLiteral("exam-settings"), this);
  m_examActions.append(m_settAct);
  connect(m_settAct, &Taction::triggered, this, &TexamExecutor::prepareToSettings);
  m_settAct->setTip(tr("Exercise or exam preferences"), QQuickItem::TopRight);
  m_helpAct = new Taction(QApplication::translate("TtoolBar", "Help"), QStringLiteral("help"), this);
  m_examActions.append(m_helpAct);
  connect(m_helpAct, &Taction::triggered, this, &TexamExecutor::showExamHelp);
  m_stopExamAct = new Taction(QApplication::translate("TtoolBar", "Stop"), QStringLiteral("stopExam"), this);
  if (m_exercise) {
      connect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExerciseSlot);
      m_stopExamAct->setTip(tr("finish exercising"), QQuickItem::TopRight);
  } else {
      connect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExamSlot);
      m_stopExamAct->setTip(tr("stop the exam"), QQuickItem::TopRight);
  }
  m_stopExamAct->setBgColor(Qt::red);
  m_examActions.append(m_stopExamAct);
  m_repeatQuestAct = new Taction(QApplication::translate("TtoolBar", "Repeat", "like a repeat question"), QStringLiteral("prevQuest"), this, false);
  connect(m_repeatQuestAct, &Taction::triggered, this, &TexamExecutor::repeatQuestion);
  m_repeatQuestAct->setBgColor(qApp->palette().highlight().color());
  m_examActions.append(m_repeatQuestAct);
  m_repeatQuestAct->createQmlShortcut(&actionsComp, "\"Backspace\"");
  m_repeatQuestAct->setTip(tr("repeat previous question (backspace)").replace(QLatin1String("("), QLatin1String("<br>(")), QQuickItem::TopRight);
  m_nextQuestAct = new Taction(QApplication::translate("TtoolBar", "Next", "like a next question"), QStringLiteral("nextQuest"), this);
  m_nextQuestAct->setBgColor(qApp->palette().highlight().color());
  m_examActions.append(m_nextQuestAct);
  connect(m_nextQuestAct, &Taction::triggered, this, &TexamExecutor::askQuestionSlot);
  m_nextQuestAct->createQmlShortcut(&actionsComp, "\"Space\"");
  m_nextQuestAct->setTip(tr("next question\n(space %1)").arg(TexamHelp::orRightButtTxt()).replace(QLatin1String("\n"), QLatin1String("<br>")), QQuickItem::TopRight);
  if (m_level.questionAs.isSound()) {
    if (m_level.answersAs[TQAtype::e_asSound].isOnScore()) {
      m_tuningForkAct = new Taction(Tnote(6, 1, 0).toRichText(), QStringLiteral("fork"), this, false);
      m_tuningForkAct->setBgColor(qApp->palette().highlight().color());
      m_examActions.append(m_tuningForkAct);
      m_tuningForkAct->setTip(tr("Play <i>middle a</i> like a tuning fork.\n(Press key 'a')").replace(QLatin1String("\n"), QLatin1String("<br>")), QQuickItem::TopRight);
      connect(m_tuningForkAct, &Taction::triggered, this, &TexamExecutor::playMiddleA);
      m_tuningForkAct->createQmlShortcut(&actionsComp, "\"a\"");
    }
    m_playAgainAct = new Taction(QApplication::translate("TtoolBar", "Play"), QStringLiteral("playMelody"), this, false);
    m_playAgainAct->setBgColor(QColor(0, 255, 0));
    m_examActions.append(m_playAgainAct);
    m_playAgainAct->setTip(tr("play sound again") + QStringLiteral("<br>(") +
          TexamHelp::pressSpaceKey().replace(QStringLiteral("<b>"), QStringLiteral(" ")).replace(QStringLiteral("</b>"), QStringLiteral(")")), QQuickItem::TopRight);
    m_playAgainAct->createQmlShortcut(&actionsComp, "\"Space\"");
    connect(m_playAgainAct, &Taction::triggered, this, &TexamExecutor::repeatSound);
    connect(SOUND, &Tsound::playingChanged, this, [=]{
      m_playAgainAct->setIconTag(SOUND->melodyIsPlaying() ? QStringLiteral("stopMelody") : QStringLiteral("playMelody"));
    });
  }
  if (m_level.canBeMelody()) {
    m_newAtemptAct = new Taction(QApplication::translate("TtoolBar", "Try again"), "prevQuest", this, false);
    m_newAtemptAct->setBgColor(qApp->palette().highlight().color());
    m_examActions.append(m_newAtemptAct);
    connect(m_newAtemptAct, &Taction::triggered, this, &TexamExecutor::newAttempt);
    m_newAtemptAct->setTip(tr("Try this melody once again. (backspace)").replace(QLatin1String("("), QLatin1String("<br>(")), QQuickItem::TopRight);
    m_newAtemptAct->createQmlShortcut(&actionsComp, "\"Backspace\"");
  }
  m_checkQuestAct = new Taction(QApplication::translate("TtoolBar", "Check", "like a check answer"), QStringLiteral("check"), this, false);
  m_checkQuestAct->setBgColor(GLOB->correctColor());
  m_examActions.append(m_checkQuestAct);
  connect(m_checkQuestAct, &Taction::triggered, this, &TexamExecutor::checkAnswerSlot);
  m_checkQuestAct->createQmlShortcut(&actionsComp, "\"Return\"");
  m_checkQuestAct->setTip(tr("check answer\n(enter %1)").arg(TexamHelp::orRightButtTxt()).replace(QLatin1String("\n"), QLatin1String("<br>")), QQuickItem::TopRight);
  if (m_exercise) {
    m_correctAct = new Taction(QApplication::translate("TtoolBar", "Correct", "like a correct answer with mistake"), QStringLiteral("correct"), this, false);
    m_correctAct->setBgColor(Qt::green);
    m_examActions.append(m_correctAct);
    connect(m_correctAct, &Taction::triggered, this, &TexamExecutor::correctAnswer);
    m_correctAct->createQmlShortcut(&actionsComp, "\"Return\"");
    m_correctAct->setTip(tr("correct answer\n(enter)").replace(QLatin1String("\n"), QLatin1String("<br>")), QQuickItem::TopRight);
  }
  emit examActionsChanged();
}


void TexamExecutor::exerciseToExam() {
  m_isAnswered = true;
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp);
// #endif
  m_exam->saveToFile();
  QString userName = m_exam->userName();
  delete m_penalty;
  delete m_exam;
  m_examActions.removeOne(m_correctAct);
  delete m_correctAct;
  m_correctAct = nullptr;
  m_exam = new Texam(&m_level, userName);
  m_exam->setTune(*GLOB->Gtune());
  delete m_exercise;
  m_exercise = nullptr;
  m_tipHandler->changeExam(m_exam);
// #if !defined (Q_OS_ANDROID) // TODO: Some hint under Android
//   m_tipHandler->levelStatusMessage();
// #endif
  m_supp->setFinished(false); // exercise had it set to true
  m_supp->resetKeyRandom(); // new set of randomized key signatures when exam requires them
  m_supp->resetMeterRandom();
  initializeExecuting();
  disconnect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExerciseSlot);
  connect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExamSlot);
  m_stopExamAct->setTip(qTR("TexamExecutor", "stop the exam"), QQuickItem::TopRight);
  clearWidgets();
  emit examActionsChanged();
  emit titleChanged();
  m_tipHandler->clearTips();
  m_tipHandler->showStartTip();
}


void TexamExecutor::stopExerciseSlot() {
  m_askAfterSummary = m_askingTimer->isActive();
  m_askingTimer->stop(); // stop questioning, if any
  stopSound();
  if (m_exam->count()) {
      if (!m_isAnswered) {
        m_penalty->pauseTime();
        m_exam->skipLast(true);
      }
      // user still can take new attempt to correct a melody, so hide it
      if (m_isAnswered && CURR_Q->melody() && CURR_Q->answerOnScore() && !CURR_Q->isCorrect()) 
        CURR_Q->melody()->setTitle(CURR_Q->melody()->title() + QLatin1String(";skip"));
      m_penalty->updateExamTimes();
      m_exerciseTmpStyle = GLOB->S->nameStyleInNoteName;
      GLOB->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show charts in user defined style

      m_summaryReason = SumFinishExer;
      if (!m_goingClosed)
        emit examSummary();
      else
        finishExerciseAfterSummary();
  } else
      closeExecutor();
}


void TexamExecutor::finishExerciseAfterSummary() {
  restoreExerciseAfterSummary();
  if ((m_exam->count() == 1 && CURR_Q->answered()) || m_exam->count() > 1)
    m_exam->saveToFile();
  closeExecutor();
}


void TexamExecutor::restoreExerciseAfterSummary() {
  if (m_summaryReason == SumFinishExer) {
    m_summaryReason = NoReason;
    if (m_isAnswered && CURR_Q->melody() && CURR_Q->answerOnScore() && !CURR_Q->isCorrect()) // revert melody title
      CURR_Q->melody()->setTitle(CURR_Q->melody()->title().remove(QLatin1String(";skip")));
    if (m_isAnswered)
        CURR_Q->setAnswered();
    GLOB->S->nameStyleInNoteName = m_exerciseTmpStyle;
// #if !defined (Q_OS_ANDROID)
//     qApp->installEventFilter(m_supp);
// #endif
  }
}


void TexamExecutor::continueExercise() {
  restoreExerciseAfterSummary();
  if (!m_isAnswered) {
    m_exam->skipLast(false);
    m_penalty->continueTime();
  }
  if (m_askAfterSummary) // ask next question if questioning was stopped
      askQuestion();
  else {// restore sniffing if necessary
      if (CURR_Q->answerAsSound())
        startSniffing();
  }
}


void TexamExecutor::stopExamSlot() {
  if (!m_isAnswered && !GLOB->E->closeWithoutConfirm) {
    m_shouldBeTerminated = true;
    int messageDuration = 3000;
    NOO->setMessageColor(GLOB->EquestionColor);
    NOO->showTimeMessage(tr("Give an answer first!<br>Then the exam will end."), messageDuration, QQuickItem::Top);
    return;
  }
  if (!m_isAnswered)
    checkAnswer(false);
  m_penalty->stopTimeView();
  stopSound();
  if (m_exam->count()) {
    if (!m_exam->fileName().isEmpty()) {
      if(!QFileInfo(m_exam->fileName()).isWritable()) {
        qDebug() << "Can't write to file. Another name is needed";
        m_exam->setFileName(QString());
      }
    }
    if (m_exam->fileName().isEmpty()) {
      if (GLOB->E->closeWithoutConfirm) {
          m_exam->setFileName(getExamFileName(m_exam) + QLatin1String(".noo"));
      } else {
          m_exam->setFileName(saveExamToFile());
          if (!m_exam->fileName().isEmpty())
            GLOB->E->examsDir = QFileInfo(m_exam->fileName()).absoluteDir().absolutePath();
      }
    }
    if (!m_exam->fileName().isEmpty()) {
      if (m_exam->melodies()) // summarize answer if not summarized yet (melodies can have such cases)
        m_penalty->setMelodyPenalties();
      m_penalty->updateExamTimes();
      GLOB->S->nameStyleInNoteName = m_glStore->nameStyleInNoteName; // restore to show in user defined style
      if (m_exam->saveToFile() == Texam::e_file_OK) {
        QStringList recentExams = GLOB->config->value(QLatin1String("recentExams")).toStringList();
        recentExams.removeAll(m_exam->fileName());
        recentExams.prepend(m_exam->fileName());
        if (recentExams.size() > RECENT_EXAMS_LIMIT)
          recentExams.removeLast();
        GLOB->config->setValue(QLatin1String("recentExams"), recentExams);
      }
      if (!m_goingClosed) { // if Nootka is closing don't show summary
        m_summaryReason = SumFinishExam;
        emit examSummary();
        return; // Exam summary dialog will call closeExecutor()
      }
    }
  }
  closeExecutor();
}


void TexamExecutor::closeExecutor() {
// #if !defined (Q_OS_ANDROID)
//   STATUS->setBackground(-1);
//   STATUS->setMessage(QString());
// #endif
  NOO->setMessageColor(QColor(0, 0xa0, 0xa0));
  NOO->showTimeMessage(tr("Such a pity."), 5000, QQuickItem::Top);

  m_tipHandler->clearTips();
  clearWidgets();
  restoreAfterExam();
  GLOB->setIsExam(false);
  deleteLater();
}


void TexamExecutor::prepareToSettings() {
  stopSound();
  emit showSettings();
}


void TexamExecutor::settingsAccepted() {
  if (m_exercise) {
      if (GLOB->E->suggestExam)
        m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
      else
        m_exercise->setSuggestionEnabled(0);
  } else {
      if (GLOB->E->autoNextQuest)
        m_stopExamAct->setEnabled(true);
  }
  if (m_exam->count() && CURR_Q->answerAsSound() && !SOUND->isSnifferPaused())//!SOUND->pitchView()->isPaused())
    startSniffing();
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp);
// #endif
}


void TexamExecutor::stopSound() {
  if (m_soundTimer->isActive())
      m_soundTimer->stop();
  SOUND->stopPlaying();
  SOUND->stopListen();
// #if !defined (Q_OS_ANDROID)
//   qApp->removeEventFilter(m_supp);
// #endif
}


void TexamExecutor::suggestDialogClosed(bool startExam) {
  if (startExam) {
      exerciseToExam();
  } else {
// #if !defined (Q_OS_ANDROID)
//       qApp->installEventFilter(m_supp);
// #endif
      if (CURR_Q->answerAsSound())
            startSniffing();
  }
}


bool TexamExecutor::closeNootka() {
  bool allowToCLose = true;
  if (m_exercise) {
      m_goingClosed = true;
      stopExerciseSlot();
      allowToCLose = true;
  } else {
      m_snifferLocked = true;
// #if !defined (Q_OS_ANDROID)
//     qApp->removeEventFilter(m_supp);
// #endif
      auto msg = new QMessageBox(nullptr);
      msg->setText(tr("Psssst... Exam is going.<br><br>"
                      "Select <b>%1</b> to check, save and exit<br>"
                      "or <b>%2</b> to continue.<br>").arg(qTR("QShortcut", "Save"), qTR("QPlatformTheme", "Retry")));
      msg->setStandardButtons(QMessageBox::Retry | QMessageBox::Save);
      msg->setWindowTitle(QStringLiteral("Psssst..."));
      if (!GLOB->E->closeWithoutConfirm)
        msg->exec();
      if (!GLOB->E->closeWithoutConfirm && msg->clickedButton() == msg->button(QMessageBox::Retry)) {
          m_snifferLocked = false;
// #if !defined (Q_OS_ANDROID)
//         qApp->installEventFilter(m_supp);
// #endif
          allowToCLose = false;
      } else {
          m_goingClosed = true;
          if (!m_isAnswered)
            checkAnswer(false);
          stopExamSlot();
          allowToCLose = true;
      }
      delete msg;
  }
  return allowToCLose;
}


QString TexamExecutor::saveExamToFile() {
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getSaveFileName(getExamFileName(m_exam), QStringLiteral("noo"));
#else
  QString fileName = TfileDialog::getSaveFileName(tr("Save exam results as:"), getExamFileName(m_exam), TexTrans::examFilterTxt());
#endif
  QString noo = QStringLiteral(".noo");
  if (fileName.isEmpty()) {
      auto msg = new QMessageBox(nullptr);
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
  if (CURR_Q->melody()) {
      if (!SOUND->melodyIsPlaying()) {
        CURR_Q->melody()->setMetronome(SOUND->tempo(), static_cast<Tmeter::EbeatUnit>(SOUND->beatUnit()));
        int nrTicksBefore = SOUND->tickBeforePlay() ? CURR_Q->melody()->meter()->countTo() : 0;
        SOUND->runMetronome(nrTicksBefore);
      }
      SOUND->playMelody(CURR_Q->melody(), CURR_Q->melody()->key() != CURR_Q->key ? CURR_Q->melody()->key().difference(CURR_Q->key) : 0);
      if (SOUND->melodyIsPlaying()) // the same method can stop a melody
        CURR_Q->lastAttempt()->melodyWasPlayed(); // so increase only when playing was started
  } else
      SOUND->play(CURR_Q->qa.note);
  connectPlayingFinished();
}


void TexamExecutor::playMiddleA() {
  SOUND->stopPlaying();
  SOUND->play(Tnote(6, 1, 0));
  connectPlayingFinished();
}


void TexamExecutor::connectPlayingFinished() {
  if (m_soundTimer->isActive())
      m_soundTimer->stop();
  if (CURR_Q->answerAsSound())
    connect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying);
}


void TexamExecutor::noteOfMelodyStarted(const TnoteStruct& n) {
  if (n.pitch.isRest() && m_melody->currentIndex() > 0) { // if no rhythms - no rest here, no need for check
    // If rest is detected but sound is expected - skip it.
    // Do not jump to the next note
    if (m_melody->nextToPlay() && !m_melody->nextToPlay()->isRest())
      return;
  }

  if (m_melody->wasIndexChanged())
    CURR_Q->lastAttempt()->melodyWasPlayed();
  m_melody->noteStarted();
  if (m_melody->currentIndex() == 0) { // first played note was detected
    CURR_Q->lastAttempt()->setPrepareTime(m_penalty->elapsedTime() - quint32(n.duration));
    m_melodySelectionIndex = 1; // reset it here
  }
  if (m_exercise && CURR_Q->melody()->meter()->meter() == Tmeter::NoMeter && GLOB->waitForCorrect()) {
    // no need to call real note id - no rests or ties when no meter
      int expected = CURR_Q->melody()->note(m_melody->currentIndex())->p().chromatic();
      int played = n.pitch.chromatic();
      if (!m_level.requireOctave) {
        expected = expected % 12;
        played = played % 12;
      }
      if (expected == played) {
          if (m_melody->currentIndex() + 1 < CURR_Q->melody()->length()) // highlight next note
            MAIN_SCORE->setSelectedItem(m_melody->currentIndex() + 1);
          MAIN_SCORE->markNoteHead(GLOB->correctColor(), m_melody->currentIndex());
          if (GLOB->extraNames())
            MAIN_SCORE->showNoteName(m_melody->currentIndex(), true);
          m_melody->setNote(n);
      } else {
          MAIN_SCORE->markNoteHead(GLOB->wrongColor(), m_melody->currentIndex());
      }
  } else { //TODO Use m_melody->realNoteId() to get score note id
      if (m_melodySelectionIndex < CURR_Q->melody()->length()) // highlight next note
        m_melodySelectionIndex += MAIN_SCORE->setSelectedItem(m_melodySelectionIndex); // tied notes respected
      else
        m_melodySelectionIndex++;
  }
}


void TexamExecutor::noteOfMelodyFinished(const TnoteStruct& n) {
  if (m_melody->currentIndex() < 0) // meanwhile new question melody was asked - some undesired note was finished
    return;

  bool waitForCorrect = m_exercise && CURR_Q->melody()->meter()->meter() == Tmeter::NoMeter && GLOB->waitForCorrect();
  if (!waitForCorrect) {
    bool doSetNote = true;
    if (n.pitch.isRest()) { // no rest here when rhythms are disabled
      if (!m_melody->currentToPlay()->isRest()) {
        // append rest duration to previously detected note,
        // but only when that note was at least 1,5 sec long.
        // Longer would be faded out, so help an user here.
        // But shorter notes will be treat as mistakes - user cut them.
        // TODO this 1.5 sec. is debatable - might be to long for some fade-out instruments
        auto currNote = m_melody->currentNote();
        if (currNote && GLOB->instrument().isFadeOut() && currNote->duration > 1.5)
          m_melody->currentNote()->duration += n.duration;
        doSetNote = false;
        qDebug() << "=== skip rest finished";
      }
    }
    if (doSetNote)
      m_melody->setNote(n);
  }
  if ((waitForCorrect && m_melody->currentIndex() == CURR_Q->melody()->length() - 1) || m_melodySelectionIndex > CURR_Q->melody()->length()) {
    if (waitForCorrect && !m_melody->wasLatestNoteSet())
      return;
    if (GLOB->E->expertsAnswerEnable)
        checkAnswer();
    else {
        m_tipHandler->playMelodyAgainMessage();
        m_tipHandler->showConfirmTip(800);
        SOUND->stopListen();
    }
  }
}


void TexamExecutor::noteOfMelodySelected(int nr) {
  m_melody->setCurrentIndex(nr);
  MAIN_SCORE->setSelectedItem(nr);
  SOUND->startListen();
  m_tipHandler->deleteConfirmTip();
  m_melodySelectionIndex = nr + 1;
// TODO: do we really need this?
// During exercises, display instrument position of clicked note for a hint
//   if (isExercise() && INSTRUMENT && CURR_Q->melody())
//     INSTRUMENT->setNote(CURR_Q->melody()->note(nr)->p(), CURR_Q->melody()->note(nr)->g().data());
}


void TexamExecutor::showExamHelp() {
  m_snifferLocked = true;
// #if !defined (Q_OS_ANDROID)
//   qApp->removeEventFilter(m_supp);
// #endif
  auto examHelpDialog = new TexamHelp(Tcolor::bgTag(Tcolor::alpha(GLOB->EquestionColor, 40)),
                                      Tcolor::bgTag(Tcolor::alpha(GLOB->EanswerColor, 40)), &GLOB->E->showHelpOnStart, nullptr);
  examHelpDialog->exec();
  delete examHelpDialog;
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp);
// #endif
  m_snifferLocked = false;
}


QString TexamExecutor::examHelpText() {
  TexamHelp examHelpDialog(Tcolor::bgTag(Tcolor::alpha(GLOB->EquestionColor, 40)), Tcolor::bgTag(Tcolor::alpha(GLOB->EanswerColor, 40)), &GLOB->E->showHelpOnStart, nullptr);
  return examHelpDialog.helpText()->toHtml();
}



void TexamExecutor::sniffAfterPlaying() {
  disconnect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying);
  if (m_soundTimer->isActive())
    m_soundTimer->stop();
  m_soundTimer->start(100);
}


void TexamExecutor::startSniffing() {
  if (m_soundTimer->isActive())
    m_soundTimer->stop();
#if !defined (Q_OS_ANDROID)
    if (CURR_Q->answerAsSound() && !GLOB->A->dumpPath.isEmpty()) {
      QString dumpFileName = QString("Question-%1").arg(m_exam->count(), 3, 'i', 0, '0');
      if (m_melody)
        dumpFileName += QString("-attempt%1").arg(CURR_Q->attemptsCount());
      SOUND->setDumpFileName(dumpFileName);
    }
#endif
  if (SOUND->isSnifferPaused())
    SOUND->unPauseSniffing();
  else
    SOUND->startListen(false);
}


void TexamExecutor::expertAnswersSlot() {
  if (!GLOB->E->expertsAnswerEnable && !m_exam->melodies()) { // no expert and no melodies
      m_tipHandler->showConfirmTip(1500);
      return;
  }
  // ignore slot when some dialog window appears or answer for melody
  if (m_snifferLocked || (m_exam->count() && CURR_Q->melody())) 
      return;

  if (CURR_Q->answerAsSound())
      SOUND->pauseSinffing();
  QTimer::singleShot(0, [=]{ checkAnswer(); });
}


/**
 * This slot is invoked  during correction of melody on the score.
 * Each note can be clicked and:
 * - corrected or show hint, if score is an answer
 * - shows position on the instrument
 * - plays its sound
 * - displays message with detected pitch if note was played wrong
 */
void TexamExecutor::correctNoteOfMelody(int noteNr) {
  if (CURR_Q->melody()) {
    MAIN_SCORE->setSelectedItem(noteNr);
    if (noteNr < CURR_Q->lastAttempt()->mistakes.size()) {
      quint32 &m = CURR_Q->lastAttempt()->mistakes[noteNr];
      if (CURR_Q->answerOnScore()) {
          if (CURR_Q->melody()->length() > noteNr) { // only dictations can be corrected
            if (m && !MAIN_SCORE->isCorrectAnimPending()) { // fix, if it has not been fixed yet
              if (!m_melody->fixed(noteNr))
                m_exercise->setCorrectedNoteId(noteNr); // TODO we are not using it anymore
              if (m_level.useRhythms()) {
                  // TODO It is impossible for now to correct melody with rhythms
                  // because when some middle note changes rhythms, ties are added and notes number changes.
                  // So score answer don't match what was checked before.
                  // So far we just displaying status message with correct note
                  NOO->setMessageColor(m_supp->answerColor(m));
                  m_tipHandler->shouldBeNoteTip(CURR_Q->melody()->note(noteNr)->p());
              } else if (!m_melody->fixed(noteNr))
                  MAIN_SCORE->correctNote(CURR_Q->melody()->note(noteNr)->p()); // selected note item is corrected
              if (!m_melody->fixed(noteNr))
                m_melody->setFixed(noteNr);
              if (m_melody->numberOfFixed() > CURR_Q->melody()->length() / 2) { // too much notes fixed - hide 'new attempt'
                m_newAtemptAct->setEnabled(false);
                m_tipHandler->showWhatNextTip(true); // cheat m_tipHandler that question is correct so 'new attempt' will not show
              }
            }
          } else {
              NOO->setMessageColor(GLOB->wrongColor());
              NOO->showTimeMessage(tr("There is not such a note in this melody!"), 3000);
          }
      } else if (CURR_Q->answerAsSound()) {
          auto corrNote = m_melody->listenedFromReal(noteNr);
          if (corrNote) {
              NOO->setMessageColor(m_supp->answerColor(m));
              if (m_level.useRhythms() || corrNote->pitch.isValid())
                m_tipHandler->detectedNoteTip(corrNote->pitch);
              else
                NOO->showTimeMessage(tr("This note was not played!"), 3000);
          } else
              qDebug() << "[TexamExecutor] FIXME! Wrong corrected note" << noteNr << ". It shouldn't happen at all!";
      }
      if (SOUND->isPlayable() && CURR_Q->melody()->length() > noteNr)
        SOUND->play(CURR_Q->melody()->note(noteNr)->p());
      if (INSTRUMENT && CURR_Q->melody()->length() > noteNr)
        INSTRUMENT->setNote(CURR_Q->melody()->note(noteNr)->p());
    }
  }
}


// void TexamExecutor::rightButtonSlot() {
//   if (m_lockRightButt)
//       return;
//   if (m_isAnswered)
//       askQuestion();
//   else {
//       checkAnswer();
//   }
// }


void TexamExecutor::tipLink(const QString& link) {
  if (link == QLatin1String("nextQuest"))
      askQuestion();
  else if (link == QLatin1String("stopExam")) {
      if (m_exercise)
        stopExerciseSlot();
      else
        stopExamSlot();
  }
  else if (link == QLatin1String("prevQuest"))
      repeatQuestion();
  else if (link == QLatin1String("checkAnswer"))
      checkAnswer();
  else if (link == QLatin1String("examHelp"))
      showExamHelp();
  else if (link == QLatin1String("correct"))
      correctAnswer();
  else if (link == QLatin1String("certClosing")) {
      m_tipHandler->deleteCertTip();
      unlockAnswerCapturing();
  } else if (link == QLatin1String("newAttempt"))
      newAttempt();
 }


void TexamExecutor::deleteExam() {
  if (m_exam) {
    delete m_exam;
    m_exam = nullptr;
  }
}


void TexamExecutor::unlockAnswerCapturing() {
  if (CURR_Q->answerAsSound())
    SOUND->startListen();
  m_penalty->continueTime();
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp); // restore grabbing right mouse button
// #endif
  m_snifferLocked = false;
}


void TexamExecutor::blindQuestion() {
  qDebug() << "Blind question - asking again";
  m_blindCounter++;
  QTimer::singleShot(10, [=]{ askQuestion(); });
}


void TexamExecutor::correctionFinishedSlot() {
  if (m_disconnectAfterAnim) {
    disconnect(sender(), SIGNAL(correctionFinished()), this, SLOT(correctionFinishedSlot()));
    m_disconnectAfterAnim = false;
  }
//   if (sender() == SCORE) { // show name on score only when it is enabled and corrected
//     if (GLOB->E->showNameOfAnswered && m_exercise->idOfCorrectedNote() > -1) {
//       Tnote::EnameStyle tmpStyle = Tnote::defaultStyle; // store current name style
//       Tnote::defaultStyle = CURR_Q->styleOfQuestion(); // set style of question
//       SCORE->noteFromId(m_exercise->idOfCorrectedNote())->showNoteName(QColor(GLOB->EanswerColor.lighter().name())); // show note name
//       Tnote::defaultStyle = tmpStyle; // restore style
//     }
//   }
  m_nextQuestAct->setEnabled(true);
  if (GLOB->E->autoNextQuest && GLOB->E->afterMistake != TexamParams::e_stop && !CURR_Q->melody()) {
    m_askingTimer->start(GLOB->E->correctPreview); // new question will be started after preview time
  }
  if (CURR_Q->melody()) { // despite of 'auto' settings when melody - auto next question will not work
      m_tipHandler->showWhatNextTip(false, false);
//     connect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody); // only once per answer
  } else if (!GLOB->E->autoNextQuest || GLOB->E->afterMistake == TexamParams::e_stop)
        m_tipHandler->showWhatNextTip(!(!m_exercise && GLOB->E->repeatIncorrect && !m_incorrectRepeated));
//   if (CURR_Q->melody() && (CURR_Q->questionOnScore() || CURR_Q->answerOnScore()))
//       m_tipHandler->melodyCorrectMessage();
//   if (!GLOB->E->autoNextQuest || !GLOB->E->showCorrected || GLOB->E->afterMistake == TexamParams::e_stop)
//       QTimer::singleShot(4000, m_tipHandler, SLOT(clearResultTip())); // exam will stop so clear result tip after correction
//   m_lockRightButt = false;
}


bool TexamExecutor::castLevelFromQVariant(const QVariant& v) {
  auto l = qvariant_cast<Tlevel*>(v);
  if (l) {
      m_level = *l;
      return true;
  } else {
      return false;
  }
}


Taction* TexamExecutor::clearScoreAct() {
  return MAIN_SCORE->clearScoreAct();
}

//#################################################################################################
//###################                QML               ############################################
//#################################################################################################
QString TexamExecutor::title() const {
  if (!m_tipHandler)
    return QStringLiteral("Nootka");

  if (m_exercise)
      return tr("Exercises with Nootka");
  else
      return tr("EXAM!") + QLatin1String(" ") + m_exam->userName() + QLatin1String(" - ") + m_level.name;
}


TtipHandler* TexamExecutor::tipHandler() { return m_tipHandler; }


bool TexamExecutor::showPitchView() const {
  return m_exam && m_exam->count() && CURR_Q->answerAsSound();
}


bool TexamExecutor::showRtmView() const {
  return m_exam && m_exam->count() && (CURR_Q->answerAsSound() || CURR_Q->questionAsSound()) && m_level.canBeMelody();
}
