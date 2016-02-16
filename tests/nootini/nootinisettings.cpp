/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "nootinisettings.h"
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <tartiniparams.h>
#include <tpitchfinder.h>
#include <widgets/tintonationview.h>
#include <../settings/tvolumeslider.h>
#include <QtWidgets/QtWidgets>


NootiniSettings::NootiniSettings(TartiniParams* tp, QWidget* parent) :
  QDialog(parent),
  m_tartiniParams(tp)
{
  setWindowTitle(tr("Parameters of processing"));
  setWindowIcon(parent->windowIcon());

  QLabel *methodLab = new QLabel(tr("pitch detection mode"), this);
  m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("MPM");
    m_methodCombo->addItem("autocorrelation");
    m_methodCombo->addItem("MPM + modified cepstrum");
  if (Tcore::gl()->A->detectMethod == e_MPM)
    m_methodCombo->setCurrentIndex(0);
  else if (Tcore::gl()->A->detectMethod == e_AUTOCORRELATION)
    m_methodCombo->setCurrentIndex(1);
  else
    m_methodCombo->setCurrentIndex(2);

  QLabel *durHeadLab = new QLabel(tr("minimum note duration"), this);
  m_durationSpin = new QSpinBox(this);
    m_durationSpin->setMinimum(10);
    m_durationSpin->setMaximum(1000);
    m_durationSpin->setSuffix("   "  + tr("[milliseconds]"));
    m_durationSpin->setSingleStep(50);
    m_durationSpin->setValue(qRound(Tcore::gl()->A->minDuration * 1000)); // minimum duration is stored in seconds but displayed in milliseconds

  QLabel *volLabel = new QLabel(tr("minimum volume"), this);
  m_volumeSlider = new TvolumeSlider(this);
    m_volumeSlider->setValue(Tcore::gl()->A->minimalVol);

  m_lowRadio = new QRadioButton(tr("low")  + " (2048)", this);
  m_middleRadio = new QRadioButton(tr("middle") + " (1024)", this);
  m_highRadio = new QRadioButton(tr("high") + " (512)", this);
  QButtonGroup *rangeGr = new QButtonGroup(this);
    rangeGr->addButton(m_lowRadio);
    rangeGr->addButton(m_middleRadio);
    rangeGr->addButton(m_highRadio);

  QLabel *frLab = new QLabel(tr("frequency:"), this);
  m_freqSpin = new QSpinBox(this);
    m_freqSpin->setMinimum(200);
    m_freqSpin->setMaximum(900);
    m_freqSpin->setSuffix(" Hz");
    m_freqSpin->setValue(qRound((pitch2freq(freq2pitch(440) + Tcore::gl()->A->a440diff))));

  QLabel *threshLab = new QLabel(tr("threshold of lowest loudness (MPM methods)"), this);
  m_thresholdSpin = new QSpinBox(this);
    m_thresholdSpin->setRange(80, 100);
    m_thresholdSpin->setSuffix(" %");
    m_thresholdSpin->setValue(m_tartiniParams->threshold);

  m_noiseFilterChB = new QCheckBox(tr("noise filter"), this);
    m_noiseFilterChB->setChecked(Tcore::gl()->A->equalLoudness);

  m_calcNoiseChB = new QCheckBox(tr("Automatically calculate noise-floor"), this);
    m_calcNoiseChB->setChecked(m_tartiniParams->doingAutoNoiseFloor);

  m_splitVolChB = new QCheckBox(tr("split when volume rise"), this);

  m_splitVolSpin = new QSpinBox(this);
  m_splitVolChB->setChecked(Tcore::gl()->A->minSplitVol > 0.0);
    m_splitVolSpin->setRange(5, 50);
    m_splitVolSpin->setSingleStep(5);
    m_splitVolSpin->setSuffix(" %");
    m_splitVolSpin->setValue(Tcore::gl()->A->minSplitVol);

  m_skipStillerChB = new QCheckBox(tr("skip stiller than"), this);
  m_skipStillerSpin = new QSpinBox(this);
    m_skipStillerSpin->setRange(10, 95);
    m_skipStillerSpin->setSingleStep(5);
    m_skipStillerSpin->setSuffix(" %");
    m_skipStillerSpin->setStatusTip(m_skipStillerChB->statusTip());
    m_skipStillerSpin->setValue(Tcore::gl()->A->skipStillerVal);
    m_skipStillerChB->setChecked(Tcore::gl()->A->skipStillerVal > 0.0);

  QLabel *dbLab = new QLabel(tr("dbFloor"), this);
  m_dbFlorSpin = new QDoubleSpinBox(this);
    m_dbFlorSpin->setRange(-300, 0);
    m_dbFlorSpin->setValue(m_tartiniParams->dBFloor);

  m_drawVolChB = new QCheckBox(tr("draw volume chart"), this);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

  QVBoxLayout *lay = new QVBoxLayout;
  QHBoxLayout *methodLay = new QHBoxLayout;
    methodLay->addStretch();
    methodLay->addWidget(methodLab);
    methodLay->addWidget(m_methodCombo);
    methodLay->addStretch();
  lay->addLayout(methodLay);
  QHBoxLayout *durLay = new QHBoxLayout;
    durLay->addStretch();
    durLay->addWidget(durHeadLab);
    durLay->addWidget(m_durationSpin, 0, Qt::AlignLeft);
    durLay->addStretch();
  lay->addLayout(durLay);
  QHBoxLayout *volLay = new QHBoxLayout;
    volLay->addWidget(volLabel);
    volLay->addWidget(m_volumeSlider);
  lay->addLayout(volLay);
  QHBoxLayout *rangeLay = new QHBoxLayout;
    rangeLay->addWidget(m_lowRadio);
    rangeLay->addWidget(m_middleRadio);
    rangeLay->addWidget(m_highRadio);
  QGroupBox *rangeBox = new QGroupBox(tr("Range of note pitches:"), this);
    rangeBox->setLayout(rangeLay);
  lay->addWidget(rangeBox);
  QHBoxLayout *freqLay = new QHBoxLayout;
    freqLay->addStretch();
    freqLay->addWidget(frLab);
    freqLay->addWidget(m_freqSpin);
    freqLay->addStretch();
  lay->addLayout(freqLay);
  QHBoxLayout *threshLay = new QHBoxLayout;
    threshLay->addStretch();
    threshLay->addWidget(threshLab);
    threshLay->addWidget(m_thresholdSpin);
    threshLay->addStretch();
  lay->addLayout(threshLay);
  QHBoxLayout *noiseLay = new QHBoxLayout;
    noiseLay->addStretch();
    noiseLay->addWidget(m_noiseFilterChB);
    noiseLay->addStretch();
    noiseLay->addWidget(m_calcNoiseChB);
    noiseLay->addStretch();
  lay->addLayout(noiseLay);
    QHBoxLayout *splitLay = new QHBoxLayout;
    splitLay->addStretch();
    splitLay->addWidget(m_splitVolChB);
    splitLay->addWidget(m_splitVolSpin);
    splitLay->addStretch();
    splitLay->addWidget(m_skipStillerChB);
    splitLay->addWidget(m_skipStillerSpin);
    splitLay->addStretch();
  lay->addLayout(splitLay);
    QHBoxLayout *dbLay = new QHBoxLayout;
    dbLay->addStretch();
    dbLay->addWidget(dbLab);
    dbLay->addWidget(m_dbFlorSpin);
    dbLay->addStretch();
  lay->addLayout(dbLay);
  QHBoxLayout *nootLay = new QHBoxLayout;
    nootLay->addStretch();
    nootLay->addWidget(m_drawVolChB);
    nootLay->addStretch();
  lay->addLayout(nootLay);
  lay->addWidget(buttonBox);
  setLayout(lay);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_splitVolChB, &QCheckBox::toggled, this, &NootiniSettings::splitByVolChanged);
  connect(m_skipStillerChB, &QCheckBox::toggled, this, &NootiniSettings::skipStillerChanged);

}


