/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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

#include "settingsdialog.h"
#include "tglobals.h"
#include "tnoteview.h"
#include "examsettings.h"
#include "tcolorbutton.h"
#if defined (Q_OS_LINUX)
  #include "pulseprober.h"
#endif
#include <QtGui>
#include <audioinsettings.h>
#include <audiooutsettings.h>



extern Tglobals *gl;



//############# GuitarSettings IMPLEMENTATION ##################

GuitarSettings::GuitarSettings(QWidget *parent) :
        QWidget(parent)
{
    
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *upLay = new QHBoxLayout;
    QGroupBox *tuneGr = new QGroupBox(tr("tune of the guitar"));
    tuneGr->setStatusTip(tr("Select appropirate tune from the list or prepare your own."));
    QVBoxLayout *tuneLay = new QVBoxLayout;
    tuneLay->setAlignment(Qt::AlignCenter);
    tuneCombo = new QComboBox(this);
    tuneLay->addWidget(tuneCombo);
    tuneView = new TscoreWidgetSimple(6,this);
    tuneLay->addWidget(tuneView);
    tuneView->setFixedWidth(280);
    tuneView->setAmbitus(Tnote(6,-2,0),Tnote(6,1,0));
    setTune(gl->Gtune());
    tuneCombo->addItem(Ttune::stdTune.name);
    if (gl->Gtune() == Ttune::stdTune)
        tuneCombo->setCurrentIndex(0);
    for (int i=0; i<4; i++) {
        tuneCombo->addItem(Ttune::tunes[i].name);
        if (gl->Gtune() == Ttune::tunes[i])
            tuneCombo->setCurrentIndex(i+1);
    }
    QString S = tr("Custom tune");
    tuneCombo->addItem(S);
    if (gl->Gtune().name == S)
        tuneCombo->setCurrentIndex(5);
    tuneGr->setLayout(tuneLay);
    upLay->addWidget(tuneGr);

    QVBoxLayout *hfLay = new QVBoxLayout;
    QGroupBox *hfGr = new QGroupBox;
    hfLay->addStretch(1);
    righthandCh = new QCheckBox(tr("guitar for right-handed"),this);
    righthandCh->setChecked(gl->GisRightHanded);
    righthandCh->setStatusTip(tr("Uncheck this if you are lefthanded<br>and your gitar has changed strings' order"));
    hfLay->addWidget(righthandCh);
    hfLay->addStretch(1);
    QLabel *fretLab = new QLabel(tr("number of frets:"),this);
    hfLay->addWidget(fretLab);
    fretsNrSpin = new QSpinBox(this);
    fretsNrSpin->setValue(gl->GfretsNumber);
    fretsNrSpin->setMaximum(24);
    fretsNrSpin->setMinimum(15);
    hfLay->addWidget(fretsNrSpin);
    hfLay->addStretch(1);
    upLay->addSpacing(3);
    hfGr->setLayout(hfLay);
    upLay->addWidget(hfGr);

    mainLay->addLayout(upLay);

    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *prefLay = new QVBoxLayout;
    QGroupBox *prefBox = new QGroupBox(tr("prefered accidentals:"),this);
    prefBox->setStatusTip(tr("Choose which accidentals will be shown in the score."));
    prefSharpBut = new QRadioButton(tr("# - sharps"),this);
    prefFlatBut = new  QRadioButton(tr("b - flats"),this);
    QButtonGroup *prefGr = new QButtonGroup(this);
    prefGr->addButton(prefSharpBut);
    prefGr->addButton(prefFlatBut);
    prefLay->addWidget(prefSharpBut);
    prefLay->addWidget(prefFlatBut);
    prefBox->setLayout(prefLay);
    if (gl->GpreferFlats) prefFlatBut->setChecked(true);
    else
        prefSharpBut->setChecked(true);
    downLay->addWidget(prefBox);

    morePosCh = new QCheckBox(tr("show all possibilities of a note"),this);
    morePosCh->setStatusTip(tr("As you know, the same note can be played in few places on a fingerboard.<br>If checked, all of them are showed."));
    downLay->addWidget(morePosCh);
    morePosCh->setChecked(gl->GshowOtherPos);

    mainLay->addLayout(downLay);
    QGridLayout *colorLay = new QGridLayout;
    QLabel *pointLab = new QLabel(tr("color of string/fret pointer"), this);
    pointColorBut = new TcolorButton(gl->GfingerColor, this);
    colorLay->addWidget(pointLab, 0, 0);
    colorLay->addWidget(pointColorBut, 0 ,1);
    QLabel *selLab = new QLabel(tr("color of selected string/fret"), this);
    selColorBut = new TcolorButton(gl->GselectedColor, this);
    colorLay->addWidget(selLab, 1, 0);
    colorLay->addWidget(selColorBut, 1, 1);
    mainLay->addLayout(colorLay);

    setLayout(mainLay);

    connect(tuneCombo, SIGNAL(activated(int)), this, SLOT(tuneSelected(int)));
    connect(tuneView, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(userTune(int,Tnote)));
    
}

