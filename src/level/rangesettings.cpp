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


#include "rangesettings.h"
#include "tglobals.h"
#include <widgets/tlevelpreview.h>
#include <score/tsimplescore.h>
#include <exam/tlevel.h>
#include <music/ttune.h>
#include <QtWidgets>

extern Tglobals *gl;
extern bool isNotSaved;


rangeSettings::rangeSettings(QWidget *parent) :
    QWidget(parent),
    m_levelIsLoading(false)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->addStretch(1);
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *allLay = new QHBoxLayout;

    QVBoxLayout *scoreLay = new QVBoxLayout;
    m_scoreRang = new TsimpleScore(2, this);
			m_scoreRang->setClef(Tclef(gl->S->clef));
			m_scoreRang->setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()), Tnote(gl->hiNote().getChromaticNrOfNote()));
			m_scoreRang->setNote(0, Tnote(1, 0));
			m_scoreRang->setNote(1, Tnote(1, 1));
			m_scoreRang->addBGglyph((int)gl->instrument);
			m_scoreRang->setControllersEnabled(true, false);
		m_fretAdjustButt = new QPushButton(tr("adjust fret range"), this);
			m_fretAdjustButt->setStatusTip(tr("Adjust fret range in a level to currently selected note range"));
    QGroupBox *notesRangGr = new QGroupBox(TlevelPreview::notesRangeTxt(), this);
    scoreLay->addWidget(m_scoreRang);
		scoreLay->addWidget(m_fretAdjustButt, 1, Qt::AlignCenter);
    notesRangGr->setLayout(scoreLay);
		m_scoreRang->setFixedSize(400, 300);
// #if defined(Q_OS_WIN)
//     m_scoreRang->setFixedHeight(300);
// #endif
    allLay->addWidget(notesRangGr);

    QVBoxLayout *guitLay = new QVBoxLayout;
    QGroupBox *fretGr = new QGroupBox(TlevelPreview::fretsRangeTxt(), this);
    QHBoxLayout *fretLay = new QHBoxLayout;
    QLabel *fromLab = new QLabel(tr("from"),this);
    m_fromSpinB = new QSpinBox(this);
    m_fromSpinB->setMaximum(gl->GfretsNumber);
    QLabel *toLab = new QLabel(tr("to"),this);
    m_toSpinB = new QSpinBox(this);
    m_toSpinB->setMaximum(gl->GfretsNumber);
    m_toSpinB->setValue(3);
    fretLay->addWidget(fromLab);
    fretLay->addWidget(m_fromSpinB);
    fretLay->addStretch(1);
    fretLay->addWidget(toLab);
    fretLay->addWidget(m_toSpinB);
		m_noteAdjustButt = new QPushButton(tr("adjust note range"), this);
		m_noteAdjustButt->setStatusTip(tr("Adjust note range in a level to currently selected fret range"));
    fretGr->setLayout(fretLay);
    guitLay->addWidget(fretGr);
		guitLay->addWidget(m_noteAdjustButt, 1, Qt::AlignCenter);
    guitLay->addStretch(1);

    QGroupBox *stringsGr = new QGroupBox(tr("available strings:"),this);
    stringsGr->setStatusTip(tr("Uncheck strings if you want to skip them<br>in an exam."));
    QGridLayout *strLay = new QGridLayout;
    for (int i = 0; i < 6; i++) {
        m_stringBut[i] = new QCheckBox(QString("%1").arg(i+1),this);
        m_stringBut[i]->setFont(QFont("nootka", qRound(font().pointSize() * 2.5), QFont::Normal));
        m_stringBut[i]->setChecked(true);
        connect(m_stringBut[i], SIGNAL(clicked()), this, SLOT(stringSelected()));
        connect(m_stringBut[i], SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
        if (i<3)
            strLay->addWidget(m_stringBut[i], 1, i + 1, 0);
        else
            strLay->addWidget(m_stringBut[i], 2, i - 2, 0);
				if (i >= gl->Gtune()->stringNr())
					m_stringBut[i]->hide();
    }
    stringsGr->setLayout(strLay);
    guitLay->addWidget(stringsGr);
    guitLay->addStretch(1);

    allLay->addLayout(guitLay);

    mainLay->addLayout(allLay);
    mainLay->addStretch(1);

    setLayout(mainLay);
		
		if (gl->instrument == e_noInstrument) {
			fretGr->hide();
			stringsGr->hide();
			m_fretAdjustButt->hide();
			m_noteAdjustButt->hide();
		}

    connect(m_scoreRang, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenParamsChanged()));
		connect(m_scoreRang, SIGNAL(clefChanged(Tclef)), this, SLOT(whenParamsChanged()));
    connect(m_fromSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
    connect(m_toSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
		connect(m_fretAdjustButt, SIGNAL(clicked()), this, SLOT(adjustFrets()));
		connect(m_noteAdjustButt, SIGNAL(clicked()), this, SLOT(adjustNotes()));
}


void rangeSettings::stringSelected() {
    if ( !m_stringBut[0]->isChecked() && !m_stringBut[1]->isChecked()
        && !m_stringBut[2]->isChecked() && !m_stringBut[3]->isChecked()
        && !m_stringBut[4]->isChecked() && !m_stringBut[5]->isChecked() ) {
        m_stringBut[0]->setChecked(true);
    }
}


void rangeSettings::loadLevel(Tlevel& level) {
	m_levelIsLoading = true;
		m_scoreRang->setClef(level.clef);
		m_scoreRang->setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()), Tnote(gl->hiNote().getChromaticNrOfNote()));
    m_scoreRang->setNote(0, level.loNote);
    m_scoreRang->setNote(1, level.hiNote);
    m_fromSpinB->setValue(level.loFret);
    m_toSpinB->setValue(level.hiFret);
    for (int i = 0; i < gl->Gtune()->stringNr(); i++)
        m_stringBut[i]->setChecked(level.usedStrings[i]);
    stringSelected();
	m_levelIsLoading = false;
}


