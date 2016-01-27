/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "audioinsettings.h"
#include "tvolumeslider.h"
#include <taudioparams.h>
#include <trtaudioin.h>
#include <tpitchfinder.h>
#include <widgets/tintonationview.h>
#include <widgets/tpitchview.h>
#include <graphics/tnotepixmap.h>
#include <music/tkeysignature.h>
#include <music/ttune.h>
#include <music/tinstrument.h>
#include <widgets/troundedlabel.h>
#include <widgets/tselectinstrument.h>
#include <tnoofont.h>
#include <QtWidgets>



bool m_paramsWereChanged = false;
AudioInSettings::AudioInSettings(TaudioParams* params, Ttune* tune, QWidget* parent) :
  QWidget(parent),
  m_audioIn(0),
  m_glParams(params),
  m_listGenerated(false),
  m_tune(tune)
{
	QString styleTxt = "background-color: palette(base); border: 1px solid palette(Text); border-radius: 5px;";
	
  m_tmpParams = new TaudioParams();
  *m_tmpParams = *m_glParams;
	
	m_toolBox = new QToolBox(this);
	QWidget *m_1_device, *m_2_middleA, *m_3_advanced; // m_4_test is declared in header
  
//################### 1. 	Input device & pitch detection #################################
	m_1_device = new QWidget();
	m_toolBox->addItem(m_1_device, "1. " + tr("Input device and pitch detection"));
  
  QLabel *devLab = new QLabel(tr("input device"), m_1_device);
  m_inDeviceCombo = new QComboBox(m_1_device);
		m_inDeviceCombo->setStatusTip(tr("Be sure your input device (microphone, webcam, instrument, etc.) is plugged in, properly configured, and working."));
    m_inDeviceCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	
	m_JACK_ASIO_ChB = new QCheckBox(this);
#if defined (Q_OS_WIN)
	m_JACK_ASIO_ChB->setText("ASIO");
#elif defined (Q_OS_LINUX)
	m_JACK_ASIO_ChB->setText("JACK");
#endif
	m_JACK_ASIO_ChB->setChecked(m_glParams->JACKorASIO);
	
	durHeadLab = new QLabel(tr("minimum note duration"), m_1_device);
	durationSpin = new QSpinBox(m_1_device);
		durationSpin->setMinimum(10);
		durationSpin->setMaximum(1000);
		durationSpin->setSuffix("   "  + tr("[milliseconds]"));
		durationSpin->setSingleStep(50);
		durationSpin->setValue(qRound(m_glParams->minDuration * 1000)); // minimum duration is stored in seconds but displayed in milliseconds
		durationSpin->setStatusTip(tr("Only sounds longer than the selected time will be pitch-detected.<br>Selecting a longer minimum note duration helps avoid capturing fret noise or other unexpected sounds but decreases responsiveness."));
  QLabel *volLabel = new QLabel(tr("minimum volume"), m_1_device);
  volumeSlider = new TvolumeSlider(m_1_device);
    volumeSlider->setValue(m_glParams->minimalVol);
    volumeSlider->setStatusTip(tr("Minimum volume of a sound to be pitch-detected"));

  TintonationCombo *intoCombo = new TintonationCombo(m_1_device);
    m_intonationCombo = intoCombo->accuracyCombo;
    m_intonationCombo->setCurrentIndex(m_glParams->intonation);

	// 1. Layout
	QVBoxLayout *deviceLay = new QVBoxLayout;
		QHBoxLayout *rtDevLay = new QHBoxLayout;
//       rtDevLay->addStretch();
      rtDevLay->addWidget(devLab);
      rtDevLay->addWidget(m_inDeviceCombo);
      rtDevLay->addWidget(m_JACK_ASIO_ChB);
//       rtDevLay->addStretch();
		deviceLay->addLayout(rtDevLay);

//   deviceLay->addStretch(1);
	QHBoxLayout *durLay = new QHBoxLayout;
		durLay->addStretch();
		durLay->addWidget(durHeadLab);
		durLay->addWidget(durationSpin, 0, Qt::AlignLeft);
		durLay->addStretch();
	deviceLay->addLayout(durLay);
// 	deviceLay->addStretch(1);
  QHBoxLayout *volLay = new QHBoxLayout;
    volLay->addWidget(volLabel);
    volLay->addWidget(volumeSlider);
  deviceLay->addLayout(volLay);
  deviceLay->addWidget(intoCombo);
  deviceLay->addStretch(1);
  m_1_device->setLayout(deviceLay);
		
//################### 2. Middle a & transposition #################################
	m_2_middleA = new QWidget();
	m_toolBox->addItem(m_2_middleA, "2. " + tr("'Middle a' and transposition"));
  
  QLabel *headLab = new QLabel("<table><tr><td valign=\"middle\">" + tr("middle A") + QString("&nbsp;&nbsp;&nbsp;%1</td></tr></table>").arg(wrapPixToHtml(Tnote(6, 1, 0), Tclef::e_treble_G, TkeySignature(0), 4.0)), m_2_middleA);
  QLabel *frLab = new QLabel(tr("frequency:"), m_2_middleA);
  
  freqSpin = new QSpinBox(m_2_middleA);
		freqSpin->setStatusTip(tr("The base frequency of <i>middle a</i>.<br>Detection of the proper pitch of notes is relative to this value. This also affects the pitch of played sounds."));
		freqSpin->setMinimum(200);
		freqSpin->setMaximum(900);
		freqSpin->setSuffix(" Hz");
  frLab->setStatusTip(freqSpin->statusTip());
  
  QLabel *intervalLab = new QLabel(tr("interval:"), m_2_middleA);
  m_intervalSpin = new QSpinBox(m_2_middleA);
		m_intervalSpin->setRange(0, 12);
		m_intervalSpin->setSpecialValueText(tr("none"));
		m_intervalSpin->setMinimumWidth(fontMetrics().boundingRect("w").width() * 15); // width of ten letters 'w' 
		m_intervalSpin->setStatusTip(tr("Shifts the frequency of <i>middle a</i>. It can be used as a transposition."));
	m_upSemiToneRadio = new QRadioButton(tr("up"), m_2_middleA);
		m_upSemiToneRadio->setIcon(QIcon(style()->standardIcon(QStyle::SP_ArrowUp)));
	m_downsSemitoneRadio = new QRadioButton(tr("down"), m_2_middleA);
		m_downsSemitoneRadio->setIcon(QIcon(style()->standardIcon(QStyle::SP_ArrowDown)));
  intervalLab->setStatusTip(m_intervalSpin->statusTip());
	QButtonGroup *upDownGroup = new QButtonGroup(m_2_middleA);
		upDownGroup->addButton(m_upSemiToneRadio);
		upDownGroup->addButton(m_downsSemitoneRadio);
	m_upSemiToneRadio->setChecked(true);
  
	tuneFreqlab = new TroundedLabel(m_2_middleA);
		tuneFreqlab->setAlignment(Qt::AlignCenter);
		QFont ff = tuneFreqlab->font();
		ff.setPixelSize(fontMetrics().boundingRect("A").height() * 1.2);
		tuneFreqlab->setFont(ff);
		tuneFreqlab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  
  // 2. Layout
	QGridLayout *midGrLay = new QGridLayout;
		midGrLay->addWidget(frLab, 0, 0);
		midGrLay->addWidget(freqSpin, 0, 1);
		midGrLay->addWidget(intervalLab, 1, 0);
		QVBoxLayout *upDownLay = new QVBoxLayout;
			upDownLay->addWidget(m_upSemiToneRadio);
			upDownLay->addWidget(m_downsSemitoneRadio);
		QHBoxLayout *intervalLay = new QHBoxLayout;
			intervalLay->addWidget(m_intervalSpin);
			intervalLay->addLayout(upDownLay);
		midGrLay->addLayout(intervalLay, 1, 1);
	QHBoxLayout *aLay = new QHBoxLayout;
		aLay->addWidget(headLab, 0, Qt::AlignCenter);
		aLay->addLayout(midGrLay);
	QVBoxLayout *middleAlay = new QVBoxLayout();
		middleAlay->addLayout(aLay);
		middleAlay->addWidget(tuneFreqlab, 0, Qt::AlignCenter);
	m_2_middleA->setLayout(middleAlay);
	
//################### 3. Advanced #################################
  m_3_advanced = new QWidget();
  m_toolBox->addItem(m_3_advanced, "3. " + tr("Advanced"));

  QLabel *methodLab = new QLabel(tr("pitch detection mode"), m_3_advanced);
  m_methodCombo = new QComboBox(m_3_advanced);
    m_methodCombo->addItem("MPM");
    m_methodCombo->addItem("autocorrelation");
    m_methodCombo->addItem("MPM + modified cepstrum");
  if (m_glParams->detectMethod == e_MPM)
    m_methodCombo->setCurrentIndex(0);
  else if (m_glParams->detectMethod == e_AUTOCORRELATION)
    m_methodCombo->setCurrentIndex(1);
  else
    m_methodCombo->setCurrentIndex(2);

  m_splitVolChB = new QCheckBox(tr("split when volume rise"), this);
  m_splitVolChB->setStatusTip(tr("Helps to properly detect the same notes repeated quickly on the guitar."));
  m_splitVolSpin = new QSpinBox(this);
    m_splitVolSpin->setRange(5, 50);
    m_splitVolSpin->setSingleStep(5);
    m_splitVolSpin->setSuffix(" %");
    m_splitVolSpin->setStatusTip(m_splitVolChB->statusTip());
  if (m_glParams->minSplitVol > 0.0) {
    m_splitVolChB->setChecked(true);
    m_splitVolSpin->setValue(m_glParams->minSplitVol);
  } else {
    m_splitVolChB->setChecked(false);
    m_splitVolSpin->setDisabled(true);
  }

  m_skipStillerChB = new QCheckBox(tr("skip stiller than"), this);
    m_skipStillerChB->setStatusTip(tr("Skips stiller sounds, below given percent of average volume. It prevents detecting of harmonics on classical or acoustic guitar but requires playing with similar strength."));
  m_skipStillerSpin = new QSpinBox(this);
    m_skipStillerSpin->setRange(10, 95);
    m_skipStillerSpin->setSingleStep(5);
    m_skipStillerSpin->setSuffix(" %");
    m_skipStillerSpin->setStatusTip(m_skipStillerChB->statusTip());
    m_skipStillerSpin->setValue(m_glParams->skipStillerVal);
    m_skipStillerChB->setChecked(m_glParams->skipStillerVal > 0.0);
    m_skipStillerSpin->setDisabled(m_glParams->skipStillerVal == 0.0);

  m_noiseFilterChB = new QCheckBox(tr("noise filter"), m_3_advanced);
    m_noiseFilterChB->setChecked(m_glParams->equalLoudness);
    m_noiseFilterChB->setStatusTip(tr("It is rather necessary for mic input but may be switched off for instrument line-in plugged with less noise."));

  QLabel *adjustLab = new QLabel(tr("adjust to instrument"),  m_3_advanced);
  m_adjustToInstrButt = new TselectInstrument(m_3_advanced, TselectInstrument::e_buttonsOnlyHorizontal);
    m_adjustToInstrButt->setGlyphSize(fontMetrics().height() * 2);

  //3. Layout
  QVBoxLayout *advLay = new QVBoxLayout;
    QHBoxLayout *methodLay = new QHBoxLayout;
      methodLay->addStretch();
      methodLay->addWidget(methodLab);
      methodLay->addWidget(m_methodCombo);
      methodLay->addStretch();
  advLay->addLayout(methodLay);
  QHBoxLayout *splitAndskipLay = new QHBoxLayout;
    splitAndskipLay->addStretch();
    splitAndskipLay->addWidget(m_splitVolChB);
    splitAndskipLay->addWidget(m_splitVolSpin);
    splitAndskipLay->addStretch();
    splitAndskipLay->addWidget(m_skipStillerChB);
    splitAndskipLay->addWidget(m_skipStillerSpin);
    splitAndskipLay->addStretch();
  advLay->addLayout(splitAndskipLay);
  advLay->addWidget(m_noiseFilterChB, 0, Qt::AlignCenter);
  advLay->addStretch(1);
  QHBoxLayout *adjustLay = new QHBoxLayout;
    adjustLay->addStretch();
    adjustLay->addWidget(adjustLab);
    adjustLay->addWidget(m_adjustToInstrButt);
    adjustLay->addStretch();
  advLay->addLayout(adjustLay);
  m_3_advanced->setLayout(advLay);


//################### 4. Test the settings #################################
	m_4_test= new QWidget();
  m_4_test->setStatusTip(tr("Check, are your audio input settings appropriate?<br>And how well does pitch detection work for your selected settings?"));
	m_toolBox->addItem(m_4_test, "4. " + tr("Test the settings"));
  
  testTxt = tr("Test");
  stopTxt = tr("Stop");
  
  testButt = new QPushButton(testTxt, m_4_test);
		testButt->setIconSize(QSize(48, 48));
  pitchView = new TpitchView(m_audioIn, m_4_test, false);
		pitchView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		pitchView->setFixedHeight(fontMetrics().boundingRect("A").height() * 5);
		pitchView->setPitchColor(Qt::darkGreen);
		pitchView->setMinimalVolume(m_glParams->minimalVol);
//     pitchView->enableAccuracyChange(false);
	
	QFont labFont = font();
	labFont.setPixelSize(fontMetrics().boundingRect("A").height() * 2);
  pitchLab = new TroundedLabel("--", this);
		pitchLab->setFont(labFont);
		pitchLab->setFixedWidth(fontMetrics().boundingRect("w").width() * 10);
		pitchLab->setStatusTip(tr("Detected pitch"));
		pitchLab->setAlignment(Qt::AlignCenter);
  
  freqLab = new TroundedLabel("--", this);
		freqLab->setFixedWidth(pitchLab->width() * 2);
		freqLab->setFont(labFont);
		freqLab->setAlignment(Qt::AlignCenter);
  getFreqStatusTip();
  
	// 4. Layout
	QHBoxLayout *labelsLay = new QHBoxLayout;
		labelsLay->addStretch();
		labelsLay->addWidget(testButt);
		labelsLay->addStretch();
		labelsLay->addWidget(pitchLab);
		labelsLay->addStretch();
		labelsLay->addWidget(freqLab);
		labelsLay->addStretch();
	QVBoxLayout *testLay = new QVBoxLayout;
		testLay->addStretch();
		testLay->addLayout(labelsLay);
		testLay->addStretch();
		testLay->addWidget(pitchView);
		testLay->addStretch();
	m_4_test->setLayout(testLay);
  
  // Whole layout
	QVBoxLayout *inLay = new QVBoxLayout();
		inLay->addWidget(m_toolBox);
	enableInBox = new QGroupBox(tr("enable pitch detection"), this);
  enableInBox->setLayout(inLay);
	QVBoxLayout *lay = new QVBoxLayout();
  lay->addWidget(enableInBox);
  setLayout(lay);
  
  m_testDisabled = false;
  setTestDisabled(true);
	enableInBox->setCheckable(true);
  enableInBox->setChecked(m_glParams->INenabled);
	freqSpin->setValue(getFreq(440.0));
	intervalFromFreq(freqSpin->value());
  
  connect(testButt, SIGNAL(clicked()), this, SLOT(testSlot()));
	connect(m_toolBox, SIGNAL(currentChanged(int)), this, SLOT(testSlot()));
  connect(m_intervalSpin, SIGNAL(valueChanged(int)), this, SLOT(intervalChanged()));
	connect(m_upSemiToneRadio, SIGNAL(clicked(bool)), this, SLOT(upDownIntervalSlot()));
	connect(m_downsSemitoneRadio, SIGNAL(clicked(bool)), this, SLOT(upDownIntervalSlot()));
  connect(freqSpin, SIGNAL(valueChanged(int)), this, SLOT(baseFreqChanged(int)));
  connect(volumeSlider, SIGNAL(valueChanged(float)), this, SLOT(minimalVolChanged(float)));
	connect(m_JACK_ASIO_ChB, &QCheckBox::clicked, this, &AudioInSettings::JACKASIOSlot);
  connect(enableInBox, &QGroupBox::clicked, this, &AudioInSettings::testSlot);
  connect(m_splitVolChB, &QCheckBox::toggled, this, &AudioInSettings::splitByVolChanged);
  connect(m_skipStillerChB, &QCheckBox::toggled, this, &AudioInSettings::skipStillerChanged);
  connect(m_adjustToInstrButt, &TselectInstrument::instrumentChanged, this, &AudioInSettings::adjustInstrSlot);
#if defined(Q_OS_WIN)
  connect(m_inDeviceCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(asioDriverChanged(int)));
#endif
}


