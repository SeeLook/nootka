/***************************************************************************
 *   Copyright (C) 2011-2018 by Tomasz Bojczuk                             *
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
#include "texamhelp.h"
#include <taudioparams.h>
#include <texamparams.h>
#include <tscoreparams.h>
// #include <tlayoutparams.h>
#include <music/tmelody.h>
#include <music/tchunk.h>
#include <taction.h>
#include <tnootkaqml.h>
#include <instruments/tcommoninstrument.h>
#include "tnameitem.h"
#include "tmainscoreobject.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qtimer.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qsettings.h>
#include <QtQml/qqmlengine.h>
#if defined (Q_OS_ANDROID)
  #include <Android/tfiledialog.h>
#else
  #include <QtWidgets/qfiledialog.h>
#endif

#include <QtCore/qdebug.h>


#if defined (Q_OS_ANDROID)
  #define WAIT_TIME (500) //[ms]
#else
  #define WAIT_TIME (150) //[ms]
#endif
#define SOUND_DURATION (1500) //[ms]

#define INSTRUMENT NOO->instrument()


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
  if (QFileInfo(fName  + QLatin1String(".noo")).exists())
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
  qDebug() << "[TexamExecutor] destroyed";
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
  if (m_exam->melodies())
    m_melody = new TexamMelody(this);
  if (m_questList.isEmpty()) {
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
//   connect(m_penalty, SIGNAL(certificate()), this, SLOT(displayCertificate()));
  if (m_exercise) {
    if (GLOB->E->suggestExam)
      m_exercise->setSuggestionEnabled(m_supp->qaPossibilities(), m_exam->melodies());
  } else {
//       connect(m_tipHandler, SIGNAL(certificateMagicKeys()), this, SLOT(displayCertificate()));
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
    m_rand = new TequalRand(m_questList.size());
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
  auto curQ = m_exam->curQ();
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
      m_tipHandler->clearCanvas();
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
      int melodyLength = qBound(qMax(2, qRound(m_level.melodyLen * 0.7)), //at least 70% of length but not less than 2
                                      qRound(((6.0 + (qrand() % 5)) / 10.0) * (qreal)m_level.melodyLen), (int)m_level.melodyLen);
      if (m_penalty->isNot()) {
        curQ->addMelody(QString("%1").arg(m_exam->count()));
        curQ->melody()->setKey(curQ->key);
        if (m_level.randMelody == Tlevel::e_randFromList) {
            QList<TQAgroup> qaList;
            m_supp->listForRandomNotes(curQ->key, qaList);
            // ignore in key (4th param) of level, notes from list are already in key (if required)
            getRandomMelodyNG(qaList, curQ->melody(), melodyLength, false, false);
        } else
            getRandomMelodyNG(m_questList, curQ->melody(), melodyLength, m_level.onlyCurrKey, m_level.endsOnTonic);
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
  if (curQ->questionOnScore()) {
    if (curQ->melody()) {
        if (!isAttempt) {
          bool ignoreTechnical = GLOB->instrument().type() != Tinstrument::Bandoneon;
          MAIN_SCORE->askQuestion(curQ->melody(), ignoreTechnical);
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
          MAIN_SCORE->setSelectedItem(0); // mark first note
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
      if (!isAttempt) // play melody but not when user tries again
        repeatSound();
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
      if (curQ->questionOnScore()) {// note has to be another than question
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

  if (curQ->answerAsSound()) {
//       SOUND->prepareAnswer();
      if (curQ->questionAsSound())
          connect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying); // sniffing after finished sound
      else
          QTimer::singleShot(WAIT_TIME, [=]{ startSniffing(); });
          // Give a student some time to prepare itself for next question in expert mode
          // It avoids capture previous played sound as current answer
  } else
      SOUND->stopListen(); // stop sniffing if answer is not a played sound

//   TOOLBAR->setForQuestion(curQ->questionAsSound(), curQ->questionAsSound() && curQ->answerAsNote());
  m_checkQuestAct->setEnabled(true);
  m_repeatQuestAct->setEnabled(false);
  m_nextQuestAct->setEnabled(false);
  if (curQ->questionAsSound() && m_playAgainAct)
    m_playAgainAct->setEnabled(true);
  // TODO tune fork act (play middle A)
  m_penalty->startQuestionTime();
  if (!curQ->answerAsSound() && SOUND->stoppedByUser())
    m_tipHandler->questionTip();
  else // As long as starting sound is expensive (PulseAudio) invoking tip animation at the same time sucks.
    QTimer::singleShot(WAIT_TIME + 100, [=]{ m_tipHandler->questionTip(); }); // So call it with delay
  m_blindCounter = 0; // question successfully asked - reset the counter
}


void TexamExecutor::checkAnswer(bool showResults) {
  TQAunit* curQ = m_exam->curQ();
  m_penalty->stopQuestionTime();
  m_checkQuestAct->setEnabled(false);
  if (m_playAgainAct)
    m_playAgainAct->setEnabled(false);
//   TOOLBAR->setAfterAnswer();
  if (curQ->answerAsSound()) {
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
//             SCORE->getMelody(&answMelody);
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
    if (!(GLOB->waitForCorrect() && m_exercise && m_melody))
      m_tipHandler->resultTip(curQ); // tip duration is calculated by itself (inside resultTip() method)
    if ((!m_exercise || (m_exercise && curQ->isCorrect())) && !autoNext)
      m_tipHandler->whatNextTip(curQ->isCorrect());
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
            m_tipHandler->whatNextTip(true, true);
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
  m_tipHandler->clearCanvas();
  TQAunit* curQ = m_exam->answList()->last();
  QColor markColor = m_supp->answerColor(curQ);
//   if (curQ->melody() && (curQ->answerAsNote() || curQ->questionAsNote())) {
//     SCORE->setReadOnlyReacting(true); // It is undone whenever unLockScore() is called
//   }
  if (curQ->answerOnScore()) {
    if (curQ->melody()) {

    } else {
        Tnote goodNote = curQ->questionOnScore() ? curQ->qa_2.note : curQ->qa.note;
        char key = MAIN_SCORE->keySignatureValue();
        bool correctAccid = curQ->wrongAccid() || curQ->wrongOctave();
//         if (curQ->wrongAccid() || curQ->wrongOctave()) // it corrects wrong octave as well
//             SCORE->correctAccidental(goodNote);
//         else if (curQ->wrongNote()) {/*
            if (m_level.manualKey /*&& curQ->key.value() != SCORE->keySignature().value()*/)
              key = curQ->key.value();
