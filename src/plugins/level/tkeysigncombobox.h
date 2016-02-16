/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   			      			                                 *
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


#ifndef TKEYSIGNCOMBOBOX_H
#define TKEYSIGNCOMBOBOX_H

#include <QComboBox>
#include <music/tkeysignature.h>

class TkeySignComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit TkeySignComboBox(QWidget *parent = 0);

    void setKeySignature(TkeySignature key);
    TkeySignature getKeySignature() { return char(currentIndex() -7); }

};

#endif // TKEYSIGNCOMBOBOX_H
