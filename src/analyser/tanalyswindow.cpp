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


#include <QtGui>
#include "tanalyswindow.h"
#include "tchart.h"
// #include "texam.h"
//#include "texamlevel.h"

TanalysWindow::TanalysWindow() :
    m_exam(0)
//     m_level(0)
{
  
  QVBoxLayout *mainLay = new QVBoxLayout;
  QWidget *widget = new QWidget(this);
  
  m_toolBar = new QToolBar("", widget);
  mainLay->addWidget(m_toolBar);

  QGridLayout *headLay = new QGridLayout;
  headLay->addWidget(new QLabel(tr("analyse type:"), this), 0, 0);
  headLay->addWidget(new QLabel(tr("user name:"), this), 0, 1);
  headLay->addWidget(new QLabel(tr("level:"), this), 0, 2);
  m_chartListComo = new QComboBox(this);
  headLay->addWidget(m_chartListComo, 1, 0);
  m_userLab = new QLabel("XX", this);
  headLay->addWidget(m_userLab, 1, 1);
  m_levelLab = new QLabel("YY", this);
  headLay->addWidget(m_levelLab, 1, 2);

  mainLay->addLayout(headLay);
  Tchart *ch = new Tchart(this);
  mainLay->addWidget(ch);
  
  widget->setLayout(mainLay);
  setCentralWidget(widget);
  
  createActions();

}

TanalysWindow::~TanalysWindow()
{}



//##########  PUBLIC METHODS #####################
void TanalysWindow::loadExam(QString &examFile) {
//     if (!m_exam) {
//         m_exam = new Texam(m_level, "");
//     }
//     m_exam->loadFromFile(examFile);
//     m_userLab->setText(m_exam->userName());
//    m_levelLab->setText(m_exam->level()->name);
}

//##########  PRIVATE METHODS #####################

void TanalysWindow::createActions() {
  m_openExamAct = new QAction(QIcon(style()->standardIcon(QStyle::SP_DirOpenIcon)), tr("Open an exam"), this);

  m_closeAct = new QAction(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)), tr("Close analyser"), this);
  connect(m_closeAct, SIGNAL(triggered()), this, SLOT(close()));
  
  m_toolBar->addAction(m_openExamAct);
  m_toolBar->addAction(m_closeAct);
}


//##########  SLOTS #####################

void TanalysWindow::loadExamSlot() {

}