//               SCORE->correctKeySignature(curQ->key);
            m_exercise->setCorrectedNoteId(0);
//             SCORE->correctNote(goodNote, markColor);
//         }
//         if (curQ->wrongKey())
//             SCORE->correctKeySignature(curQ->key);*/
        MAIN_SCORE->correctNote(goodNote, key, correctAccid);
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
//     m_lockRightButt = true;
  } else
      correctionFinishedSlot();
}


void TexamExecutor::newAttempt() {
  m_tipHandler->tryAgainTip(3000);
//   QTimer::singleShot(2000, m_tipHandler, SLOT(clearResultTip())); 
  if (m_exam->curQ()->answerOnScore() || m_exam->curQ()->questionOnScore()) { // remove names and marks from score notes
//     for (int i = 0; i < MAIN_SCORE->notesCount(); ++i) {
//       if (m_exercise) {
//           SCORE->deleteNoteName(i);
//           if (m_exam->curQ()->lastAttempt()->mistakes[i] != TQAunit::e_correct)
//             SCORE->markQuestion(-1, i); // leave note marks (colored borders) only for correct answers
//       } else // although clear all marked notes in exams
//           SCORE->markQuestion(-1, i);
    }
  // prepare list to store notes played by user or clear it
  m_melody->newMelody(m_exam->curQ()->answerAsSound() ? m_exam->curQ()->melody()->length() : 0);
//   m_melody->clearToFix(m_exam->curQ()->melody()->length());
  m_penalty->newAttempt();
  if (m_exam->curQ()->answerAsSound())
        m_exam->curQ()->lastAttempt()->melodyWasPlayed(); // we can suppose that user will play an answer for sure 
  askQuestion(true);
}


