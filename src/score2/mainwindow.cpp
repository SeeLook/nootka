/*
 *                                                                        *
 * Copyright (C) 2013 by Tomasz Bojczuk                                   *
 * tomaszbojczuk@gmail.com                                                *
 *                                                                        *
 * This program is free software; you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation; either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                        *
 * 
 */

#include "mainwindow.h"
#include "score/tsimplescore.h"
#include <tmainscore.h>
#include <tpushbutton.h>
#include <qstatusbar.h>
#include <QHBoxLayout>
#include <qcheckbox.h>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *lay = new QVBoxLayout;
	
// 	m_mainScore = new TmainScore(w);
// 	lay->addWidget(m_mainScore);
  m_simpleScore = new TsimpleScore(w);
  lay->addWidget(m_simpleScore);
	m_simpleScore->setPianoStaff(true);
	
	QHBoxLayout *settLay = new QHBoxLayout;
	settLay->addStretch();
	m_keyBox = new QCheckBox("key", w);
	settLay->addWidget(m_keyBox);
	settLay->addStretch();
	m_pianBox = new QCheckBox("piano staff", w);
	settLay->addWidget(m_pianBox);
	settLay->addStretch();
	lay->addLayout(settLay);	
	
	w->setLayout(lay);
	
  TpushButton::setCheckColor(palette().highlight().color(), palette().highlightedText().color());
	
	setCentralWidget(w);
	
  m_bar = statusBar();
  setStatusBar(m_bar);
  
  m_bar->showMessage("ready");
	
	connect(m_keyBox, SIGNAL(toggled(bool)), this, SLOT(keySignBoxChanged(bool)));
  
// 	connect(m_mainScore, SIGNAL(statusTip(QString)), this, SLOT(updateStatusTip(QString)));
  connect(m_simpleScore, SIGNAL(statusTip(QString)), this, SLOT(updateStatusTip(QString)));
  
  resize(600, 480);
}

void MainWindow::updateStatusTip(QString status) {
  m_bar->showMessage(status);
}


void MainWindow::keySignBoxChanged(bool enable) {
	m_simpleScore->setEnableKeySign(m_keyBox->isChecked());
}

void MainWindow::pianoBoxChanged(bool enable)
{

}





