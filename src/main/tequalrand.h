/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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

#ifndef TEQUALRAND_H
#define TEQUALRAND_H


#include <QtGlobal>

/**
 * This class randomizes values in given range
 * but tries to keep all occurrences with the same number of times.
 * So, for range [1-10] and 10 randoms, every value will occur once.
 * If there will be more randoms then after every @p 10 randoms it will be reset.
 * Also it supports equality for greater number of randoms.
 *
 * By default begin value of the range is @p 0 and can be changed in constructor.
 * In this case certain random value is 'blocked' after it has been randomized declared number of times.
 * This is called 'depth' and can be set directly by @p setDepth() 
 * or calculated from total number of randoms @p setTotalRandoms().
 * WARRING!
 * It could be CPU expensive if value range is bigger.
 * 1000 (1k) is OK but 1M or 1G could block an application for a while.
 * For Nootka purpose it is more than sufficient.
 */
class TequalRand
{

public:

  TequalRand(int range, int startVal = 0);
  virtual ~TequalRand();

      /**
       * The depth determines how many times certain value can be randomized.
       * By default it is @p 1 and [1-255] depth is supported.
       */
  void setDepth(int d) { m_depth = d; m_total = d * m_range; }
  int depth() const { return m_depth; }

      /**
       * By setting this value the depth can be obtained.
       */
  void setTotalRandoms(int total);
  int range() const { return m_range; }
  int startValue() const { return m_offset; }

      /**
       * Randomizes and returns a value.
       */
  int get();

      /**
       * Number of randoms already performed.
       */
  int count() const { return m_count; }

      /**
       * Resets a memory of randomized values.
       */
  void reset();

protected:
      /**
       * Looks for next available value than @p currVal
       */
  int next(int currVal);

private:
  int              m_range, m_depth, m_offset, m_total, m_count;
  quint8          *m_shotTable;
  bool             m_isLastLoop; /**< @p True when depth is achieved but m_depth * m_range < m_total */
};

#endif // TEQUALRAND_H
