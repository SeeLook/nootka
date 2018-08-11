/***************************************************************************
 *   Copyright (C) 2018 by Tomasz Bojczuk                                  *
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

#ifndef TRTMGROUP_H
#define TRTMGROUP_H


#include <nootkacoreglobal.h>
#include "trhythm.h"
#include <QtCore/qobject.h>


#define BASIC_GR_COUNT      (22) /** Number of enumerators representing basic rhythmic groups (without dots) */


/**
 * @class TrtmGroup describes rhythmic group of notes
 * It defines rhythmic groups in @p ErtmGroup enumerator 
 */
class NOOTKACORE_EXPORT TrtmGroup
{

  Q_GADGET

public:
  enum ErtmGroup : quint8 {
      Gr_undefined = 0,
    // Special
      Gr_1,
      Gr_2,
    // Duple metre (duration: 4 -> one quarter) ==== SIMPLE
      Gr_4,
      Gr_8_8,
      Gr_8_16_16,
      Gr_16_16_16_16,
      Gr_16_16_8,
      Gr_16_8_16,
    // Triple metre (duration: 4. -> one quarter and eight) ==== SIMPLE
      Gr_8_8_8,
      Gr_4_8,
      Gr_8_4,
      Gr_8_8_16_16,
      Gr_8_16_16_8,
      Gr_16_16_8_8,
      Gr_8_16_16_16_16,
      Gr_16_16_16_16_8,
      Gr_16_16_8_16_16,
      Gr_16_16_16_16_16_16,
    // Triple metre (duration: 4. -> one quarter and eight) ==== ADVANCED
      Gr_16_8_8_16,
      Gr_16_8_16_8,
      Gr_16_16_16_8_16,
      Gr_16_8_16_16_16,
    // Special
      Gr_1dot,
      Gr_2dot,
    // Triple or special
      Gr_4dot,
    // Duple metre (duration: 4 -> one quarter) ==== DOTS
      Gr_8dot_16,
      Gr_16_8dot,
    // Triple metre (duration: 4. -> one quarter and eight) ==== DOTS
      Gr_8dot_8dot,
      Gr_8dot_16_8,
      Gr_8dot_8_16,
      Gr_16_8dot_8,
      Gr_8_8dot_16,
      Gr_8dot_16_16_16,
      Gr_16_8dot_16_16,
      Gr_16_16_8dot_16,
      Gr_16_16_16_8dot
  };
  Q_ENUM(ErtmGroup)


  TrtmGroup() {}
  TrtmGroup(ErtmGroup rGr) : m_gr(rGr) {}

  ErtmGroup group() const { return m_gr; }
  void setGroup(ErtmGroup g) { m_gr = g; }

      /**
       * Text that represents rhythm of this group in Scorek (Bravura) font
       */
  QString text() const;

      /**
       * Rhythm of this group in @p Trhythm values
       */
  TrhythmList rhythm() const;

      /**
       * Special rhythmic group means a group longer than duple (quarter) or triple (quarter and dot)
       * There are: whole, half and whole with dot and half with dot.
       * @p Gr_1, @p Gr_2, @p Gr_1dot, @p Gr_2dot
       */
  bool isSpecial() const;
  bool isDuple() const;
  bool isTriple() const;

      /**
       * Duration of the group.
       * Use this method wisely - it is expensive!
       */
  int duration() const;

private:
  ErtmGroup          m_gr = Gr_undefined;
};

#endif // TRTMGROUP_H
