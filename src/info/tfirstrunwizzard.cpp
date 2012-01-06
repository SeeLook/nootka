/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tfirstrunwizzard.h"
#include "taboutnootka.h"
#include "tglobals.h"
#include "select7note.h"
#include <QtGui>

extern Tglobals *gl;


TfirstRunWizzard::TfirstRunWizzard(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Nootka   "+tr("First run wizzard"));
    QVBoxLayout *lay = new QVBoxLayout;
    pagesLay = new QStackedLayout;
    lay->addLayout(pagesLay);

    QHBoxLayout *buttLay = new QHBoxLayout;
    buttLay->addStretch(1);
    skipButt = new QPushButton(tr("skip wizzard"), this);
    buttLay->addWidget(skipButt);
    buttLay->addStretch(2);
    prevButt = new QPushButton(tr("prev"), this);
    prevButt->setDisabled(true);
    buttLay->addWidget(prevButt);
    nextButt = new QPushButton(nextText(), this);
    buttLay->addWidget(nextButt);
    buttLay->addStretch(1);

    lay->addLayout(buttLay);
    setLayout(lay);

    Tabout *aboutNoot = new Tabout();
    QTextEdit *notationLab = new QTextEdit(tr("<center>Guitar notation uses treble clef with \"eight\" digit below (even if some editors are forgeting about this digit).<br><br>Try to understand this. <br><br><p> <img src=\"%1\"> <img src=\"%2\"><br><span style=\"font-size:20px;\">Both pictures above show the same note: c<sup>1</sup></span><br>(note c in one-line octave)</center></p>").arg(gl->path+"picts/c1-trebe.png").arg(gl->path+"picts/c1-treble_8.png"));
    notationLab->setWordWrapMode(QTextOption::WordWrap);
//    notationLab->setWordWrap(true);

    page3 = new Tpage_3();
    page4 = new Tpage_4();

    pagesLay->addWidget(aboutNoot);
    pagesLay->addWidget(notationLab);
    pagesLay->addWidget(page3);
    pagesLay->addWidget(page4);

    connect(skipButt, SIGNAL(clicked()), this, SLOT(close()));
    connect(prevButt, SIGNAL(clicked()), this, SLOT(prevSlot()));
    connect(nextButt, SIGNAL(clicked()), this, SLOT(nextSlot()));
}

void TfirstRunWizzard::prevSlot() {
    switch (pagesLay->currentIndex()) {
    case 0 :
        break;
    case 1 :
        prevButt->setDisabled(true);
        pagesLay->setCurrentIndex(0);
        break;
    case 2 :
        pagesLay->setCurrentIndex(1);
        break;
    case 3 :
        nextButt->setText(nextText());
        pagesLay->setCurrentIndex(2);
        break;
    }
}

void TfirstRunWizzard::nextSlot() {
    switch (pagesLay->currentIndex()) {
    case 0 :
        prevButt->setDisabled(false);
        pagesLay->setCurrentIndex(1);
        break;
    case 1 :
        pagesLay->setCurrentIndex(2);
        break;
    case 2 :
        nextButt->setText(tr("Finish"));
        pagesLay->setCurrentIndex(3);
        break;
    case 3 :
        if (page3->select7->is7th_B())
            gl->seventhIs_B = true;
        else {
            gl->seventhIs_B = false;
            gl->NnameStyleInNoteName = Tnote::e_norsk_Hb;
            gl->SnameStyleInKeySign = Tnote::e_norsk_Hb;
        }
        gl->doubleAccidentalsEnabled = page3->dblAccChB->isChecked();
        gl->showEnharmNotes = page3->enharmChB->isChecked();
        gl->SkeySignatureEnabled = page3->useKeyChB->isChecked();
        close();
        break;
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
    select7->set7th_B(true);

    lay->addStretch(1);

    dblAccChB = new QCheckBox(tr("I know about double sharps (x) and double flats (bb)"), this);
    lay->addWidget(dblAccChB, 0, Qt::AlignCenter);
    lay->addStretch(1);

    enharmChB = new QCheckBox(tr("I know that e# is the same as f"), this);
    lay->addWidget(enharmChB, 0, Qt::AlignCenter);
    lay->addStretch(1);

    useKeyChB = new QCheckBox(tr("I know about key signatures"), this);
    lay->addWidget(useKeyChB, 0, Qt::AlignCenter);
    lay->addStretch(1);

    setLayout(lay);
}

//###############################################  Tpage_4  ###############################################

Tpage_4::Tpage_4(QWidget* parent) :
    QWidget(parent)
{ 
  QVBoxLayout *lay = new QVBoxLayout;
  lay->addStretch(1);
  QLabel *lab = new QLabel(QString("<center><img src=\"%1\"><br><br><br>").arg(gl->path+"picts/pitch.png") +
    tr("Nootka can recognize played or sung sound<br>but it requires adjustment and some practise.<br>All needed settings are in <b>Preferences->Sound<b>.")
    , this );
  lay->addWidget(lab);
  lay->addStretch(1);
  setLayout(lay);
}