void GuitarSettings::setTune(Ttune tune) {
    for (int i=0; i<6; i++) {
        tuneView->setNote(i,tune[6-i]);
        tuneView->noteViews[i]->setString(6-i);
    }
}

void GuitarSettings::tuneSelected(int tuneId) {
    if (tuneId == 0)
        setTune(Ttune::stdTune);
    else
        if (tuneId != tuneCombo->count()-1) //the last is custom
        setTune(Ttune::tunes[tuneId-1]);
}

void GuitarSettings::userTune(int, Tnote) {
    tuneCombo->setCurrentIndex(tuneCombo->count()-1);
}

void GuitarSettings::saveSettings() {
    gl->GisRightHanded = righthandCh->isChecked();
    gl->GfretsNumber = fretsNrSpin->value();
    gl->setTune(Ttune(tuneCombo->currentText(), tuneView->getNote(5), tuneView->getNote(4),          tuneView->getNote(3), tuneView->getNote(2), tuneView->getNote(1), tuneView->getNote(0)));
    gl->GshowOtherPos = morePosCh->isChecked();
    if (prefFlatBut->isChecked()) gl->GpreferFlats = true;
    else gl->GpreferFlats = false;
    gl->GfingerColor = pointColorBut->getColor();
    gl->GfingerColor.setAlpha(200);
    gl->GselectedColor = selColorBut->getColor();
}

//############# GlobalSettings IMPLEMENTATION ##################

GlobalSettings::GlobalSettings(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout();
    lay->setAlignment(Qt::AlignCenter);
    otherEnharmChBox = new QCheckBox(tr("show other enharmonics variants of a note"),this);
    otherEnharmChBox->setStatusTip(tr("Shows enharmonical variants of a note.<br>F.e.: E note is also Fb (F flat) and Dx (D with double sharp)."));
    otherEnharmChBox->setChecked(gl->showEnharmNotes);
	lay->addWidget(otherEnharmChBox);
	QHBoxLayout *colorLay = new QHBoxLayout;
	QLabel *colorLab = new QLabel(tr("color of enharminic notes/names"), this);
	enharmColorBut = new TcolorButton(gl->enharmNotesColor, this);
	colorLay->addWidget(colorLab);
	colorLay->addWidget(enharmColorBut);
	lay->addLayout(colorLay);
	lay->addStretch(1);
    dblAccChBox = new QCheckBox(tr("use double accidentals"),this);
    dblAccChBox->setStatusTip(tr("If checked, you can use double sharps and double flats."));
    dblAccChBox->setChecked(gl->doubleAccidentalsEnabled);
    lay->addWidget(dblAccChBox);
    lay->addStretch(1);
    hintsEnabledChBox = new QCheckBox(tr("show hints"), this);
    hintsEnabledChBox->setChecked(gl->hintsEnabled);
    hintsEnabledChBox->setStatusTip(tr("Show descriptions of interface's elements."));
    lay->addWidget(hintsEnabledChBox);
    lay->addStretch(1);
	QHBoxLayout *langLay = new QHBoxLayout;
	langLay->addStretch(1);
	QLabel *langLab = new QLabel(tr("Application's language"), this);
	langLay->addWidget(langLab);
	langLay->addStretch(1);
	langCombo = new QComboBox(this);
	langLay->addWidget(langCombo);
	langLay->addStretch(1);
	langCombo->setStatusTip(tr("Select a language.<br><span style=\"color: red;\">It requires restart of application !!</span>"));
	langList[""] = tr("default");
	langList["cs"] = QString::fromUtf8("český");
	langList["en"] = "english";
  langList["fr"] = QString::fromUtf8("français");
	langList["pl"] = "polski";
	QMapIterator<QString, QString> i(langList);
	int id = 0;
	while (i.hasNext()) {
		i.next();
		langCombo->addItem(QIcon(gl->path + "picts/flags-" + i.key() + ".png"), i.value());
		if (i.key() == gl->lang)
			langCombo->setCurrentIndex(id);
		id++;
	}
	langCombo->insertSeparator(1);
	lay->addLayout(langLay);
	lay->addStretch(1);
    setLayout(lay);
}