AudioInSettings::~AudioInSettings()
{
  if (m_audioIn)
    m_audioIn->stopListening();
  if (m_paramsWereChanged) { // params were changed but not accepted
    *m_glParams = *m_tmpParams; // restore them
    m_audioIn->updateAudioParams();
  }
  delete m_tmpParams;
}

//------------------------------------------------------------------------------------
//------------          methods       ------------------------------------------------
//------------------------------------------------------------------------------------
void AudioInSettings::setTestDisabled(bool disabled) {
  if (m_testDisabled != disabled) {
    m_testDisabled = disabled;
    if (disabled) {
      pitchLab->setText("--");
      freqLab->setText("--");
      pitchLab->setDisabled(true);
      freqLab->setDisabled(true);
      pitchView->setDisabled(true);
  // enable the rest of widget
      m_inDeviceCombo->setDisabled(false);
      m_methodCombo->setDisabled(false);
      m_intervalSpin->setDisabled(false);
      m_upSemiToneRadio->setDisabled(false);
      m_downsSemitoneRadio->setDisabled(false);
      volumeSlider->setDisabled(false);
      durationSpin->setDisabled(false);
      m_intonationCombo->setDisabled(false);
    } else {
      pitchLab->setDisabled(false);
      freqLab->setDisabled(false);
      pitchView->setDisabled(false);
  // disable the rest of widget
      m_inDeviceCombo->setDisabled(true);
      m_intervalSpin->setDisabled(true);
      m_upSemiToneRadio->setDisabled(true);
      m_downsSemitoneRadio->setDisabled(true);
      m_methodCombo->setDisabled(true);
      volumeSlider->setDisabled(true);
      durationSpin->setDisabled(true);
      m_intonationCombo->setDisabled(true);
    }
  }
}


