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


#include "tpitchview.h"
#include "tvolumeview.h"
#include "tintonationview.h"
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QPainter>
#include <QApplication>
#include <QDebug>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent, bool withButtons): 
  QWidget(parent),
  m_audioIN(audioIn),
  m_pitchColor(Qt::red),
  m_isPaused(false),
  m_hideCnt(8),
  m_withButtons(withButtons),
  m_bgColor(Qt::transparent)
{
	QHBoxLayout *outLay = new QHBoxLayout;
  m_lay = new QBoxLayout(QBoxLayout::TopToBottom);
  if (m_withButtons) {
			m_pauseChBox = new QCheckBox(this);
      m_pauseChBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      m_pauseChBox->setStatusTip(tr("Switch on/off pitch detection"));
			m_pauseChBox->setChecked(true);
  } else {
    m_pauseChBox = 0;
  }
    
  m_intoView = new TintonationView(TintonationView::e_perfect, this);
		m_intoView->setStatusTip(tr("Intonation - clarity of the sound. Is it in tune."));
		m_intoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
  m_volMeter = new TvolumeView(this);
		m_volMeter->setStatusTip(tr("Shows volume level of input sound and indicates when the note was pitch-detected.") + "<br>" +
				tr("Drag a knob to adjust minimum input volume."));
		m_volMeter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
	m_lay->addWidget(m_intoView, 0, Qt::AlignBottom);
	QHBoxLayout *volLay = new QHBoxLayout;
		volLay->addWidget(m_volMeter, 0, Qt::AlignBottom);
	m_lay->addLayout(volLay);
	outLay->addLayout(m_lay);
	if (m_withButtons)
				volLay->addWidget(m_pauseChBox, 0, Qt::AlignHCenter);
  setLayout(outLay);
  
  m_volTimer = new QTimer(this);
  connect(m_volTimer, &QTimer::timeout, this, &TpitchView::updateLevel);
  if (m_withButtons) {
    connect(m_pauseChBox, &QCheckBox::clicked, this, &TpitchView::pauseClicked);
  }
}


//------------------------------------------------------------------------------------
//------------          methods     --------------------------------------------------
//------------------------------------------------------------------------------------

void TpitchView::setAudioInput(TaudioIN* audioIn) {
  m_audioIN = audioIn;
  if (m_audioIN)
		connect(m_audioIN, &TaudioIN::noteStarted, this, &TpitchView::noteSlot);
}


void TpitchView::startVolume() {
	m_prevPitch = -1.0;
  if (m_audioIN) {
		connect(m_volMeter, SIGNAL(minimalVolume(float)), this, SLOT(minimalVolumeChanged(float)));
    m_volMeter->setDisabled(false);
    m_volTimer->start(75);
  }
}


void TpitchView::stopVolume() {
	stopTimerDelayed();
}


void TpitchView::setPitchColor(QColor col) {
  m_pitchColor = col;
  m_volMeter->setPitchColor(col);
}


void TpitchView::setMinimalVolume(float vol) {
  m_volMeter->setMinimalVolume(vol);
}


void TpitchView::setDisabled(bool isDisabled) {
  QWidget::setDisabled(isDisabled);
  m_volMeter->setDisabled(isDisabled);
	m_intoView->setDisabled(isDisabled);
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
  m_volMeter->setFixedHeight(qRound((float)fontSize * 0.9));
  m_intoView->setFixedHeight(qRound((float)fontSize * 0.9));
	if (m_withButtons) {
		m_pauseChBox->setFixedSize(qRound((float)fontSize * 0.8), qRound((float)fontSize * 0.8));
  }
}


void TpitchView::markAnswer(const QColor& col) {
	setBgColor(col);
	update();
}


void TpitchView::outOfTuneAnim(float outTune, int duration) {
	setBgColor(palette().window().color());
	m_intoView->outOfTuneAnim(outTune, duration);
}




//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------


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
	m_volMeter->setVolume(m_audioIN->volume(), a);
	if (m_intoView->accuracy() != TintonationView::e_noCheck && m_prevPitch != m_audioIN->lastChunkPitch())
			m_intoView->pitchSlot(m_audioIN->lastChunkPitch());
// 	m_prevVolume = m_audioIN->maxPeak();
	m_prevPitch = m_audioIN->lastChunkPitch();
}


void TpitchView::pauseClicked() {
	if (m_isPaused) {
		m_isPaused = false;
		if (m_intoView->accuracy() != TintonationView::e_noCheck)
				m_intoView->setDisabled(false); // else is already disabled
		m_audioIN->startListening();
		startVolume();
	} else {
		m_isPaused = true;
		m_audioIN->stopListening();
		stopVolume();
		m_volMeter->setDisabled(true);
		m_intoView->setDisabled(true);
	}
}


void TpitchView::minimalVolumeChanged(float minVol) {
		m_audioIN->setMinimalVolume(minVol);
}


void TpitchView::animationFinishedSlot() {
	
}


//------------------------------------------------------------------------------------
//------------          events      --------------------------------------------------
//------------------------------------------------------------------------------------

void TpitchView::paintEvent(QPaintEvent* )
{
	if (m_bgColor != Qt::transparent) {
			QPainter painter(this);
			painter.setPen(Qt::NoPen);
			painter.setBrush(QBrush(m_bgColor));
			painter.drawRoundedRect(painter.viewport(), 2, 2 );
	}
}


void TpitchView::stopTimerDelayed() {
	m_volTimer->stop();
	m_volMeter->setVolume(0.0);
	m_volMeter->setVolume(0.0); // it has to be called twice to reset
	m_intoView->pitchSlot(0.0);
}

