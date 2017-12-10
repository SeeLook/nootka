/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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

#include "tglobalexamstore.h"
#include "tglobals.h"
#include <taudioparams.h>
#include <tscoreparams.h>
#include <exam/tlevel.h>
#include <tlayoutparams.h>



TglobalExamStore::TglobalExamStore(Tglobals* globals) :
  m_globals(globals)
{}


void TglobalExamStore::storeSettings() {
//     nameStyleInNoteName = mW->noteName->style();
  nameStyleInNoteName = m_globals->S->nameStyleInNoteName;
  showEnharmNotes = m_globals->S->showEnharmNotes;
  showKeySignName = m_globals->S->showKeySignName;
  showOtherPos = m_globals->GshowOtherPos;
  useDblAccids = m_globals->S->doubleAccidentalsEnabled;
  useKeySign = m_globals->S->keySignatureEnabled;
  octaveInName = m_globals->S->octaveInNoteNameFormat;
  clef = Tclef(m_globals->S->clef);
  instrument = m_globals->instrument().type();
  intonation = m_globals->A->intonation;
  if (m_globals->A->midiEnabled)
    playbackInstr = m_globals->A->midiInstrNr;
  else
    playbackInstr = m_globals->A->audioInstrNr;
  isSingleNoteMode = m_globals->S->isSingleNoteMode;
  namesOnScore = m_globals->S->namesOnScore;
  OUTenabled = m_globals->A->OUTenabled;
  INenabled = m_globals->A->INenabled;
  showSoundView = m_globals->L->soundViewEnabled;
  showGuitar = m_globals->L->guitarEnabled;
}


void TglobalExamStore::restoreSettings() {
  m_globals->S->showEnharmNotes = showEnharmNotes;
  m_globals->S->showKeySignName = showKeySignName;
  m_globals->GshowOtherPos = showOtherPos;
  m_globals->S->doubleAccidentalsEnabled  = useDblAccids;
  m_globals->S->keySignatureEnabled = useKeySign;
  m_globals->setTune(tune);
  m_globals->S->octaveInNoteNameFormat = octaveInName;
  m_globals->GfretsNumber = fretsNumber;
  m_globals->S->clef = clef.type();
  m_globals->S->isSingleNoteMode = isSingleNoteMode;
  m_globals->S->namesOnScore = namesOnScore;
  m_globals->instrument().setType(instrument);
  m_globals->A->intonation = intonation;
  if (m_globals->A->midiEnabled)
    m_globals->A->midiInstrNr = playbackInstr;
  else
    m_globals->A->audioInstrNr = playbackInstr;
  m_globals->A->INenabled = INenabled;
  m_globals->A->OUTenabled = OUTenabled;
  m_globals->L->soundViewEnabled = showSoundView;
  m_globals->L->guitarEnabled = showGuitar;
}


void TglobalExamStore::prepareGlobalsToExam(const Tlevel& level) {
  m_globals->S->showEnharmNotes = false;
  m_globals->S->showKeySignName = false;
  m_globals->GshowOtherPos = false;
  m_globals->S->doubleAccidentalsEnabled = level.withDblAcc;
  m_globals->S->keySignatureEnabled = level.useKeySign;
  m_globals->S->octaveInNoteNameFormat = true;
  m_globals->S->clef = level.clef.type();
  m_globals->S->namesOnScore = false;
  if (level.answerIsSound()) {
      if (!m_globals->A->INenabled) {
        m_globals->A->INenabled = true;
      }
  }
  if (level.questionAs.isSound() && !m_globals->A->OUTenabled) {
    m_globals->A->OUTenabled = true;
  }
  m_globals->A->intonation = level.intonation;
  m_globals->S->isSingleNoteMode = !level.canBeMelody();
  m_globals->L->soundViewEnabled = level.answerIsSound();
  if (level.canBeGuitar()) // enable guitar if necessary, but don't disable when it is visible
    m_globals->L->guitarEnabled = true;
    // change output instrument type when necessary (exam instrument differs from user) TODO
//     if (m_globals->instrument != NoInstrument && m_globals->instrument != instrument)
//         AudioOutSettings::adjustOutToInstrument(m_globals->A, (int)m_globals->instrument);
}