void AudioInSettings::grabParams(TaudioParams *params) {
  if (freqSpin->value() == 440 )
      params->a440diff = 0.0;
  else
      params->a440diff = getDiff(freqSpin->value());
  params->INdevName = m_inDeviceCombo->currentText();
  params->INenabled = enableInBox->isChecked();
  params->minimalVol = volumeSlider->value();
	params->minDuration = (qreal)durationSpin->value() / 1000.0;
	params->intonation = m_intonationCombo->currentIndex();
	params->JACKorASIO = m_JACK_ASIO_ChB->isChecked();
  params->equalLoudness = m_noiseFilterChB->isChecked();
  params->minSplitVol = m_splitVolChB->isChecked() ? (qreal)m_splitVolSpin->value() : 0.0;
  params->skipStillerVal = m_skipStillerChB->isChecked() ? (qreal)m_skipStillerSpin->value() : 0.0;
  params->detectMethod = m_methodCombo->currentIndex();
}


void AudioInSettings::restoreDefaults() {
	if (!m_testDisabled)
		testSlot();
	enableInBox->setChecked(true);
	freqSpin->setValue(440);
	m_intervalSpin->setValue(0);
	m_inDeviceCombo->setCurrentIndex(0);
	m_methodCombo->setCurrentIndex(2);
	volumeSlider->setValue(0.4); // It is multiplied by 100
	durationSpin->setValue(150);
	m_intonationCombo->setCurrentIndex(3); // normal
  adjustInstrSlot((int)e_classicalGuitar);
}



