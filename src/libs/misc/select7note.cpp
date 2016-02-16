/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   						                                       *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "select7note.h"
#include <QtWidgets>


Select7note::Select7note(QWidget *parent) :
    QWidget(parent)
{
  QGroupBox *bGr = new QGroupBox(this);
  QLabel *seventhLab = new QLabel(tr("7th note is:"),this);
  QButtonGroup *bButtGr = new QButtonGroup(this);
  isBRadio = new QRadioButton("B",this);
  isBRadio->setStatusTip(tr("7th note is <b>B</b> and with flat is <b>Bb</b> or <b>bes</b> or <b>bs</b>"));
  bButtGr->addButton(isBRadio);
  isHRadio = new QRadioButton("H",this);
  isHRadio->setStatusTip(tr("7th note is <b>H</b> and with flat is <b>Hb</b> or <b>B</b>"));
  bButtGr->addButton(isHRadio);
  QHBoxLayout *bLay = new QHBoxLayout;
    bLay->setAlignment(Qt::AlignCenter);
//     bLay->addStretch(1);
    bLay->addWidget(seventhLab);
    bLay->addSpacing(15);
    bLay->addWidget(isBRadio);
    bLay->addSpacing(15);
    bLay->addWidget(isHRadio);
//     bLay->addStretch(1);
  bGr->setLayout(bLay);
  
  QVBoxLayout *lay = new QVBoxLayout;
    lay->addWidget(bGr);

  setLayout(lay);

  connect(bButtGr, SIGNAL(buttonClicked(int)), this, SLOT(namechanged()));

}

void Select7note::namechanged() {
    if(isBRadio->isChecked())
        emit seventhIsBchanged(true);
    else
        emit seventhIsBchanged(false);
}

void Select7note::set7th_B(bool isB) {
    if(isB)
        isBRadio->setChecked(true);
    else
        isHRadio->setChecked(true);
}

