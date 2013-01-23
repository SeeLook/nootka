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


#include "tgroupedqaunit.h"
#include "tqaunit.h"
#include "texam.h"


/*static*/
bool TgroupedQAunit::m_skipWrong = true;



TgroupedQAunit::TgroupedQAunit() :
  m_desc(""),
  m_mistakes(0),
  m_halfMist(0)
{

}

TgroupedQAunit::~TgroupedQAunit()
{
    list.clear();
}


void TgroupedQAunit::addQAunit(TQAunit* qaUnit, unsigned int questNr) {
    TqaPtr qaPtr;
    qaPtr.nr = questNr;
    qaPtr.qaPtr = qaUnit;
    list << qaPtr;
}


void TgroupedQAunit::resume(QString desc, QString fullDesc) {
    setDescription(desc);
//     m_fullDesc = fullDesc + " <span style=\"font-size: 20px;\">" + desc + "</span>";
    m_fullDesc = fullDesc;
    m_mistakes = 0;
    m_halfMist = 0;
    double okTime = 0.0;
    double badTime = 0.0;
    int cnt = 0; // number of answers in average
    for (int i = 0; i < list.size(); i++) {
      if (list[i].qaPtr->isWrong())
        m_mistakes++;
      else if (list[i].qaPtr->isNotSoBad())
        m_halfMist++;
      if (skipWrong() && (list[i].qaPtr->wrongNote() || list[i].qaPtr->wrongPos()) ) 
          badTime += list[i].qaPtr->time;
      else {
        okTime += list[i].qaPtr->time;
        cnt++;
      }
    }
    if (cnt)
      m_averTime = okTime / cnt;
    else
      m_averTime = badTime / list.size();
    m_effectiveness = Texam::effectiveness(list.size(), m_mistakes,m_halfMist);
}


