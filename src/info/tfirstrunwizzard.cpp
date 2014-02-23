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


#include "tfirstrunwizzard.h"
#include "taboutnootka.h"
#include "tglobals.h"
#include "select7note.h"
#include "tpixmaker.h"
#include "tmainhelp.h"
#include "widgets/troundedlabel.h"
#include "widgets/tselectinstrument.h"
#include "ttipchart.h"
#include "tkeysignature.h"
#include <ttune.h>
#include <tsimplescore.h>
#include <taudioparams.h>
#include <tscalepreviewlabel.h>
#include <QtGui>
#include <complex>

extern Tglobals *gl;


TfirstRunWizzard::TfirstRunWizzard(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
#if defined(Q_OS_LINUX)
    setWindowIcon(QIcon(gl->path + "picts/nootka.svg"));
#else
    setWindowIcon(QIcon(gl->path + "picts/about.png"));
#endif	
    setWindowTitle("Nootka   " + tr("First run wizard"));
    QVBoxLayout *lay = new QVBoxLayout;
    m_pagesLay = new QStackedLayout;
    lay->addLayout(m_pagesLay);

    QHBoxLayout *buttLay = new QHBoxLayout;
    buttLay->addStretch(1);
    m_skipButt = new QPushButton(tr("skip wizard"), this);
    buttLay->addWidget(m_skipButt);
    buttLay->addStretch(2);
    m_prevButt = new QPushButton(tr("previous"), this);
    m_prevButt->setDisabled(true);
    buttLay->addWidget(m_prevButt);
    m_nextButt = new QPushButton(nextText(), this);
    buttLay->addWidget(m_nextButt);
    buttLay->addStretch(1);

    lay->addLayout(buttLay);
    setLayout(lay);
	// First page
    Tabout *aboutNoot = new Tabout();
	// One and half page
		m_selectInstr = new TselectInstrument(this);
		m_selectInstr->setHeadLabel("<b><span style=\"font-size: 20px;\">" + tr("What instrument do you play?")  + "</span></b>");
		m_selectInstr->setInstrument(int(e_classicalGuitar));
	// Second Page
    m_notationWidget = new Tpage_2(this);
		whenInstrumentChanged(1);

    m_page3 = new Tpage_3();
    m_page4 = new TmainHelp(gl->path);

    m_pagesLay->addWidget(aboutNoot);
		m_pagesLay->addWidget(m_selectInstr);
    m_pagesLay->addWidget(m_notationWidget);
    m_pagesLay->addWidget(m_page3);
    m_pagesLay->addWidget(m_page4);
    
    // grab 7-th note from translation
    if (Tpage_3::note7txt().toLower() == "b") {
        gl->seventhIs_B = true; // rest NnameStyleInNoteName
        if (m_page3->keyNameStyle() == "solfege")
          gl->SnameStyleInKeySign = Tnote::e_italiano_Si;
        else
          gl->SnameStyleInKeySign = Tnote::e_nederl_Bis;
    }
    else {
        gl->seventhIs_B = false;
        gl->NnameStyleInNoteName = Tnote::e_norsk_Hb;
        if (m_page3->keyNameStyle() == "solfege")
          gl->SnameStyleInKeySign = Tnote::e_italiano_Si;
        else
          gl->SnameStyleInKeySign = Tnote::e_deutsch_His;
    }

    connect(m_skipButt, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_prevButt, SIGNAL(clicked()), this, SLOT(prevSlot()));
    connect(m_nextButt, SIGNAL(clicked()), this, SLOT(nextSlot()));
		connect(m_selectInstr, SIGNAL(instrumentChanged(int)), this, SLOT(whenInstrumentChanged(int)));
}


void TfirstRunWizzard::prevSlot() {
    switch (m_pagesLay->currentIndex()) {
    case 0 :
        break;
    case 1 :
        m_prevButt->setDisabled(true);
        m_pagesLay->setCurrentIndex(0);
        break;
    case 2 :
        m_pagesLay->setCurrentIndex(1);
        break;
		case 3 :
				m_pagesLay->setCurrentIndex(2);
        break;
    case 4 :
        m_nextButt->setText(nextText());
        m_pagesLay->setCurrentIndex(3);
        break;
    }
}


