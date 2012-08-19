/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#include "tprogresswidget.h"
#include <QtGui>

TprogressWidget::TprogressWidget(QWidget* parent) :
  QWidget(parent),
  m_totalNr(0),
  m_answersNr(0)
{
  QHBoxLayout *lay = new QHBoxLayout;
  m_answLab = new QLabel(zeroLabTxt(), this);
  lay->addWidget(m_answLab);
  m_bar = new QProgressBar(this);
  lay->addWidget(m_bar);
  m_totalLab = new QLabel(zeroLabTxt(), this);
  lay->addWidget(m_totalLab);
  
  setLayout(lay);
  
  setStatusTip(progressExamTxt());
  setDisabled(true);
}

TprogressWidget::~TprogressWidget()
{}

void TprogressWidget::activate(int answers, int total) {
  setDisabled(false);
}

void TprogressWidget::progress(int total) {

}

void TprogressWidget::terminate() {
  setDisabled(true);

}

void TprogressWidget::resize(int fontSize) {

}

QString TprogressWidget::zeroLabTxt() {
  return "0<span style=\"color: transparent;\">000</span>";
}



