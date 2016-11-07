/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#ifndef TSCALEDANIM_H
#define TSCALEDANIM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"


class NOOTKACORE_EXPORT TscaledAnim : public TabstractAnim
{
  Q_OBJECT

public:
  explicit TscaledAnim(QGraphicsItem *item, QObject* parent = 0);

      /** Starts performing scaling. Start scale is current item scale.
       * If @p midScale is set (0.0 or greater) this value will be achieved
       * in the middle of animation time */
  void startScaling(qreal endScale, qreal midScale = -1.0);

protected:
  virtual void animationRoutine();

private:
  qreal        m_beginScale, m_midScale, m_endScale, m_scaleToGo;

};

#endif // TSCALEDANIM_H
