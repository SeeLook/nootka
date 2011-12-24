/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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
#include "tvolumemeter.h"
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent, bool withButtons): 
  QWidget(parent),
  m_audioIN(audioIn),
  m_pitchColor(Qt::red),
  m_isPaused(false),
  m_isVoice(false),
  m_hideCnt(8),
  m_withButtons(withButtons)
{
  QHBoxLayout *lay = new QHBoxLayout();
  if (m_withButtons) {
      voiceButt = new QPushButton("g", this);
      voiceButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      lay->addWidget(voiceButt);
      voiceButt->setStatusTip(tr("Toggles between pitch detection for singing and for playing"));
      voiceButt->setFont(QFont("nootka", 15));
  } else {
    voiceButt = 0;
    pauseButt = 0;
  }
  m_volMeter = new TvolumeMeter(this);
  lay->addWidget(m_volMeter);
  m_volMeter->setStatusTip(tr("Shows volume level of input sound and indicates when note was detected"));
  
  if (m_withButtons) {
      pauseButt = new QPushButton("n", this);
      pauseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      lay->addWidget(pauseButt);
      pauseButt->setStatusTip(tr("Switch on/off the pitch detection"));
      pauseButt->setFont(QFont("nootka", 15));
  }
  setLayout(lay);  
  
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

void TpitchView::startVolume() {
  if (m_audioIN) {
    connect(m_audioIN, SIGNAL(noteDetected(Tnote)), this, SLOT(noteSlot(Tnote)));
    m_volTimer->start(75);
  }
}

void TpitchView::stopVolume() {
	m_volTimer->stop();
	m_volMeter->setVolume(0.0);
}

void TpitchView::setPitchColor(QColor col) {
  m_pitchColor = col;
  m_volMeter->setPitchColor(col);
}

void TpitchView::resize(int fontSize) {
  if (m_withButtons) {
    voiceButt->setFont(QFont("nootka", fontSize));
    voiceButt->setFixedWidth(3*fontSize/2);
    pauseButt->setFont(QFont("nootka", fontSize));
    pauseButt->setFixedWidth(3*fontSize/2);
  }
  m_volMeter->setFixedHeight(qRound((float)height()*0.70));
}

void TpitchView::setIsVoice(bool isVoice) {
  if (isVoice) {
      voiceButt->setText("v"); // singer symbol for voice mode
      m_isVoice = true;
    } else {
      voiceButt->setText("g"); // guitar symbol for instruments mode
      m_isVoice = false;
    }
    m_audioIN->setIsVoice(m_isVoice);
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
    m_volMeter->setVolume(qreal(m_audioIN->maxPeak()) / 32768.0, a);
    m_hideCnt++;
}

void TpitchView::pauseClicked() {
    if (m_isPaused) {
      pauseButt->setText("n");
      m_isPaused = false;
      m_volMeter->setDisabled(false);
      m_audioIN->startListening();
      m_volTimer->start(75);
    } else {
      pauseButt->setText("o");
      m_isPaused = true;
      m_audioIN->stopListening();
      stopVolume();
      m_volMeter->setDisabled(true);
    }

}

void TpitchView::voiceClicked() {
      setIsVoice(!m_isVoice); // switch to opposite
}



