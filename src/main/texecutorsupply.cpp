/***************************************************************************
 *   Copyright (C) 2012-2021 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "texecutorsupply.h"
#include "tequalrand.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/tattempt.h>
#include <music/ttune.h>
#include <music/tmelody.h>
#include <instruments/tbandoneonbg.h>
#include <tnootkaqml.h>
#include <tglobals.h>
#include <tscoreparams.h>
#if defined (Q_OS_ANDROID)
//   #include <ttouchmessage.h>
#else
//   #include <QMouseEvent>
  #include <iostream>
#endif
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>


#if !defined (Q_OS_ANDROID)
void debugMelody(Tmelody* mel) {
  for (int i = 0; i < mel->length(); ++i)
    std::cout << mel->note(i)->p().toText().toStdString() << "\t";
  std::cout << "\n";
}


void debugNotesStruct(QList<TnoteStruct>& st) {
  Tmelody mel;
  mel.fromNoteStruct(st);
  debugMelody(&mel);
}
#endif

/*static*/

bool TexecutorSupply::m_paramsMessage = false;
bool TexecutorSupply::m_playCorrections = true;


void TexecutorSupply::checkPlayCorrected(Tlevel* level) {
  m_playCorrections = true;
  if (level->instrument == Tinstrument::NoInstrument) {
    if (level->answerIsSound())
      if (GLOB->instrument().type() != Tinstrument::NoInstrument)
        if (level->inScaleOf(GLOB->loString().chromatic(), GLOB->hiNote().chromatic()))
          m_playCorrections = false;
  } else
      m_playCorrections = false;
}


void TexecutorSupply::checkGuitarParamsChanged(Texam* exam) {
  checkPlayCorrected(exam->level());
  QString changesMessage;
  if (exam->level()->instrument != Tinstrument::NoInstrument) { // when instrument is guitar it has a matter
      if (exam->level()->instrument != GLOB->instrument().type())
          changesMessage = tr("Instrument type was changed!");
      GLOB->setInstrument(exam->level()->instrument);
  } // otherwise it reminds unchanged
  auto tmpTune = *GLOB->Gtune();
  int fretCount = GLOB->fretNumber();
  bool guitarParamsChanged = false;
  if ((exam->level()->canBeGuitar() || exam->level()->canBeSound()) && !m_playCorrections &&
    exam->tune() != *GLOB->Gtune() ) { // Is tune the same?
      if (!changesMessage.isEmpty())
        changesMessage += QLatin1String("<br>");
      tmpTune = exam->tune();
      guitarParamsChanged = true;
      changesMessage += tr("Tuning of the guitar was changed to:") + QLatin1String(" <b> ") + GLOB->Gtune()->name + QLatin1String("!</b>");
  }
  if (GLOB->instrument().isGuitar() && static_cast<unsigned int>(exam->level()->hiFret) > GLOB->GfretsNumber) { // Are enough frets?
      if (!changesMessage.isEmpty())
        changesMessage += QLatin1String("<br>");
      changesMessage += tr("Guitar fret number was changed!");
      fretCount = exam->level()->hiFret;
      guitarParamsChanged = true;
  }
  if (guitarParamsChanged)
    GLOB->setGuitarParams(fretCount, tmpTune);
  if (!changesMessage.isEmpty()) {
    QMessageBox::information(nullptr, QString(), changesMessage);
      m_paramsMessage = true;
  } else
      m_paramsMessage = false;
}


QColor& TexecutorSupply::answerColor(const TQAunit* answer) {
  return answerColor(answer->mistake());
}


QColor& TexecutorSupply::answerColor(quint32 mistake) {
  if (mistake == 0) // correct: TQAunit::e_correct
    return GLOB->EanswerColor;
  else if (!(mistake & TQAunit::e_wrongPos) && !(mistake & TQAunit::e_wrongNote) && !(mistake & TQAunit::e_veryPoor))
    return GLOB->EnotBadColor;
  else
    return GLOB->EquestionColor;
}

/* end of static */


