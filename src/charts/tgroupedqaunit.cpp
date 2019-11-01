/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <exam/tqaunit.h>
#include <exam/texam.h>


/*static*/
bool TgroupedQAunit::m_skipWrong = true;


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


void TgroupedQAunit::resume(const QString& desc, const QString& fullDesc) {
    setDescription(desc);
    m_fullDesc = fullDesc;
    m_mistakes = 0;
    m_halfMist = 0;
    qreal okTime = 0.0;
    qreal badTime = 0.0;
    int cnt = 0; // number of answers in average
    qreal effSum = 0.0;
    for (int i = 0; i < list.size(); i++) {
      TqaPtr& p = list[i];
      effSum += p.qaPtr->effectiveness();
      if (p.qaPtr->isWrong())
        m_mistakes++;
      else if (p.qaPtr->isNotSoBad())
        m_halfMist++;
      if (skipWrong() && (p.qaPtr->wrongNote() || p.qaPtr->wrongPos()) ) 
          badTime += static_cast<qreal>(p.qaPtr->time);
      else {
        okTime += static_cast<qreal>(p.qaPtr->time);
        cnt++;
      }
    }
    if (cnt)
      m_averTime = okTime / static_cast<qreal>(cnt);
    else
      m_averTime = badTime / static_cast<qreal>(list.size());
    m_effectiveness = effSum / static_cast<qreal>(list.size());
}


