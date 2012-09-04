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
  m_bar->setValue(0);
  lay->addWidget(m_bar);
  m_totalLab = new QLabel(zeroLabTxt(), this);
  lay->addWidget(m_totalLab);
  
  setLayout(lay);
  
  setStatusTip(progressExamTxt());
  setDisabled(true);
}

TprogressWidget::~TprogressWidget()
{}

void TprogressWidget::activate(int answers, int total, int penaltys) {
  setDisabled(false);
  m_totalNr = total;
  m_answersNr = answers;
  updateLabels(penaltys);
}

void TprogressWidget::progress(int penaltys) {
  m_answersNr++;
  updateLabels(penaltys);

}

void TprogressWidget::updateLabels(int penaltys) {
  int remained = qMax(0, m_totalNr + penaltys - m_answersNr);
  m_answLab->setText(QString("%1 + %2").arg(m_answersNr).arg(remained));
  m_answLab->setStatusTip(tr("Answered questions") + QString(": %1").arg(m_answersNr) +
    "<br>" + tr("Remained") + QString(": %1 ").arg(remained)
  );
  m_totalLab->setText(QString(" %1 (%2)").arg(m_totalNr + penaltys).arg(penaltys));
  m_bar->setMinimum(0);
  m_bar->setMaximum(m_totalNr + penaltys);
  if (remained)
    m_bar->setValue(m_answersNr);
  else
    m_bar->setValue(m_totalNr + penaltys);
}


void TprogressWidget::terminate() {
  m_answLab->setText(zeroLabTxt());
  m_totalLab->setText(zeroLabTxt());
  m_bar->reset();
  setDisabled(true);
}

void TprogressWidget::resize(int fontSize) {

}

QString TprogressWidget::zeroLabTxt() {
  return "<span style=\"color: transparent;\">(000) + </span>0<span style=\"color: transparent;\">000</span>";
}