void AudioInSettings::saveSettings() {
  if (m_listGenerated && enableInBox->isChecked()) {
      grabParams(m_glParams);
  } else // do not save any state to global params if disabled
    m_glParams->INenabled = false;
    m_paramsWereChanged = false; // params were accepted
}


void AudioInSettings::generateDevicesList() {
  if (m_listGenerated)
    return;
  updateAudioDevList();
  m_listGenerated = true;
}


void AudioInSettings::updateAudioDevList() {
  m_inDeviceCombo->blockSignals(true);
  m_inDeviceCombo->clear();
  m_inDeviceCombo->addItems(TaudioIN::getAudioDevicesList());
  if (m_inDeviceCombo->count()) {
        QString currentDevName = TrtAudio::inputName();
        if (currentDevName.isEmpty() || !enableInBox->isChecked())
          currentDevName = m_glParams->INdevName;
        int id = m_inDeviceCombo->findText(currentDevName);
        if (id != -1)
            m_inDeviceCombo->setCurrentIndex(id);
				m_inDeviceCombo->setDisabled(false);
    } else {
        m_inDeviceCombo->addItem(tr("no devices found"));
        m_inDeviceCombo->setDisabled(true);
  }
  m_inDeviceCombo->blockSignals(false);
}


float AudioInSettings::offPitch(float pitch) {
  return pitch2freq(pitch + m_glParams->a440diff);
}


