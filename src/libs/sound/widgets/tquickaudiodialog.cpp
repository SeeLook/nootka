/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tquickaudiodialog.h"
#include <music/tnotestruct.h>
#include "tpitchview.h"
#include "tvolumeview.h"
#include <tsound.h>
#include <tpath.h>
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <tqtaudioin.h>
#include <tmtr.h>
#include <QtWidgets/QtWidgets>
#include <QtAndroidExtras/qandroidfunctions.h>
#include <QtAndroidExtras/qandroidjnienvironment.h>


static QString noneText = QStringLiteral("--");


TquickAudioDialog::TquickAudioDialog(QWidget* parent) :
  QDialog(parent)
{
  showMaximized();

  int maxWidgetW = qMin(Tmtr::fingerPixels() * 12, Tmtr::longScreenSide() - 6); // about 4"

  m_outVolSlider = new QSlider(Qt::Horizontal, this);
    m_outVolSlider->setStyle(Tcore::androidStyle);
    m_outVolSlider->setRange(0, QAndroidJniObject::callStaticMethod<jint>("net/sf/nootka/ToutVolume", "maxStreamVolume"));
    m_outVolSlider->setValue(QAndroidJniObject::callStaticMethod<jint>("net/sf/nootka/ToutVolume", "streamVolume"));
    m_outVolSlider->setFixedWidth(maxWidgetW);

  int Aheight = fontMetrics().boundingRect(QStringLiteral("A")).height();

  m_pitchView = new TpitchView(nullptr, this, false);
    m_pitchView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pitchView->setFixedHeight(Aheight * 5);
    m_pitchView->setPitchColor(Qt::darkGreen);
    m_pitchView->volumeView()->setKnobAlwaysVisible(true);
    m_pitchView->volumeView()->setPauseActive(true);
    m_pitchView->setFixedSize(maxWidgetW, qRound(Tmtr::fingerPixels() * 1.8));

  QString labelsStyle = QString("background-color: %1").arg(qApp->palette().window().color().lighter(105).name());
  QFont labFont = font();
  labFont.setPixelSize(Aheight * 2);
  m_pitchLab = new QLabel(noneText, this);
    m_pitchLab->setFont(labFont);
    m_pitchLab->setFixedWidth(fontMetrics().boundingRect("w").width() * 10);
//     pitchLab->setStatusTip(qTR("AudioInSettings", "Detected pitch"));
    m_pitchLab->setAlignment(Qt::AlignCenter);
    m_pitchLab->setStyleSheet(labelsStyle);

  m_freqLab = new QLabel(noneText, this);
    m_freqLab->setFixedWidth(m_pitchLab->width() * 2);
    m_freqLab->setFont(labFont);
    m_freqLab->setAlignment(Qt::AlignCenter);
    m_freqLab->setStyleSheet(labelsStyle);

  m_sysVolButt = new QPushButton(QIcon(Tpath::img("systemsettings")), QString(), this);
  int bigIconH = qRound(Tmtr::fingerPixels() * 1.3);
    m_sysVolButt->setIconSize(QSize(bigIconH, bigIconH));
    m_sysVolButt->setFlat(true);
  m_exitButt = new QPushButton(QIcon(Tpath::img("exit")), QString(), this);
    m_exitButt->setIconSize(QSize(Tmtr::fingerPixels(), Tmtr::fingerPixels()));
    m_exitButt->setFlat(true);

  m_tuneLab = new QLabel(this);
    labFont.setPixelSize(qRound(Aheight * 1.3));
    m_tuneLab->setFont(labFont);
    m_tuneLab->setStyleSheet(labelsStyle);
    m_tuneLab->setAlignment(Qt::AlignCenter);
    m_tuneLab->setFixedSize(maxWidgetW, Aheight * 3);
    setTuningText();

  // layout
  auto topButtLay = new QHBoxLayout;
    topButtLay->addWidget(m_sysVolButt);
    topButtLay->addStretch();
    topButtLay->addWidget(m_exitButt);
  auto labelsLay = new QHBoxLayout;
    labelsLay->addStretch();
    labelsLay->addWidget(m_pitchLab);
    labelsLay->addWidget(m_freqLab);
    labelsLay->addStretch();
  auto lay = new QVBoxLayout;
    lay->setAlignment(Qt::AlignCenter);
    lay->addLayout(topButtLay);
    lay->addStretch();
    lay->addWidget(m_outVolSlider, 0, Qt::AlignCenter);
    lay->addLayout(labelsLay);
    lay->addWidget(m_tuneLab, 0, Qt::AlignCenter);
    lay->addWidget(m_pitchView, 0, Qt::AlignCenter);
    lay->addStretch();

  setLayout(lay);

  connect(m_exitButt, &QPushButton::clicked, this, &TquickAudioDialog::exitSlot);
  connect(m_sysVolButt, &QPushButton::clicked, this, &TquickAudioDialog::exitSlot);
  connect(m_outVolSlider, &QSlider::valueChanged, this, &TquickAudioDialog::volChangedSlot);

  if (SOUND->sniffer) {
    pitchView()->setAudioInput(SOUND->sniffer);
    pitchView()->setMinimalVolume(SOUND->sniffer->minimalVolume());
    pitchView()->setIntonationAccuracy(SOUND->sniffer->intonationAccuracy());
    SOUND->sniffer->stopTouchHandle();
    connect(SOUND->sniffer, &TaudioIN::noteStarted, this, &TquickAudioDialog::noteSlot);
    pitchView()->setDisabled(false);
    pitchView()->pauseAction()->setChecked(true);
    pitchView()->volumeView()->setPaused(false);
    if (SOUND->melodyIsPlaying()) { // wait for finishing...
        connect(SOUND, &Tsound::plaingFinished, [=]{
            SOUND->blockSignals(true);
            SOUND->sniffer->startListening();
        });
    } else {
        SOUND->blockSignals(true); // do not refresh detected notes on main score
        SOUND->sniffer->startListening();
    }
  } else
      pitchView()->setDisabled(true);
}


