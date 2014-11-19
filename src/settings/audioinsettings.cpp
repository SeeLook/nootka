/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include <widgets/troundedlabel.h>
#include <QtWidgets>


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
	QWidget *m_1_device, *m_2_params, *m_3_middleA; // m_4_test is declared in header
  
//################### 1. 	Input device & pitch detection #################################
	m_1_device = new QWidget();
	m_toolBox->addItem(m_1_device, tr("1. Input device and pitch detection"));  
  
  QLabel *devLab = new QLabel(tr("input device"), m_1_device);
  m_inDeviceCombo = new QComboBox(m_1_device);
		m_inDeviceCombo->setStatusTip(tr("Be sure your input device (microphone, webcam, instrument, etc.) is plugged in, properly configured, and working."));
	
	m_JACK_ASIO_ChB = new QCheckBox(this);
#if defined (Q_OS_WIN)
	m_JACK_ASIO_ChB->setText("ASIO");
#elif defined (Q_OS_LINUX)
	m_JACK_ASIO_ChB->setText("JACK");
#endif
	m_JACK_ASIO_ChB->setChecked(m_glParams->JACKorASIO);
  m_mpmRadio = new QRadioButton("MPM", m_1_device);
	m_correlRadio = new QRadioButton("autocorrelation", m_1_device);
	m_cepstrumRadio = new QRadioButton("MPM + modified cepstrum", m_1_device);
  QButtonGroup *butGr = new QButtonGroup(m_1_device);
	butGr->addButton(m_mpmRadio);
  butGr->addButton(m_correlRadio);
	butGr->addButton(m_cepstrumRadio);
	if (m_glParams->detectMethod == e_MPM)
		m_mpmRadio->setChecked(true);
	else if (m_glParams->detectMethod == e_AUTOCORRELATION)
		m_correlRadio->setChecked(true);
	else
		m_cepstrumRadio->setChecked(true);
	
	durHeadLab = new QLabel(tr("minimum note duration"), m_1_device);
	durationSpin = new QSpinBox(m_1_device);
		durationSpin->setMinimum(10);
		durationSpin->setMaximum(1000);
		durationSpin->setSuffix("  "  + tr("[milliseconds]"));
		durationSpin->setSingleStep(50);
		durationSpin->setValue(m_glParams->minDuration * 1000); // minimum duration is stored in seconds but displayed in milliseconds
		durationSpin->setStatusTip(tr("Only sounds longer than the selected time will be pitch-detected.<br>Selecting a longer minimum note duration helps avoid capturing fret noise or other unexpected sounds but decreases responsiveness."));
	// 1. Layout
	QVBoxLayout *deviceLay = new QVBoxLayout;
		QHBoxLayout *rtDevLay = new QHBoxLayout;
		deviceLay->addWidget(devLab);
		rtDevLay->addWidget(m_inDeviceCombo);
			rtDevLay->addWidget(m_JACK_ASIO_ChB);
		deviceLay->addLayout(rtDevLay);
		deviceLay->addStretch(1);
	QHBoxLayout *modeButtonsLay = new QHBoxLayout;
		modeButtonsLay->addStretch(2);
		modeButtonsLay->addWidget(m_mpmRadio);
		modeButtonsLay->addStretch(1);
		modeButtonsLay->addWidget(m_correlRadio);
		modeButtonsLay->addStretch(1);
		modeButtonsLay->addWidget(m_cepstrumRadio);
		modeButtonsLay->addStretch(2);
	QVBoxLayout *modeLay = new QVBoxLayout;
		modeLay->addLayout(modeButtonsLay);
		modeLay->addStretch(1);
	modeGr = new QGroupBox(tr("pitch detection mode"), m_1_device);
		modeGr->setLayout(modeLay);
	deviceLay->addWidget(modeGr);
  deviceLay->addStretch(1);
	QHBoxLayout *durLay = new QHBoxLayout;
		durLay->addStretch();
		durLay->addWidget(durHeadLab);
		durLay->addWidget(durationSpin, 0, Qt::AlignLeft);
		durLay->addStretch();
	deviceLay->addLayout(durLay);
	deviceLay->addStretch(1);
  m_1_device->setLayout(deviceLay);
	
