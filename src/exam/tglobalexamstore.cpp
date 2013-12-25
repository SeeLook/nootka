/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tglobalexamstore.h"
#include "tglobals.h"
#include <taudioparams.h>
#include <audiooutsettings.h>
#include <tlevel.h>



TglobalExamStore::TglobalExamStore(Tglobals* globals) :
	m_globals(globals)
{

}


void TglobalExamStore::storeSettings() {
// 		nameStyleInNoteName = mW->noteName->style();
		nameStyleInNoteName = m_globals->NnameStyleInNoteName;
    showEnharmNotes = m_globals->showEnharmNotes;
    showKeySignName = m_globals->SshowKeySignName;
    showOtherPos = m_globals->GshowOtherPos;
    useDblAccids = m_globals->doubleAccidentalsEnabled;
    useKeySign = m_globals->SkeySignatureEnabled;
    octaveInName = m_globals->NoctaveInNoteNameFormat;
		clef = Tclef(m_globals->Sclef);
// 		instrument = m_globals->instrument;
		detectRange = (int)m_globals->A->range;
		intonation = m_globals->A->intonation;
		if (m_globals->A->midiEnabled)
			playbackInstr = m_globals->A->midiInstrNr;
		else
			playbackInstr = m_globals->A->audioInstrNr;
}


void TglobalExamStore::restoreSettings() {
		m_globals->showEnharmNotes = showEnharmNotes;
    m_globals->SshowKeySignName = showKeySignName;
    m_globals->GshowOtherPos = showOtherPos;
    m_globals->doubleAccidentalsEnabled  = useDblAccids;
    m_globals->SkeySignatureEnabled = useKeySign;
    m_globals->setTune(tune);
    m_globals->NoctaveInNoteNameFormat = octaveInName;
    m_globals->GfretsNumber = fretsNumber;
		m_globals->Sclef = clef.type();
		m_globals->instrument = instrument;
		m_globals->A->range = (TaudioParams::Erange)detectRange;
		m_globals->A->intonation = intonation;
		if (m_globals->A->midiEnabled)
			m_globals->A->midiInstrNr = playbackInstr;
		else
			m_globals->A->audioInstrNr = playbackInstr;
}


void TglobalExamStore::prepareGlobalsToExam(Tlevel& level) {
		m_globals->showEnharmNotes = false;
    m_globals->SshowKeySignName = false;
    m_globals->GshowOtherPos = false;
    m_globals->doubleAccidentalsEnabled = level.withDblAcc;
    m_globals->SkeySignatureEnabled = level.useKeySign;
    m_globals->NoctaveInNoteNameFormat = true;
		m_globals->Sclef = level.clef.type();
		if (level.answerIsSound()) {
				if (level.loNote.getChromaticNrOfNote() > Tnote(6, 0, 0).getChromaticNrOfNote())
						m_globals->A->range = TaudioParams::e_high;
				else if (level.loNote.getChromaticNrOfNote() > Tnote(5, -2, 0).getChromaticNrOfNote())
						m_globals->A->range = TaudioParams::e_middle;
				else
						m_globals->A->range = TaudioParams::e_low;
		}
		m_globals->A->intonation = level.intonation;
		// change output instrument type when necessary (exam instrument differs from user)
		if (m_globals->instrument != e_noInstrument && m_globals->instrument != instrument)
				AudioOutSettings::adjustOutToInstrument(m_globals->A, (int)m_globals->instrument);
}










