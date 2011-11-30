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
#include <QGroupBox>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent): 
  QWidget(parent),
  m_audioIN(audioIn)
{
  QHBoxLayout *lay = new QHBoxLayout();
  m_volMeter = new TvolumeMeter(this);
  lay->addWidget(m_volMeter);
  QVBoxLayout *statLay = new QVBoxLayout();
  m_stateLabel = new QLabel(this);
  statLay->addWidget(m_stateLabel);
  QGroupBox *gr = new QGroupBox(this);
  gr->setLayout(statLay);
  m_stateLabel->setFixedWidth(20);
  m_stateLabel->setFont(QFont("nootka", 15));
  m_stateLabel->setStatusTip(tr("state of pitch detection."));
//   m_stateLabel->setStyleSheet(QString("border-radius: 6px; background-color: palette(Highlight); color: %1;").
// 		  arg(palette().highlightedText().color().name()));
  lay->addWidget(gr);
  setLayout(lay);
  
  
  m_volTimer = new QTimer(this);
  connect(m_volTimer, SIGNAL(timeout()), this, SLOT(updateLevel()));
}


TpitchView::~TpitchView()
{
}

void TpitchView::startVolume() {
	connect(m_audioIN, SIGNAL(noteDetected(Tnote)), this, SLOT(noteSlot(Tnote)));
	m_volTimer->start(75);
}

void TpitchView::stopVolume() {
	m_volTimer->stop();
	m_volMeter->setVolume(0.0);
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

int hideCnt = 0; // counter of m_volTimer loops. After 7 loop text is hidden (7 * 75ms = 525 ms)

void TpitchView::noteSlot(Tnote note) {
  Q_UNUSED(note)
  m_stateLabel->setText("n");
  hideCnt = 0;
}

void TpitchView::updateLevel() {
	m_volMeter->setVolume(qreal(m_audioIN->maxPeak()) / 32768.0);
	hideCnt++;
	if (hideCnt == 7)
	  m_stateLabel->setText("");
}



