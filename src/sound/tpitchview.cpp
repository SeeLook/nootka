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


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent): 
  QWidget(parent),
  m_audioIN(audioIn),
  m_pitchColor(Qt::red),
  m_isPaused(false)
{
  QHBoxLayout *lay = new QHBoxLayout();
  voiceButt = new QPushButton("I", this);
  voiceButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  lay->addWidget(voiceButt);
  voiceButt->setStatusTip(tr("Toggles between pitch detection for human voice and for instruments"));
  pauseButt = new QPushButton("n", this);
  pauseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  lay->addWidget(pauseButt);
  pauseButt->setStatusTip(tr("Switch on/off the pitch detection"));
  pauseButt->setFont(QFont("nootka"));
  
  m_volMeter = new TvolumeMeter(this);
  lay->addWidget(m_volMeter);
  m_volMeter->setStatusTip(tr("Shows volume level of input sound and indicates when note was detected"));
  
  setLayout(lay);  
  
  m_volTimer = new QTimer(this);
  connect(m_volTimer, SIGNAL(timeout()), this, SLOT(updateLevel()));
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

void TpitchView::resize() {
  //TODO font size on buttons
  m_volMeter->setFixedHeight(height()-4);
  m_volMeter->resize();
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

int hideCnt = 8; // counter of m_volTimer loops. After 7 loop text is hidden (7 * 75ms = 525 ms)

void TpitchView::noteSlot(Tnote note) {
  Q_UNUSED(note)
  hideCnt = 0;
}

void TpitchView::updateLevel() {
  int a = 0;
  if (hideCnt < 8)
    switch (hideCnt) {
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
    hideCnt++;
}

void TpitchView::pauseClicked() {
  

}

void TpitchView::voiceClicked() {


}



