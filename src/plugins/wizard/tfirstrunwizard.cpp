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


#include "tfirstrunwizard.h"
#include <taboutnootka.h>
#include <tscalepreviewlabel.h>
#include <select7note.h>
#include <help/tmainhelp.h>
#include <tmisctrans.h>
#include <tinitcorelib.h>
#include <graphics/tnotepixmap.h>
#include <widgets/troundedlabel.h>
#include <widgets/tselectinstrument.h>
#include <graphics/tnotepixmap.h>
#include <music/tkeysignature.h>
#include <music/ttune.h>
#include <score/tsimplescore.h>
#include <taudioparams.h>
#include <tscoreparams.h>
#include <tlayoutparams.h>
#include <QtWidgets>



TfirstRunWizard::TfirstRunWizard(QWidget *parent) :
    QWizard(parent)
{
  setWindowIcon(QIcon(Tcore::gl()->path + "picts/nootka.png"));
  setWindowTitle("Nootka   " + tr("First run wizard"));
  setWizardStyle(ClassicStyle);
  QPixmap bgPix(Tcore::gl()->path + "picts/wizard-left.png");
  setPixmap(WatermarkPixmap, bgPix);  
  
// 1. About Nootka page
  Tabout *aboutNoot = new Tabout();
  QWizardPage *aboutPage = new QWizardPage;
  QVBoxLayout *aLay = new QVBoxLayout;
  aLay->addWidget(aboutNoot);
  aboutPage->setLayout(aLay);
// 2. Select instrument page
  m_selectInstr = new TselectInstrument();
  m_selectInstr->setInstrument(int(e_classicalGuitar));
  QWizardPage *instrumentPage = new QWizardPage;
  instrumentPage->setTitle("<center>" + tr("What instrument do you play?") + "</center>");
  QVBoxLayout *iLay = new QVBoxLayout;
  iLay->addStretch();
  iLay->addWidget(m_selectInstr, 0, Qt::AlignCenter);
  iLay->addStretch();
  instrumentPage->setLayout(iLay);
// 3. Select clef and instrument note range
  m_notationWidget = new Tpage_2;
  whenInstrumentChanged(1);
// 4. Some options page 
  m_page3 = new Tpage_3();
// 5. Last page with some help
  m_page4 = new TmainHelp();
  QWizardPage *helpPage = new QWizardPage;
  QVBoxLayout *hLay = new QVBoxLayout;
  hLay->addWidget(m_page4);
  helpPage->setLayout(hLay);

  addPage(aboutPage);
  addPage(instrumentPage);
  addPage(m_notationWidget);
  addPage(m_page3);
  addPage(helpPage);
  
  
  // grab 7-th note from translation
  if (TmiscTrans::note7txt().toLower() == "b") {
      Tcore::gl()->S->seventhIs_B = true; // rest S->nameStyleInNoteName
      if (TmiscTrans::keyNameStyle() == "solfege")
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_nederl_Bis;
  }
  else {
      Tcore::gl()->S->seventhIs_B = false;
      Tcore::gl()->S->nameStyleInNoteName = Tnote::e_norsk_Hb;
      if (TmiscTrans::keyNameStyle() == "solfege")
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_deutsch_His;
  }

  connect(this, &TfirstRunWizard::currentIdChanged, this, &TfirstRunWizard::pageChanged);
  connect(m_selectInstr, &TselectInstrument::instrumentChanged, this, &TfirstRunWizard::whenInstrumentChanged);
}


void TfirstRunWizard::pageChanged(int pageNr) {
  if (pageNr == 1) { // Take out keyboard focus
    if (qApp->focusWidget()) // instrument page highlights 2nd button (classicla) by default, but keyboard focus goes to first one
      qApp->focusWidget()->clearFocus(); // it may be confusing if some style have the same colors for focus and highlight.
  }
}


