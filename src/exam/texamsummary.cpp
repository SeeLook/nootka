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
#include "tqaunit.h"

  /** returns 2 columns row of table */
QString row2(QString S1, QString S2) {
  return QString("<tr><td>%1: </td><td><b>%2</b></td></tr>").arg(S1).arg(S2);
}


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
  QLabel *timeLab = new QLabel("<table>" + 
    row2(TexamView::totalTimetxt(), TexamView::formatedTotalTime(exam->totalTime()*1000)) +
    row2(tr("Time of work"), TexamView::formatedTotalTime(exam->workTime()*100)) +
    row2(TexamView::averAnsverTimeTxt(), QString("%1 s").arg(exam->averageReactonTime()/ 10, 0, 'f', 1, '0')) +
    "</table>", this);
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
  QString effStr = "";
  if (exam->mistakes()) {
    effStr = row2(TexamView::mistakesNrTxt(), QString::number(exam->mistakes()));
    int wAccid = 0, wKey = 0, wNote = 0, wOctave = 0, wStyle = 0, wPos = 0;
    for(int i=0; i<exam->count(); i++) {
      if (!exam->q(i).correct()) {
        if(exam->q(i).wrongAccid()) wAccid++;
        if(exam->q(i).wrongKey()) wKey++;
        if(exam->q(i).wrongNote()) wNote++;
        if(exam->q(i).wrongOctave()) wOctave++;
        if(exam->q(i).wrongStyle()) wStyle++;
        if(exam->q(i).wrongPos()) wPos++;
      }
    }
    if (wNote)
      effStr += row2("Wrong notes", QString::number(wNote));
    if (wAccid)
      effStr += row2("Wrong accidentals", QString::number(wAccid));
    if (wKey)
      effStr += row2("Wrong key signatures", QString::number(wKey));
    if (wPos)
      effStr += row2("Wrong position", QString::number(wPos));
  }
	QLabel *resLab = new QLabel("<table>" +
    row2(TexamView::effectTxt(), QString::number(qRound(eff)) + "%") +
    effStr +
    "</table>", this);
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