void TfirstRunWizzard::nextSlot() {
    switch (m_pagesLay->currentIndex()) {
    case 0 :
        m_prevButt->setDisabled(false);
        m_pagesLay->setCurrentIndex(1);
        break;
    case 1 :
				m_pagesLay->setCurrentIndex(2);
        break;
		case 2 :
        m_pagesLay->setCurrentIndex(3);
        break;
    case 3 :
        m_nextButt->setText(tr("Finish"));
        m_pagesLay->setCurrentIndex(4);
        break;
    case 4 :
        if (m_page3->select7->is7th_B()) {
            gl->seventhIs_B = true;
            gl->NnameStyleInNoteName = Tnote::e_english_Bb;
            if (m_page3->keyNameStyle() == "solfege")
              gl->SnameStyleInKeySign = Tnote::e_italiano_Si;
            else
              gl->SnameStyleInKeySign = Tnote::e_nederl_Bis;
        }
        else {
            gl->seventhIs_B = false;
            gl->NnameStyleInNoteName = Tnote::e_norsk_Hb;
            if (m_page3->keyNameStyle() == "solfege")
              gl->SnameStyleInKeySign = Tnote::e_italiano_Si;
            else
              gl->SnameStyleInKeySign = Tnote::e_deutsch_His;
        }
        gl->doubleAccidentalsEnabled = m_page3->dblAccChB->isChecked();
        gl->showEnharmNotes = m_page3->enharmChB->isChecked();
        gl->SkeySignatureEnabled = m_page3->useKeyChB->isChecked();
				if (gl->instrument == e_bassGuitar) {
            gl->setTune(Ttune::bassTunes[0]);
            gl->A->range = TaudioParams::e_low;
            gl->Sclef = Tclef::e_bass_F_8down;
            gl->A->audioInstrNr = (int)e_bassGuitar;
            gl->GfretsNumber = 20;
				} else if (gl->instrument == e_electricGuitar) {
            gl->A->audioInstrNr = (int)e_electricGuitar;
            gl->GfretsNumber = 23;
        } else if (gl->instrument == e_noInstrument) {
						gl->Sclef = m_notationWidget->score()->clef().type();
						Tnote hiN, loN; // fix notes order
						if (m_notationWidget->score()->getNote(1).getChromaticNrOfNote() <
										m_notationWidget->score()->getNote(0).getChromaticNrOfNote()) {
								hiN = m_notationWidget->score()->getNote(1);
								loN = m_notationWidget->score()->getNote(0);
						} else {
								hiN = m_notationWidget->score()->getNote(0);
								loN = m_notationWidget->score()->getNote(1);
						}
						Ttune instrScale("scale", Tnote(hiN.getChromaticNrOfNote() - gl->GfretsNumber), loN);
						gl->setTune(instrScale);
        }
        close();
        break;
    }
}

// To write notes of bass guitar this application uses <b>bass dropped clef</b> (bass clef with \"eight\" digit below) but common practice is to skip this digit and write it in ordinary bass clef. Remember, bass guitar sounds octave lower than notes written in 'normal' bass clef.
void TfirstRunWizzard::whenInstrumentChanged(int instr) {
	gl->instrument = Einstrument(instr);
	m_notationWidget->setNoteForInstrument(instr);
	if ((Einstrument)instr == e_bassGuitar)
				m_notationWidget->notationNote()->setHtml(QString("<center>%1<br>").
				arg(TtipChart::wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F, TkeySignature(0), 5.0)) +
				tr("When writing notation for bass guitar, the <b>bass clef</b> is used but the played notes sound an octave lower. The proper clef is <b>bass dropped clef</b> (with the digit \"eight\" written below) In this clef, the notes sound exactly as written. This clef is used in Nootka for bass guitar.") +
					"<br><br>" + TtipChart::wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F_8down, TkeySignature(0), 8.0));
	else if ((Einstrument)instr == e_classicalGuitar || (Einstrument)instr == e_electricGuitar)
				m_notationWidget->notationNote()->setHtml("<br><br><center>" + tr("Guitar notation uses the treble clef with the digit \"eight\" written below (even if some editors are forgetting about this digit).<br><br>Try to understand this. <br><br><p> %1 %2<br><span style=\"font-size:20px;\">Both pictures above show the same note: c<sup>1</sup></span><br>(note c in one-line octave)</p>").
				arg(TtipChart::wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G, TkeySignature(0), 6.0)).
				arg(TtipChart::wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G_8down, TkeySignature(0), 6.0)) + "</center>");
}

