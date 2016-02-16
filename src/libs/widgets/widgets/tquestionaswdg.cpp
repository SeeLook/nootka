/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <tnoofont.h>
#include <QtWidgets>

/* static */
QString TquestionAsWdg::qaTypeText(TQAtype::Etype type) {
    QString txt;
    switch (type) {
      case TQAtype::e_asNote : txt = TquestionAsWdg::asNoteTxt(); break;
      case TQAtype::e_asName : txt = TquestionAsWdg::asNameTxt(); break;
      case TQAtype::e_asFretPos : txt = TquestionAsWdg::asFretPosTxt(); break;
      case TQAtype::e_asSound : txt = TquestionAsWdg::asSoundTxt(); break;    
    }
    return txt;
}


QString TquestionAsWdg::qaTypeSymbol(TQAtype::Etype type) {
    QString symbol;
    switch (type) {
      case TQAtype::e_asNote : symbol = "s"; break;
      case TQAtype::e_asName : symbol = "c"; break;
      case TQAtype::e_asFretPos : symbol = "g"; break;
      case TQAtype::e_asSound : symbol = "n"; break;    
    }
    return symbol;
}

TquestionAsWdg::TquestionAsWdg(TQAtype::Etype type, QGridLayout* gridLay, int row, QWidget* parent) :
    QObject(parent)
{
    QString title = qaTypeText(type);    
    questLab = new QLabel(title, parent);
    questLab->setStatusTip(tr("Select the type of answers for a question."));
    gridLay->addWidget(questLab, row, 0, Qt::AlignRight);
    enableChBox = new QCheckBox(parent);
    enableChBox->setStatusTip(questionTxt() + " - " + title);
    gridLay->addWidget(enableChBox, row, 1, Qt::AlignCenter);
    int nootFontSize = enableChBox->fontMetrics().boundingRect("A").height() * 2;
    
    asNoteChB = new QCheckBox(parent);
    asNoteChB->setStatusTip("<table valign=\"middle\" align=\"center\"><tr><td>" + 
    TnooFont::span(qaTypeSymbol(type) + "?", nootFontSize) + "</td>" + 
    "<td align=\"center\">" + questionTxt() + " - " + qaTypeText(type)  + "<br> " + 
    answerTxt() + " - " + qaTypeText(TQAtype::e_asNote) + " </td> " +
    "<td>" + TnooFont::span(qaTypeSymbol(TQAtype::e_asNote) + "!", nootFontSize) + "</td></tr></table>");
    gridLay->addWidget(asNoteChB, row, 2, Qt::AlignCenter);
    
    asNameChB = new QCheckBox(parent);
    asNameChB->setStatusTip("<table valign=\"middle\" align=\"center\"><tr><td>" + 
    TnooFont::span(qaTypeSymbol(type) + "?", nootFontSize) + "</td>" + 
    "<td align=\"center\">" + questionTxt() + " - " + qaTypeText(type)  + "<br> " + 
    answerTxt() + " - " + qaTypeText(TQAtype::e_asName) + " </td> " +
    "<td>" + TnooFont::span(qaTypeSymbol(TQAtype::e_asName) + "!", nootFontSize) + "</td></tr></table>");
    gridLay->addWidget(asNameChB, row, 3, Qt::AlignCenter);
    
    asFretPosChB = new QCheckBox(parent);
    asFretPosChB->setStatusTip("<table valign=\"middle\" align=\"center\"><tr><td>" + 
    TnooFont::span(qaTypeSymbol(type) + "?", nootFontSize) + "</td>" + 
    "<td align=\"center\">" + questionTxt() + " - " + qaTypeText(type)  + "<br> " + 
    answerTxt() + " - " + qaTypeText(TQAtype::e_asFretPos) + " </td> " +
    "<td>" + TnooFont::span(qaTypeSymbol(TQAtype::e_asFretPos) + "!", nootFontSize) + "</td></tr></table>");
    gridLay->addWidget(asFretPosChB, row, 4, Qt::AlignCenter);
    
    asSoundChB = new QCheckBox(parent);
    asSoundChB->setStatusTip("<table valign=\"middle\" align=\"center\"><tr><td>" + 
    TnooFont::span(qaTypeSymbol(type) + "?", nootFontSize) + "</td>" + 
    "<td align=\"center\">" + questionTxt() + " - " + qaTypeText(type)  + "<br> " + 
    answerTxt() + " - " + qaTypeText(TQAtype::e_asSound) + " </td> " +
    "<td>" + TnooFont::span(qaTypeSymbol(TQAtype::e_asSound) + "!", nootFontSize) + "</td></tr></table>");
    gridLay->addWidget(asSoundChB, row, 5, Qt::AlignCenter);

    connect(asNoteChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asNameChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asFretPosChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(asSoundChB, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    connect(enableChBox, SIGNAL(clicked()), this, SLOT(groupChecked()));
    connect(enableChBox, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}


void TquestionAsWdg::buttonClicked() {
    if (!asNoteChB->isChecked() && !asNameChB->isChecked() && !asFretPosChB->isChecked() && !asSoundChB->isChecked())
				setChecked(false);
    else
				setChecked(true);
    emit answerStateChanged();
}


void TquestionAsWdg::groupChecked() {
    if (isChecked()) { 
        asNoteChB->setChecked(true);
        asNameChB->setChecked(true);
				// For this moment only guitar check boxes can be invisible, no need to unchecking others
				if (asFretPosChB->isVisible())
						asFretPosChB->setChecked(true);
				else
						asFretPosChB->setChecked(false);
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


void TquestionAsWdg::setChecked(bool checked) {
  enableChBox->setChecked(checked);
  if (!checked) {
    asNoteChB->setChecked(false);
    asNameChB->setChecked(false);
    asFretPosChB->setChecked(false);
    asSoundChB->setChecked(false);
  }

}


