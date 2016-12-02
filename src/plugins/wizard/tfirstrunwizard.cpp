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


#include "tfirstrunwizard.h"
#include <tabout.h>
#include <tscalepreviewlabel.h>
#include <select7note.h>
#include <help/tmainhelp.h>
#include <tmisctrans.h>
#include <tinitcorelib.h>
#include <graphics/tnotepixmap.h>
#include <widgets/troundedlabel.h>
#include <widgets/tselectinstrument.h>
#include <graphics/tnotepixmap.h>
#include <music/tkeysignature.h>
#include <music/ttune.h>
#include <tsimplescore.h>
#include <taudioparams.h>
#include <tscoreparams.h>
#include <tlayoutparams.h>
#include <tpath.h>
#include <qtr.h>
#include <QtWidgets/QtWidgets>


#if defined (Q_OS_ANDROID)
  const QString headSpan = QLatin1String("<span style=\"font-size: large;\"><b>");
#else
  const QString headSpan = QLatin1String("<span style=\"font-size: x-large;\"><b>");
#endif
const QString headClose = QLatin1String("</b></span>");

TfirstRunWizard::TfirstRunWizard(QWidget *parent) :
  QWizard(parent)
{
#if defined (Q_OS_ANDROID) // there is no other way than fixed size of available screen
  setFixedSize(qApp->desktop()->availableGeometry().size());
#else
  setWindowIcon(QIcon(Tpath::img("nootka")));
  setWindowTitle(QLatin1String("Nootka   ") + tr("First run wizard"));
  QPixmap bgPix(Tpath::img("wizard-left"));
  setPixmap(WatermarkPixmap, bgPix);
#endif
  setWizardStyle(ClassicStyle);
  setOption(IgnoreSubTitles);

// 1. About Nootka page
  auto aboutPage = new QWizardPage(this);
  auto aboutNootka = new Tabout();
  auto aboutLay = new QVBoxLayout;
    aboutLay->addWidget(aboutNootka);
#if defined (Q_OS_ANDROID)
    aboutLay->setContentsMargins(0, 0, 0, 0);
#endif
  aboutPage->setLayout(aboutLay);

// 2. Select instrument page
  auto instrumentPage = new QWizardPage(this);
  auto whatLabel = new QLabel(headSpan + tr("What instrument do you play?") + headClose, instrumentPage);
  m_selectInstr = new TselectInstrument(instrumentPage);
  m_selectInstr->setInstrument(int(e_classicalGuitar));
  auto instrLay = new QVBoxLayout;
    instrLay->addStretch();
    instrLay->addWidget(whatLabel, 0, Qt::AlignCenter);
    instrLay->addStretch();
    instrLay->addWidget(m_selectInstr, 0, Qt::AlignCenter);
    instrLay->addStretch();
#if defined (Q_OS_ANDROID)
    instrLay->setContentsMargins(0, 0, 0, 0);
#endif
  instrumentPage->setLayout(instrLay);

// 3. Select clef and instrument note range
  m_notationWidget = new Tpage_2;
  whenInstrumentChanged(1);

// 4. Some options page 
  m_page3 = new Tpage_3(this);

// 5. Last page with some help
  auto helpPage = new QWizardPage(this);
  m_page4 = new TmainHelp(helpPage);
  auto helpLay = new QVBoxLayout;
    helpLay->addWidget(m_page4);
#if defined (Q_OS_ANDROID)
    helpLay->setContentsMargins(0, 0, 0, 0);
#endif
  helpPage->setLayout(helpLay);

  addPage(aboutPage);
  addPage(instrumentPage);
  addPage(m_notationWidget);
  addPage(m_page3);
  addPage(helpPage);

  // grab 7-th note from translation
  Tcore::gl()->S->seventhIs_B = m_page3->select7->is7th_B();
  Tcore::gl()->S->nameStyleInNoteName = m_page3->nameStyle();
  if (TmiscTrans::note7txt().toLower() == QLatin1String("b")) {
      if (TmiscTrans::keyNameStyle() == QLatin1String("solfege"))
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_nederl_Bis;
  } else {
      Tcore::gl()->S->nameStyleInNoteName = Tnote::e_norsk_Hb;
      if (TmiscTrans::keyNameStyle() == QLatin1String("solfege"))
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_deutsch_His;
  }

  connect(this, &TfirstRunWizard::currentIdChanged, this, &TfirstRunWizard::pageChanged);
  connect(m_selectInstr, &TselectInstrument::instrumentChanged, this, &TfirstRunWizard::whenInstrumentChanged);
}


