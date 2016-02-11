/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#include "tselectinstrument.h"
#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <music/tinstrument.h>
#include <widgets/tpushbutton.h>


TselectInstrument::TselectInstrument(QWidget* parent, TselectInstrument::Elayout buttonLayout) :
	QWidget(parent),
	m_gridLay(0),
	m_header(0)
{
	m_mainLay = new QVBoxLayout;
	m_mainLay->setAlignment(Qt::AlignCenter);
	setLayout(m_mainLay);
	for (int i = 0; i < BUTTON_COUNT; i++) {
		m_buttons[i] = new TpushButton(instrumentToGlyph(Einstrument(i)), this);
		m_buttons[i]->setStatusTip(instrumentToText(Einstrument(i)));
		m_buttons[i]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(m_buttons[i], SIGNAL(pressed()), this, SLOT(buttonPressed()));
		m_labels[i] = new QLabel(this);
		m_labels[i]->setAlignment(Qt::AlignCenter);
	}
	setGlyphSize(50);
	setButtonLayout(buttonLayout);	
}


void TselectInstrument::setInstrument(int instr) {
	if (instr >= 0 && instr <= BUTTON_COUNT)
			m_buttons[instr]->click();
}


void TselectInstrument::setButtonLayout(TselectInstrument::Elayout l) {
	if (m_gridLay) {
		for (int i = 0; i < BUTTON_COUNT; i++) {
			m_gridLay->removeWidget(m_buttons[i]);
			m_gridLay->removeWidget(m_labels[i]);
		}
		delete m_gridLay;
	}
	m_gridLay = new QGridLayout;
	m_mainLay->addLayout(m_gridLay, 0);
	switch (l) {
		case e_buttonAndTextV:
			for (int i = 0; i < BUTTON_COUNT; i++) {
				m_gridLay->addWidget(m_buttons[i], i, 0);
				m_labels[i]->setText(instrumentToText(Einstrument(i)));
				m_labels[i]->show();
				m_gridLay->addWidget(m_labels[i], i, 1);
			}
			break;
		case e_textUnderButtonH:
			for (int i = 0; i < BUTTON_COUNT; i++) {
				m_gridLay->addWidget(m_buttons[i], 0, i, Qt::AlignCenter);
				m_labels[i]->setText(instrumentToText(Einstrument(i)).replace(" ", "<br>"));
				m_labels[i]->show();
				m_gridLay->addWidget(m_labels[i], 1, i, Qt::AlignCenter);
			}
			break;
		case e_buttonsOnlyGrid:
			for (int i = 0; i < BUTTON_COUNT; i++) {
				m_gridLay->addWidget(m_buttons[i], i / 2, i % 2);
				m_labels[i]->hide();
			}
			break;
    case e_buttonsOnlyHorizontal:
      for (int i = 0; i < BUTTON_COUNT; i++) {
        m_gridLay->addWidget(m_buttons[i], 0, i);
        m_labels[i]->hide();
      }
      break;
	}
}


void TselectInstrument::setGlyphSize(int siz) {
	m_glypshSize = siz;
	int glyphS = m_glypshSize;
	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (i == 0)
				glyphS = m_glypshSize - 10; // 'singer' glyph is a bit bigger than it should
		m_buttons[i]->setFont(QFont("nootka", glyphS, QFont::Normal));
	}
}


void TselectInstrument::setHeadLabel(QString text) {
	if (text != "") {
		if (!m_header) {
			m_header = new QLabel(text, this);
			m_header->setAlignment(Qt::AlignCenter);
			m_mainLay->insertWidget(0, m_header);
		} else
			m_header->setText(text);			
	} else {
		if (m_header) {
			delete m_header;
			m_header = 0;
		}
	}
}


void TselectInstrument::buttonPressed() {
	int instr = 0;
	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (sender() == m_buttons[i]) {
				m_buttons[i]->setChecked(true);
				instr = i;
		} else
				m_buttons[i]->setChecked(false);
	}
	m_instr = instr;
	emit instrumentChanged(instr);
}