TexecutorSupply::TexecutorSupply(Tlevel* level, QObject* parent) :
  QObject(parent),
  m_level(level),
  m_prevAccid(Tnote::e_Natural),
  m_dblAccidsCntr(0),
  m_eisCesCntr(0),
  m_isSolfege(false),
  m_wasFinished(false)
{
  m_loFret = m_level->loFret;
  m_hiFret = m_level->hiFret;
  calcQAPossibleCount();
  checkPlayCorrected(level);
  if (m_level->useKeySign && !m_level->isSingleKey)
    m_randKey = new TequalRand(m_level->keysInRange(), m_level->loKey.value());
  if (m_level->useRhythms()) {
    quint16 meterMask = 1;
    for (int b = 0; b < 12; ++b) {
      if (m_level->meters & meterMask)
        m_meterList << meterMask;
      meterMask <<= 1;
    }
    if (m_meterList.count() > 1)
      m_randMeter = new TequalRand(m_meterList.count());
  }
}


TexecutorSupply::~TexecutorSupply()
{
  if (m_randKey)
    delete m_randKey;
  if (m_randMeter)
    delete m_randMeter;
}


//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################


void TexecutorSupply::createQuestionsList(QList<TQAgroup> &list) {
  char openStr[6];
  for (int i = 0; i < GLOB->Gtune()->stringNr(); i++)
      openStr[i] = GLOB->Gtune()->str(i + 1).chromatic();

    /**
     * FIXING MISTAKE RELATED WITH A NEW VALIDATION WAY DURING SAVING NEW LEVEL
     * When there is no guitar in a level,
     * add to question list only the lowest position sounds.
     * In this way the list contains proper number of questions.
     */
  if (!m_level->canBeGuitar() && !m_level->answerIsSound())  // adjust fret range
    m_level->onlyLowPos = true;

  if (m_level->isMelodySet()) {
      qDebug() << "[TexecutorSupply] melody list, no question list";
  } else if (Tinstrument(m_level->instrument).isGuitar() && (!m_playCorrections || m_level->showStrNr || m_level->canBeGuitar())) {
      qDebug() << "[TexecutorSupply] Question list created fret by fret. Tune:" << GLOB->Gtune()->name << GLOB->Gtune()->stringNr();
      if (m_level->instrument == Tinstrument::NoInstrument && GLOB->instrument().type() != Tinstrument::NoInstrument) {
        char hi = m_hiFret, lo = m_loFret;
        if (!m_level->adjustFretsToScale(lo, hi))
            qDebug() << "Cant adjust fret range. Corrections will be played!";
        m_loFret = lo; 
        m_hiFret = hi;
      }
      if (m_level->loFret != m_loFret || m_level->hiFret != m_hiFret)
          qDebug() << "Fret range of a level adjusted to current instrument [" << m_loFret << m_hiFret << "]";
      for(int s = 0; s < GLOB->Gtune()->stringNr(); s++) {
          if (m_level->usedStrings[GLOB->strOrder(s)])// check string by strOrder
              for (int f = m_loFret; f <= m_hiFret; f++) {
                  Tnote n = Tnote(GLOB->Gtune()->str(GLOB->strOrder(s) + 1).chromatic() + f);
                if (n.chromatic() >= m_level->loNote.chromatic() &&
                      n.chromatic() <= m_level->hiNote.chromatic()) {
                  bool hope = true; // we still have hope that note is proper for the level
                  if (m_level->onlyLowPos) {
                    if (s > 0) {
                        // we have to check when note is on the lowest positions.
                        // Is it really lowest position when strOrder[s] is 0 - it is the highest sting
                        char diff = openStr[GLOB->strOrder(s-1)] - openStr[GLOB->strOrder(s)];
                        if( (f - diff) >= m_loFret && (f - diff) <= m_hiFret)
                            hope = false; //There is the same note on highest string
                        else
                            hope = true;
                    }
                  }
                  if (hope && m_level->useKeySign && m_level->onlyCurrKey)
                    hope = isNoteInKey(n);
                  if (hope) {
                      if (n.alter() && (!m_level->withFlats && !m_level->withSharps))
                          continue;
                      else {
                        Ttechnical tt(TfingerPos(GLOB->strOrder(s) + 1, f).data());
                        addToList(list, n, tt);
                      }
                  }
              }
          }
      }
  } else {
      qDebug() << "[TexecutorSupply] Question list created note by note";
      for (int nNr = m_level->loNote.chromatic(); nNr <= m_level->hiNote.chromatic(); nNr++) {
        Tnote n = Tnote(nNr);
        bool hope = true; // we still have hope that note is proper for the level
        if (hope && m_level->useKeySign && m_level->onlyCurrKey)
            hope = isNoteInKey(n);
        if (hope) {
          if (n.alter() && (!m_level->withFlats && !m_level->withSharps))
              continue;
          else {
              Ttechnical tt; // empty
              if (m_level->instrument == Tinstrument::Bandoneon/* && !m_level->onlyLowPos*/) {
                  auto bando = qobject_cast<TbandoneonBg*>(NOO->instrument());
                  if (bando) { // add to list every each occurrence of the same note on the bandoneon
                    short chromatic = n.chromatic();
                    if (bando->canBeRightOpen(chromatic)) {
                      n.setOnUpperStaff(true);
                      tt.setBowing(Ttechnical::BowDown);
                      addToList(list, n, tt);
                    }
                    if (bando->canBeRightClose(chromatic)) {
                      n.setOnUpperStaff(true);
                      tt.setBowing(Ttechnical::BowUp);
                      addToList(list, n, tt);
                    }
                    if (bando->canBeLeftOpen(chromatic)) {
                      n.setOnUpperStaff(false);
                      tt.setBowing(Ttechnical::BowDown);
                      addToList(list, n, tt);
                    }
                    if (bando->canBeLeftClose(chromatic)) {
                      n.setOnUpperStaff(false);
                      tt.setBowing(Ttechnical::BowUp);
                      addToList(list, n, tt);
                    }
                  }
              } else
                  addToList(list, n, tt);
          }
        }
      }
  }

//    for (int i = 0; i < list.size(); i++)
//        qDebug() << i << (int)list[i].pos.str() << "f"
//                << (int)list[i].pos.fret() << " note: "
//                << QString::fromStdString(list[i].note.getName());

  // generate m_fretFretList if needed
    m_fretFretList.clear();
    if (m_level->questionAs.isOnInstr() && m_level->answersAs[TQAtype::e_onInstr].isOnInstr()) {
      QList<TfingerPos> tmpSameList;
      for (int i = 0; i < list.size(); i++) {
        tmpSameList.clear();
        getTheSamePos(list[i].pos(), tmpSameList);
          if (!tmpSameList.isEmpty())
            m_fretFretList << static_cast<quint16>(i);
      }
//       for (int i = 0; i < m_fretFretList.size(); i++)
//        qDebug() << m_fretFretList.at(i) << (int)list[m_fretFretList.at(i)].pos.str() << "f"
//                << (int)list[m_fretFretList.at(i)].pos.fret() << " note: "
//                << QString::fromStdString(list[m_fretFretList.at(i)].note.getName());
//       qDebug() << "question list for only guitar created\nnumber:" << m_fretFretList.size() <<
//           " among:" << list.size();
    }

    qsrand(QDateTime::currentDateTimeUtc().toTime_t());

    if (m_level->canBeMelody()) {
        if (m_level->isMelodySet())
          m_obligQuestNr = m_level->melodySet.count() * m_level->repeatNrInSet * m_level->keysInRange();
        else
          m_obligQuestNr = qBound(5, 250 / m_level->melodyLen, 30); // longer melody - less questions
    } else
        m_obligQuestNr = qBound(20, list.size() * 4, 250);

    if (m_level->useKeySign && !m_level->isSingleKey)
        m_obligQuestNr = qMax(m_obligQuestNr, m_level->keysInRange() * 5);
    m_obligQuestNr = qMax(qaPossibilities() * 4, m_obligQuestNr);
}


