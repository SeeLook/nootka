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

#include "tpixmapradio.h"
#include <QLabel>
#include "troundedlabel.h"
#include <QRadioButton>


TpixmapRadio::TpixmapRadio(const QString& imgHtml, const QString& label, QWidget* parent) :
	QWidget(parent),
	m_oreder(e_radioLabelPix) // not default just fake
{
	m_label = new QLabel(label, this);
	m_pixLabel = new TroundedLabel(imgHtml, this);
	m_radio = new QRadioButton(this);
	
	m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	setLayout(m_layout);
	
	setOrder(e_pixRadioLabel); // here the order is set to default
	
	connect(m_radio, SIGNAL(toggled(bool)), this, SLOT(radioCheched()));
}


void TpixmapRadio::setOrder(TpixmapRadio::Eorder order) {
	if (order != m_oreder) {
		m_oreder = order;
		if (m_layout->count()) {
			m_layout->removeWidget(m_label);
			m_layout->removeWidget(m_radio);
			m_layout->removeWidget(m_pixLabel);
		}
		switch(m_oreder) {
			case e_labelRadioPix:
				m_layout->addWidget(m_label, 0, Qt::AlignCenter);
				m_layout->addWidget(m_radio, 0, Qt::AlignCenter);
				m_layout->addWidget(m_pixLabel, 0, Qt::AlignCenter);
				break;
			case e_pixRadioLabel:
				m_layout->addWidget(m_pixLabel, 0, Qt::AlignCenter);
				m_layout->addWidget(m_radio, 0, Qt::AlignCenter);
				m_layout->addWidget(m_label, 0, Qt::AlignCenter);
				break;
			case e_radioLabelPix:
				m_layout->addWidget(m_radio, 0, Qt::AlignCenter);
				m_layout->addWidget(m_label, 0, Qt::AlignCenter);
				m_layout->addWidget(m_pixLabel, 0, Qt::AlignCenter);
				break;
		}
	}
}


void TpixmapRadio::radioCheched() {
	if (m_radio->isChecked())
		m_pixLabel->setDisabled(false);
	else
		m_pixLabel->setDisabled(true);
}




