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


#include "tlevelheaderwdg.h"

TlevelHeaderWdg::TlevelHeaderWdg(QString &lName, QString &lDesc, QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *nameLab = new QLabel(tr("Level's name:"),this);
    mainLay->addWidget(nameLab);
    nameEd = new QLineEdit(this);
    nameEd->setMaxLength(20);
    mainLay->addWidget(nameEd);
    QLabel *descLab = new QLabel(tr("Level's description:"),this);
    mainLay->addWidget(descLab);
    descEd = new QTextEdit(this);
    mainLay->addWidget(descEd);
}
