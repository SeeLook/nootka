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


#include "tquestionaswdg.h"

/*static*/
QString TquestionAsWdg::asNoteTxt =QObject::tr("as note in the score");
QString TquestionAsWdg::asNameTxt =QObject::tr("as note's name");
QString TquestionAsWdg::asFretPosTxt =QObject::tr("as position on the fingerboard");
QString TquestionAsWdg::asSoundTxt =QObject::tr("as played sound");
QString TquestionAsWdg::questionTxt =QObject::tr("question");
QString TquestionAsWdg::questionsTxt =QObject::tr("questions");
QString TquestionAsWdg::answerTxt =QObject::tr("answer");
QString TquestionAsWdg::answersTxt =QObject::tr("answers");


TquestionAsWdg::TquestionAsWdg(QWidget *parent) :
    QGroupBox(parent)
{
    setStatusTip(tr("select answers for question as mentioned."));
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *answersLab = new QLabel(answersTxt+":",this);
    mainLay->addWidget(answersLab,0,Qt::AlignCenter);
    asNoteChB = new QCheckBox(asNoteTxt,this);
    mainLay->addWidget(asNoteChB);
    asNameChB = new QCheckBox(asNameTxt,this);
    mainLay->addWidget(asNameChB);
    asFretPosChB = new QCheckBox(asFretPosTxt,this);
    mainLay->addWidget(asFretPosChB);
    asSoundChB = new QCheckBox(asSoundTxt,this);
    asSoundChB->setDisabled(true);
    asSoundChB->setStatusTip(tr("not implemented yet"));
    mainLay->addWidget(asSoundChB);
    mainLay->addStretch(1);
    setLayout(mainLay);

    setCheckable(true);

    connect(asNoteChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asNameChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asFretPosChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asSoundChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    connect(this, SIGNAL(clicked()), this, SLOT(groupChecked()));

}

void TquestionAsWdg::buttonClicked() {
    if (!asNoteChB->isChecked() && !asNameChB->isChecked() && !asFretPosChB->isChecked()
        && !asSoundChB->isChecked())
        setChecked(false);
    emit answerStateChanged();
}



void TquestionAsWdg::groupChecked() {
    if (isChecked()) {
        asNoteChB->setChecked(true);
        asNameChB->setChecked(true);
        asFretPosChB->setChecked(true);
//        asSoundChB->setChecked(true);
    } else {
        asNoteChB->setChecked(false);
        asNameChB->setChecked(false);
        asFretPosChB->setChecked(false);
//        asSoundChB->setChecked(false);
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
