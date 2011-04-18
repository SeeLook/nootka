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
QString TquestionAsWdg::asNoteTxt = tr("as note in the score");
QString TquestionAsWdg::asNameTxt = tr("as note's name");
QString TquestionAsWdg::asFretPosTxt = tr("as position on the fingerboard");
QString TquestionAsWdg::asSoundTxt = tr("as played sound");
QString TquestionAsWdg::questionTxt = tr("question");
QString TquestionAsWdg::questionsTxt = tr("questions");
QString TquestionAsWdg::answerTxt = tr("answer");
QString TquestionAsWdg::answersTxt = tr("answers");


TquestionAsWdg::TquestionAsWdg(QWidget *parent) :
    QGroupBox(parent)
{
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

}

void TquestionAsWdg::buttonClicked() {
    emit answerStateChenged();
}