void TexamExecutor::markAnswer(TQAunit* curQ) {
  QColor markColor = m_supp->answerColor(curQ);
  if (curQ->melody()) {
    for (int i = 0; i < curQ->lastAttempt()->mistakes.size(); ++i) {
      MAIN_SCORE->markNoteHead(m_supp->answerColor(curQ->lastAttempt()->mistakes[i]), i);
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
  m_tipHandler->tryAgainTip(3000);
//   m_lockRightButt = false;
  m_incorrectRepeated = true;
  m_isAnswered = false;
//   if (GLOB->E->showNameOfAnswered) {
//     for (int i = 0; i < 2; i++)
//       SCORE->deleteNoteName(i);
//     INSTRUMENT->deleteNoteName();
//   }
// for melodies it never comes here - questions are newer repeated - copying of TQAunit is safe 
  TQAunit curQ(*m_exam->curQ()); // copy last unit as a new one

  if (!GLOB->E->autoNextQuest)
      m_tipHandler->clearCanvas();
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
//   TOOLBAR->setForQuestion(m_exam->curQ()->questionAsSound(), m_exam->curQ()->questionAsSound() && m_exam->curQ()->answerAsNote());
  if (m_exam->curQ()->questionAsSound())
    repeatSound();
  m_tipHandler->questionTip();
  m_penalty->startQuestionTime();
}


// void TexamExecutor::displayCertificate() {
//   m_snifferLocked = true;
//   SOUND->stopListen();
//   m_penalty->pauseTime();
// #if !defined (Q_OS_ANDROID)
//   qApp->removeEventFilter(m_supp); // stop grabbing right button and calling checkAnswer()
// #endif
//   m_tipHandler->certificateTip();
// }

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


// #if !defined (Q_OS_ANDROID) // Do not show it user Android - it sucks there
//   SOUND->pitchView()->setVisible(GLOB->L->soundViewEnabled);
// #endif
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
  connect(m_tipHandler, &TtipHandler::destroyTips, this, &TexamExecutor::destroyTips);
  emit tipHandlerCreated();
  m_tipHandler->startTip();
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

//   SOUND->pitchView()->setVisible(GLOB->L->soundViewEnabled);
//   INSTRUMENT->setVisible(GLOB->L->guitarEnabled);
  
// #if defined (Q_OS_ANDROID) // revert actions
//   if (!m_level.answerIsSound()) {
//     SOUND->pitchView()->pauseAction()->setVisible(true);
//     MAINVIEW->flyActions()->append(SOUND->pitchView()->pauseAction());
//   }
// #endif
  SOUND->acceptSettings();
//   SOUND->pitchView()->setIntonationAccuracy(GLOB->A->intonation);
//   SOUND->pitchView()->enableAccuracyChange(true);

  if (NOTENAME) {
    NOTENAME->setDisabled(false);
    NOTENAME->setButtonNameStyle(GLOB->S->nameStyleInNoteName);
  }
  INSTRUMENT->setEnabled(true);

  if (m_tipHandler)
      m_tipHandler->deleteLater();
  m_tipHandler = nullptr;
  emit titleChanged();

  MAIN_SCORE->setReadOnly(false);
//   INSTRUMENT->deleteRangeBox();
  SOUND->restoreAfterExam();
}


void TexamExecutor::disableWidgets() {
  MAIN_SCORE->setReadOnly(true);
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
  INSTRUMENT->setNote(Tnote());
  INSTRUMENT->markSelected(Qt::transparent);
//   SOUND->restoreAfterAnswer();
}


void TexamExecutor::createActions() {
  QQmlComponent actionsComp(NOO->qmlEngine(), this);
  m_settAct = new Taction(QApplication::translate("TtoolBar", "Settings"), QStringLiteral("exam-settings"), this);
  m_examActions.append(m_settAct);
  connect(m_settAct, &Taction::triggered, this, &TexamExecutor::prepareToSettings);
  m_helpAct = new Taction(QApplication::translate("TtoolBar", "Help"), QStringLiteral("help"), this);
  m_examActions.append(m_helpAct);
  connect(m_helpAct, &Taction::triggered, this, &TexamExecutor::showExamHelp);
  m_stopExamAct = new Taction(QApplication::translate("TtoolBar", "Stop"), QStringLiteral("stopExam"), this);
  if (m_exercise)
    connect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExerciseSlot);
  else
    connect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExamSlot);
  m_examActions.append(m_stopExamAct);
  m_repeatQuestAct = new Taction(QApplication::translate("TtoolBar", "Repeat", "like a repeat question"), QStringLiteral("prevQuest"), this, false);
  connect(m_repeatQuestAct, &Taction::triggered, this, &TexamExecutor::repeatQuestion);
  m_examActions.append(m_repeatQuestAct);
  actionsComp.setData("import QtQuick 2.9; Shortcut { sequence: \"Backspace\" }", QUrl());
  m_repeatQuestAct->setShortcut(createQmlShortcut(&actionsComp));
  m_nextQuestAct = new Taction(QApplication::translate("TtoolBar", "Next", "like a next question"), QStringLiteral("nextQuest"), this);
  m_examActions.append(m_nextQuestAct);
  connect(m_nextQuestAct, &Taction::triggered, this, &TexamExecutor::askQuestionSlot);
  actionsComp.setData("import QtQuick 2.9; Shortcut { sequence: \"Space\" }", QUrl());
  m_nextQuestAct->setShortcut(createQmlShortcut(&actionsComp));
  if (m_level.canBeMelody()) {
    m_newAtemptAct = new Taction(QApplication::translate("TtoolBar", "Try again"), "prevQuest", this, false);
    m_examActions.append(m_newAtemptAct);
    connect(m_newAtemptAct, &Taction::triggered, this, &TexamExecutor::newAttempt);
  }
  if (m_level.questionAs.isSound()) {
    m_playAgainAct = new Taction(QApplication::translate("TtoolBar", "Play"), QStringLiteral("playMelody"), this, false);
    m_examActions.append(m_playAgainAct);
  }
  m_checkQuestAct = new Taction(QApplication::translate("TtoolBar", "Check", "like a check answer"), QStringLiteral("check"), this, false);
  m_examActions.append(m_checkQuestAct);
  connect(m_checkQuestAct, &Taction::triggered, this, &TexamExecutor::checkAnswerSlot);
  actionsComp.setData("import QtQuick 2.9; Shortcut { sequence: \"Return\" }", QUrl());
  m_checkQuestAct->setShortcut(createQmlShortcut(&actionsComp));
  if (m_exercise) {
    m_correctAct = new Taction(QApplication::translate("TtoolBar", "Correct", "like a correct answer with mistake"), QStringLiteral("correct"), this, false);
    m_examActions.append(m_correctAct);
    connect(m_correctAct, &Taction::triggered, this, &TexamExecutor::correctAnswer);
    m_correctAct->setShortcut(createQmlShortcut(&actionsComp)); // Enter (Return) key
  }
  emit examActionsChanged();

