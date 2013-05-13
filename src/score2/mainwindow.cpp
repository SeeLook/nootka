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
#include <qstatusbar.h>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  m_score = new TsimpleScore(this);
  setCentralWidget(m_score);
  
  m_bar = statusBar();
  setStatusBar(m_bar);
  
  m_bar->showMessage("ready");
  
  connect(m_score, SIGNAL(statusTip(QString)), this, SLOT(updateStatusTip(QString)));
  
  resize(600, 480);
}

void MainWindow::updateStatusTip(QString status) {
  m_bar->showMessage(status);
}