Tnote TexecutorSupply::determineAccid(const Tnote& n) {
    Tnote nA = n;
    bool notFound = true;
    if (m_level->withSharps || m_level->withFlats || m_level->withDblAcc) {
        if (m_level->withDblAcc) {
            m_dblAccidsCntr++;
            if (m_dblAccidsCntr == 4) { //double accid note occurs every 4-th question
                if ( (qrand() % 2) ) // randomize dblSharp or dblFlat
                    nA = n.showWithDoubleSharp();
                else
                    nA = n.showWithDoubleFlat();
                if (nA == n) // dbl accids are not possible
                    m_dblAccidsCntr--;
                else {
                    m_dblAccidsCntr = 0;
                    notFound = false;
                }
            }
        }
        if (notFound && m_prevAccid != Tnote::e_Flat && m_level->withFlats) {
          if ((n.note() == 3 || n.note() == 7) && n.alter() == 0 ) { // increase counter for f and c notes
            m_eisCesCntr++;
            if (m_eisCesCntr == 3) { // fes or ces can occur every 3 e or b occurences
              m_eisCesCntr = 0;
              nA = n.showWithFlat();
              notFound = false;
            }
          } else { // other notes can be with flat allways
            nA = n.showWithFlat();
            notFound = false;
          }
        }
        if (notFound && m_prevAccid != Tnote::e_Sharp && m_level->withSharps) {
          if ((n.note() == 4 || n.note() == 1) && n.alter() == 0) { // increase counter for f and c notes
            m_eisCesCntr++;
            if (m_eisCesCntr == 3) { // eis or bis can occur every 3 f or c occurences
              nA = n.showWithSharp();
              m_eisCesCntr = 0;
            }
          } else { // other notes can be with sharp allways
            nA = n.showWithSharp();
          }
        }
    }
    m_prevAccid = (Tnote::Ealter)nA.alter();
    return nA;
}


