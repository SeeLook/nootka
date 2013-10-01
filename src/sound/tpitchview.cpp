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


#include "tpitchview.h"
#include "tvolumeview.h"
#include "tintonationview.h"
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QPainter>
#include <QDebug>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent, bool withButtons): 
  QWidget(parent),
  m_audioIN(audioIn),
  m_pitchColor(Qt::red),
  m_isPaused(false),
  m_isVoice(false),
  m_hideCnt(8),
  m_withButtons(withButtons),
  m_bgColor(Qt::transparent)
{
	QHBoxLayout *outLay = new QHBoxLayout;
  m_lay = new QBoxLayout(QBoxLayout::TopToBottom);
  if (m_withButtons) {
			m_lay->setDirection(QBoxLayout::LeftToRight);
      voiceButt = new QPushButton("g", this);
      voiceButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//       lay->addWidget(voiceButt);
      voiceButt->setStatusTip(tr("Toggles between pitch detection for singing and for playing"));
// #if defined(Q_OS_MAC)
//       voiceButt->setFont(QFont("nootka", 35));
// #else
      voiceButt->setFont(QFont("nootka", 15));
// #endif
  } else {
    voiceButt = 0;
    pauseButt = 0;
  }
    
  m_intoView = new TintonationView(TintonationView::e_perfect, this);
//   viewLay->addWidget(m_intoView);
  m_intoView->setStatusTip(tr("Intonation - clarity of the sound. Is it in tune."));
  
  m_volMeter = new TvolumeView(this);
//   viewLay->addWidget(m_volMeter);
  m_volMeter->setStatusTip(tr("Shows volume level of input sound and indicates when the note was pitch-detected.") + "<br>" +
				tr("Drag a knob to adjust minimal input volume."));
//   QHBoxLayout *viewLay = new QHBoxLayout;
// 		viewLay->addWidget(m_volMeter);
// 		viewLay->addWidget(m_intoView);
//   lay->addLayout(viewLay);
  
  if (m_withButtons) {
      pauseButt = new QPushButton("n", this);
      pauseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//       lay->addWidget(pauseButt);
      pauseButt->setStatusTip(tr("Switch on/off pitch detection"));
      pauseButt->setFont(QFont("nootka", 15));
  }
	if (m_withButtons)
		outLay->addWidget(voiceButt, 0, Qt::AlignBottom);
// 		m_lay->addWidget(m_volMeter, 0, Qt::AlignBottom);
// 		m_lay->addWidget(m_intoView, 0, Qt::AlignBottom);
		
// 	} else {
		m_lay->addWidget(m_intoView, 0, Qt::AlignBottom);
		m_lay->addWidget(m_volMeter, 0, Qt::AlignBottom);
		outLay->addLayout(m_lay);
	if (m_withButtons)
			outLay->addWidget(pauseButt, 0, Qt::AlignBottom);
// 	}

  setLayout(outLay);
  
  m_volTimer = new QTimer(this);
  connect(m_volTimer, SIGNAL(timeout()), this, SLOT(updateLevel()));
  if (m_withButtons) {
    connect(pauseButt, SIGNAL(clicked()), this, SLOT(pauseClicked()));
    connect(voiceButt, SIGNAL(clicked()), this, SLOT(voiceClicked()));
  }
}


TpitchView::~TpitchView()
{
}

//------------------------------------------------------------------------------------
//------------          methods     --------------------------------------------------
//------------------------------------------------------------------------------------

void TpitchView::setAudioInput(TaudioIN* audioIn) {
  m_audioIN = audioIn;
  connect(m_audioIN, SIGNAL(noteDetected(Tnote)), this, SLOT(noteSlot(Tnote)));
}


void TpitchView::startVolume() {
  if (m_audioIN) {
		connect(m_volMeter, SIGNAL(minimalVolume(float)), this, SLOT(minimalVolumeChanged(float)));
    m_volMeter->setDisabled(false);
    m_volTimer->start(75);
		if (m_intoView->accuracy() != TintonationView::e_noCheck)
			connect(m_audioIN, SIGNAL(chunkPitch(float)), m_intoView, SLOT(pitchSlot(float)));
  }
}


