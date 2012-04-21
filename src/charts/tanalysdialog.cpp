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
#include "tanalysdialog.h"
#include "texam.h"
#include "texamlevel.h"
#include "tstartexamdlg.h"
#include "tbasechart.h"
#include "tmainchart.h"

TanalysDialog::TanalysDialog(QWidget *parent) :
    QDialog(parent),
    m_exam(0),
    m_level(new TexamLevel()),
    m_chart(0)
{
 
  setWindowTitle(tr("Analyse of the results"));
  QVBoxLayout *lay = new QVBoxLayout;
  
  lay = new QVBoxLayout;
  
  m_toolBar = new QToolBar("", this);
  lay->addWidget(m_toolBar);

  QGridLayout *headLay = new QGridLayout;
  headLay->addWidget(new QLabel(tr("analyse type:"), this), 0, 0);
  headLay->addWidget(new QLabel(tr("user name:"), this), 0, 1);
  headLay->addWidget(new QLabel(tr("level:"), this), 0, 2);
  m_chartListComo = new QComboBox(this);
  m_chartListComo->addItem(tr("main analyse"));
  headLay->addWidget(m_chartListComo, 1, 0);
  m_userLab = new QLabel(" ", this);
  headLay->addWidget(m_userLab, 1, 1);
  m_levelLab = new QLabel(" ", this);
  headLay->addWidget(m_levelLab, 1, 2);

  lay->addLayout(headLay);

//   m_chart = new TbaseChart(this);
//   lay->addWidget(m_chart);
  m_plotLay = new QVBoxLayout;
//   m_plotLay->setS
  lay->addLayout(m_plotLay);
  
  setLayout(lay);
  
  createActions();

}

TanalysDialog::~TanalysDialog()
{}



//##########  PUBLIC METHODS #####################
void TanalysDialog::loadExam(QString& examFile) {

    if (!m_exam) {
        m_exam = new Texam(m_level, "");
    }
    m_exam->loadFromFile(examFile);
    m_userLab->setText(m_exam->userName());
    m_levelLab->setText(m_exam->level()->name);
	
	if (m_chart) {
		m_plotLay->removeWidget(m_chart);
		delete m_chart;
	}
	m_chart = new TmainChart(m_exam, this);
	m_plotLay->addWidget(m_chart);
    

}

//##########  PRIVATE METHODS #####################

void TanalysDialog::createActions() {
  
  m_openExamAct = new QAction(QIcon(style()->standardIcon(QStyle::SP_DirOpenIcon)), tr("Open an exam"), this);
  connect(m_openExamAct, SIGNAL(triggered()), this, SLOT(loadExamSlot()));

  m_closeAct = new QAction(QIcon(style()->standardIcon(QStyle::SP_DialogCloseButton)), tr("Close analyser"), this);
  connect(m_closeAct, SIGNAL(triggered()), this, SLOT(close()));
  
  m_toolBar->addAction(m_openExamAct);
  m_toolBar->addAction(m_closeAct);
}


//##########  SLOTS #####################

void TanalysDialog::loadExamSlot() {
  
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load an exam's' file"),
                               QDir::homePath(), TstartExamDlg::examFilterTxt());
  loadExam(fileName);

}