//   if (m_level.questionAs.isSound() && m_level.answersAs[TQAtype::e_asSound].isNote()) {
//     TOOLBAR->createTuneForkAction();
//     connect(TOOLBAR->tuneForkAct, SIGNAL(triggered()), this, SLOT(playMiddleA()));
//   }
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
  initializeExecuting();
  disconnect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExerciseSlot);
  connect(m_stopExamAct, &Taction::triggered, this, &TexamExecutor::stopExamSlot);
  clearWidgets();
  emit examActionsChanged();
  emit titleChanged();
  m_tipHandler->clearCanvas();
  m_tipHandler->startTip();
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
      if (m_isAnswered && m_exam->curQ()->melody() && m_exam->curQ()->answerOnScore() && !m_exam->curQ()->isCorrect()) 
        m_exam->curQ()->melody()->setTitle(m_exam->curQ()->melody()->title() + QLatin1String(";skip"));
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
  if ((m_exam->count() == 1 && m_exam->curQ()->answered()) || m_exam->count() > 1)
    m_exam->saveToFile();
  closeExecutor();
}


void TexamExecutor::restoreExerciseAfterSummary() {
  if (m_summaryReason == SumFinishExer) {
    m_summaryReason = NoReason;
    if (m_isAnswered && m_exam->curQ()->melody() && m_exam->curQ()->answerOnScore() && !m_exam->curQ()->isCorrect()) // revert melody title
      m_exam->curQ()->melody()->setTitle(m_exam->curQ()->melody()->title().remove(QLatin1String(";skip")));
    if (m_isAnswered)
        m_exam->curQ()->setAnswered();
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
      if (m_exam->curQ()->answerAsSound())
        startSniffing();
  }
}