void AudioInSettings::getFreqStatusTip() {
  QString freqTxt = "";
  if (m_tune->stringNr() > 2) { // guitar
    for (int i = 1; i <= m_tune->stringNr(); i++) {
      freqTxt += QString("<span style=\"font-family: nootka;\">%1</span>%2 = %3 Hz, ").arg(i).
      arg((m_tune->str(i)).toRichText()).
      arg(offPitch((float)m_tune->str(i).chromatic() + 47), 0, 'f', 1);
      if (i % 2 == 0 && i < 6)
          freqTxt += "<br>"; // two entries per line
      else if (i % 3 == 0 && i < 6)
          freqTxt += "ALT_BR"; // three entries per line
    }
  } else { // no guitar - C-major scale frequencies
    for (int i = 1; i < 8; i++) {
      freqTxt += QString("<b>%1</b> = %2 Hz, ").arg(Tnote(i, 1, 0).toRichText()).
          arg(offPitch((float)Tnote(i, 1, 0).chromatic() + 47), 0, 'f', 1);
      if (i % 2 == 0 && i < 7)
          freqTxt += "<br>"; // three entries per line
      else if (i % 3 == 0 && i < 7)
          freqTxt += "ALT_BR"; // four entries per line
    }
  }
  QString freq2 = freqTxt;
  freqLab->setStatusTip(tr("Frequency of detected note. You can use this for tuning.") + "<br>" +
        freq2.replace("<br>", "").replace("ALT_BR", "<br>")); // three entries per line in status tip
  tuneFreqlab->setText(freqTxt.replace("ALT_BR", "")); // two entries per line on the label
}