/**
 * if level has key range @p !Tlevel::isSingleKey and notes comes only from current key @p Tlevel::onlyCurrKey
 * transpose list to selected key. If double accidentals are allowed, keep intervals otherwise change to neutrals.
 * If only current key is not used - simply copy @p Tlevel::notesList to @p qaList
 */
void TexecutorSupply::listForRandomNotes(TkeySignature k, QList<TQAgroup> &qaList) {
  QList<Tnote> *listPtr = &m_level->notesList;
  QList<Tnote> transposedList;
  if (!m_level->isSingleKey) {
      if (k.value() != m_level->keyOfrandList.value()) { // transpose
          int hiNoteChrom = GLOB->hiNote().chromatic();
          int loNoteChrom = GLOB->loNote().chromatic();
          k.setMinor(false); // convert to major to correctly obtain transpose interval
          // looking for highest and lowest notes in the list
          int loInList = hiNoteChrom, hiInList = loNoteChrom;
          int chromaticArr[m_level->notesList.size()]; // pack chromatic notes into array to reuse
          for (int n = 0; n < m_level->notesList.size(); ++n) {
              chromaticArr[n] = m_level->notesList[n].chromatic();
              loInList = qMin(loInList, chromaticArr[n]);
              hiInList = qMax(hiInList, chromaticArr[n]);
          }
          int step = qAbs(k.tonicNote().chromatic() - m_level->keyOfrandList.tonicNote().chromatic());
//           if (hiNoteChrom - (hiInList + step) < (loInList + (step - 12)) - loNoteChrom) //  equal option - transpose where is more space
          if (loInList + (step - 12) >= loNoteChrom) // prefer to transpose down
            step = step - 12;
          for (int n = 0; n < m_level->notesList.size(); ++n) {
              int transChrom = chromaticArr[n] + step;
              if (transChrom >= loNoteChrom && transChrom <= hiNoteChrom) {
                Tnote note(transChrom);
                if (m_level->onlyCurrKey)
                  note = k.inKey(note);
                if (note.isValid())
                  transposedList << Tnote(transChrom);
              }
          }
          listPtr = &transposedList;
      }
  }
  // Workaround when transposition is impossible - there is not any note capable for it
  if (listPtr == &transposedList && transposedList.isEmpty()) {
    qDebug() << "[TexecutorSupply] transposition of notes list failed. Using list without transposition";
    listPtr = &m_level->notesList;
  }
  for (int n = 0; n < listPtr->size(); ++n) {
    // TODO: add positions if guitar is enabled
    TQAgroup qaG;
    qaG.note = listPtr->operator[](n);
    qaList << qaG;
  }
}


Tnote TexecutorSupply::forceEnharmAccid(const Tnote& n) {
    Tnote nX;
    char acc = m_prevAccid;
     for (int i = 0; i < 6; i++) { // to be sure that all cases are checked
        acc++;
        if (acc > 2) acc = -2;

        if (acc == Tnote::e_DoubleFlat && m_level->withDblAcc)
            nX = n.showWithDoubleFlat();
        if (acc == Tnote::e_Flat && m_level->withFlats)
            nX = n.showWithFlat();
        if (acc == Tnote::e_Natural)
            nX = n.showAsNatural();
        if (acc == Tnote::e_Sharp && m_level->withSharps)
            nX = n.showWithSharp();
        if (acc == Tnote::e_DoubleSharp && m_level->withDblAcc)
            nX = n.showWithDoubleSharp();
        if (nX.note() && n != nX) break;
     }
    m_prevAccid = (Tnote::Ealter)acc;
    if (nX.note())
        return nX;
    else return n;
}


