/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk  				                   *
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


#ifndef SELECT7NOTE_H
#define SELECT7NOTE_H

#include "nootkamiscglobal.h"
#include <QWidget>
#include <QRadioButton>

class NOOTKAMISC_EXPORT Select7note : public QWidget
{
    Q_OBJECT
public:
    explicit Select7note(QWidget *parent = 0);
    void set7th_B (bool isB);
    bool is7th_B () { return isBRadio->isChecked(); }

signals:
    void seventhIsBchanged (bool isB);


private:
    QRadioButton *isBRadio, *isHRadio;

private slots:
    void namechanged();

};

#endif // SELECT7NOTE_H