void TexamExecutor::stopExamSlot() {
  if (!m_isAnswered && !GLOB->E->closeWithoutConfirm) {
    m_shouldBeTerminated = true;
//   int messageDuration = 2000;
#if defined (Q_OS_ANDROID)
  messageDuration = 5000;
#else
    QColor c = GLOB->GfingerColor;
    c.setAlpha(30);
//     STATUS->setBackground(c);
#endif
//     m_tipHandler->setStatusMessage(tr("Give an answer first!<br>Then the exam will end."), messageDuration);
    qDebug() << "[TexamExecutor] Give an answer first! Then the exam will end.";
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
//   m_tipHandler->setStatusMessage(tr("Such a pity."), 5000);

  m_tipHandler->clearCanvas();
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
  }
  if (m_exam->count() && m_exam->curQ()->answerAsSound() && !SOUND->isSnifferPaused())//!SOUND->pitchView()->isPaused())
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
      if (m_exam->curQ()->answerAsSound())
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
                      "or <b>%2</b> to continue.<br>")
                  .arg(qTR("QShortcut", "Save"))
                  .arg(qTR("QPlatformTheme", "Retry")));
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
  QString fileName = TfileDialog::getSaveFileName(nullptr, getExamFileName(m_exam), QStringLiteral("noo"));
#else
  QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save exam results as:"), getExamFileName(m_exam), TexTrans::examFilterTxt());
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
    connect(SOUND, &Tsound::plaingFinished, this, &TexamExecutor::sniffAfterPlaying);
}


void TexamExecutor::noteOfMelodyStarted(const TnoteStruct& n) {
  if (m_melody->wasIndexChanged())
    m_exam->curQ()->lastAttempt()->melodyWasPlayed();
  m_melody->noteStarted();
  if (m_melody->currentIndex() == 0) // first played note was detected
    m_exam->curQ()->lastAttempt()->setPrepareTime(m_penalty->elapsedTime() - quint32(n.duration));
  if (m_exercise && GLOB->waitForCorrect()) {
      int expected = m_exam->curQ()->melody()->note(m_melody->currentIndex())->p().chromatic();
      int played = n.pitch.chromatic();
      if (!m_level.requireOctave) {
        expected = expected % 12;
        played = played % 12;
      }
      if (expected == played) {
          if (m_melody->currentIndex() + 1 < m_exam->curQ()->melody()->length()) // highlight next note
            MAIN_SCORE->setSelectedItem(m_melody->currentIndex() + 1);
          MAIN_SCORE->markNoteHead(GLOB->correctColor(), m_melody->currentIndex());
          if (GLOB->extraNames())
            MAIN_SCORE->showNoteName(m_melody->currentIndex(), true);
          m_melody->setNote(n);
      } else {
          MAIN_SCORE->markNoteHead(GLOB->wrongColor(), m_melody->currentIndex());
      }
  } else {
      if (m_melody->currentIndex() + 1 < m_exam->curQ()->melody()->length()) // highlight next note
        MAIN_SCORE->setSelectedItem(m_melody->currentIndex() + 1);
  }
}


void TexamExecutor::noteOfMelodyFinished(const TnoteStruct& n) {
  if (m_melody->currentIndex() < 0) // meanwhile new question melody was asked - some undesired note was finished
    return;

  bool waitForCorrect = GLOB->waitForCorrect() && m_exercise;
  if (!waitForCorrect)
    m_melody->setNote(n);
  if (m_melody->currentIndex() == m_exam->curQ()->melody()->length() - 1) {
    if (waitForCorrect && !m_melody->wasLatestNoteSet())
      return;
    if (GLOB->E->expertsAnswerEnable)
        checkAnswer();
    else {
        if (waitForCorrect)
            checkAnswer();
        else {
  //       m_tipHandler->playMelodyAgainMessage();
            m_tipHandler->confirmTip(800);
        }
      SOUND->stopListen();
    }
  }
}


