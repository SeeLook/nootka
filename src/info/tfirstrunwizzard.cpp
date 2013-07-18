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


#include "tfirstrunwizzard.h"
#include "taboutnootka.h"
#include "tglobals.h"
#include "select7note.h"
#include "tpixmaker.h"
#include "ttipchart.h"
#include "tkeysignature.h"
#include <tsimplescore.h>
#include <QtGui>
#include <complex>

extern Tglobals *gl;


TfirstRunWizzard::TfirstRunWizzard(QWidget *parent) :
    QDialog(parent)
{
#if defined(Q_OS_LINUX)
    setWindowIcon(QIcon(gl->path + "picts/nootka.svg"));
#else
    setWindowIcon(QIcon(gl->path + "picts/about.png"));
#endif	
    setWindowTitle("Nootka   " + tr("First run wizzard"));
    QVBoxLayout *lay = new QVBoxLayout;
    m_pagesLay = new QStackedLayout;
    lay->addLayout(m_pagesLay);

    QHBoxLayout *buttLay = new QHBoxLayout;
    buttLay->addStretch(1);
    m_skipButt = new QPushButton(tr("skip wizzard"), this);
    buttLay->addWidget(m_skipButt);
    buttLay->addStretch(2);
    m_prevButt = new QPushButton(tr("prev"), this);
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
	// Second Page
    m_notationNote = new QTextEdit(this);
		whenInstrumentChanged(1);
    m_notationNote->setWordWrapMode(QTextOption::WordWrap);

    m_page3 = new Tpage_3();
    m_page4 = new Tpage_4();

    m_pagesLay->addWidget(aboutNoot);
		m_pagesLay->addWidget(m_selectInstr);
    m_pagesLay->addWidget(m_notationNote);
    m_pagesLay->addWidget(m_page3);
    m_pagesLay->addWidget(m_page4);
    
    // grab 7-th note from translation
    if (Tpage_3::note7txt().toLower() == "b") {
        gl->seventhIs_B = true; // rest (NnameStyleInNoteName
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
		case 3 : {
				if (m_selectInstr->otherRadio->isChecked()) // skip guita notation theory
						m_pagesLay->setCurrentIndex(1); // when other instrument is selected
				else
						m_pagesLay->setCurrentIndex(2);
        break;
		}
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
    case 1 : {
				if (m_selectInstr->otherRadio->isChecked()) // skip guita notation theory
						m_pagesLay->setCurrentIndex(3); // when other instrument is selected
				else
						m_pagesLay->setCurrentIndex(2);
        break;
		}
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
        close();
        break;
    }
}

// To write notes of bass guitar this application uses <b>bass dropped clef</b> (bass clef with \"eight\" digit below) but common practice is to skip this digit and write it in ordinary bass clef. Remember, bass guitar sounds octave lower than notes written in 'normal' bass clef.
void TfirstRunWizzard::whenInstrumentChanged(int instr) {
		if ((Einstrument)instr == e_bassGuitar)
				m_notationNote->setHtml(QString("<br><br><center>%1<br>").
				arg(TtipChart::wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F, TkeySignature(0), 5.0)) +
				tr("To write notes for bass guitat the <b>bass clef</b> is used but played notes sound octave down. The propper clef is <b>bass dropped clef</b> (with \"eight\" digit below) where notes sound exactly as written and this clef is used in Nootka for bass guitar.") +
					"<br><br>" + TtipChart::wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F_8down, TkeySignature(0), 8.0)	);
		else
				m_notationNote->setHtml("<br><br><center>" + tr("Guitar notation uses treble clef with \"eight\" digit below (even if some editors are forgeting about this digit).<br><br>Try to understand this. <br><br><p> %1 %2<br><span style=\"font-size:20px;\">Both pictures above show the same note: c<sup>1</sup></span><br>(note c in one-line octave)</p>").
				arg(TtipChart::wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G, TkeySignature(0), 6.0)).
				arg(TtipChart::wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G_8down, TkeySignature(0), 6.0)) + "</center>");
}


//###############################################  TselectInstrument   ######################################
TselectInstrument::TselectInstrument(QWidget* parent) :
	QWidget(parent)
{
		
		QLabel *whatLab = 
						new QLabel("<b><span style=\"font-size: 20px;\">" + tr("What instrument do you play?")  + "</span></b>", this);
		whatLab->setAlignment(Qt::AlignCenter);
		classicalRadio = new QRadioButton(instrumentToText(e_classicalGuitar), this);
		electricRadio = new QRadioButton(instrumentToText(e_electricGuitar), this);
		electricRadio->hide();
		bassRadio = new QRadioButton(instrumentToText(e_bassGuitar), this);
		otherRadio = new QRadioButton(instrumentToText(e_noInstrument), this);
		QButtonGroup *instrGr = new QButtonGroup(this);
		instrGr->addButton(classicalRadio);
		instrGr->addButton(electricRadio);
		instrGr->addButton(bassRadio);
		instrGr->addButton(otherRadio);
	// Layout
		QVBoxLayout *lay = new QVBoxLayout;
		lay->addWidget(whatLab, 0, Qt::AlignCenter);
		lay->addStretch(1);
		lay->addWidget(classicalRadio, 0, Qt::AlignCenter);
		lay->addWidget(electricRadio, 0, Qt::AlignCenter);
		lay->addWidget(bassRadio, 0, Qt::AlignCenter);
		lay->addWidget(otherRadio, 0, Qt::AlignCenter);
		
		QGroupBox *instrBox = new QGroupBox(this);
		instrBox->setLayout(lay);
		QVBoxLayout *mainLay = new QVBoxLayout;
		mainLay->addStretch(1);
		mainLay->addWidget(instrBox);
		mainLay->addStretch(1);
		setLayout(mainLay);
		
		classicalRadio->setChecked(true);
		connect(instrGr, SIGNAL(buttonClicked(int)), this, SLOT(buttonPressed(int)));
}