void TfirstRunWizard::done(int result) {
  if (result == QDialog::Rejected) { // restore defaults
    Tcore::gl()->instrument = e_classicalGuitar;
    m_page3->select7->set7th_B(TmiscTrans::note7txt().toLower() == QLatin1String("b"));
    m_page3->dblAccChB->setChecked(false);
    m_page3->enharmChB->setChecked(false);
    m_page3->useKeyChB->setChecked(false);
  }
  if (m_page3->select7->is7th_B()) {
      Tcore::gl()->S->seventhIs_B = true;
      Tcore::gl()->S->nameStyleInNoteName = Tnote::e_english_Bb;
      if (TmiscTrans::keyNameStyle() == "solfege")
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_nederl_Bis;
  } else {
      Tcore::gl()->S->seventhIs_B = false;
      Tcore::gl()->S->nameStyleInNoteName = Tnote::e_norsk_Hb;
      if (TmiscTrans::keyNameStyle() == "solfege")
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_deutsch_His;
  }
  if (QLocale::system().name().contains("ru")) // override name style for Russian localization
    Tcore::gl()->S->nameStyleInNoteName = Tnote::e_russian_Ci;        
  Tcore::gl()->S->doubleAccidentalsEnabled = m_page3->dblAccChB->isChecked();
  Tcore::gl()->S->showEnharmNotes = m_page3->enharmChB->isChecked();
  Tcore::gl()->S->keySignatureEnabled = m_page3->useKeyChB->isChecked();
  if (Tcore::gl()->instrument == e_bassGuitar) {
      Tcore::gl()->setTune(Ttune::bassTunes[0]);
      Tcore::gl()->S->clef = Tclef::e_bass_F_8down;
      Tcore::gl()->A->audioInstrNr = (int)e_bassGuitar;
      Tcore::gl()->GfretsNumber = 20;
      Tcore::gl()->A->minSplitVol = 7.0;
  } else if (Tcore::gl()->instrument == e_electricGuitar) {
      Tcore::gl()->A->audioInstrNr = (int)e_electricGuitar;
      Tcore::gl()->GfretsNumber = 23;
      Tcore::gl()->A->minSplitVol = 7.0;
  } else if (Tcore::gl()->instrument == e_noInstrument) {
      Tcore::gl()->L->guitarEnabled = false;
      Tcore::gl()->S->clef = m_notationWidget->score()->clef().type();
      Tnote hiN, loN; // fix notes order
      if (m_notationWidget->score()->getNote(1).chromatic() <
              m_notationWidget->score()->getNote(0).chromatic()) {
          hiN = m_notationWidget->score()->getNote(0);
          loN = m_notationWidget->score()->getNote(1);
      } else {
          hiN = m_notationWidget->score()->getNote(1);
          loN = m_notationWidget->score()->getNote(0);
      }
      Ttune instrScale("scale", Tnote(hiN.chromatic() - Tcore::gl()->GfretsNumber), loN);
      Tcore::gl()->setTune(instrScale);
      Tcore::gl()->A->detectMethod = 0; // MPM for other instruments
      Tcore::gl()->A->minSplitVol = 0.0;
      Tcore::gl()->A->skipStillerVal = 0.0;
  }
  QWizard::done(result);
}


// To write notes of bass guitar this application uses <b>bass dropped clef</b> (bass clef with \"eight\" digit below) but common practice is to skip this digit and write it in ordinary bass clef. Remember, bass guitar sounds octave lower than notes written in 'normal' bass clef.
void TfirstRunWizard::whenInstrumentChanged(int instr) {
	Tcore::gl()->instrument = Einstrument(instr);
	m_notationWidget->setNoteForInstrument(instr);
	if ((Einstrument)instr == e_bassGuitar)
				m_notationWidget->notationNote()->setHtml(QString("<center>%1<br>").
				arg(wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F, TkeySignature(0), 5.0)) +
				tr("When writing notation for bass guitar, the <b>bass clef</b> is used but the played notes sound an octave lower. The proper clef is <b>bass dropped clef</b> (with the digit \"eight\" written below) In this clef, the notes sound exactly as written. This clef is used in Nootka for bass guitar.") +
					"<br><br>" + wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F_8down, TkeySignature(0), 8.0));
	else if ((Einstrument)instr == e_classicalGuitar || (Einstrument)instr == e_electricGuitar)
				m_notationWidget->notationNote()->setHtml("<br><br><center>" + tr("Guitar notation uses the treble clef with the digit \"eight\" written below (even if some editors are forgetting about this digit).<br><br>Try to understand this. <br><br><p> %1 %2<br><b><big>Both pictures above show the same note: c<sup>1</sup></big></b><br>(note c in one-line octave)</p>").
				arg(wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G, TkeySignature(0), 6.0)).
				arg(wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G_8down, TkeySignature(0), 6.0)) + "</center>");
}