Tnote::EnameStyle TexecutorSupply::randomNameStyle(int style) {
    if (style != -1) {
        if ((Tnote::EnameStyle)style == Tnote::e_italiano_Si || (Tnote::EnameStyle)style == Tnote::e_russian_Ci)
            m_isSolfege = true;
        else
            m_isSolfege = false;
    }
    if (m_isSolfege) {
        m_isSolfege = false;
        if (qrand() % 2) { // full name like cis, gisis
            if (GLOB->S->seventhIs_B)
                return Tnote::e_nederl_Bis;
            else
                return Tnote::e_deutsch_His;
        } else { // name and sign like c#, gx
            if (GLOB->S->seventhIs_B)
                return Tnote::e_english_Bb;
            else
                return Tnote::e_norsk_Hb;
        }
    } else {
        m_isSolfege = true;
        return Tnote::e_italiano_Si;
    }
}


void TexecutorSupply::getTheSamePos(TfingerPos& fingerPos, QList<TfingerPos>& posList, bool strCheck, bool order) {
  int chStr = GLOB->Gtune()->str(strNr(fingerPos.str() - 1, order) + 1).chromatic();
  for (int i = 0; i < GLOB->Gtune()->stringNr(); i++)
    if (i != strNr(fingerPos.str() - 1, order)) { 
      if (strCheck && !m_level->usedStrings[i])
          continue; // skip unavailable strings when strCheck is true
      int fret = chStr + fingerPos.fret() - GLOB->Gtune()->str(strNr(i, order) + 1).chromatic();
      if (fret >= m_level->loFret && fret <= m_level->hiFret) {
        posList << TfingerPos(strNr(i, order) + 1, fret);
      }
    }
}


void TexecutorSupply::getTheSamePosNoOrder(TfingerPos& fingerPos, QList<TfingerPos>& posList, bool strCheck) {
  getTheSamePos(fingerPos, posList, strCheck, false);
}


void TexecutorSupply::calcQAPossibleCount() {
  m_qaPossib = 0;
  if (m_level->questionAs.isOnScore()) {
    if (m_level->answersAs[TQAtype::e_onScore].isOnScore())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_onScore].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_onScore].isOnInstr())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_onScore].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isName()) {
    if (m_level->answersAs[TQAtype::e_asName].isOnScore())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isOnInstr())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asName].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isOnInstr()) {
    if (m_level->answersAs[TQAtype::e_onInstr].isOnScore())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_onInstr].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_onInstr].isOnInstr())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_onInstr].isSound())
      m_qaPossib++;
  }
  if (m_level->questionAs.isSound()) {
    if (m_level->answersAs[TQAtype::e_asSound].isOnScore())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isName())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isOnInstr())
      m_qaPossib++;
    if (m_level->answersAs[TQAtype::e_asSound].isSound())
      m_qaPossib++;
  }
}


void TexecutorSupply::checkNotes(TQAunit* curQ, Tnote& expectedNote, Tnote& userNote, bool reqOctave, bool reqAccid) {
  Tnote exN = expectedNote, retN = userNote;
  if (retN.note()) {
    Tnote nE = exN.showAsNatural();
    Tnote nR = retN.showAsNatural();
    if (exN != retN) {
      if (reqOctave) {
          if (nE.note() == nR.note() && nE.alter() == nR.alter()) {
              if (nE.octave() != nR.octave())
                curQ->setMistake(TQAunit::e_wrongOctave);
          } else {
              curQ->setMistake(TQAunit::e_wrongNote);
          }
      }
      if (!curQ->wrongNote()) { // There is still something to check
        if (exN.note() != retN.note() || exN.alter() != retN.alter()) {// if they are equal it means that only octaves were wrong
            exN = exN.showAsNatural();
            retN = retN.showAsNatural();
            if (reqAccid) {
                if (exN.note() == retN.note() && exN.alter() == retN.alter())
                    curQ->setMistake(TQAunit::e_wrongAccid);
                else
                    curQ->setMistake(TQAunit::e_wrongNote);
            } else {
                if (exN.note() != retN.note() || exN.alter() != retN.alter())
                  curQ->setMistake(TQAunit::e_wrongNote);
            }
        }
      }
    }
  } else
      curQ->setMistake(TQAunit::e_wrongNote);
  if (!curQ->wrongNote() && m_level->instrument == Tinstrument::Bandoneon && m_level->clef.type() == Tclef::PianoStaffClefs
      && curQ->questionOnScore() && curQ->answerOnInstr()) {
      if (expectedNote.onUpperStaff() != userNote.onUpperStaff())
        qDebug() << "[TexecutorSupply check notes] mistake: wrong bandoneon side";
  }
}


