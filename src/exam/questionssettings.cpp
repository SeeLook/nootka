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


//################################################################

TasNoteWdg::TasNoteWdg(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;
    asNoteGr = new TquestionAsWdg(this);
    mainLay->addWidget(asNoteGr);
    mainLay->addStretch(1);
    setLayout(mainLay);
}



TasNameWdg::TasNameWdg(QWidget *parent) :
    QWidget(parent)
{

}