int AudioInSettings::getFreq(double freq) {
    return qRound((pitch2freq(freq2pitch(freq) + m_glParams->a440diff)));
}


float AudioInSettings::getDiff(int freq) {
   return float(freq2pitch((double)freq) - freq2pitch(440.0)); // in semitones
}


//#################################################################################################
//###################        PUBLIC SLOTS              ############################################
//#################################################################################################


void AudioInSettings::tuneWasChanged(Ttune* tune) {
  m_tune = tune;
  getFreqStatusTip();
}


void AudioInSettings::stopSoundTest() {
  if (m_audioIn)
    testSlot();
}


void AudioInSettings::whenInstrumentChanged(int instr) {
  adjustInstrSlot(instr);
}

#if defined(Q_OS_WIN)
void AudioInSettings::asioDeviceSlot(int id) {
  if (TaudioIN::currentRtAPI() == "ASIO") {
    if (id < m_inDeviceCombo->count()) {
      m_inDeviceCombo->blockSignals(true);
      m_inDeviceCombo->setCurrentIndex(id);
      m_inDeviceCombo->blockSignals(false);
    }
  }
}
#endif

//#################################################################################################
//###################      PROTECTED SLOTS             ############################################
//#################################################################################################

void AudioInSettings::minimalVolChanged(float vol) {
  pitchView->setMinimalVolume(vol);
}


void AudioInSettings::testSlot() {
  bool tempTestState = m_testDisabled;
  if (sender() == testButt)
    setTestDisabled(!m_testDisabled);
	else if (sender() == m_toolBox) {
		if (m_toolBox->currentIndex() == 3) // 4. test settings page
			setTestDisabled(false);
		else
			setTestDisabled(true);
	} else {
      if (enableInBox->isChecked() && m_toolBox->isVisible()) {
        if (m_toolBox->currentIndex() == 3) // 4. test settings page
          setTestDisabled(false);
        else
          setTestDisabled(true);
      } else
        setTestDisabled(true);
	}
	if (tempTestState != m_testDisabled) {
    if (!m_testDisabled) { // start a test
      grabParams(m_glParams);
      m_paramsWereChanged = true;
      if (!m_audioIn) { // create new audio-in device
        if (TaudioIN::instance()) {
          m_audioIn = TaudioIN::instance();
          m_audioIn->updateAudioParams();
        } else {
          m_audioIn = new TaudioIN(m_glParams);
        }
        pitchView->setAudioInput(m_audioIn);
        connect(m_audioIn, &TaudioIN::noteStarted, this, &AudioInSettings::noteSlot);
      } else { // set parameters to existing device
          m_audioIn->updateAudioParams();
      }
    // ambitus is lowest note of instrument scale dropped on 2 major and highest Tartini note (140 in MIDI)
      m_audioIn->setAmbitus(Tnote(m_tune->str(m_tune->stringNr()).chromatic() - 2), Tnote(93));
      testButt->setText(stopTxt);
      testButt->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPause)));
      m_audioIn->startListening();
      pitchView->watchInput();
      pitchView->setIntonationAccuracy(m_glParams->intonation);
    } else { // stop a test
      if (m_audioIn) {
        pitchView->stopWatching();
        m_audioIn->stopListening();
        m_intonationCombo->setCurrentIndex(m_audioIn->intonationAccuracy());
      }
      testButt->setText(testTxt);
      testButt->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
      setTestDisabled(true);
    }
  }
}


