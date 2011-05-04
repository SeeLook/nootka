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


#include "tkeysigncombobox.h"
#include "tglobals.h"

extern Tglobals *gl;

TkeySignComboBox::TkeySignComboBox(QWidget *parent) :
    QComboBox(parent)
{
    if (TkeySignature::majorNames[0] == "")
        TkeySignature::setNameStyle(
                gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);
    for (int i=-7; i<8; i++) {
        TkeySignature k = TkeySignature(i);
        addItem("(" + k.accidNumber() + ") " + TkeySignature::majorNames[i+7] + " / "
                + TkeySignature::minorNames[i+7]);
    }
}

void TkeySignComboBox::setKeySignature(TkeySignature key) {
    setCurrentIndex(key.value()+7);
}