void GlobalSettings::saveSettings() {
	gl->doubleAccidentalsEnabled = dblAccChBox->isChecked();
	gl->showEnharmNotes = otherEnharmChBox->isChecked();
	gl->hintsEnabled = hintsEnabledChBox->isChecked();
	gl->enharmNotesColor = enharmColorBut->getColor();
	QMapIterator<QString, QString> i(langList);
	while (i.hasNext()) {
		i.next();
		if (langCombo->currentText() == i.value()) {
			gl->lang = i.key();
			break;
		}
	}
}



//############# SettingsDialog IMPLEMENTATION ##################
SettingsDialog::SettingsDialog(QWidget *parent) :
        TsettingsDialogBase(parent)
{
    setWindowTitle("Nootka - "+tr("application's settings"));

    navList->setFixedWidth(110);
    navList->addItem(tr("Common"));
    navList->item(0)->setIcon(QIcon(gl->path+"picts/global.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Score"));
    navList->item(1)->setIcon(QIcon(gl->path+"picts/scoreSettings.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Names","name-calling"));
    navList->item(2)->setIcon(QIcon(gl->path+"picts/nameSettings.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Guitar"));
    navList->item(3)->setIcon(QIcon(gl->path+"picts/guitarSettings.png"));
    navList->item(3)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Exam"));
    navList->item(4)->setIcon(QIcon(gl->path+"picts/questionsSettings.png"));
    navList->item(4)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Sound"));
    navList->item(5)->setIcon(QIcon(gl->path+"picts/soundSettings.png"));
    navList->item(5)->setTextAlignment(Qt::AlignCenter);
    
    
    m_globalSett = new GlobalSettings();
    m_scoreSett = new ScoreSettings();
    m_nameSett = new NameSettings();
    m_guitarSett = new GuitarSettings();
    m_examSett = new ExamSettings(gl->E, &gl->EquestionColor, &gl->EanswerColor, &gl->EnotBadColor);
#if defined (Q_OS_LINUX)
    TpulseWarring *pulseLab = 0;
    if (checkForPulse())
      m_sndInSett = new AudioInSettings(gl->A, gl->path);
    else {
      m_sndInSett = 0;
      pulseLab = new TpulseWarring();
    }
#else  
    m_sndInSett = new AudioInSettings(gl->A, gl->path);
#endif
    m_sndOutSett = new AudioOutSettings(gl->A, m_sndInSett); // m_sndInSett is bool - true when exist
    QTabWidget *sndTTab = new QTabWidget();
    if (m_sndInSett)
        sndTTab->addTab(m_sndInSett, tr("listening"));
    else
        sndTTab->addTab(pulseLab, tr("listening"));
    sndTTab->addTab(m_sndOutSett, tr("playing"));    

    stackLayout->addWidget(m_globalSett);
    stackLayout->addWidget(m_scoreSett);
    stackLayout->addWidget(m_nameSett);
    stackLayout->addWidget(m_guitarSett);
    stackLayout->addWidget(m_examSett);
    stackLayout->addWidget(sndTTab);

    connect(navList, SIGNAL(currentRowChanged(int)), this, SLOT(changeSettingsWidget(int)));
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(m_nameSett, SIGNAL(seventhIsBChanged(bool)), m_scoreSett, SLOT(seventhIsBChanged(bool)));

    navList->setCurrentRow(0);

}

/** To avoid generating audio devices list every opening Nootka prefereces
     * witch is slow for pulseaudio, the list is generated on demand.
     * When user first time opens Sound settings widget.*/
void SettingsDialog::changeSettingsWidget(int index) {
  stackLayout->setCurrentIndex(index);
  if (index == 5 && m_sndInSett) { // generate devices list for sound settings if sound is available
      m_sndInSett->generateDevicesList();
      m_sndOutSett->generateDevicesList();
  }
}



void SettingsDialog::saveSettings() {
    m_scoreSett->saveSettings();
    m_globalSett->saveSettings();
    m_nameSett->saveSettings();
    m_guitarSett->saveSettings();
    m_examSett->saveSettings();
    m_sndOutSett->saveSettings();
    if (m_sndInSett)
      m_sndInSett->saveSettings();
}