void TexecutorSupply::compareMelodies(Tmelody* q, Tmelody* a, Tattempt* att) {
  int notesCount = qMax(q->length(), a->length());
  for (int i = 0; i < notesCount; ++i) {
    TQAunit tmpUnit;
    if (a->length() > i && q->length() > i)
      checkNotes(&tmpUnit, q->note(i)->p(), a->note(i)->p(), m_level->requireOctave, m_level->forceAccids);
    else
      tmpUnit.setMistake(TQAunit::e_wrongNote);
    att->add(tmpUnit.mistake()); // times are ignored in that type of answer/attempt
  }
  att->updateEffectiveness();
}


void TexecutorSupply::compareMelodies(Tmelody* q, QList<TnoteStruct>& a, Tattempt* att) {
//   debugMelody(q);
//   debugNotesStruct(a);
  int notesCount = qMax(q->length(), a.size());
  for (int i = 0; i < notesCount; ++i) {
    TQAunit tmpUnit;
    if (a.size() > i && q->length() > i) {
      checkNotes(&tmpUnit, q->note(i)->p(), a[i].pitch, m_level->requireOctave, m_level->forceAccids);
      // TODO !!!!!!!!!!!!  Enable it again !!!!!!!!!!!!!!!!
//       if (!tmpUnit.isWrong() && m_level->intonation != TintonationView::e_noCheck) {
//         if (!TnoteStruct(Tnote(), a[i].pitchF).inTune(TintonationView::getThreshold(m_level->intonation)))
//           tmpUnit.setMistake(TQAunit::e_wrongIntonation);
//       }
    } else
        tmpUnit.setMistake(TQAunit::e_wrongNote);
    att->add(tmpUnit.mistake());
  }
  att->updateEffectiveness();
}


void TexecutorSupply::compareWrittenFromPlayed(Tmelody* q, Tmelody* a, Tattempt* attempt, int transposition) {
  QVector<TnoteToPlay> quesList = TnoteToPlay::fromMelody(q, transposition);
  QVector<TnoteToPlay> answList = TnoteToPlay::fromMelody(a, 0); // already transposed
  int notesCount = qMax(quesList.size(), answList.size());
  for (int i = 0; i < notesCount; ++i) {
    TQAunit tmpUnit;
    if (quesList.size() > i && answList.size() > i) {
        if (q->clef() != Tclef::NoClef) // if there is some melody in a melody
          tmpUnit.setMistake(comparePitches(quesList[i].pitch(), answList[i].pitch(), m_level->requireOctave));
        if (quesList[i].duration() != answList[i].duration()) { // wrong rhythm
          tmpUnit.setMistake(TQAunit::e_wrongRhythm);
        }
    } else {
        // If user put more notes than question has - the overload is mistake
        tmpUnit.setMistake(TQAunit::e_wrongNote);
    }
    attempt->add(tmpUnit.mistake());
    if (quesList.size() > i) {
      if (q->note(quesList[i].index())->p().rtm.tie()) { // copy committed mistake type for every tied note in questioned melody
        int tieId = quesList[i].index() + 1;
        while (tieId < q->length() && q->note(tieId)->p().rtm.tie() && q->note(tieId)->p().rtm.tie() != Trhythm::e_tieStart) {
          attempt->add(tmpUnit.mistake());
          tieId++;
        }
      }
      // As long as we treat continuous rests as the single one,
      // we have to keep 'mistake' for every such rest in questioned melody (similar to ties)
      if (q->note(quesList[i].index())->p().isRest()) {
        int restId = quesList[i].index() + 1;
        while (restId < q->length() && q->note(restId)->p().isRest()) {
          attempt->add(tmpUnit.mistake());
          restId++;
        }
      }
    }
  }
  attempt->updateEffectiveness();
}


