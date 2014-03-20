/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tscorecontrol.h"
#include "tscorescene.h"
#include <widgets/tpushbutton.h>
#include <QVBoxLayout>

TscoreControl::TscoreControl(QWidget* parent):
  QWidget(parent)
{    
    m_dblSharpBut = new TpushButton("x", this);
    setButtons(m_dblSharpBut);
		m_dblSharpBut->setStatusTip(tr("<b>double sharp</b> - raises a note by two semitones (whole tone).<br>On the guitar it is two frets up."));
    m_sharpBut = new TpushButton("#", this);
    setButtons(m_sharpBut);
		m_sharpBut->setStatusTip(tr("<b>sharp</b> - raises a note by a half tone (semitone).<br>On the guitar it is one fret up."));
    m_flatBut = new TpushButton("b", this);
    setButtons(m_flatBut);
		m_flatBut->setStatusTip(tr("<b>flat</b> - lowers a note by a half tone (semitone).<br>On the guitar it is one fret down."));
    m_dblFlatBut = new TpushButton("B", this);
    setButtons(m_dblFlatBut);
		m_dblFlatBut->setStatusTip(tr("<b>double flat</b> - lowers a note by two semitones (whole tone).<br>On the guitar it is two frets down."));
    QVBoxLayout *butLay = new QVBoxLayout;
    butLay->addStretch(1);
    butLay->addWidget(m_dblSharpBut);
    butLay->addWidget(m_sharpBut);
    butLay->addSpacing(5);
    butLay->addWidget(m_flatBut);
    butLay->addWidget(m_dblFlatBut);
    butLay->addStretch(1);
    setLayout(butLay);
    m_dblFlatBut->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_flatBut->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_sharpBut->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_dblSharpBut->setAttribute(Qt::WA_LayoutUsesWidgetRect);

		
		connect(m_dblFlatBut, SIGNAL(clicked()), this, SLOT(onAcidButtonPressed()));
		connect(m_flatBut, SIGNAL(clicked()), this, SLOT(onAcidButtonPressed()));
		connect(m_sharpBut, SIGNAL(clicked()), this, SLOT(onAcidButtonPressed()));
		connect(m_dblSharpBut, SIGNAL(clicked()), this, SLOT(onAcidButtonPressed()));
    
}


void TscoreControl::setAccidental(int accNr) {
		m_dblSharpBut->setChecked(false);
    m_sharpBut->setChecked(false);
    m_flatBut->setChecked(false);
    m_dblFlatBut->setChecked(false);
    switch (accNr) {
				case -2:
					m_dblFlatBut->setChecked(true); break;
				case -1: 
					m_flatBut->setChecked(true); break;
				case 1: 
					m_sharpBut->setChecked(true); break;
				case 2: 
					m_dblSharpBut->setChecked(true); break;
		}
		emit accidButtonPressed(accNr);
}


void TscoreControl::enableDoubleAccidentals(bool isEnabled) {
		if (isEnabled) {
				m_dblFlatBut->show();
				m_dblSharpBut->show();
		} else {
			m_dblFlatBut->hide();
			m_dblFlatBut->setChecked(false);
			m_dblSharpBut->hide();
			m_dblSharpBut->setChecked(false);
		}
}



//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreControl::onAcidButtonPressed() {
		if (sender() != m_sharpBut)
			m_sharpBut->setChecked(false);
		if (sender() != m_dblSharpBut)
			m_dblSharpBut->setChecked(false);
		if (sender() != m_dblFlatBut)
			m_dblFlatBut->setChecked(false);
		if (sender() != m_flatBut)
			m_flatBut->setChecked(false);
		int ac;
		TpushButton *button = static_cast<TpushButton *>(sender());
		button->setChecked(!button->isChecked());
			if (sender() == m_flatBut) {
					if (m_flatBut->isChecked()) 
						ac = -1;
					else 
						ac = 0;
			} else if (sender() == m_sharpBut) {
								if (m_sharpBut->isChecked()) 
									ac = 1;
								else
									ac = 0;
				} else if (sender() == m_dblFlatBut)   {
									if (m_dblFlatBut->isChecked()) 
										ac = -2;
									else 
										ac = 0;
							} else if (m_dblSharpBut->isChecked()) 
										ac = 2;
									else 
										ac = 0;
		emit accidButtonPressed(ac);
}



void TscoreControl::setButtons(TpushButton* button) {
    button->setFixedSize(40, 45);
    QFont nf("nootka");
    nf.setPointSizeF(25.0);
    QFontMetrics fm(nf);
    nf.setPointSizeF(qRound(nf.pointSizeF() * (nf.pointSizeF() / fm.boundingRect("x").height())));
    button->setFont(nf);
}