TquickAudioDialog::~TquickAudioDialog() {
  if (SOUND) {
    if (SOUND->sniffer) {
      disconnect(SOUND->sniffer, &TaudioIN::noteStarted, this, &TquickAudioDialog::noteSlot);
      SOUND->sniffer->startTouchHandle();
    }
    SOUND->blockSignals(false);
  }
}


void TquickAudioDialog::volUp() {
  m_outVolSlider->setValue(m_outVolSlider->value() + 1);
}


void TquickAudioDialog::volDown() {
  m_outVolSlider->setValue(m_outVolSlider->value() - 1);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TquickAudioDialog::noteSlot(const TnoteStruct& ns) {
  m_pitchLab->setText(QLatin1String("<b>") + ns.pitch.toRichText() + QLatin1String("</b>"));
  m_freqLab->setText(QString("%1 Hz").arg(ns.freq, 0, 'f', 1, '0'));
}


void TquickAudioDialog::exitSlot() {
  emit exit(sender() == m_exitButt ? static_cast<int>(e_accepted) : static_cast<int>(e_audioSettings));
  done(QDialog::Accepted);
}


void TquickAudioDialog::sysVolSlot() {
  QAndroidJniObject::callStaticMethod<void>("net/sf/nootka/ToutVolume", "show");
  QAndroidJniEnvironment env;
  if (env->ExceptionCheck())
    env->ExceptionClear();
}


void TquickAudioDialog::volChangedSlot(int v) {
  QAndroidJniObject::callStaticMethod<void>("net/sf/nootka/ToutVolume", "setStreamVolume", "(I)V", v);
}


void TquickAudioDialog::keyPressEvent(QKeyEvent* e) {
  auto k = static_cast<Qt::Key>(e->key());
  if (k == Qt::Key_VolumeDown)
      volDown();
  else if (k == Qt::Key_VolumeUp)
      volUp();
  else if (k == Qt::Key_Back) {
      emit exit(static_cast<int>(e_discarded));
      done(QDialog::Rejected);
      return;
  }
  QDialog::keyReleaseEvent(e);
}


void TquickAudioDialog::setTuningText() {
  QString br = QStringLiteral("<br>");
  QString tuneText = TabstractSoundView::getStringsFreqText(Tcore::gl()->Gtune(), Tcore::gl()->A->a440diff);
  m_tuneLab->setText(tuneText.replace(br, QString()).replace(QLatin1String("ALT_BR"), br));
}

