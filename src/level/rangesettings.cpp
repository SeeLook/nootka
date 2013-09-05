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
#include "tlevelpreview.h"
#include "tsimplescore.h"
#include <ttune.h>
#include <QtGui>

extern Tglobals *gl;
extern bool isNotSaved;

rangeSettings::rangeSettings(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->addStretch(1);
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *allLay = new QHBoxLayout;

    QVBoxLayout *scoreLay = new QVBoxLayout;
    m_scoreRang = new TsimpleScore(3, this); // third note is dummy
		m_scoreRang->setNoteDisabled(2, true); // and is disabled and empty
		m_scoreRang->setClef(Tclef(gl->Sclef));
//     m_scoreRang->setAmbitus(Tnote(gl->loString().getChromaticNrOfNote()),
//                Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));
    m_scoreRang->setNote(0, Tnote(1, 0));
    m_scoreRang->setNote(1, Tnote(1, 1));
    QGroupBox *notesRangGr = new QGroupBox(TlevelPreview::notesRangeTxt(), this);
    scoreLay->addWidget(m_scoreRang);
    notesRangGr->setLayout(scoreLay);
#if defined(Q_OS_WIN)
    m_scoreRang->setFixedHeight(300);
#endif
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
    fretGr->setLayout(fretLay);
    guitLay->addWidget(fretGr);
    guitLay->addStretch(1);

    QGroupBox *stringsGr = new QGroupBox(tr("avaiable strings:"),this);
    stringsGr->setStatusTip(tr("uncheck strings if You want to skip them<br>in an exam."));
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

    connect (m_scoreRang, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenParamsChanged()));
    connect (m_fromSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
    connect (m_toSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
}

void rangeSettings::stringSelected() {
    if ( !m_stringBut[0]->isChecked() && !m_stringBut[1]->isChecked()
        && !m_stringBut[2]->isChecked() && !m_stringBut[3]->isChecked()
        && !m_stringBut[4]->isChecked() && !m_stringBut[5]->isChecked() ) {
        m_stringBut[0]->setChecked(true);
    }
}

void rangeSettings::loadLevel(TexamLevel level) {
    disconnect (m_fromSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
    disconnect (m_toSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
		m_scoreRang->setClef(level.clef);
    m_scoreRang->setNote(0, level.loNote);
    m_scoreRang->setNote(1, level.hiNote);
    m_fromSpinB->setValue(level.loFret);
    m_toSpinB->setValue(level.hiFret);
    for (int i = 0; i < gl->Gtune()->stringNr(); i++)
        m_stringBut[i]->setChecked(level.usedStrings[i]);
    stringSelected();
    connect (m_fromSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
    connect (m_toSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
}

void rangeSettings::whenParamsChanged() {
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

void rangeSettings::saveLevel(TexamLevel &level) {
    if (m_scoreRang->getNote(0).getChromaticNrOfNote() <=
        m_scoreRang->getNote(0).getChromaticNrOfNote()) {
            level.loNote = m_scoreRang->getNote(0);
            level.hiNote = m_scoreRang->getNote(1);
        }
    else {
        level.loNote = m_scoreRang->getNote(1);
        level.hiNote = m_scoreRang->getNote(0);
    }
    if (m_fromSpinB->value() <= m_toSpinB->value()) {
        level.loFret = m_fromSpinB->value();
        level.hiFret = m_toSpinB->value();
    }
    else {
        level.loFret = m_toSpinB->value();
        level.hiFret = m_fromSpinB->value();
    }

    for (int i = 0; i < gl->Gtune()->stringNr(); i++)
        level.usedStrings[i] = m_stringBut[i]->isChecked();
		level.clef = m_scoreRang->clef();

}
