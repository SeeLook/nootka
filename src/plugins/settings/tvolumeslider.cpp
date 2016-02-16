/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
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
    m_spinBox->setSuffix(" %");
    m_spinBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
//     m_spinBox->setRange(0, 100);
    
//     m_slider->setMinimum(10);
//     m_slider->setMaximum(80);
    m_slider->setRange(0, 100);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setTickInterval(10);
//     m_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    
    connect(m_slider, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
    connect(m_spinBox, SIGNAL(valueChanged(int)), m_slider, SLOT(setValue(int)));
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderSlot(int)));
    m_slider->setValue(40);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
}


void TvolumeSlider::setValue(float val) {
    m_slider->setValue(val * 100);
}


float TvolumeSlider::value() {
    return (float)m_spinBox->value() / 100.0f;
}


void TvolumeSlider::sliderSlot(int val) {
  if (val > 80)
    m_slider->setValue(80);
  else if (val < 10)
    m_slider->setValue(10);
  emit valueChanged(value());
}
/*
void TvolumeSlider::spinSlot(int val)
{

}

*/

