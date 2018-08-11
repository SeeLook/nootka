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

#include "trtmselectoritem.h"

#include <QtCore/qdebug.h>


TrtmSelectorItem::TrtmSelectorItem(QQuickItem* parent) :
  QQuickItem(parent)
{
//   quint64 mask = 0;
//   for (qreal p = 0; p < BASIC_GR_COUNT; ++p) {
//     mask += bitOf(p);
//     qDebug() << p << "mask" << mask;
//   }
}


TrtmSelectorItem::~TrtmSelectorItem()
{}


QString TrtmSelectorItem::getGroupText(int rtmGrEnum) {
  return TrtmGroup(static_cast<TrtmGroup::ErtmGroup>(rtmGrEnum)).text();
}


void TrtmSelectorItem::setBasicMask(quint32 v) {
  if (v != m_basicMask) {
    m_basicMask = v;
    emit basicMaskChanged();
  }
}


void TrtmSelectorItem::setDotsMask(quint32 dm) {
  if (dm != m_dotsMask) {
    m_dotsMask = dm;
    emit dotsMaskChanged();
  }
}


void TrtmSelectorItem::userChangedGroup(int grId, bool checked) {
  if (grId < static_cast<int>(TrtmGroup::Gr_1dot) - 1) {
      auto b = bitOf(static_cast<qreal>(grId));
      if (checked)
        m_basicMask |= b;
      else
        m_basicMask &= ~b;
      emit basicMaskChanged();
      emit basicGroupChanged();
  } else {
      auto b = bitOf(static_cast<qreal>(grId - static_cast<int>(TrtmGroup::Gr_1dot) + 1));
      if (checked)
        m_dotsMask |= b;
      else
        m_dotsMask &= ~b;
      emit dotsMaskChanged();
      emit dotsGroupChanged();
  }
}
