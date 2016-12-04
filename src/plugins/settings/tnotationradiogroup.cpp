/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "tnotationradiogroup.h"
#include <music/tnamestylefilter.h>
#include <QtWidgets/QtWidgets>


QString TnotationRadioGroup::strNorskExampl = QStringLiteral("(C, C#, Db ... Hb, H)");
QString TnotationRadioGroup::strItalExampl = QStringLiteral("(Do, Do#, Reb ... Sib, Si)");
QString TnotationRadioGroup::strDeutschExampl = QStringLiteral("(C, Cis, Des ... B, H)");
QString TnotationRadioGroup::strEnglishExampl = QStringLiteral("(C, C#, Db ... Bb, B)");
QString TnotationRadioGroup::strNederExampl = QStringLiteral("(C, Cis, Des ... Bes, B)");
QString TnotationRadioGroup::strRusExampl = QString::fromUtf8("(До, До# Реb ... Сиb, Си)");


TnotationRadioGroup::TnotationRadioGroup(Tnote::EnameStyle _notation, bool lettersAndSolfege, QWidget* parent) :
  QGroupBox(parent),
  m_solfegeButtGroup(0)
{
  notation = _notation;
  setTitle(tr("Naming style"));
  setStatusTip(tr("Naming style of note. The main difference is the 7th note.<br>Is it B and B flat, or H and B?"));
  const QString space = QStringLiteral(" ");
  m_norskButt = new QRadioButton(strNorsk() + space + strNorskExampl ,this);
  m_italianoButt = new QRadioButton(strItal() + space + strItalExampl, this);
  m_deutschButt = new QRadioButton(strDeutsch() + space + strDeutschExampl ,this);
  m_englishButt = new QRadioButton(strEnglish() + space + strEnglishExampl, this);
  m_nederlButt = new QRadioButton(strNeder() + space + strNederExampl, this);
  m_rusButt = new QRadioButton(strRus() + space + strRusExampl, this);
  if (lettersAndSolfege) {
      m_letterRadio = new QRadioButton(tr("letter names"), this);
      m_solfegeRadio = new QRadioButton(tr("solfege names"), this);
  }
// Layout
  QVBoxLayout *lettetLay = new QVBoxLayout(this);
    lettetLay->addWidget(m_norskButt);
    lettetLay->addWidget(m_deutschButt);
    lettetLay->addWidget(m_englishButt);
    lettetLay->addWidget(m_nederlButt);
  if (!lettersAndSolfege) {
    lettetLay->addWidget(m_italianoButt);
    lettetLay->addWidget(m_rusButt);
  }
    lettetLay->addStretch(1);
  QGroupBox *solfegeGrPtr = 0;
  if (lettersAndSolfege) {
    QVBoxLayout *solfegeLay = new QVBoxLayout;
      solfegeLay->addWidget(m_italianoButt);
      solfegeLay->addWidget(m_rusButt);
      solfegeLay->addStretch(1);
    QGroupBox *solfegeGr = new QGroupBox(this);
    solfegeGr->setLayout(solfegeLay);
    solfegeGrPtr = solfegeGr;
  }
  auto letterGr = new QGroupBox(this);

  letterGr->setLayout(lettetLay);

  auto leftLay = new QVBoxLayout;
    if (lettersAndSolfege)
        leftLay->addWidget(m_letterRadio);
    leftLay->addWidget(letterGr);
  QVBoxLayout *rightLay = new QVBoxLayout;
  if (lettersAndSolfege) {
      rightLay->addWidget(m_solfegeRadio);
      rightLay->addWidget(solfegeGrPtr);
  }
#if defined (Q_OS_ANDROID)
  QVBoxLayout *lay = new QVBoxLayout;
  lay->setContentsMargins(1, 1, 1, 1);
#else
  QHBoxLayout *lay = new QHBoxLayout;
#endif
    lay->addLayout(leftLay);
    lay->addLayout(rightLay);
  
  setLayout(lay);

  seventhNoteWasChanged(*(TnameStyleFilter::is7th_B()));

  m_letterButtGroup = new QButtonGroup(this);
    m_letterButtGroup->addButton(m_norskButt);
    m_letterButtGroup->addButton(m_deutschButt);
    m_letterButtGroup->addButton(m_englishButt);
    m_letterButtGroup->addButton(m_nederlButt);
  if (lettersAndSolfege) {
    m_solfegeButtGroup = new QButtonGroup(this);
      m_solfegeButtGroup->addButton(m_italianoButt);
      m_solfegeButtGroup->addButton(m_rusButt);
  } else {
    m_letterButtGroup->addButton(m_italianoButt);
    m_letterButtGroup->addButton(m_rusButt);
  }

  connect (m_letterButtGroup, SIGNAL(buttonClicked(int)) ,this, SLOT(noteNameStyleWasClicked()));
  if (lettersAndSolfege) {
    connect(m_deutschButt, SIGNAL(clicked(bool)), this, SLOT(styleButtonClicked()));
    connect(m_englishButt, SIGNAL(clicked(bool)), this, SLOT(styleButtonClicked()));
    connect(m_italianoButt, SIGNAL(clicked(bool)), this, SLOT(styleButtonClicked()));
    connect(m_nederlButt, SIGNAL(clicked(bool)), this, SLOT(styleButtonClicked()));
    connect(m_norskButt, SIGNAL(clicked(bool)), this, SLOT(styleButtonClicked()));
    connect(m_rusButt, SIGNAL(clicked(bool)), this, SLOT(styleButtonClicked()));
    connect (m_solfegeButtGroup, SIGNAL(buttonClicked(int)) ,this, SLOT(noteNameStyleWasClicked()));
    QButtonGroup *letterSolfegeButtGr = new QButtonGroup(this);
      letterSolfegeButtGr->addButton(m_solfegeRadio);
      letterSolfegeButtGr->addButton(m_letterRadio);
    connect (letterSolfegeButtGr, SIGNAL(buttonClicked(int)) ,this, SLOT(noteNameStyleWasClicked()));
    if (notation == Tnote::e_italiano_Si || notation == Tnote::e_russian_Ci) {
      // select some style for letters when solfege is main settings
      if (*(TnameStyleFilter::is7th_B()))
          m_englishButt->setChecked(true);
      else
          m_norskButt->setChecked(true);
    } else
      setNameStyle(*(TnameStyleFilter::solfegeStyle())); // select solfege style column
  }

  setNameStyle(notation);
}


