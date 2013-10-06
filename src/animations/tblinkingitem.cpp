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

#include "tblinkingitem.h"
#include <QTimer>


TblinkingItem::TblinkingItem(QGraphicsItem* item, QObject* parent ):
	QObject(parent), 
	m_item(item)
{

}


void TblinkingItem::startBlinking(int count) {
	m_maxCount = count * 2;
	m_blinkPhase = 0;
	blinkingSlot();
}


void TblinkingItem::blinkingSlot() {
		m_blinkPhase++;
		if (m_blinkPhase <= m_maxCount) {
			if (m_blinkPhase % 2) { // phase 1, 3, ...
					m_item->hide();
			} else { // phase 2, 4, ...
					m_item->show();
			}
			QTimer::singleShot(150, this, SLOT(blinkingSlot()));
		} else {
				emit blinkingFinished();
		}
}