void TexecutorSupply::comparePlayedFromScore(Tmelody* q, QVector<TnoteToPlay>& toPlay,  QVector<TnoteStruct>& a, Tattempt* att, int transposition) {
  int notesCount = qMax(toPlay.size(), a.size());
  qreal tempoSum = 0.0;
  int tempoCounter = 0;
  int fstQestId = toPlay.first().isRest() ? 1 : 0; // first question with note
  // calculate tempo factor by duration of first played note
  qreal tempoFactor = a.count() > fstQestId ? static_cast<qreal>(toPlay[fstQestId].duration()) / a[fstQestId].duration : 1.0;
  bool lastIsRest = toPlay.last().isRest() && q->meter()->meter() != Tmeter::NoMeter;
  for (int i = 0; i < notesCount; ++i) {
    TQAunit tmpUnit;
    if (a.size() > i && toPlay.size() > i) {
      if (!(i == 0 && toPlay[0].isRest()) // skip first rest - it is correct
        && (!lastIsRest || (lastIsRest && toPlay[i].index() < q->length() - 1))) // skip checking the last rest as well
      {
        auto answ = &a[i];
        if (q->clef() != Tclef::NoClef) // if there is some melody in a melody - compare pitches
          tmpUnit.setMistake(comparePitches(toPlay[i].pitch(), answ->pitchF == 0.0 ? 127 : answ->pitch.chromatic(), m_level->requireOctave));

        if (q->meter()->meter() != Tmeter::NoMeter) {
          qreal dur = tempoFactor * answ->duration;
          // Calculate quantization as a half of expected rhythm value (without dot).
          // It avoids some extraordinary values like 4.. or even worst
          int quantization = qMax(6, toPlay[i].duration() / 2);
          int normDur = qRound(dur / static_cast<qreal>(quantization)) * quantization;
          answ->pitch.rtm.setRhythm(normDur); // store detected duration in answers list
          if (!answ->pitch.rtm.isValid()) {
              if (answ->duration) // null duration means note was not played by user. skip debug message then
                qDebug() << "====" << i << "note has invalid duration of" << normDur << ", expected:" << q->note(i)->p().rtm.string();
              tmpUnit.setMistake(TQAunit::e_wrongRhythm);
          } else {
              if (answ->pitch.rtm.duration() == toPlay[i].duration()) {
                  qreal tempo = 60.0 / ((24.0 / static_cast<qreal>(toPlay[i].duration())) * answ->duration);
                  tempoCounter++;
                  tempoSum += tempo;
//                   qDebug() << i << "rhythm was correct. Tempo is" << tempo;
              } else if (i < notesCount - 1) { // do note check last note duration - it is very hard to play it correctly
                  tmpUnit.setMistake(TQAunit::e_wrongRhythm);
//                   qDebug() << i << "WRONG RHYTHM! Expected:" << Trhythm(quesList[i].duration()).string() << "got:" << r.string() << dur << normDur << quantization;
              }
          }
        }
      }
      att->add(tmpUnit.mistake());
      if (q->note(toPlay[i].index())->p().rtm.tie()) { // copy committed mistake type for every tied note in questioned melody
        int tieId = toPlay[i].index() + 1;
        while (tieId < q->length() && q->note(tieId)->p().rtm.tie() && q->note(tieId)->p().rtm.tie() != Trhythm::e_tieStart) {
          att->add(tmpUnit.mistake());
          tieId++;
        }
      }
//       As long as we treat continuous rests as the single one,
//       we have to keep 'mistake' for every such rest in questioned melody (similar to ties)
      if (q->note(toPlay[i].index())->p().isRest()) {
        int restId = toPlay[i].index() + 1;
        while (restId < q->length() && q->note(restId)->p().isRest()) {
          att->add(tmpUnit.mistake());
          restId++;
        }
      }
    }
  }
//   for (int i = 0; i < notesCount; ++i)
//     qDebug() << "checked" << i << q->note(toPlay[i].index())->p().toText() << toPlay[i].duration() << att->mistakes[i];
  att->updateEffectiveness();
}


quint32 TexecutorSupply::comparePitches(int p1, int p2, bool requireOctave) {
  int witoutOctave1 = (p1 + 143) % 12;
  int witoutOctave2 = (p2 + 143) % 12;
  if (witoutOctave1 == witoutOctave2) {
    if (requireOctave)
      return p1 == p2 ? static_cast<quint32>(TQAunit::e_correct) : static_cast<quint32>(TQAunit::e_wrongOctave);
  }
  //TODO intonation check
  return static_cast<quint32>(TQAunit::e_wrongNote);
}


