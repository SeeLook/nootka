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
// #include "../exam/texam.h"

TanalysWindow::TanalysWindow(Texam* exam, QWidget* parent, Qt::WindowFlags flags): 
	QMainWindow(parent, flags),
	m_exam(exam)
{
  
  QVBoxLayout *mainLay = new QVBoxLayout;
  QWidget *widget = new QWidget(this);
  
  m_toolBar = new QToolBar("", widget);
  mainLay->addWidget(m_toolBar);
  
  
  widget->setLayout(mainLay);
  setCentralWidget(widget);
  
  createActions();

}


TanalysWindow::~TanalysWindow()
{}

void TanalysWindow::createActions() {
  m_openExamAct = new QAction(QIcon(style()->standardIcon(QStyle::SP_DirOpenIcon)), tr("Open an exam"), this);
  
  m_toolBar->addAction(m_openExamAct);
}