//################### 2. Pitch detection parameters #################################
	m_2_params = new QWidget();
	m_toolBox->addItem(m_2_params, tr("2. Pitch detection parameters"));
	
	QLabel *volLabel = new QLabel(tr("minimum volume"), m_2_params);
	volumeSlider = new TvolumeSlider(m_2_params);
		volumeSlider->setValue(m_glParams->minimalVol);
		volumeSlider->setStatusTip(tr("Minimum volume of a sound to be pitch-detected"));
	
	lowRadio = new QRadioButton(tr("low"), m_2_params);
		lowRadio->setStatusTip(tr("The lowest notes.<br>Suitable for bass guitar, double bass, etc."));
	middleRadio = new QRadioButton(tr("middle"), m_2_params);
		middleRadio->setStatusTip(tr("Notes above <b>A contra</b>.<br>Suitable for guitar, cello, human voice, etc."));
	highRadio = new QRadioButton(tr("high"), m_2_params);
		highRadio->setStatusTip(tr("Notes above <b>small g</b>.<br>Suitable for high pitched instruments such as flute, piccolo, etc."));
	QButtonGroup *rangeGr = new QButtonGroup(m_2_params);
		rangeGr->addButton(lowRadio);
		rangeGr->addButton(middleRadio);
		rangeGr->addButton(highRadio);
	if (m_glParams->range == TaudioParams::e_low)
		lowRadio->setChecked(true);
	else if (m_glParams->range == TaudioParams::e_middle)
		middleRadio->setChecked(true);
	else
		highRadio->setChecked(true);
	
	TintonationCombo *intoCombo = new TintonationCombo(m_2_params);
		m_intonationCombo = intoCombo->accuracyCombo;
		m_intonationCombo->setCurrentIndex(m_glParams->intonation);
	// 2. Layout
	QVBoxLayout *paramsLay = new QVBoxLayout;
	QHBoxLayout *volLay = new QHBoxLayout;
		volLay->addWidget(volLabel);
		volLay->addWidget(volumeSlider);
	paramsLay->addLayout(volLay);
	QHBoxLayout *rangeLay = new QHBoxLayout;
		rangeLay->addWidget(lowRadio);
		rangeLay->addWidget(middleRadio);
		rangeLay->addWidget(highRadio);
	QGroupBox *rangeBox = new QGroupBox(tr("Range of note pitches:"), m_2_params);
		rangeBox->setLayout(rangeLay);
	paramsLay->addWidget(rangeBox);
	paramsLay->addWidget(intoCombo, 1, Qt::AlignCenter);
	m_2_params->setLayout(paramsLay);
		
//################### 3. Middle a & transposition #################################
	m_3_middleA= new QWidget();
	m_toolBox->addItem(m_3_middleA, tr("3. 'Middle a' and transposition"));
  
  QLabel *headLab = new QLabel("<table><tr><td valign=\"middle\">" + tr("middle A") + QString("&nbsp;&nbsp;&nbsp;%1</td></tr></table>").arg(wrapPixToHtml(Tnote(6, 1, 0), Tclef::e_treble_G, TkeySignature(0), 4.0)), m_3_middleA);
  QLabel *frLab = new QLabel(tr("frequency:"), m_3_middleA);
  
  freqSpin = new QSpinBox(m_3_middleA);
		freqSpin->setStatusTip(tr("The base frequency of <i>middle a</i>.<br>Detection of the proper pitch of notes is relative to this value. This also affects the pitch of played sounds."));
		freqSpin->setMinimum(200);
		freqSpin->setMaximum(900);
		freqSpin->setSuffix(" Hz");
  frLab->setStatusTip(freqSpin->statusTip());
  
  QLabel *intervalLab = new QLabel(tr("interval:"), m_3_middleA);
  m_intervalSpin = new QSpinBox(m_3_middleA);
		m_intervalSpin->setRange(0, 12);
		m_intervalSpin->setSpecialValueText(tr("none"));
		m_intervalSpin->setMinimumWidth(fontMetrics().boundingRect("w").width() * 15); // width of ten letters 'w' 
		m_intervalSpin->setStatusTip(tr("Shifts the frequency of <i>middle a</i>. It can be used as a transposition."));
	m_upSemiToneRadio = new QRadioButton(tr("up"), m_3_middleA);
		m_upSemiToneRadio->setIcon(QIcon(style()->standardIcon(QStyle::SP_ArrowUp)));
	m_downsSemitoneRadio = new QRadioButton(tr("down"), m_3_middleA);
		m_downsSemitoneRadio->setIcon(QIcon(style()->standardIcon(QStyle::SP_ArrowDown)));
  intervalLab->setStatusTip(m_intervalSpin->statusTip());
	QButtonGroup *upDownGroup = new QButtonGroup(m_3_middleA);
		upDownGroup->addButton(m_upSemiToneRadio);
		upDownGroup->addButton(m_downsSemitoneRadio);
	m_upSemiToneRadio->setChecked(true);
  
	tuneFreqlab = new TroundedLabel(m_3_middleA);
		tuneFreqlab->setAlignment(Qt::AlignCenter);
		QFont ff = tuneFreqlab->font();
		ff.setPixelSize(fontMetrics().boundingRect("A").height() * 1.2);
		tuneFreqlab->setFont(ff);
		tuneFreqlab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  
  // 3. Layout
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
	m_3_middleA->setLayout(middleAlay);
	