TkeySignature TexecutorSupply::getKey(Tnote& note) {
  Tnote tmpNote = note;
  TkeySignature key; // C-major by default
  if (m_level->isSingleKey) { // for single key
      key = m_level->loKey;
      if (m_level->onlyCurrKey) {
        if (!m_level->canBeMelody()) {
            tmpNote = m_level->loKey.inKey(note);
            if (!tmpNote.isValid())
              qDebug() << "There is no" << tmpNote.toText() << "in level with single key:" << m_level->loKey.getName() <<
                    "It should never happened!";
        }
      }
  } else { // for many key signatures
      if (m_randKey)
          key = TkeySignature(m_randKey->get());
      if (note.isValid() && m_level->onlyCurrKey && !m_level->canBeMelody()) { // if note is in current key only
          int keyRangeWidth = m_level->hiKey.value() - m_level->loKey.value();
          int patience = 0; // we are looking for suitable key
          char keyOff = key.value() - m_level->loKey.value();
          tmpNote = key.inKey(note);
          while(tmpNote.note() == 0 && patience <= keyRangeWidth) {
              keyOff++;
              if (keyOff > keyRangeWidth) 
                keyOff = 0;
              key = TkeySignature(m_level->loKey.value() + keyOff);
              patience++;
              tmpNote = key.inKey(note);
              if (patience > keyRangeWidth) {
                  qDebug() << "Oops! It should never happened. Can not find key signature for" << note.toText();
                  break;
              }
          }
      }
  }
  note = tmpNote;
  key.setMinor(bool(qrand() % 2));
  return key;
}


void TexecutorSupply::resetKeyRandom() {
  if (m_randKey)
    m_randKey->reset();
}


int TexecutorSupply::randomMeter() {
  if (m_meterList.count() > 1 && m_randMeter)
    return m_meterList.at(m_randMeter->get());
  return m_meterList.first();
}


void TexecutorSupply::resetMeterRandom() {
  if (m_randMeter)
    m_randMeter->reset();
}


int TexecutorSupply::getBarNumber(int questNr, int penallNr) {
  if (m_level->variableBarNr && m_level->barNumber > 2) {
    int minBarNr = qMax(2, m_level->barNumber / 8);
    int step = m_level->barNumber % 2 ? 1 : 2;
    int changes = (m_level->barNumber - minBarNr) / step + 1;
    int questPerChange = (m_obligQuestNr + penallNr) / changes;
    return qMin(static_cast<int>(m_level->barNumber), minBarNr + (questNr / questPerChange) * step);
  }
  return m_level->barNumber;
}


int TexecutorSupply::findQuantization(Tmelody* m) {
  int q = 12;
  for (int n = 0; n < m->length(); ++n) {
    // Look for duration less than 12 (8th), then return quantization of 6 (16th)
    if (m->note(n)->p().duration() < 12) {
      q = 6;
      break;
    }
  }
  return q;
}


//##########################################################################################
//#######################     PRIVATE        ###############################################
//##########################################################################################

bool TexecutorSupply::isNoteInKey(Tnote& n) {
  if (m_level->isSingleKey) {
    if (m_level->loKey.inKey(n).isValid())
        return true;
    } else {
        for (int k = m_level->loKey.value(); k <= m_level->hiKey.value(); k++) {
          if (TkeySignature::inKey(TkeySignature(k), n).note() != 0)
            return true;
        }
    }
  return false;
}


void TexecutorSupply::addToList(QList<TQAgroup>& list, Tnote& n, Ttechnical &t) {
  TQAgroup g;
  g.note = n;
  g.technical = t;
  list << g;
}


quint8 TexecutorSupply::strNr(quint8 str0to6, bool ordered) {
  if (ordered)
    return GLOB->strOrder((char)str0to6);
  else
    return str0to6;
}

//##########################################################################################
//#######################     EVENTS        ################################################
//##########################################################################################

#if !defined (Q_OS_ANDROID)
bool TexecutorSupply::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (me->button() == Qt::RightButton) {
        emit rightButtonClicked();
        return true;
    }
  }
  return QObject::eventFilter(obj, event);
}
#endif




