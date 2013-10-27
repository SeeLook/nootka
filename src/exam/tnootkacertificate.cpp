/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tnootkacertificate.h"
#include "texam.h"
#include "texamview.h"
#include <texamlevel.h>
#include <QDate>


QString spanPx = "<span style=\"font-size: 20px;\">";

QString finishExamText(Texam* exam) {
// 		QString txt = "<div>" +  //"<div style=\"padding-left: 10px; background-color: gray;\">" +
		QString txt = "<div><p style=\"text-align: right;\">" + QString("date: %1").arg(QDate::currentDate().toString("d MMMM yyyy")) + "</p>" +
    "<h4 style=\"text-align: right; margin-right:30px;\">" + "Nootka<br>Akademy Of Music" + "</h4>" +
    QString("Student %1 ").arg(spanPx + "<b>" + exam->userName().toUpper() + "</span></b><br>") +
    "Has been awarded the" + "<h2 style=\"text-align: center;\">" + 
		"Certificate Of Exam Completion" + "</h2>" +
    "Passing the exam on the level" + spanPx + " <b>" + exam->level()->name + "</span></b>,<br>" +
    "having answered the required" + spanPx + QString("<b> %1 </span></b>").arg(exam->count()) + "answers<br>" +
    "in time" + spanPx + " <b>" + TexamView::formatedTotalTime(exam->workTime() * 1000) + "</b></span><br>" +
    "and achieving the score" + ": <span style=\"font-size: 30px;\"><b>" + 
		QString("<b>%1%</b>").arg(exam->effectiveness(), 0, 'f', 1, '0')+ "</span></b><br><br>" +
    "As a witness to this accomplishment,<br>we hereby award this certificate on " + "<b>" +
		QDate::currentDate().toString("d MMMM yyyy") +	"</b><br><br>" +
    "<p style=\"text-align: right;\">" + "examining board:" + "<br>" +
    "president: Nootka itself" + "<br>" +
    "profesor Processor &amp; Mrs RAM his assistant" + "<br>" +
    "secretary: Mr Disk" + "<br>" +
//     <img src="http://nootka.googlecode.com/hg/unused-picts/stamp.png">
    "</p></div>";
    return txt;
}



TnootkaCertificate::TnootkaCertificate(Texam* exam) :
  TgraphicsTextTip(finishExamText(exam), QColor("feffbf")),
  m_exam(exam)
{
    
}