bool NootiniSettings::drawVolumeChart() {
  return m_drawVolChB->isChecked();
}


void NootiniSettings::setDrawVolumeChart(bool draw) {
  m_drawVolChB->setChecked(draw);
}


void NootiniSettings::setRange(int r) {
  TpitchFinder::Erange rang = (TpitchFinder::Erange)r;
  switch (rang) {
    case TpitchFinder::e_high:
      m_highRadio->setChecked(true); break; // e_high - lowest pitch is F small
    case TpitchFinder::e_low:
      m_lowRadio->setChecked(true); break; // e_low - lowest pitch is ... very low
    default:
      m_middleRadio->setChecked(true); break; // e_middle - lowest pitch is F contra
  }
}


int NootiniSettings::range() {
  if (m_highRadio->isChecked())
    return TpitchFinder::e_high;
  else if (m_lowRadio->isChecked())
    return TpitchFinder::e_low;
  else
    return TpitchFinder::e_middle;
}



//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void NootiniSettings::accept() {
  Tcore::gl()->A->detectMethod = m_methodCombo->currentIndex();
  Tcore::gl()->A->minDuration = (qreal)m_durationSpin->value() / 1000.0;
  Tcore::gl()->A->minimalVol = m_volumeSlider->value();
  if (m_freqSpin->value() == 440 )
      Tcore::gl()->A->a440diff = 0.0;
  else
      Tcore::gl()->A->a440diff = float(freq2pitch((double)m_freqSpin->value()) - freq2pitch(440.0));

  m_tartiniParams->threshold = m_thresholdSpin->value();
  Tcore::gl()->A->equalLoudness = m_noiseFilterChB->isChecked();
  Tcore::gl()->A->minSplitVol = m_splitVolChB->isChecked() ? (qreal)m_splitVolSpin->value() : 0.0;
  Tcore::gl()->A->skipStillerVal = m_skipStillerChB->isChecked() ? (qreal)m_skipStillerSpin->value() : 0.0;
  m_tartiniParams->dBFloor = m_dbFlorSpin->value();
  m_tartiniParams->doingAutoNoiseFloor = m_calcNoiseChB->isChecked();

  QDialog::accept();
}






