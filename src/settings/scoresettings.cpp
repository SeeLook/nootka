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


#include "scoresettings.h"
#include "tglobals.h"
#include "tkeysignature.h"

extern Tglobals *gl;

/*static*/
const QString ScoreSettings::forExample = tr("for example");
const QString ScoreSettings::showKeySigName = tr("show names of key signature");

ScoreSettings::ScoreSettings(QWidget *parent) :
    QWidget(parent)
{

    m_workStyle = gl->SnameStyleInKeySign;
    QVBoxLayout *mainLay = new QVBoxLayout();
    enablKeySignCh = new QCheckBox(tr("enable key signature"),this);
    enablKeySignCh->setChecked(gl->SkeySignatureEnabled);
    mainLay->addWidget(enablKeySignCh);
    QHBoxLayout *nameLay = new QHBoxLayout();
    enablKeyNameGr = new QGroupBox(showKeySigName,this);
    enablKeyNameGr->setCheckable(true);
    enablKeyNameGr->setChecked(gl->SshowKeySignName);
    enablKeyNameGr->setDisabled(!gl->SkeySignatureEnabled);

    nameStyleGr = new TnotationRadioGroup(gl->SnameStyleInKeySign,this);
    nameLay->addWidget(nameStyleGr);

    nameExtGr = new QGroupBox(tr("Nameing extension"));

    QVBoxLayout *majLay = new QVBoxLayout();
    majExtLab = new QLabel(tr("in the major keys:"),this);
    majLay->addWidget(majExtLab,0,Qt::AlignCenter);
    majEdit = new QLineEdit(gl->SmajKeyNameSufix,this);
    majEdit->setMaxLength(10);
    majEdit->setAlignment(Qt::AlignCenter);
    majLay->addWidget(majEdit,0,Qt::AlignCenter);
    majExampl = new QLabel(this);
    majExampl->setAlignment(Qt::AlignCenter);
    majLay->addWidget(majExampl,0,Qt::AlignCenter);
    majLay->addStretch(1);

    QVBoxLayout *minLay = new QVBoxLayout();
    minExtLab = new QLabel(tr("in the minor keys:"));
    minLay->addWidget(minExtLab,0,Qt::AlignCenter);
    minEdit = new QLineEdit(gl->SminKeyNameSufix,this);
    minEdit->setMaxLength(10);
    minEdit->setAlignment(Qt::AlignCenter);
    minLay->addWidget(minEdit,0,Qt::AlignCenter);
    minExampl = new QLabel(this);
    minExampl->setAlignment(Qt::AlignCenter);
    minLay->addWidget(minExampl,0,Qt::AlignCenter);
    minLay->addStretch(1);

    QHBoxLayout *nameExtLay = new QHBoxLayout();
    nameExtLay->addLayout(majLay);
    nameExtLay->addLayout(minLay);
    nameExtGr->setLayout(nameExtLay);

    nameLay->addWidget(nameExtGr);

    enablKeyNameGr->setLayout(nameLay);
    mainLay->addWidget(enablKeyNameGr);

    setLayout(mainLay);

    connect(enablKeySignCh, SIGNAL(toggled(bool)), this, SLOT(enableKeySignGroup(bool)));
    connect(nameStyleGr, SIGNAL(noteNameStyleWasChanged(Tnote::EnameStyle)), this, SLOT(nameStyleWasChanged(Tnote::EnameStyle)));
    connect(majEdit ,SIGNAL(textChanged(QString)), this, SLOT(majorExtensionChanged()));
    connect(minEdit ,SIGNAL(textChanged(QString)), this, SLOT(minorExtensionChanged()));
    majExampl->setText(getMajorExample(m_workStyle));
    minExampl->setText(getMinorExample(m_workStyle));
}

void ScoreSettings::enableKeySignGroup(bool enable) {
    enablKeyNameGr->setDisabled(!enable);
}

void ScoreSettings::majorExtensionChanged() {
    majExampl->setText(getMajorExample(m_workStyle));
}

void ScoreSettings::minorExtensionChanged() {
    minExampl->setText(getMinorExample(m_workStyle));
}

QString ScoreSettings::getMajorExample(Tnote::EnameStyle nameStyle) {
    Tnote noteE = Tnote(3,0,0);
    Tnote noteBflat = Tnote(7,0,-1);
    QString S;
    if (majEdit->text().isEmpty()) S = "";
      else S = "-"+majEdit->text();
      return forExample + "<br><b>" + QString::fromStdString(noteE.getName(nameStyle,false)) + S +
              "<br>" + QString::fromStdString(noteBflat.getName(nameStyle,false)) + S + "</b>";
}

QString ScoreSettings::getMinorExample(Tnote::EnameStyle nameStyle) {
    Tnote noteCsharp = Tnote(1,0,1);
    Tnote noteG = Tnote(5,0,0);
    QString S;
    if (minEdit->text().isEmpty()) S = "";
      else S = "-"+minEdit->text();
      return forExample + "<br><b>" +
              QString::fromStdString(noteCsharp.getName(nameStyle,false)).toLower()+ S + "<br>" +
              QString::fromStdString(noteG.getName(nameStyle,false)).toLower() + S + "</b>";
}


void ScoreSettings::nameStyleWasChanged(Tnote::EnameStyle nameStyle) {
    m_workStyle = nameStyle;
    majExampl->setText(getMajorExample(m_workStyle));
    minExampl->setText(getMinorExample(m_workStyle));
}

/** @todo make order with keys*/
void ScoreSettings::saveSettings() {
    gl->SkeySignatureEnabled = enablKeySignCh->isChecked();
    if (gl->SkeySignatureEnabled) { //changed only if key signature is enabled
        gl->SmajKeyNameSufix = majEdit->text();
        gl->SminKeyNameSufix = minEdit->text();
        gl->SnameStyleInKeySign = nameStyleGr->getNameStyle();
        gl->SshowKeySignName = enablKeyNameGr->isChecked();
        TkeySignature::setNameStyle(
                gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);
    }
}

void ScoreSettings::seventhIsBChanged(bool isB) {
    nameStyleGr->seventhNoteWasChanged(isB);
}


