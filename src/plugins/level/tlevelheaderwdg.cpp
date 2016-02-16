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


#include "tlevelheaderwdg.h"
#include <QtWidgets/QtWidgets>


TlevelHeaderWdg::TlevelHeaderWdg(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *nameLab = new QLabel(tr("Level name:"), this);
    mainLay->addWidget(nameLab);
    nameEd = new QLineEdit(this);
    nameEd->setMaxLength(20);
    nameEd->setText(tr("new level"));
    mainLay->addWidget(nameEd);
    QLabel *descLab = new QLabel(tr("Level description:"), this);
    mainLay->addWidget(descLab);
    descEd = new QTextEdit(this);
    descEd->setFixedHeight(70);
    descEd->setLineWrapMode(QTextEdit::FixedColumnWidth);
    descEd->setLineWrapColumnOrWidth(35);
    mainLay->addWidget(descEd);
    okBut = new QPushButton(tr("OK"), this);
    mainLay->addWidget(okBut,1,Qt::AlignCenter);

    setLayout(mainLay);

    connect(descEd, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(okBut, SIGNAL(clicked()), this, SLOT(accept()));
}

QStringList TlevelHeaderWdg::getLevelName() {
    exec();
    QStringList list;
    list << nameEd->text() << descEd->toPlainText();
    return list;
}

void TlevelHeaderWdg::onTextChanged() {
    if (descEd->toPlainText().length() > 120 )
        descEd->setPlainText(descEd->toPlainText().right(120));
}