//###############################################  Tpage_2   ###############################################

Tpage_2::Tpage_2(QWidget* parent) :
	QWidget(parent),
	m_notationNote(0),
	m_score(0)
{
	m_lay = new QVBoxLayout;
	setLayout(m_lay);
}


void Tpage_2::setNoteForInstrument(int instr) {
	if ((Einstrument)instr == e_noInstrument) {
		if (!m_score) {
			if (m_notationNote) {
				delete m_notationNote;
				m_notationNote = 0;
			}
			m_scoreHint = new TroundedLabel(this);
			m_lay->addWidget(m_scoreHint);
			m_scoreHint->setFixedHeight(fontMetrics().boundingRect("A").height() * 4);
			m_scoreHint->setWordWrap(true);
			m_scoreHint->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			m_score = new TsimpleScore(2, this);
			m_lay->addWidget(m_score, 0, Qt::AlignCenter);
			m_score->addBGglyph((int)e_noInstrument);
			m_score->setClef(Tclef(Tclef::e_treble_G));
			clefChanged();
			scoreHint("");
			connect(m_score, SIGNAL(statusTip(QString)), this, SLOT(scoreHint(QString)));
			connect(m_score, SIGNAL(clefChanged(Tclef)), this, SLOT(clefChanged()));
			connect(m_score, SIGNAL(pianoStaffSwitched()), this, SLOT(clefChanged()));
		}
	} else {
		if (!m_notationNote) {
			if (m_score) {
				delete m_score;
				m_score = 0;
				delete m_scoreHint;
			}
			m_notationNote = new QTextEdit(this);
			layout()->addWidget(m_notationNote);
			m_notationNote->setWordWrapMode(QTextOption::WordWrap);
			m_notationNote->setReadOnly(true);
		}
	}
}


void Tpage_2::scoreHint(QString hint) {
	if (hint == "") 
		m_scoreHint->setText("<center>" + tr("Select a clef and scale of notes appropriate for your instrument."));
	else
		m_scoreHint->setText("<center>" + hint);
}


void Tpage_2::clefChanged() {
		m_score->setNote(0, m_score->lowestNote());
		m_score->setNote(1, m_score->highestNote());
}


//###############################################  Tpage_3   ###############################################

Tpage_3::Tpage_3(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    lay->setAlignment(Qt::AlignCenter);
    TroundedLabel *seventhLab = new TroundedLabel("<center>" + 
				tr("7th note can be B or H, depends on country<br>What is the name of 7th note in your country?") + 
				"</center>", this);
		seventhLab->setWordWrap(true);
		seventhLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay->addWidget(seventhLab);

    select7 = new Select7note(this);
    lay->addWidget(select7);
    if (Tpage_3::note7txt().toLower() == "b")
      select7->set7th_B(true);
    else
      select7->set7th_B(false);
		scaleLab = new TscalePreviewLabel(select7->is7th_B()? Tnote::e_english_Bb : Tnote::e_norsk_Hb, false, this);
		lay->addWidget(scaleLab, 0, Qt::AlignCenter);
    lay->addStretch(1);
		connect(select7, SIGNAL(seventhIsBchanged(bool)), this, SLOT(seventhNoteChanged(bool)));

    dblAccChB = new QCheckBox(tr("I know about double sharps (x) and double flats (bb)"), this);
    lay->addWidget(dblAccChB, 0, Qt::AlignCenter);
    dblAccChB->setChecked(gl->doubleAccidentalsEnabled);
//     lay->addStretch(1);

    enharmChB = new QCheckBox(tr("I know that e# is the same as f"), this);
    lay->addWidget(enharmChB, 0, Qt::AlignCenter);
    enharmChB->setChecked(gl->showEnharmNotes);
//     lay->addStretch(1);

    useKeyChB = new QCheckBox(tr("I know about key signatures"), this);
    lay->addWidget(useKeyChB, 0, Qt::AlignCenter);
    useKeyChB->setChecked(gl->SkeySignatureEnabled);
    lay->addStretch(1);

    setLayout(lay);
}

void Tpage_3::seventhNoteChanged(bool is7_B) {
		scaleLab->changeStyle(is7_B? Tnote::e_english_Bb : Tnote::e_norsk_Hb);
}

