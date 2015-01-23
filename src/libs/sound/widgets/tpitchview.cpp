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


#include "tpitchview.h"
#include "tvolumeview.h"
#include "tintonationview.h"
#include <tcolor.h>
#include <QTimer>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QPainter>
#include <QApplication>
#include <QDebug>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent, bool withButtons): 
  QWidget(parent),
  m_audioIN(audioIn),
  m_pitchColor(Qt::red),
  m_hideCnt(8),
  m_withButtons(withButtons),
  m_bgColor(Qt::transparent),
  m_prevState((int)TaudioIN::e_paused)
{
	QHBoxLayout *outLay = new QHBoxLayout;
  m_lay = new QBoxLayout(QBoxLayout::TopToBottom);
  if (m_withButtons) {
			m_pauseButton = new QRadioButton(this);
      m_pauseButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      m_pauseButton->setStatusTip(tr("Switch on/off pitch detection"));
			m_pauseButton->setChecked(true);
  } else {
      m_pauseButton = 0;
  }
    
  m_intoView = new TintonationView(TintonationView::e_perfect, this);
		m_intoView->setStatusTip(tr("Intonation - clarity of the sound. Is it in tune."));
		m_intoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
  m_volumeView = new TvolumeView(this);
		m_volumeView->setStatusTip(tr("Shows volume level of input sound and indicates when the note was pitch-detected.") + "<br>" +
				tr("Drag a knob to adjust minimum input volume."));
		m_volumeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
	m_lay->addWidget(m_intoView, 0, Qt::AlignBottom);
	QHBoxLayout *volLay = new QHBoxLayout;
		volLay->addWidget(m_volumeView, 0, Qt::AlignBottom);
	m_lay->addLayout(volLay);
	outLay->addLayout(m_lay);
	if (m_withButtons)
				volLay->addWidget(m_pauseButton, 0, Qt::AlignHCenter);
  setLayout(outLay);
  
  m_watchTimer = new QTimer(this);
  connect(m_watchTimer, &QTimer::timeout, this, &TpitchView::updateLevel);
  if (m_withButtons) {
    connect(m_pauseButton, &QRadioButton::clicked, this, &TpitchView::pauseClicked);
  }
  connect(m_volumeView, SIGNAL(minimalVolume(float)), this, SLOT(minimalVolumeChanged(float)));
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TpitchView::setAudioInput(TaudioIN* audioIn) {
  m_audioIN = audioIn;
  if (m_audioIN) {
    connect(m_audioIN, &TaudioIN::stateChanged, this, &TpitchView::inputStateChanged);
    connect(m_audioIN, &TaudioIN::destroyed, this, &TpitchView::inputDeviceDeleted);
    setDisabled(false);
  } else 
      inputDeviceDeleted();
}


void TpitchView::watchInput() {
  if (isEnabled() && isVisible() && m_audioIN && m_audioIN->state() == TaudioIN::e_listening) {
    m_prevPitch = -1.0;
    m_volumeView->setDisabled(false);
    m_watchTimer->start(75);
    connect(m_audioIN, &TaudioIN::noteStarted, this, &TpitchView::noteSlot);
    if (m_intoView->accuracy() != TintonationView::e_noCheck)
        m_intoView->setDisabled(false);
  }
}


void TpitchView::stopWatching() {
  if (m_watchTimer->isActive()) {
    m_watchTimer->stop();
    disconnect(m_audioIN, &TaudioIN::noteStarted, this, &TpitchView::noteSlot);
    m_volumeView->setVolume(0.0);
    m_volumeView->setVolume(0.0); // it has to be called twice to reset
    if (m_intoView->isEnabled())
      m_intoView->pitchSlot(0.0);
  }
  m_volumeView->setDisabled(true);
  m_intoView->setDisabled(true);
}


void TpitchView::setPitchColor(QColor col) {
  m_pitchColor = col;
  m_volumeView->setPitchColor(col);
}


void TpitchView::setMinimalVolume(float vol) {
  m_volumeView->setMinimalVolume(vol);
}


void TpitchView::setDisabled(bool isDisabled) {
  QWidget::setDisabled(isDisabled);
  if (isDisabled)
    stopWatching();
  else
    watchInput();
}


void TpitchView::setIntonationAccuracy(int accuracy) {
	m_intoView->setAccuracy(accuracy);
	if (m_audioIN) {
		if (TintonationView::Eaccuracy(accuracy) == TintonationView::e_noCheck || isPaused()) {
				m_intoView->setDisabled(true); // intonation check disabled
		} else {
				m_intoView->setDisabled(false);
		}
  }
}


void TpitchView::resize(int fontSize) {
	fontSize = qRound((float)fontSize * 1.4);
  m_volumeView->setFixedHeight(qRound((float)fontSize * 0.9));
  m_intoView->setFixedHeight(qRound((float)fontSize * 0.9));
	if (m_withButtons) {
		m_pauseButton->setFixedSize(qRound((float)fontSize * 0.8), qRound((float)fontSize * 0.8));
  }
}


void TpitchView::markAnswer(const QColor& col) {
  if (col == Qt::transparent)
    setBgColor(col);
  else
    setBgColor(Tcolor::merge(col, palette().window().color()));
	update();
//   if (col == Qt::transparent)
//     setStyleSheet("");
//   else {
//     QColor bgColor = Tcolor::merge(col, palette().window().color());
//     bgColor.setAlpha(230);
//       setStyleSheet(QString("border-radius: 10px; %1;").arg(Tcolor::bgTag(bgColor)));
//   }
}


void TpitchView::outOfTuneAnim(float outTune, int duration) {
	setBgColor(palette().window().color());
	m_intoView->outOfTuneAnim(outTune, duration);
}


bool TpitchView::isPaused() {
  if (m_audioIN && m_withButtons)
    return !m_pauseButton->isChecked();
  return false;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TpitchView::noteSlot() {
  m_hideCnt = 0;
}


void TpitchView::updateLevel() {
  int a = 0;
  if (m_hideCnt < 8)
    switch (m_hideCnt) {
      case 0 : a = 255; break;
      case 1 : a = 225; break;
      case 2 : a = 200; break;
      case 3 : a = 175; break;
      case 4 : a = 150; break;
      case 5 : a = 110; break;
      case 6 : a = 75;  break;
      case 7 : a = 40;  break;
    }
  m_hideCnt++;
	m_volumeView->setVolume(m_audioIN->volume(), a);
	if (m_intoView->accuracy() != TintonationView::e_noCheck && m_prevPitch != m_audioIN->lastChunkPitch())
			m_intoView->pitchSlot(m_audioIN->lastChunkPitch());
	m_prevPitch = m_audioIN->lastChunkPitch();
}


void TpitchView::pauseClicked() {
  if (m_audioIN)
    m_audioIN->setStoppedByUser(!m_pauseButton->isChecked());
	if (m_pauseButton->isChecked()) {
// 		if (m_intoView->accuracy() != TintonationView::e_noCheck)
// 				m_intoView->setDisabled(false); // else is already disabled
		m_audioIN->startListening();
	} else {
		m_audioIN->stopListening();
// 		m_volumeView->setDisabled(true);
// 		m_intoView->setDisabled(true);
	}
}


void TpitchView::minimalVolumeChanged(float minVol) {
  m_audioIN->setMinimalVolume(minVol);
}


void TpitchView::inputStateChanged(int inSt) {
  if (isEnabled() && inSt != m_prevState) {
    if (m_withButtons) {
      TaudioIN::Estate inState = (TaudioIN::Estate)inSt;
      if (inState == TaudioIN::e_stopped) {
        m_pauseButton->setChecked(false);
        stopWatching();
      } else if (inState == TaudioIN::e_listening) {
        m_pauseButton->setChecked(true);
        watchInput();
      }
    }
    m_prevState = inSt;
  }
}

void TpitchView::inputDeviceDeleted() {
  setDisabled(true);
}



//#################################################################################################
//###################              EVENTS              ############################################
//#################################################################################################

void TpitchView::paintEvent(QPaintEvent* ) {
	if (m_bgColor != Qt::transparent) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(m_bgColor));
    painter.drawRoundedRect(painter.viewport(), 10, 10);
	}
}


void TpitchView::showEvent(QShowEvent* e) {
	if (!isPaused() && m_audioIN)
		watchInput();
	QWidget::showEvent(e);
}


void TpitchView::hideEvent(QHideEvent* e) {
	if (!isPaused() && m_audioIN)
		stopWatching();
	QWidget::hideEvent(e);
}