void rangeSettings::whenParamsChanged() {
		if (m_levelIsLoading)
				return;
		
		m_scoreRang->setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()), Tnote(gl->hiNote().getChromaticNrOfNote()));
    if (!isNotSaved) {
        isNotSaved = true;
        emit rangeChanged();
    }
    if (!m_stringBut[0]->isChecked() || !m_stringBut[1]->isChecked()
        || !m_stringBut[2]->isChecked() || !m_stringBut[3]->isChecked()
        || !m_stringBut[4]->isChecked() || !m_stringBut[5]->isChecked() )
      emit allStringsChecked(false);
    else
      emit allStringsChecked(true);
        /** It has two reasons:
         1. when questions list is created there is no conditions to check
            unavailable (unchecked) strings
         2. in level validation method is hard to determine dependency unchecked
            strings between range of frets and notes. */
}


void rangeSettings::saveLevel(Tlevel &level) {
	// Fixing empty notes
		if (m_scoreRang->getNote(0).note == 0)
				m_scoreRang->setNote(0, 
										Tnote(qMax(gl->loString().getChromaticNrOfNote(), m_scoreRang->lowestNote().getChromaticNrOfNote())));
		if (m_scoreRang->getNote(1).note == 0)
				m_scoreRang->setNote(1, 
										Tnote(qMin(gl->hiNote().getChromaticNrOfNote(), m_scoreRang->highestNote().getChromaticNrOfNote())));
				
    if (m_scoreRang->getNote(0).getChromaticNrOfNote() <= m_scoreRang->getNote(1).getChromaticNrOfNote()) {
				level.loNote = m_scoreRang->getNote(0);
				level.hiNote = m_scoreRang->getNote(1);
		} else {
        level.loNote = m_scoreRang->getNote(1);
        level.hiNote = m_scoreRang->getNote(0);
    }
    if (m_fromSpinB->value() <= m_toSpinB->value()) {
        level.loFret = m_fromSpinB->value();
        level.hiFret = m_toSpinB->value();
    } else {
        level.loFret = m_toSpinB->value();
        level.hiFret = m_fromSpinB->value();
    }
    for (int i = 0; i < gl->Gtune()->stringNr(); i++)
        level.usedStrings[i] = m_stringBut[i]->isChecked();
		level.clef = m_scoreRang->clef();
}


void rangeSettings::adjustFrets() {
	char loF, hiF;
	Tlevel lev;
	saveLevel(lev);
	if (!lev.loNote.acidental && !lev.hiNote.acidental) {
			lev.withFlats = false; // TODO: grab it from accidental settings
			lev.withSharps = false;
	}
	if (lev.adjustFretsToScale(loF, hiF)) {
			m_fromSpinB->setValue(loF);
			m_toSpinB->setValue(hiF);
			emit rangeChanged();
	} else {
			qDebug() << "Can't adjust fret range!";
	}
}


void rangeSettings::adjustNotes() {
	m_scoreRang->setNote(0, Tnote(gl->loString().getChromaticNrOfNote() + m_fromSpinB->value()));
	m_scoreRang->setNote(1, Tnote(gl->hiString().getChromaticNrOfNote() + m_toSpinB->value()));
	emit rangeChanged();
}









