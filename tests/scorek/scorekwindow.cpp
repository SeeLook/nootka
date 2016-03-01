/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "scorekwindow.h"
#include <tmultiscore.h>
#include <tscorescene.h>
#include <music/tmetrum.h>
#include <widgets/tpushbutton.h>
#include <QtWidgets/QtWidgets>


ScorekWindow::ScorekWindow(QWidget* parent) :
  QMainWindow(parent)
{
  setGeometry(100, 100, 1600, 768);
  m_score = new TmultiScore(this, this);
  auto statusLabel = new QLabel(this);
    statusLabel->setFixedHeight(fontMetrics().boundingRect("A").height() * 3);
    statusLabel->setWordWrap(true);
    statusLabel->setAlignment(Qt::AlignCenter);

  auto lay = new QVBoxLayout;
    lay->addWidget(m_score);
    lay->addWidget(statusLabel);
  auto centralW = new QWidget(this);
    centralW->setLayout(lay);

  setCentralWidget(centralW);

  m_score->setEnabledDblAccid(true);
  m_score->setEnableKeySign(true);
  m_score->scoreScene()->setRhythmEnabled(true);
  m_score->setTimeSignature(Tmetrum(Tmetrum::e_9_8));

  connect(m_score, &TmultiScore::statusTip, [=](const QString& s){ statusLabel->setText(s); });
  TpushButton::setCheckColor(palette().highlight().color(), palette().highlightedText().color());
}


void ScorekWindow::resizeEvent(QResizeEvent*) {
//   m_score->resize(contentsRect().size());
}

