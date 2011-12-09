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
#include "texamview.h"

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
  QVBoxLayout *timeLay = new QVBoxLayout();
  QGroupBox *timeGr = new QGroupBox(tr("times:"), this);
  QLabel *timeLab = new QLabel(
    TexamView::totalTimetxt() + ": <b>" + TexamView::formatedTotalTime(exam->totalTime()*1000) +
    "</b><br>" + tr("Time of work") + ": <b>" + TexamView::formatedTotalTime(exam->workTime()*100) +
    "</b><br>" + TexamView::averAnsverTimeTxt() + ": <b>" +
    QString("%1 s").arg(exam->averageReactonTime()/ 10, 0, 'f', 1, '0') + "</b>"
    , this);
  timeLay->addWidget(timeLab);
  
  timeGr->setLayout(timeLay);
  leftLay->addWidget(timeGr);
	
  
  QPushButton *okButt = new QPushButton(tr("Close"), this);
  leftLay->addWidget(okButt);
  
	lay->addLayout(leftLay);
//-------  right layout -----------------------	
	QVBoxLayout *rightLay = new QVBoxLayout();
	TlevelSummaryWdg *levelWdg = new TlevelSummaryWdg(this);
	rightLay->addWidget(levelWdg);
	levelWdg->setLevel(*(exam->level()));
	QVBoxLayout *resLay = new QVBoxLayout();
	QGroupBox *resGr = new QGroupBox(tr("Results:"), this);
	qreal eff = (((qreal)exam->count() - (qreal)exam->mistakes()) / (qreal)exam->count()) * 100;
	QLabel *resLab = new QLabel(TexamView::effectTxt() + QString(": <b>%1%</b><br>").arg(qRound(eff)) +
		TexamView::mistakesNrTxt() + QString(": <b>%1</b>").arg(exam->mistakes()), this);
	resLay->addWidget(resLab);
	
	resGr->setLayout(resLay);
	rightLay->addWidget(resGr);
	
	lay->addLayout(rightLay);
	
  setLayout(lay);
  
  connect(okButt, SIGNAL(clicked()), this, SLOT(accept()));
  
}

TexamSummary::~TexamSummary()
{

}

