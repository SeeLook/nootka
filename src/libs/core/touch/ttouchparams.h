/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TTOUCHPARAMS_H
#define TTOUCHPARAMS_H

#include <nootkacoreglobal.h>

/**
 * Parameters related to touch in Nootka.
 * This is singleton class created once in @class Tglobals
 * and available through @p TtouchParams::i()
 */
class NOOTKACORE_EXPORT TtouchParams {

public:
  TtouchParams();

  ~TtouchParams();

  static TtouchParams* i() { return m_instance; } /**< Single instance of TtouchParams object */

  bool scoreWasTouched; /**< Becomes @p TRUE when user first time touched the score widget and read how it works */
  bool guitarWasTouched; /**< Becomes @p TRUE when user first time touched the fingerboard widget and read how it works */
  bool clefWasTouched; /**< Becomes @p TRUE when user first time touched clef */

private:
  static TtouchParams            *m_instance;
};

#endif // TTOUCHPARAMS_H
