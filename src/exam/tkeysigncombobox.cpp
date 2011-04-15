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
#include "tkeysignatureview.h"

TkeySignComboBox::TkeySignComboBox(QWidget *parent, bool isMinor) :
    QComboBox(parent)
{
    m_isMinor = isMinor;
    setMaxVisibleItems(5);
    for (int i=0; i<15; i++) {
        if (m_isMinor)
            addItem(TkeySignatureView::minorKeysNames[i]);
        else
            addItem(TkeySignatureView::majorKeysNames[i]);
    }
}