//###############################################  Tpage_2   ###############################################

Tpage_2::Tpage_2(QWidget* parent) :
	QWizardPage(parent),
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
      setTitle("<center>" + tr("Select a clef and scale of notes appropriate for your instrument.") + "</center>");
			m_score = new TsimpleScore(2, this);
      m_lay->addStretch();
			m_lay->addWidget(m_score, 0, Qt::AlignCenter);
			m_score->addBGglyph((int)e_noInstrument);
			m_score->setClef(Tclef(Tclef::e_treble_G));
      m_score->setControllersEnabled(false, false); // hide both note controllers
      m_score->setMinimumSize(contentsRect().size() / 1.5);
			clefChanged();
      m_lay->addStretch();
      m_scoreHint = new TroundedLabel(this);
      m_lay->addWidget(m_scoreHint);
      m_scoreHint->setFixedHeight(fontMetrics().boundingRect("A").height() * 4);
      m_scoreHint->setWordWrap(true);
      m_scoreHint->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			scoreHint("");
			connect(m_score, SIGNAL(statusTip(QString)), this, SLOT(scoreHint(QString)));
			connect(m_score, SIGNAL(clefChanged(Tclef)), this, SLOT(clefChanged()));
		}
	} else {
		if (!m_notationNote) {
			if (m_score) {
				delete m_score;
				m_score = 0;
				delete m_scoreHint;
        setTitle("");
        QLayoutItem *child;
        while ((child = m_lay->takeAt(0)) != 0)
            delete child;
			}
			m_notationNote = new QTextEdit(this);
			layout()->addWidget(m_notationNote);
			m_notationNote->setWordWrapMode(QTextOption::WordWrap);
			m_notationNote->setReadOnly(true);
		}
	}
}


void Tpage_2::scoreHint(QString hint) {
  m_scoreHint->setText("<center>" + hint);
}


void Tpage_2::clefChanged() {
  m_score->setNote(0, m_score->lowestNote());
  m_score->setAmbitus(0, m_score->lowestNote(), m_score->highestNote());
  m_score->setNote(1, m_score->highestNote());
  m_score->setAmbitus(1, m_score->lowestNote(), m_score->highestNote());
}


//###############################################  Tpage_3   ###############################################

Tpage_3::Tpage_3(QWidget *parent) :
        QWizardPage(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    lay->setAlignment(Qt::AlignCenter);
    setTitle("<center>" + tr("7th note can be B or H, depends on country<br>What is the name of 7th note in your country?").replace("<br>", " ") 
            + "</center>");

    select7 = new Select7note(this);
    lay->addWidget(select7);
    select7->set7th_B(TmiscTrans::note7txt().toLower() == "b");
		scaleLab = new TscalePreviewLabel(select7->is7th_B()? Tnote::e_english_Bb : Tnote::e_norsk_Hb, false, this);
		lay->addWidget(scaleLab, 0, Qt::AlignCenter);
    lay->addStretch(1);
		connect(select7, SIGNAL(seventhIsBchanged(bool)), this, SLOT(seventhNoteChanged(bool)));
    lay->addStretch(1);
    dblAccChB = new QCheckBox(tr("I know about double sharps (x) and double flats (bb)"), this);
    lay->addWidget(dblAccChB, 0, Qt::AlignCenter);
    dblAccChB->setChecked(Tcore::gl()->S->doubleAccidentalsEnabled);

    enharmChB = new QCheckBox(tr("I know that e# is the same as f"), this);
    lay->addWidget(enharmChB, 0, Qt::AlignCenter);
    enharmChB->setChecked(Tcore::gl()->S->showEnharmNotes);
//     lay->addStretch(1);

    useKeyChB = new QCheckBox(tr("I know about key signatures"), this);
    lay->addWidget(useKeyChB, 0, Qt::AlignCenter);
    useKeyChB->setChecked(Tcore::gl()->S->keySignatureEnabled);
    lay->addStretch(1);

    setLayout(lay);
}

void Tpage_3::seventhNoteChanged(bool is7_B) {
		scaleLab->changeStyle(is7_B? Tnote::e_english_Bb : Tnote::e_norsk_Hb);
}

