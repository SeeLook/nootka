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

#ifndef TFADEINANIM_H
#define TFADEINANIM_H

#include <nootkacoreglobal.h>
#include "tabstractanim.h"


/**
 * This class perform fade in/out animation on given item.
 * Remember to set start opacity of the item before!!
 */
class NOOTKACORE_EXPORT TfadeAnim : public TabstractAnim
{
  Q_OBJECT
  
public:
  
  explicit TfadeAnim(QGraphicsItem* item, QObject* parent = 0);
  
			/** @p endOpacity determines fade 'direction':
			 * @p endOpacity = 1.0 -> fade In
			 * @p endOpacity = 0.0 -> fade Out
			 * Start value is taken from item opacity. */
  void startFade(qreal endOpacity, qreal midOpacity = -1);
  
protected:
  virtual void animationRoutine();
	
private:
	qreal				m_startOp, m_endOp, m_midOp, m_opacityToGo;
	

};

#endif // TFADEINANIM_H