void TfirstRunWizard::pageChanged(int pageNr) {
  if (pageNr == 1) { // Take out keyboard focus
    if (qApp->focusWidget()) // instrument page highlights 2nd button (classical) by default, but keyboard focus goes to first one
        qApp->focusWidget()->clearFocus(); // it may be confusing if some style have the same colors for focus and highlight.
  }
}


void TfirstRunWizard::done(int result) {
  if (result == QDialog::Rejected) { // restore defaults
    Tcore::gl()->instrument = e_classicalGuitar;
    m_page3->select7->set7th_B(TmiscTrans::note7txt().toLower() == QLatin1String("b"));
    if (m_page3->localization.name().contains(QLatin1String("ru")))
      m_page3->solfegeRadio->setChecked(true);
    m_page3->dblAccChB->setChecked(false);
    m_page3->enharmChB->setChecked(false);
    m_page3->useKeyChB->setChecked(false);
  }
  Tcore::gl()->S->seventhIs_B = m_page3->select7->is7th_B();
  Tcore::gl()->S->nameStyleInNoteName = m_page3->nameStyle();
//   Tnote::defaultStyle = Tcore::gl()->S->nameStyleInNoteName;
  Tcore::gl()->S->solfegeStyle = m_page3->localization.name().contains(QLatin1String("ru")) ? Tnote::e_russian_Ci : Tnote::e_italiano_Si;
  if (m_page3->select7->is7th_B()) {
      if (TmiscTrans::keyNameStyle() == QLatin1String("solfege"))
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_nederl_Bis;
  } else {
      if (TmiscTrans::keyNameStyle() == QLatin1String("solfege"))
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_italiano_Si;
      else
        Tcore::gl()->S->nameStyleInKeySign = Tnote::e_deutsch_His;
  }
  Tcore::gl()->S->doubleAccidentalsEnabled = m_page3->dblAccChB->isChecked();
  Tcore::gl()->S->showEnharmNotes = m_page3->enharmChB->isChecked();
  Tcore::gl()->S->keySignatureEnabled = m_page3->useKeyChB->isChecked();
  if (Tcore::gl()->instrument == e_bassGuitar) {
      Tcore::gl()->setTune(Ttune::bassTunes[0]);
      Tcore::gl()->S->clef = Tclef::e_bass_F_8down;
      Tcore::gl()->A->audioInstrNr = (int)e_bassGuitar;
      Tcore::gl()->GfretsNumber = 20;
      Tcore::gl()->A->minSplitVol = 7.0;
  } else if (Tcore::gl()->instrument == e_electricGuitar) {
      Tcore::gl()->A->audioInstrNr = (int)e_electricGuitar;
      Tcore::gl()->GfretsNumber = 23;
      Tcore::gl()->A->minSplitVol = 7.0;
  } else if (Tcore::gl()->instrument == e_noInstrument) {
      Tcore::gl()->L->guitarEnabled = false;
      Tcore::gl()->S->clef = m_notationWidget->score()->clef().type();
      Tnote hiN, loN; // fix notes order
      if (m_notationWidget->score()->getNote(1).chromatic() < m_notationWidget->score()->getNote(0).chromatic()) {
          hiN = m_notationWidget->score()->getNote(0);
          loN = m_notationWidget->score()->getNote(1);
      } else {
          hiN = m_notationWidget->score()->getNote(1);
          loN = m_notationWidget->score()->getNote(0);
      }
      Ttune instrScale(QLatin1String("scale"), Tnote(hiN.chromatic() - Tcore::gl()->GfretsNumber), loN);
      Tcore::gl()->setTune(instrScale);
      Tcore::gl()->A->detectMethod = 0; // MPM for other instruments
      Tcore::gl()->A->minSplitVol = 0.0;
      Tcore::gl()->A->skipStillerVal = 0.0;
  }
  QWizard::done(result);
}


