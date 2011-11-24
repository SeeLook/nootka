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
#include "../tvolumemeter.h"
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent): 
  QWidget(parent),
  m_audioIN(audioIn)
{
  QHBoxLayout *lay = new QHBoxLayout();
  m_volMeter = new TvolumeMeter(this);
  lay->addWidget(m_volMeter);
  m_stateLabel = new QLabel(this);
  lay->addWidget(m_stateLabel);
  setLayout(lay);
  
  
  m_volTimer = new QTimer(this);
//   connect(m_audioIN, SIGNAL(stateChanged(TaudioIN::Estate)), this, SLOT(pitchState(TaudioIN::Estate)));
  connect(m_volTimer, SIGNAL(timeout()), this, SLOT(updateLevel()));
}


TpitchView::~TpitchView()
{
}

void TpitchView::startVolume() {
	connect(m_audioIN, SIGNAL(stateChanged(TaudioIN::Estate)), this, SLOT(pitchState(TaudioIN::Estate)));
	m_volTimer->start(75);
}

void TpitchView::stopVolume() {
	m_volTimer->stop();
	m_volMeter->setVolume(0.0);
}




//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

void TpitchView::pitchState(TaudioIN::Estate state) {
  switch (state) {
	case TaudioIN::e_disabled :
	  break;
	case TaudioIN::e_paused :
	  m_stateLabel->setText("");
	  m_volTimer->stop();
	  break;
	case TaudioIN::e_ready :
	  m_stateLabel->setText("<span style\"font-family: nootka; color: grey; \">n</span>");
	  break;
	case TaudioIN::e_noteStarted :
	  m_stateLabel->setText("<span style\"font-family: nootka; color: red; \">n</span>");
	  break;
  }

}

void TpitchView::noteSlot(Tnote note) {
  m_stateLabel->setText("<span style\"font-family: nootka; color: green; \">n</span>");
}


void TpitchView::updateLevel() {
	m_volMeter->setVolume(qreal(m_audioIN->maxPeak()) / 32768.0);
}



