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


#include "tselectinstrument.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <tinstrument.h>


TselectInstrument::TselectInstrument(TselectInstrument::Elayout buttonLayout, QWidget* parent) :
	QWidget(parent),
	m_gridLay(0),
	m_glypshSize(50)
{
	QButtonGroup *buttonGr = new QButtonGroup(this);
	for (int i = 0; i < BUTTON_COUNT; i++) {
		int glyphS = m_glypshSize;
		if (i == 0)
				glyphS = m_glypshSize - 5; // 'singer' glyph is a bit bigger than it should
		m_buttons[i] = new QPushButton(instrumentToGlyph(Einstrument(i)), this);
		m_buttons[i]->setStatusTip(instrumentToText(Einstrument(i)));
		m_buttons[i]->setFont(QFont("nootka", glyphS, QFont::Normal));
		m_buttons[i]->setCheckable(true);
		connect(m_buttons[i], SIGNAL(toggled()), this, SLOT(buttonPressed()));
		buttonGr->addButton(m_buttons[i]);
		m_labels[i] = new QLabel(this);
	}
	setLay(buttonLayout);	
}


void TselectInstrument::setLay(TselectInstrument::Elayout l) {
	if (m_gridLay) {
		for (int i = 0; i < BUTTON_COUNT; i++) {
			m_gridLay->removeWidget(m_buttons[i]);
			m_gridLay->removeWidget(m_labels[i]);
		}
		delete m_gridLay;
	}
	m_gridLay = new QGridLayout(this);
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
				m_gridLay->addWidget(m_buttons[i], 0, i);
				m_labels[i]->setText(instrumentToText(Einstrument(i)).replace(" ", "<br>"));
				m_labels[i]->show();
				m_gridLay->addWidget(m_labels[i], 1, i);
			}
			break;
		case e_buttonsOnlyGrid:
			for (int i = 0; i < BUTTON_COUNT; i++) {
				m_gridLay->addWidget(m_buttons[i], i / 2, i % 2);
				m_labels[i]->hide();
			}
			break;
	}
}


void TselectInstrument::buttonPressed() {
	int instr = 0;
	for (int i = 0; i < BUTTON_COUNT; i++)
		if (m_buttons[i]->isChecked()) {
				instr = i;
				break;
		}
	emit instrumentChanged(instr);
}







