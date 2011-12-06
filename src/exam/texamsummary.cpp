/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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


#include "texamsummary.h"
#include <QtGui>
#include "texam.h"
#include "tlevelselector.h"

TexamSummary::TexamSummary(Texam* exam, QWidget* parent) :
  QDialog(parent)
{
  QHBoxLayout *lay = new QHBoxLayout();
//-------  left layout -----------------------
	QVBoxLayout *leftLay = new QVBoxLayout();
	
	QLabel *studentLab = new QLabel(tr("student:"), this);
	leftLay->addWidget(studentLab);
  QLabel *userNameLab = new QLabel(QString("<b style=\"font-size: 20px\">%1</b>").arg(exam->userName()), this);
  leftLay->addWidget(userNameLab, 0, Qt::AlignCenter);
	QLabel *questNrLAb = new QLabel(tr("Questions number:  ") + 
											QString("<b style=\"font-size: 20px\">%1</b>").arg(exam->count()), this);
	leftLay->addWidget(questNrLAb);
	
  
  QPushButton *okButt = new QPushButton(tr("OK"), this);
  leftLay->addWidget(okButt);
  
	lay->addLayout(leftLay);
//-------  right layout -----------------------	
	QVBoxLayout *rightLay = new QVBoxLayout();
	TlevelSummaryWdg *levelWdg = new TlevelSummaryWdg(this);
	rightLay->addWidget(levelWdg);
	levelWdg->setLevel(*(exam->level()));
	
	lay->addLayout(rightLay);
	
  setLayout(lay);
  
  connect(okButt, SIGNAL(clicked()), this, SLOT(accept()));
  
}

TexamSummary::~TexamSummary()
{

}

