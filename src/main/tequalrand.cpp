/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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

#include "tequalrand.h"


TequalRand::TequalRand(int range, int startVal) :
  m_range(range),
  m_depth(1),
  m_offset(startVal),
  m_count(0),
  m_isLastLoop(false)
{
  m_total = m_range * m_depth;
  m_shotTable = new quint8[m_range];
  reset();
}


TequalRand::~TequalRand()
{
  delete m_shotTable;
}


void TequalRand::setTotalRandoms(int total) {
  if (total / m_range < 256) {
    m_depth = qMax(1, total / m_range);
    m_total = total;
  }
}


void TequalRand::reset() {
  for (int i = 0; i < m_range; ++i) {
    m_shotTable[i] = 0;
  }
}


int TequalRand::get() {
  m_count++;
  if ((m_total % m_range) && (m_count % m_total == 0)) { // reset if last loop occurred
    reset();
    m_isLastLoop = false;
    setTotalRandoms(m_total); // restore depth
  }
  int val = qrand() % m_range;
  if (m_shotTable[val] == m_depth)
    val = next(val);
  m_shotTable[val] += 1;
  return val + m_offset;
}


int TequalRand::next(int currVal) {
  int dir = 1; // Random direction of searching next value
  if (qrand() % 2)
    dir = -1;
  int result = currVal;
  do {
    result = result + dir;
    if (result >= m_range)
      result = 0;
    else if (result < 0)
      result = m_range - 1;
    if (m_shotTable[result] < m_depth)
      return result;
  } while (result != currVal);
  if (m_total % m_range && !m_isLastLoop) {
      m_depth = 1;
      m_isLastLoop = true;
  }
  reset();
  return currVal;
}





