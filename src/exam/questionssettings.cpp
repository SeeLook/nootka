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


#include "questionssettings.h"

questionsSettings::questionsSettings(QWidget *parent) :
    QWidget(parent)
{
    questAsToolBox = new QToolBox(this);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel(TquestionAsWdg::questionsTxt, this);
    mainLay->addWidget(headLab, 0, Qt::AlignCenter);
    mainLay->addWidget(questAsToolBox);
    setLayout(mainLay);

    TasNoteWdg *asNoteWdg = new TasNoteWdg();
    TasNameWdg *asNameWdg = new TasNameWdg();

    questAsToolBox->addItem(asNoteWdg, TquestionAsWdg::asNoteTxt);
    questAsToolBox->addItem(asNameWdg, TquestionAsWdg::asNameTxt);

}


//############################# AS NOTE IN SCORE ###################################

TasNoteWdg::TasNoteWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    QHBoxLayout *upperLay = new QHBoxLayout;
    asNoteGr = new TquestionAsWdg(this);
    upperLay->addWidget(asNoteGr);

    QVBoxLayout *accLay = new QVBoxLayout;
    sharpsChB = new QCheckBox(tr("sharps"),this);
    flatsChB = new QCheckBox(tr("flats"),this);
    doubleAccChB = new QCheckBox(tr("double accidentals"),this);
    accLay->addWidget(sharpsChB);
    accLay->addWidget(flatsChB);
    accLay->addWidget(doubleAccChB);
    accidGr = new QGroupBox(tr("accidentals"),this);
    accidGr->setCheckable(true);
    accidGr->setLayout(accLay);
    upperLay->addWidget(accidGr);

    mainLay->addLayout(upperLay);

    QHBoxLayout *keyLay = new QHBoxLayout;
    keySignGr = new QGroupBox(tr("use keys singature"),this);
    keySignGr->setCheckable(true);

    QVBoxLayout *rangeLay = new QVBoxLayout;
    singleKeyRadio = new QRadioButton(tr("single key"),this);
    rangeKeysRadio = new QRadioButton(tr("range of keys"),this);
    rangeButGr = new QButtonGroup(this);
    rangeButGr->addButton(singleKeyRadio);
    rangeButGr->addButton(rangeKeysRadio);
    rangeLay->addWidget(singleKeyRadio);
    rangeLay->addWidget(rangeKeysRadio);
    QHBoxLayout *comboLay = new QHBoxLayout;
    fromKeyCombo = new TkeySignComboBox(this,false);
    toKeyCombo = new TkeySignComboBox(this,false);
    comboLay->addWidget(fromKeyCombo);
    comboLay->addStretch(1);
    comboLay->addWidget(toKeyCombo);
    rangeLay->addLayout(comboLay);
    keyLay->addLayout(rangeLay);
    keyLay->addStretch(1);

    QVBoxLayout *modeLay = new QVBoxLayout;
    majorOnlyRadio = new QRadioButton(tr("major keys only"),this);
    minorOnlyRadio = new QRadioButton(tr("minor keys only"),this);
    bothKeysRadio = new QRadioButton(tr("major and minor"),this);
    modeButGr = new QButtonGroup(this);
    modeButGr->addButton(majorOnlyRadio);
    modeButGr->addButton(minorOnlyRadio);
    modeButGr->addButton(bothKeysRadio);
    modeLay->addWidget(majorOnlyRadio);
    modeLay->addWidget(minorOnlyRadio);
    modeLay->addWidget(bothKeysRadio);
    keyLay->addLayout(modeLay);

    keySignGr->setLayout(keyLay);

    mainLay->addWidget(keySignGr);



    setLayout(mainLay);
}

//############################# AS NOTE'S NAME  ###################################

TasNameWdg::TasNameWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asNameGr = new TquestionAsWdg(this);
    mainLay->addWidget(asNameGr);

    setLayout(mainLay);
}
