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


#include "tfinishtip.h"
#include "texam.h"
#include <QDate>



QString finishExamText(Texam* exam) {
  QString text = "<p style=\"font-size: 20px;\">" + 
      QString("Today at %1 <br>").arg(QDate::currentDate().toString("d MMMM yyyy")) + " user Czesio <br>";
    return text;
}



TfinishTip::TfinishTip(Texam* exam) :
  TgraphicsTextTip(finishExamText(exam), QColor("feffbf")),
  m_exam(exam)
{
    
}