void TnotationRadioGroup::setNameStyle(Tnote::EnameStyle style) {
  switch (style)  {
    case Tnote::e_norsk_Hb : 
        m_norskButt->setChecked(true); break;
    case Tnote::e_deutsch_His : 
        m_deutschButt->setChecked(true); break;
    case Tnote::e_italiano_Si : 
        m_italianoButt->setChecked(true); break;
    case Tnote::e_english_Bb : 
        m_englishButt->setChecked(true); break;
    case Tnote::e_nederl_Bis : 
        m_nederlButt->setChecked(true); break;
    case Tnote::e_russian_Ci:
        m_rusButt->setChecked(true); break;
  }
  if (m_solfegeButtGroup) {
    if (style == Tnote::e_italiano_Si || style == Tnote::Tnote::e_russian_Ci)
      m_solfegeRadio->setChecked(true);
    else
      m_letterRadio->setChecked(true);
  }
}


Tnote::EnameStyle TnotationRadioGroup::getLetterStyle() {
    if (m_norskButt->isChecked())
        return Tnote::e_norsk_Hb;
    else if (m_deutschButt->isChecked()) 
        return Tnote::e_deutsch_His;
    else if (m_englishButt->isChecked()) 
        return Tnote::e_english_Bb;
    else
        return Tnote::e_nederl_Bis;
}


Tnote::EnameStyle TnotationRadioGroup::getSolfegeStyle() {
  if (m_italianoButt->isChecked()) 
    return Tnote::e_italiano_Si;
  else
    return Tnote::e_russian_Ci;
}



Tnote::EnameStyle TnotationRadioGroup::getNameStyle() {
  if (m_solfegeButtGroup) {
    if (m_letterRadio->isChecked())
        return getLetterStyle();
    else 
        return getSolfegeStyle();
  } else {
    if (m_norskButt->isChecked())
        return Tnote::e_norsk_Hb;
    else if (m_deutschButt->isChecked()) 
        return Tnote::e_deutsch_His;
    else if (m_italianoButt->isChecked()) 
        return Tnote::e_italiano_Si;
    else if (m_englishButt->isChecked()) 
        return Tnote::e_english_Bb;
    else if (m_rusButt->isChecked())
        return Tnote::e_russian_Ci;
    else
        return Tnote::e_nederl_Bis;
  }
}


void TnotationRadioGroup::noteNameStyleWasClicked() {
    emit noteNameStyleWasChanged(getNameStyle());    
}


void TnotationRadioGroup::seventhNoteWasChanged(bool isB) {
  if (isB) {
      m_norskButt->hide();
      m_deutschButt->hide();
      m_englishButt->show();
      m_nederlButt->show();
      if (m_deutschButt->isChecked()) 
          m_nederlButt->setChecked(true);
      if (m_norskButt->isChecked()) 
          m_englishButt->setChecked(true);
  } else {
      m_norskButt->show();
      m_deutschButt->show();
      m_englishButt->hide();
      m_nederlButt->hide();
      if (m_nederlButt->isChecked()) 
          m_deutschButt->setChecked(true);
      if (m_englishButt->isChecked()) 
          m_norskButt->setChecked(true);
  }
}


void TnotationRadioGroup::styleButtonClicked() {
  if (m_solfegeButtGroup) { // select letter or solfege radio button appropriate to selected name style
    if (sender() == m_italianoButt || sender() == m_rusButt)
      m_solfegeRadio->setChecked(true);
    else
      m_letterRadio->setChecked(true);
  }
}