void TexamExecutor::noteOfMelodySelected(int nr) {
  m_melody->setCurrentIndex(nr);
  MAIN_SCORE->setSelectedItem(nr);
  SOUND->startListen();
//   m_tipHandler->clearConfirmTip();
//   if (isExercise() && INSTRUMENT->isVisible() && m_exam->curQ()->melody()) // in exercises, display guitar position of clicked note for a hint
//       INSTRUMENT->setFinger(m_exam->curQ()->melody()->note(nr)->g());
}


void TexamExecutor::showExamHelp() {
  m_snifferLocked = true;
// #if !defined (Q_OS_ANDROID)
//   qApp->removeEventFilter(m_supp);
// #endif
  auto examHelpDialog = new TexamHelp(Tcolor::bgTag(GLOB->EquestionColor), Tcolor::bgTag(GLOB->EanswerColor), &GLOB->E->showHelpOnStart, nullptr);
  examHelpDialog->exec();
  delete examHelpDialog;
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp);
// #endif
  m_snifferLocked = false;
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
    if (m_exam->curQ()->answerAsSound() && !GLOB->A->dumpPath.isEmpty()) {
      QString dumpFileName = QString("Question-%1").arg(m_exam->count(), 3, 'i', 0, '0');
      if (m_melody)
        dumpFileName += QString("-attempt%1").arg(m_exam->curQ()->attemptsCount());
      SOUND->setDumpFileName(dumpFileName);
    }
#endif
  if (SOUND->isSnifferPaused())
    SOUND->unPauseSniffing();
  else
    SOUND->startListen();
}


void TexamExecutor::expertAnswersSlot() {
  if (!GLOB->E->expertsAnswerEnable && !m_exam->melodies()) { // no expert and no melodies
      m_tipHandler->confirmTip(1500);
      return;
  }
  // ignore slot when some dialog window appears or answer for melody
  if (m_snifferLocked || (m_exam->count() && m_exam->curQ()->melody())) 
      return;

  if (m_exam->curQ()->answerAsSound())
      SOUND->pauseSinffing();
  QTimer::singleShot(0, [=]{ checkAnswer(); });
}


// /** This slot is invoked  during correction of melody on the score.
//  * Each note can be clicked and:
//  * - corrected if score is an answer
//  * - shows position on the instrument
//  * - plays its sound
//  * - displays message with detected pitch if note was played wrong  */
// void TexamExecutor::correctNoteOfMelody(int noteNr) {
//   if (m_exam->curQ()->melody()) {
//     SCORE->selectNote(noteNr);
//     if (noteNr < m_exam->curQ()->lastAttempt()->mistakes.size()) {
//       quint32 &m = m_exam->curQ()->lastAttempt()->mistakes[noteNr];
//       if (m_exam->curQ()->answerAsNote() && m_exam->curQ()->melody()->length() > noteNr) { // only dictations can be corrected
//         if (m && !m_melody->fixed(noteNr) && !SCORE->isCorrectAnimPending()) { // fix if it has not been fixed yet
//           m_exercise->setCorrectedNoteId(noteNr);
//           SCORE->correctNote(m_exam->curQ()->melody()->note(noteNr)->p(), m_supp->answerColor(m), noteNr);
//           m_melody->setFixed(noteNr);
//           if (m_melody->numberOfFixed() > m_exam->curQ()->melody()->length() / 2) { // to much fixed - block new attempt
//             TOOLBAR->removeAction(TOOLBAR->attemptAct);
//             m_tipHandler->whatNextTip(true); // it will cheat m_tipHandler that question is correct and 'new attempt' will be blocked as well
//           }
//         }
//       }
//       if (SOUND->isPlayable() && m_exam->curQ()->melody()->length() > noteNr)
//           SOUND->play(m_exam->curQ()->melody()->note(noteNr)->p());
//       if (INSTRUMENT->isVisible() && m_exam->curQ()->melody()->length() > noteNr)
//         INSTRUMENT->setFinger(m_exam->curQ()->melody()->note(noteNr)->p());
//       if (m && m_exam->curQ()->answerAsSound()) {
//         if (m_melody->listened()[noteNr].pitch.isValid())
//           m_tipHandler->detectedNoteTip(m_melody->listened()[noteNr].pitch);
//         else
//           m_tipHandler->setStatusMessage(m_tipHandler->detectedText(tr("This note was not played!")), 3000);
//       }
//     }
//   }
// }
// 
// 
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
//   else if (name == QLatin1String("certClosing"))
//       unlockAnswerCapturing();
  else if (link == QLatin1String("newAttempt"))
      newAttempt();
 }


