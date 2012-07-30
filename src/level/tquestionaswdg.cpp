/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tquestionaswdg.h"
#include <QtGui>



TquestionAsWdg::TquestionAsWdg(QString title, QGridLayout* gridLay, int row, QWidget* parent) :
    QObject(parent)
{
//     setStatusTip(tr("Select a type of answers for a question."));
//     QHBoxLayout *mainLay = new QHBoxLayout;
//     mainLay->addStretch(1);
//     QLabel *answersLab = new QLabel(answersTxt() + ":",this);
//     mainLay->addWidget(answersLab,0,Qt::AlignCenter);
//     asNoteChB = new QCheckBox(asNoteTxt(), this);
    questLab = new QLabel(title, parent);
    questLab->setStatusTip(tr("Select a type of answers for a question."));
    gridLay->addWidget(questLab, row, 0, Qt::AlignRight);
//     mainLay->addWidget(questLab);
    enableChBox = new QCheckBox(parent);
    gridLay->addWidget(enableChBox, row, 1);
//     mainLay->addWidget(enableChBox);
    asNoteChB = new QCheckBox(parent);
    gridLay->addWidget(asNoteChB, row, 2);
//     mainLay->addWidget(asNoteChB);
//     asNameChB = new QCheckBox(asNameTxt(), this);
    asNameChB = new QCheckBox(parent);
    gridLay->addWidget(asNameChB, row, 3);
//     mainLay->addWidget(asNameChB);
//     asFretPosChB = new QCheckBox(asFretPosTxt(), this);
    asFretPosChB = new QCheckBox(parent);
    gridLay->addWidget(asFretPosChB, row, 4);
//     mainLay->addWidget(asFretPosChB);
//     asSoundChB = new QCheckBox(asSoundTxt(), this);
    asSoundChB = new QCheckBox(parent);
    gridLay->addWidget(asSoundChB, row, 5);
//     mainLay->addWidget(asSoundChB);
//     mainLay->addStretch(1);
//     setLayout(mainLay);

//     setCheckable(true);

    connect(asNoteChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asNameChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asFretPosChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asSoundChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));

//     connect(this, SIGNAL(clicked()), this, SLOT(groupChecked()));
    connect(enableChBox, SIGNAL(clicked()), this, SLOT(groupChecked()));

}

void TquestionAsWdg::buttonClicked() {
    if (!asNoteChB->isChecked() && !asNameChB->isChecked() && !asFretPosChB->isChecked()
        && !asSoundChB->isChecked())
//         setChecked(false);
    enableChBox->setChecked(false);
    emit answerStateChanged();
}



void TquestionAsWdg::groupChecked() {
//     if (isChecked()) {
  if (enableChBox->isChecked()) {
        asNoteChB->setChecked(true);
        asNameChB->setChecked(true);
        asFretPosChB->setChecked(true);
        asSoundChB->setChecked(true);
    } else {
        asNoteChB->setChecked(false);
        asNameChB->setChecked(false);
        asFretPosChB->setChecked(false);
        asSoundChB->setChecked(false);
    }
}

void TquestionAsWdg::setAnswers(TQAtype types) {
    asNoteChB->setChecked(types.isNote());
    asNameChB->setChecked(types.isName());
    asFretPosChB->setChecked(types.isFret());
    asSoundChB->setChecked(types.isSound());
}

TQAtype TquestionAsWdg::getAnswers() {
    TQAtype t;
    t.setAsNote(asNoteChB->isChecked());
    t.setAsName(asNameChB->isChecked());
    t.setAsFret(asFretPosChB->isChecked());
    t.setAsSound(asSoundChB->isChecked());
    return t;
}

void TquestionAsWdg::setTitle(QString title) {
  questLab->setText(title);
}
