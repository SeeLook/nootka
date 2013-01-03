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


void TgroupedQAunit::resume() {

}