void TexamExecutor::deleteExam() {
  if (m_exam) {
    delete m_exam;
    m_exam = nullptr;
  }
}


// void TexamExecutor::delayerTip() {
//   m_lockRightButt = false;
//   m_tipHandler->whatNextTip(!(!m_exercise && GLOB->E->repeatIncorrect && !m_incorrectRepeated));
//   /** When exam mode and mistake occurred it will be true,
//    * so whatNextTip(false) is invoked - whatNextTip displays repeat question hint
//    * otherwise (exercise and/or correct answer) @p whatNextTip(true) goes. */
// }


// void TexamExecutor::unlockAnswerCapturing() {
//   if (m_exam->curQ()->answerAsSound())
//     SOUND->startListen();
//   m_penalty->continueTime();
// #if !defined (Q_OS_ANDROID)
//   qApp->installEventFilter(m_supp); // restore grabbing right mouse button
// #endif
//   m_snifferLocked = false;
// }


void TexamExecutor::blindQuestion() {
  qDebug() << "Blind question - asking again";
  m_blindCounter++;
  QTimer::singleShot(10, [=]{ askQuestion(); });
}


void TexamExecutor::correctionFinishedSlot() {
  disconnect(sender(), SIGNAL(correctionFinished()), this, SLOT(correctionFinishedSlot()));
//   if (sender() == SCORE) { // show name on score only when it is enabled and corrected
//     if (GLOB->E->showNameOfAnswered && m_exercise->idOfCorrectedNote() > -1) {
//       Tnote::EnameStyle tmpStyle = Tnote::defaultStyle; // store current name style
//       Tnote::defaultStyle = m_exam->curQ()->styleOfQuestion(); // set style of question
//       SCORE->noteFromId(m_exercise->idOfCorrectedNote())->showNoteName(QColor(GLOB->EanswerColor.lighter().name())); // show note name
//       Tnote::defaultStyle = tmpStyle; // restore style
//     }
//   }
  m_nextQuestAct->setEnabled(true);
  if (GLOB->E->autoNextQuest && GLOB->E->afterMistake != TexamParams::e_stop && !m_exam->curQ()->melody()) {
    m_askingTimer->start(GLOB->E->correctPreview); // new question will be started after preview time
  }
  if (m_exam->curQ()->melody()) { // despite of 'auto' settings when melody - auto next question will not work
      m_tipHandler->whatNextTip(false, false);
//     connect(SCORE, &TmainScore::lockedNoteClicked, this, &TexamExecutor::correctNoteOfMelody); // only once per answer
  } else if (!GLOB->E->autoNextQuest || GLOB->E->afterMistake == TexamParams::e_stop)
        m_tipHandler->whatNextTip(!(!m_exercise && GLOB->E->repeatIncorrect && !m_incorrectRepeated));
//   if (m_exam->curQ()->melody() && (m_exam->curQ()->questionOnScore() || m_exam->curQ()->answerOnScore()))
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
  } else { // TODO It should never happened, delete it when checked
      qDebug() << "[TexamExecutor] CAN'T CAST Tlevel* FROM QVariant!!!";
      return false;
  }
}


QObject* TexamExecutor::createQmlShortcut(QQmlComponent* qmlComp) {
  auto shortcut = qmlComp->create(qmlContext(this));
  if (shortcut)
    shortcut->setParent(this);
  else
    qDebug() << "[TexamExecutor] Can't create shortcut";
  return shortcut;
}


//#################################################################################################
//###################                QML               ############################################
//#################################################################################################
QString TexamExecutor::title() const {
  if (!m_tipHandler)
    return QStringLiteral("Nootka");

  if (m_exercise) {
      return tr("Exercises with Nootka");
//       TOOLBAR->startExamAct->setStatusTip(tr("finish exercising"));
  } else {
      return tr("EXAM!") + QLatin1String(" ") + m_exam->userName() + QLatin1String(" - ") + m_level.name;
//       TOOLBAR->startExamAct->setStatusTip(tr("stop the exam"));
  }
}


TtipHandler* TexamExecutor::tipHandler() { return m_tipHandler; }