//################### 4. Test the settings #################################
	m_4_test= new QWidget();
	m_toolBox->addItem(m_4_test, tr("4. Test the settings"));  
  
  testTxt = tr("Test");
  stopTxt = tr("Stop");
  
  testButt = new QPushButton(testTxt, m_4_test);
		testButt->setStatusTip(tr("Check, are your audio input settings appropriate?<br>And how well does pitch detection work for your selected settings?"));
		testButt->setIconSize(QSize(48, 48));
  pitchView = new TpitchView(m_audioIn, m_4_test, false);
		pitchView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		pitchView->setPitchColor(Qt::darkGreen);
		pitchView->setMinimalVolume(m_glParams->minimalVol);
	
	QFont labFont = font();
	labFont.setPixelSize(fontMetrics().boundingRect("A").height() * 2);
  pitchLab = new TroundedLabel("--", this);
		pitchLab->setFont(labFont);
		pitchLab->setFixedWidth(fontMetrics().boundingRect("w").width() * 10);
// 		pitchLab->setStyleSheet(styleTxt);
		pitchLab->setStatusTip(tr("Detected pitch"));
		pitchLab->setAlignment(Qt::AlignCenter);
  
  freqLab = new TroundedLabel("--", this);
		freqLab->setFixedWidth(pitchLab->width() * 2);
		freqLab->setFont(labFont);
// 		freqLab->setStyleSheet(styleTxt);
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
}


AudioInSettings::~AudioInSettings()
{
  pitchView->stopVolume();
  if (m_audioIn) {
    m_audioIn->stopListening();
    delete m_audioIn;
  }
  delete m_tmpParams;
}

//------------------------------------------------------------------------------------
//------------          methods       ------------------------------------------------
//------------------------------------------------------------------------------------
void AudioInSettings::setTestDisabled(bool disabled) {
  m_testDisabled = disabled;
  if (disabled) {
			pitchLab->setText("--");
			freqLab->setText("--");
			pitchLab->setDisabled(true);
			freqLab->setDisabled(true);
			pitchView->setDisabled(true);
	// enable the rest of widget
			m_inDeviceCombo->setDisabled(false);
			modeGr->setDisabled(false);
			m_intervalSpin->setDisabled(false);
			m_upSemiToneRadio->setDisabled(false);
			m_downsSemitoneRadio->setDisabled(false);
			volumeSlider->setDisabled(false);
			lowRadio->setDisabled(false);
			middleRadio->setDisabled(false);
			highRadio->setDisabled(false);
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
			modeGr->setDisabled(true);
			volumeSlider->setDisabled(true);
			lowRadio->setDisabled(true);
			middleRadio->setDisabled(true);
			highRadio->setDisabled(true);
			durationSpin->setDisabled(true);
			m_intonationCombo->setDisabled(true);
  }
}


void AudioInSettings::grabParams(TaudioParams *params) {
  if (freqSpin->value() == 440 )
      params->a440diff = 0.0;
  else
      params->a440diff = getDiff(freqSpin->value());
  params->INdevName = m_inDeviceCombo->currentText();
  if (m_mpmRadio->isChecked())
		params->detectMethod = 0;
	else if (m_correlRadio->isChecked())
		params->detectMethod = 1;
	else
		params->detectMethod = 2;
  params->INenabled = enableInBox->isChecked();
  params->minimalVol = volumeSlider->value();
	if (lowRadio->isChecked())
			params->range = TaudioParams::e_low;
	else if (middleRadio->isChecked())
			params->range = TaudioParams::e_middle;
	else
			params->range = TaudioParams::e_high;
	params->minDuration = (float)durationSpin->value() / 1000.0f;
	params->intonation = m_intonationCombo->currentIndex();
}