// To write notes of bass guitar this application uses <b>bass dropped clef</b> (bass clef with \"eight\" digit below) but common practice is to skip this digit and write it in ordinary bass clef. Remember, bass guitar sounds octave lower than notes written in 'normal' bass clef.
void TfirstRunWizard::whenInstrumentChanged(int instr) {
    Tcore::gl()->instrument = Einstrument(instr);
    m_notationWidget->setNoteForInstrument(instr);
    if ((Einstrument)instr == e_bassGuitar)
                m_notationWidget->notationNote()->setHtml(QString("<center>%1<br>").
                arg(wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F, TkeySignature(0), 5.0)) +
                tr("When writing notation for bass guitar, the <b>bass clef</b> is used but the played notes sound an octave lower. The proper clef is <b>bass dropped clef</b> (with the digit \"eight\" written below) In this clef, the notes sound exactly as written. This clef is used in Nootka for bass guitar.") +
                    QLatin1String("<br><br>") + wrapPixToHtml(Tnote(0, 0, 0), Tclef::e_bass_F_8down, TkeySignature(0), 8.0));
    else if ((Einstrument)instr == e_classicalGuitar || (Einstrument)instr == e_electricGuitar)
                m_notationWidget->notationNote()->setHtml(QLatin1String("<center>") + tr("Guitar notation uses the treble clef with the digit \"eight\" written below (even if some editors are forgetting about this digit).<br><br>Try to understand this. <br><br><p> %1 %2<br><b><big>Both pictures above show the same note: c<sup>1</sup></big></b><br>(note c in one-line octave)</p>").
                arg(wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G, TkeySignature(0), 6.0)).
                arg(wrapPixToHtml(Tnote(1, 1, 0), Tclef::e_treble_G_8down, TkeySignature(0), 6.0)) + QLatin1String("</center>"));
}

//###############################################  Tpage_2   ###############################################

Tpage_2::Tpage_2(QWidget* parent) :
    QWizardPage(parent),
    m_notationNote(0),
    m_score(0)
{
    m_lay = new QVBoxLayout;
#if defined (Q_OS_ANDROID)
  m_lay->setContentsMargins(0, 0, 0, 0);
#endif
    setLayout(m_lay);
}


void Tpage_2::setNoteForInstrument(int instr) {
  if ((Einstrument)instr == e_noInstrument) {
    if (!m_score) {
      if (m_notationNote) {
        delete m_notationNote;
        m_notationNote = 0;
      }
      m_lay->addStretch();
      m_headLabel = new QLabel(headSpan + tr("Select a clef and scale of notes appropriate for your instrument.") + headClose, this);
      m_headLabel->setWordWrap(true);
      m_headLabel->setAlignment(Qt::AlignCenter);
      m_lay->addWidget(m_headLabel);
      m_score = new TsimpleScore(2, this);
      m_lay->addStretch();
      m_lay->addWidget(m_score, 0, Qt::AlignCenter);
      m_score->addBGglyph((int)e_noInstrument);
      m_score->setClef(Tclef(Tclef::e_treble_G));
      m_score->setControllersEnabled(false, false); // hide both note controllers
      m_score->setMinimumSize(contentsRect().size() / 1.5);
      m_score->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
      clefChanged();
      m_lay->addStretch();
      m_scoreHint = new TroundedLabel(this);
      m_lay->addWidget(m_scoreHint);
#if defined (Q_OS_ANDROID)
      m_scoreHint->setAlignment(Qt::AlignCenter);
      m_scoreHint->setText(qTR("TouchHelp", "Touch a clef for a while to change it."));
#else
      m_scoreHint->setFixedHeight(fontMetrics().boundingRect(QLatin1String("A")).height() * 4);
      m_scoreHint->setWordWrap(true);
      m_scoreHint->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      scoreHint(QString());
      connect(m_score, SIGNAL(statusTip(QString)), this, SLOT(scoreHint(QString)));
#endif
      connect(m_score, SIGNAL(clefChanged(Tclef)), this, SLOT(clefChanged()));
    }
  } else {
    if (!m_notationNote) {
      if (m_score) {
        delete m_score;
        m_score = 0;
        delete m_headLabel;
        delete m_scoreHint;
        QLayoutItem *child;
        while ((child = m_lay->takeAt(0)) != 0)
          delete child;
      }
      m_notationNote = new QTextEdit(this);
      m_lay->addWidget(m_notationNote);
      m_notationNote->setWordWrapMode(QTextOption::WordWrap);
      m_notationNote->setReadOnly(true);
#if defined (Q_OS_ANDROID)
      m_notationNote->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      m_notationNote->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
    }
  }
}