void TselectInstrument::buttonPressed(int butt) {
		if (bassRadio->isChecked()) {
			emit instrumentChanged((int)e_bassGuitar);
			gl->instrument = e_bassGuitar;
		}
		else {
			emit instrumentChanged((int)e_classicalGuitar);
			if (classicalRadio->isChecked())
				gl->instrument = e_classicalGuitar;
			else if (electricRadio->isChecked())
				gl->instrument = e_electricGuitar;
			else
				gl->instrument = e_noInstrument;
		}
}


//###############################################  Tpage_3   ###############################################

Tpage_3::Tpage_3(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    lay->setAlignment(Qt::AlignCenter);
    QLabel *seventhLab = new QLabel(tr("<center>7-th note can be B or H, depends on country<br>Which one is Yours?<br></center>"), this);
    lay->addWidget(seventhLab, 0, Qt::AlignCenter);

    select7 = new Select7note(this);
    lay->addWidget(select7);
    if (Tpage_3::note7txt().toLower() == "b")
      select7->set7th_B(true);
    else
      select7->set7th_B(false);

    lay->addStretch(1);

    dblAccChB = new QCheckBox(tr("I know about double sharps (x) and double flats (bb)"), this);
    lay->addWidget(dblAccChB, 0, Qt::AlignCenter);
    dblAccChB->setChecked(gl->doubleAccidentalsEnabled);
    lay->addStretch(1);

    enharmChB = new QCheckBox(tr("I know that e# is the same as f"), this);
    lay->addWidget(enharmChB, 0, Qt::AlignCenter);
    enharmChB->setChecked(gl->showEnharmNotes);
    lay->addStretch(1);

    useKeyChB = new QCheckBox(tr("I know about key signatures"), this);
    lay->addWidget(useKeyChB, 0, Qt::AlignCenter);
    useKeyChB->setChecked(gl->SkeySignatureEnabled);
    lay->addStretch(1);

    setLayout(lay);
}

//###############################################  Tpage_4  ###############################################

#define PIXSIZE (28)

Tpage_4::Tpage_4(QWidget* parent) :
    QWidget(parent)
{ 
  QVBoxLayout *lay = new QVBoxLayout;
//   QLabel *lab = new QLabel(QString("<center><img src=\"%1\"><br><br><br>").arg(gl->path+"picts/pitch.png") +
//     tr("Nootka can recognize played or sung sound<br>but it requires adjustment and some practise.<br>All needed settings are in <b>Preferences->Sound<b>.")
//     , this );
  QString helpTxt = "<br><b>" + tr("A play with Nootka we can divide on three stages:", "Don't try to translate the entries in this context/section too strict, rather use some nice words into Your language to describe it. The statements like '%1' are images(icons) in text");
  helpTxt += "</b><table><tr><td>&nbsp;</td><td><hr><b>";
  
  helpTxt += tr("I. Discovering") + "</b><br>";
  helpTxt += tr("Explore the interface of the application and how musical scores work. Just click on elements of the interface see and get to know. Also You can play or sing if You have any mike or web-cam.") + "<br>";
  helpTxt += tr("Press %1 buttons to glimpse this help and %2 button to adjust the application to yourself.").
      arg(" &nbsp; " + pixToHtml(gl->path+"picts/about.png", PIXSIZE) + " <span style=\"font-size: 25px;\"> + </span> " + pixToHtml(gl->path+"picts/help.png", PIXSIZE)  + " &nbsp; ").
      arg(" &nbsp; " + pixToHtml(gl->path+"picts/systemsettings.png", PIXSIZE) + " &nbsp; ");
      
  helpTxt += "<hr><b>" + tr("II. Playing with Nootka") + "</b><br>";
  helpTxt += tr("Literally and metaphorically. The application will be Your strict and \"old school\" master. Press %1 button, select some level and try to pass an exam by answering on questions. To answer You can play, sing, put a name of a note and so on. Depends on chosen level.<br>When You miss, You got penalties...", "hmmm, 'play' has doble meaning (a fun and performing) in english only, so adjust it as You feel").arg(" &nbsp; " + pixToHtml(gl->path+"picts/startExam.png", PIXSIZE) + " &nbsp; ");
  helpTxt += "<br>" + tr("Press %1 button to create Your own questions set (a level).").
    arg(" &nbsp; " + pixToHtml(gl->path+"picts/levelCreator.png", PIXSIZE) + " &nbsp; ");
  
  helpTxt += "<hr><b>" + tr("III. Analyseing") + "</b><br>";
  helpTxt += tr("Nootka will tell You about what You've been thinking for so long...<br>Press %1 button to watch and to analyse results of Your exams. Find Your weak points and improve.").
    arg(" &nbsp; " + pixToHtml(gl->path+"picts/charts.png", PIXSIZE) + " &nbsp; ");;
  helpTxt += "</td></tr></table>";
  helpTxt += "<hr><b><span style=\"font-size: 25px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + tr("Have a good fun!") + "</span></b>";
  QTextEdit *helpEdit = new QTextEdit(this);
  helpEdit->setHtml(helpTxt);
  lay->addWidget(helpEdit);
  setLayout(lay);
}