void AudioInSettings::restoreDefaults() {
	if (!m_testDisabled)
		testSlot();
	enableInBox->setChecked(true);
	freqSpin->setValue(440);
	m_intervalSpin->setValue(0);
	m_inDeviceCombo->setCurrentIndex(0);
	m_mpmRadio->setChecked(true);
	volumeSlider->setValue(0.4); // It is multipled by 100
	middleRadio->setChecked(true);
	durationSpin->setValue(90);
	m_intonationCombo->setCurrentIndex(3); // normal
}



void AudioInSettings::saveSettings() {
  if (m_listGenerated)
      grabParams(m_glParams);
}


void AudioInSettings::generateDevicesList() {
  if (m_listGenerated)
    return;
  updateAudioDevList();
  m_listGenerated = true;
}


void AudioInSettings::updateAudioDevList() {
  m_inDeviceCombo->clear();
  m_inDeviceCombo->addItems(TaudioIN::getAudioDevicesList());
  if (m_inDeviceCombo->count()) {
        int id = m_inDeviceCombo->findText(m_glParams->INdevName);
        if (id != -1)
            m_inDeviceCombo->setCurrentIndex(id);
				m_inDeviceCombo->setDisabled(false);
    } else {
        m_inDeviceCombo->addItem(tr("no devices found"));
        m_inDeviceCombo->setDisabled(true);
  }
}


float AudioInSettings::offPitch(float pitch) {
  return pitch2freq(pitch + m_tmpParams->a440diff);
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
    return qRound((pitch2freq(freq2pitch(freq) + m_tmpParams->a440diff)));
}


float AudioInSettings::getDiff(int freq) {
   return float(freq2pitch((double)freq) - freq2pitch(440.0)); // in semitones
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

void AudioInSettings::tuneWasChanged(Ttune* tune) {
		m_tune = tune;
		getFreqStatusTip();
}



void AudioInSettings::minimalVolChanged(float vol) {
  pitchView->setMinimalVolume(vol);
}


void AudioInSettings::stopSoundTest() {
	if (m_audioIn)
		testSlot();
}


void AudioInSettings::testSlot() {
	if (sender() == m_toolBox) {
		if (m_toolBox->currentIndex() == 3) // 4. test settings page
			setTestDisabled(false);
		else
			setTestDisabled(true);
	} else {
		if (m_toolBox->isVisible())
			setTestDisabled(!m_testDisabled);
		else
			setTestDisabled(true);
	}
  if (!m_testDisabled) { // start a test
    grabParams(m_tmpParams);
    if (!m_audioIn) { // create new audio-in device
				m_audioIn = new TaudioIN(m_tmpParams, this);
				pitchView->setAudioInput(m_audioIn);
				connect(m_audioIn, &TaudioIN::noteStarted, this, &AudioInSettings::noteSlot);
		} else { // set parameters to existing device
				m_audioIn->updateAudioParams();
				m_audioIn->setAudioInParams();
		}
    testButt->setText(stopTxt);
		testButt->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPause)));
    m_audioIn->startListening();
    pitchView->startVolume();
		pitchView->setIntonationAccuracy(m_tmpParams->intonation);
  } else { // stop a test
		if (m_audioIn) {
			pitchView->stopVolume();
			m_audioIn->stopListening();
		}
    testButt->setText(testTxt);
		testButt->setIcon(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)));
    setTestDisabled(true);
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
		m_tmpParams->a440diff = getDiff(freqSpin->value());
		getFreqStatusTip();
}


void AudioInSettings::intervalChanged() {
	if (m_intervalSpin->hasFocus()) {
		upDownIntervalSlot();
	}
}


void AudioInSettings::baseFreqChanged(int bFreq) {
	if (freqSpin->hasFocus()) {
    m_tmpParams->a440diff = getDiff(freqSpin->value());
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


/** This is not so pretty (piano staff invokes low range) */
void AudioInSettings::whenLowestNoteChanges(Tnote loNote) {
	char noteNr = loNote.chromatic();
	if (noteNr > Tnote(6, 0, 0).chromatic())
		highRadio->setChecked(true);
	else if (noteNr > Tnote(5, -2, 0).chromatic())
		middleRadio->setChecked(true);
	else
		lowRadio->setChecked(true);
}


void AudioInSettings::JACKASIOSlot() {
	TrtAudio::setJACKorASIO(m_JACK_ASIO_ChB->isChecked());
	updateAudioDevList();
	emit rtApiChanged();
}