void AudioInSettings::noteSlot(const TnoteStruct& ns) {
	Tnote n = ns.pitch;
	pitchLab->setText("<b>" + n.toRichText() + "</b>");
	freqLab->setText(QString("%1 Hz").arg(ns.freq, 0, 'f', 1, '0'));
}


void AudioInSettings::upDownIntervalSlot() {
	int upDown = 1;
		if (m_downsSemitoneRadio->isChecked())
				upDown = -1;
		setTransposeInterval(m_intervalSpin->value() * upDown);
		freqFromInterval(m_intervalSpin->value() * upDown);
		m_glParams->a440diff = getDiff(freqSpin->value());
		getFreqStatusTip();
}


void AudioInSettings::intervalChanged() {
	if (m_intervalSpin->hasFocus()) {
		upDownIntervalSlot();
	}
}


void AudioInSettings::baseFreqChanged(int bFreq) {
	if (freqSpin->hasFocus()) {
    m_glParams->a440diff = getDiff(freqSpin->value());
		intervalFromFreq(freqSpin->value());
    getFreqStatusTip();
	}
}


void AudioInSettings::setTransposeInterval(int interval) {
	int n = qAbs(interval);
	QString suff = "";
	if (n)
		suff = tr("%n semitone(s)", "", n).replace(QString("%1").arg(n), "");
	m_intervalSpin->setSuffix(suff);
	if (interval < 0)
			m_downsSemitoneRadio->setChecked(true);
	else
			m_upSemiToneRadio->setChecked(true);
}


void AudioInSettings::intervalFromFreq(int bFreq) {
	int interval = qRound(freq2pitch((double)bFreq)  - 69.0);
	m_intervalSpin->setValue(qAbs(interval));
	setTransposeInterval(interval);
}


void AudioInSettings::freqFromInterval(int interval) {
	freqSpin->setValue(pitch2freq(69 + interval));
}


void AudioInSettings::JACKASIOSlot() {
#if defined (Q_OS_LINUX) || defined (Q_OS_WIN)
	TrtAudio::setJACKorASIO(m_JACK_ASIO_ChB->isChecked());
	updateAudioDevList();
	emit rtApiChanged();
#endif
}


void AudioInSettings::splitByVolChanged(bool enab) {
  m_splitVolSpin->setDisabled(!enab);
  if (enab)
    m_splitVolSpin->setValue(10);
}


void AudioInSettings::skipStillerChanged(bool enab) {
  m_skipStillerSpin->setDisabled(!enab);
  if (enab)
    m_skipStillerSpin->setValue(80);
}


void AudioInSettings::adjustInstrSlot(int instr) {
  switch ((Einstrument)instr) {
    case e_noInstrument:
      m_methodCombo->setCurrentIndex(0); // MPM
      m_splitVolChB->setChecked(false);
      m_skipStillerChB->setChecked(false);
      break;
    case e_classicalGuitar:
      m_methodCombo->setCurrentIndex(2); // MPM + modified cepstrum
      m_splitVolChB->setChecked(true);
      m_splitVolSpin->setValue(10);
      m_skipStillerChB->setChecked(true);
      m_skipStillerSpin->setValue(80);
      break;
    case e_electricGuitar:
    case e_bassGuitar:
      m_methodCombo->setCurrentIndex(2); // MPM + modified cepstrum
      m_splitVolChB->setChecked(true);
      m_splitVolSpin->setValue(7);
      m_skipStillerChB->setChecked(true);
      m_skipStillerSpin->setValue(80);
      break;
    default:
      break;
  }
}









