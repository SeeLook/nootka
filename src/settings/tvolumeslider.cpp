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


#include "tvolumeslider.h"
#include <QVBoxLayout>
#include <QSlider>
#include <QSpinBox>

TvolumeSlider::TvolumeSlider(QWidget* parent) :
  QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout;
    m_slider = new QSlider(Qt::Horizontal, this);
    lay->addWidget(m_slider);
    m_spinBox = new QSpinBox(this);
    lay->addWidget(m_spinBox);
    setLayout(lay);
    
    m_spinBox->setMinimum(10);
    m_spinBox->setMaximum(80);
//     m_spinBox->setRange(0, 100);
    
//     m_slider->setMinimum(10);
//     m_slider->setMaximum(80);
    m_slider->setRange(0, 100);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setTickInterval(10);
    
    connect(m_slider, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
    connect(m_spinBox, SIGNAL(valueChanged(int)), m_slider, SLOT(setValue(int)));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderSlot(int)));
    m_slider->setValue(40);
}


void TvolumeSlider::setValue(int val) {
    m_slider->setValue(val);
}


int TvolumeSlider::value() {
    return m_slider->value();
}


void TvolumeSlider::sliderSlot(int val) {
  if (val > 80)
    m_slider->setValue(80);
  else if (val < 10)
    m_slider->setValue(10);
}
/*
void TvolumeSlider::spinSlot(int val)
{

}

*/

