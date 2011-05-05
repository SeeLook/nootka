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


#include "tstartexamdlg.h"
#include "tlevelselector.h"

TstartExamDlg::TstartExamDlg(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    levelRadio = new QRadioButton(tr("start new exam"), this);
    levLay->addWidget(levelRadio);
    QHBoxLayout *nameLay = new QHBoxLayout;
    QLabel *nameLab = new QLabel(tr("student's name:"), this);
    nameLay->addWidget(nameLab);
    nameEdit = new QLineEdit(this);
    nameEdit->setMaxLength(20);
    nameLay->addWidget(nameEdit);
    levLay->addLayout(nameLay);
    levelsView = new TlevelSelector(this);
    levLay->addWidget(levelsView);
    createBut = new QPushButton(tr("create new level"),this);
    levLay->addWidget(createBut);
    levelGr = new QGroupBox(this);
    levelGr->setLayout(levLay);

    mainLay->addWidget(levelGr);

    QVBoxLayout *exLay = new QVBoxLayout;
    contRadio = new QRadioButton(tr("continue exam"), this);
    exLay->addWidget(contRadio);
    examCombo = new QComboBox(this);
    exLay->addWidget(examCombo);
    loadExamBut = new QPushButton(tr("load exam from file"), this);
    exLay->addWidget(loadExamBut);
    examGr = new QGroupBox(this);
    examGr->setLayout(exLay);

    mainLay->addWidget(examGr);

    QGroupBox *hGr = new QGroupBox(this);
    QVBoxLayout *hLay = new QVBoxLayout;
    hint = new QLabel(this);
    hLay->addWidget(hint);
    hGr->setLayout(hLay);
    hint->setFixedHeight(70);
    hint->setWordWrap(true);

    mainLay->addWidget(hGr);

    QHBoxLayout *butLay = new QHBoxLayout;
    startBut = new QPushButton(tr("Start"), this);
    butLay->addWidget(startBut);
    cancelBut = new QPushButton(tr("Discard"), this);
    butLay->addWidget(cancelBut);

    mainLay->addLayout(butLay);
    setLayout(mainLay);


}

TstartExamDlg::Eactions TstartExamDlg::showDialog(QString &txt) {
    exec();

    return e_none;
}

bool TstartExamDlg::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        hint->setText("<center>"+se->tip()+"</center>");
    }
    return QDialog::event(event);
}