void TpitchView::stopVolume() {
// 	m_volTimer->stop();
// 	m_volMeter->setVolume(0.0);
   QTimer::singleShot(600, this, SLOT(stopTimerDelayed()));
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
		if (TintonationView::Eaccuracy(accuracy) == TintonationView::e_noCheck) { // intonation check disabled
				disconnect(m_audioIN, SIGNAL(chunkPitch(float)), m_intoView, SLOT(pitchSlot(float)));
				m_intoView->setDisabled(true);
		} else {
				connect(m_audioIN, SIGNAL(chunkPitch(float)), m_intoView, SLOT(pitchSlot(float)));
				m_intoView->setDisabled(false);
		}
}


void TpitchView::resize(int fontSize) {
	if (m_lay->direction() == QBoxLayout::TopToBottom)
			fontSize = qRound((float)fontSize * 1.4);
  if (m_withButtons) {
#if defined(Q_OS_MAC)
    voiceButt->setFont(QFont("nootka", (fontSize*2)/*/2*/)); // prev was 3/2 (*1.5)
    pauseButt->setFont(QFont("nootka", (fontSize*2)/*/2*/));
//     voiceButt->setFixedHeight(2*fontSize);
//     pauseButt->setFixedHeight(2*fontSize);
#else
    voiceButt->setFont(QFont("nootka", fontSize * 1.3));
    pauseButt->setFont(QFont("nootka", fontSize * 1.3));
#endif
    voiceButt->setFixedWidth(1.5 *fontSize);
    pauseButt->setFixedWidth(1.5 *fontSize);
		qreal sizeF = 1.0;
		if (m_lay->direction() == QBoxLayout::TopToBottom)
			sizeF = 1.5;
    voiceButt->setFixedHeight(2 * fontSize * sizeF);
    pauseButt->setFixedHeight(2 * fontSize * sizeF);
  }
  m_volMeter->setFixedHeight(qRound((float)fontSize * 1.2));
  m_intoView->setFixedHeight(qRound((float)fontSize * 1.2));
}


void TpitchView::setIsVoice(bool isVoice) {
  if (isVoice) {
				voiceButt->setText("v"); // singer symbol for voice mode
				m_isVoice = true;
	//       m_intoView->setAccuracy(TintonationView::e_sufficient);
    } else {
				voiceButt->setText("g"); // guitar symbol for instruments mode
				m_isVoice = false;
	//       m_intoView->setAccuracy(TintonationView::e_perfect);
    }
    if (m_audioIN)
				m_audioIN->setIsVoice(m_isVoice);
}


void TpitchView::setHorizontal(bool isHorizontal) {
		if (isHorizontal)
			m_lay->setDirection(QBoxLayout::TopToBottom);
		else
			m_lay->setDirection(QBoxLayout::RightToLeft);
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------


void TpitchView::noteSlot(Tnote note) {
  Q_UNUSED(note)
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
    m_volMeter->setVolume(m_audioIN->maxPeak(), a);
    m_hideCnt++;
}


void TpitchView::pauseClicked() {
    if (m_isPaused) {
      pauseButt->setText("n"); // note symbol
      m_isPaused = false;
      m_intoView->setDisabled(false);
      m_audioIN->startListening();
			startVolume();
    } else {
      pauseButt->setText("o"); // stroked note symbol
      m_isPaused = true;
      m_audioIN->stopListening();
      stopVolume();
      m_volMeter->setDisabled(true);
      m_intoView->setDisabled(true);
    }

}


void TpitchView::voiceClicked() {
      setIsVoice(!m_isVoice); // switch to opposite
}


void TpitchView::minimalVolumeChanged(float minVol) {
		m_audioIN->setMinimalVolume(minVol);
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
   disconnect(m_audioIN, SIGNAL(chunkPitch(float)), m_intoView, SLOT(pitchSlot(float)));
   m_intoView->pitchSlot(0.0);
}