void Tpage_2::scoreHint(QString hint) {
#if !defined (Q_OS_ANDROID)
  m_scoreHint->setText(QLatin1String("<center>") + hint);
#endif
}


void Tpage_2::clefChanged() {
  m_score->setNote(0, m_score->lowestNote());
  m_score->setAmbitus(0, m_score->lowestNote(), m_score->highestNote());
  m_score->setNote(1, m_score->highestNote());
  m_score->setAmbitus(1, m_score->lowestNote(), m_score->highestNote());
}


//###############################################  Tpage_3   ###############################################

Tpage_3::Tpage_3(QWidget *parent) :
  QWizardPage(parent)
{
  auto headLab = new QLabel(headSpan +
                tr("7th note can be B or H, depends on country<br>What is the name of 7th note in your country?") +
                headClose, this);
  headLab->setWordWrap(true);
  headLab->setAlignment(Qt::AlignCenter);
  select7 = new Select7note(this);
  select7->set7th_B(TmiscTrans::note7txt().toLower() == QLatin1String("b"));
  m_nameStyle = select7->is7th_B()? Tnote::e_english_Bb : Tnote::e_norsk_Hb;
  scaleLab = new TscalePreviewLabel(m_nameStyle, false, this);
  connect(select7, &Select7note::seventhIsBchanged, this, &Tpage_3::seventhNoteChanged);

  letterRadio = new QRadioButton(qTR("TnotationRadioGroup", "letter names"), this);
  solfegeRadio = new QRadioButton(qTR("TnotationRadioGroup", "solfege names"), this);

  auto notationGr = new QButtonGroup(this);
  notationGr->addButton(solfegeRadio);
  notationGr->addButton(letterRadio);
  connect(notationGr, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), this, &Tpage_3::notationSlot);

  if (TmiscTrans::keyNameStyle() == QLatin1String("solfege"))
    solfegeRadio->setChecked(true);
  else
    letterRadio->setChecked(true);
  notationSlot();

  dblAccChB = new QCheckBox(tr("I know about double sharps (x) and double flats (bb)"), this);
  dblAccChB->setChecked(Tcore::gl()->S->doubleAccidentalsEnabled);

  enharmChB = new QCheckBox(tr("I know that e# is the same as f"), this);
  enharmChB->setChecked(Tcore::gl()->S->showEnharmNotes);

  useKeyChB = new QCheckBox(tr("I know about key signatures"), this);
  useKeyChB->setChecked(Tcore::gl()->S->keySignatureEnabled);

  QVBoxLayout *lay = new QVBoxLayout;
    lay->setAlignment(Qt::AlignCenter);
    lay->addStretch();
    lay->addWidget(headLab);
    lay->addStretch();
    lay->addWidget(select7);
    lay->addWidget(scaleLab, 0, Qt::AlignCenter);
    auto notationLay = new QHBoxLayout;
      notationLay->addStretch();
      notationLay->addWidget(letterRadio);
      notationLay->addStretch();
      notationLay->addWidget(solfegeRadio);
      notationLay->addStretch();
    lay->addLayout(notationLay);
    lay->addStretch();
    lay->addWidget(dblAccChB, 0, Qt::AlignCenter);
    lay->addWidget(enharmChB, 0, Qt::AlignCenter);
    lay->addWidget(useKeyChB, 0, Qt::AlignCenter);
    lay->addStretch();

  setLayout(lay);
}

void Tpage_3::seventhNoteChanged(bool is7_B) {
  m_nameStyle = is7_B ? Tnote::e_english_Bb : Tnote::e_norsk_Hb;
  scaleLab->changeStyle(m_nameStyle);
  letterRadio->setChecked(true);
}


void Tpage_3::notationSlot() {
  if (solfegeRadio->isChecked())
    m_nameStyle = localization.name().contains(QLatin1String("ru")) ? Tnote::e_russian_Ci : Tnote::e_italiano_Si;
  else
    m_nameStyle = select7->is7th_B() ? Tnote::e_english_Bb : Tnote::e_norsk_Hb;
  scaleLab->changeStyle(m_nameStyle);
}





