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


#include "tnotationradiogroup.h"
#include "tglobals.h"

extern Tglobals *gl;

const QString TnotationRadioGroup::strNeder = tr("Dutch");	//nederlands in Lilypond
const QString TnotationRadioGroup::strNorsk = tr("Scandinavian");//norsk in Lilypond
const QString TnotationRadioGroup::strItal = tr("Italian");
const QString TnotationRadioGroup::strEnglish = tr("English");
const QString TnotationRadioGroup::strDeutsch = tr("German");
const QString TnotationRadioGroup::strNorskExampl = "(C, C#, Db ... Hb, H)";
const QString TnotationRadioGroup::strItalExampl = "(Do, Do#, Reb ... Sib, Si)";
const QString TnotationRadioGroup::strDeutschExampl = "(C, Cis, Des ... B, H)";
const QString TnotationRadioGroup::strEnglishExampl = "(C, C#, Db ... Bb, B)";
const QString TnotationRadioGroup::strNederExampl = "(C, Cis, Des ... Bes, B)";


TnotationRadioGroup::TnotationRadioGroup( Tnote::EnameStyle _notation, QWidget * parent )
        : QGroupBox(parent)
{
    notation = _notation;
    setTitle(tr("Nameing style"));
    setStatusTip(tr("Nameing style of a note. The main difference is 7-th note.<br>Is it B and B flat, or H and B ?"));
    norskButt = new QRadioButton(strNorsk+" "+strNorskExampl ,this);
    italianoButt = new QRadioButton(strItal+" "+strItalExampl,this);
    deutschButt = new QRadioButton(strDeutsch+" "+strDeutschExampl,this);
    englishButt = new QRadioButton(strEnglish+" "+strEnglishExampl,this);
    nederlButt = new QRadioButton(strNeder+" "+strNederExampl,this);
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(norskButt);
    lay->addWidget(italianoButt);
    lay->addWidget(deutschButt);
    lay->addWidget(englishButt);
    lay->addWidget(nederlButt);
    lay->addStretch(1);
    setLayout(lay);

    seventhNoteWasChanged(gl->seventhIs_B);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(norskButt);
    buttonGroup->addButton(italianoButt);
    buttonGroup->addButton(deutschButt);
    buttonGroup->addButton(englishButt);
    buttonGroup->addButton(nederlButt);

    connect (buttonGroup, SIGNAL(buttonClicked(int)) ,this, SLOT(noteNameStyleWasClicked()) );
        // 			RESTORING SETTINGS
    switch (notation)	{
        case Tnote::e_norsk_Hb : norskButt->setChecked(true); break;
        case Tnote::e_deutsch_His : deutschButt->setChecked(true); break;
        case Tnote::e_italiano_Si : italianoButt->setChecked(true); break;
        case Tnote::e_english_Bb : englishButt->setChecked(true); break;
        case Tnote::e_nederl_Bis : nederlButt->setChecked(true); break;
    }
}



Tnote::EnameStyle TnotationRadioGroup::getNameStyle() {
    if (norskButt->isChecked()) return Tnote::e_norsk_Hb;
    else
        if (deutschButt->isChecked()) return Tnote::e_deutsch_His;
        else
            if (italianoButt->isChecked()) return Tnote::e_italiano_Si;
            else
                if (englishButt->isChecked()) return Tnote::e_english_Bb;
                else
                    return Tnote::e_nederl_Bis;
}

void TnotationRadioGroup::noteNameStyleWasClicked() {
   emit noteNameStyleWasChanged(getNameStyle());
}

void TnotationRadioGroup::seventhNoteWasChanged(bool isB) {
    if (isB) {
        norskButt->setDisabled(true);
        deutschButt->setDisabled(true);
        englishButt->setDisabled(false);
        nederlButt->setDisabled(false);
        if (deutschButt->isChecked()) nederlButt->setChecked(true);
        if (norskButt->isChecked()) englishButt->setChecked(true);
    } else {
        norskButt->setDisabled(false);
        deutschButt->setDisabled(false);
        englishButt->setDisabled(true);
        nederlButt->setDisabled(true);
        if (nederlButt->isChecked()) deutschButt->setChecked(true);
        if (englishButt->isChecked()) norskButt->setChecked(true);
    }


}


