/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#ifndef TLEVELCREATORITEM_H
#define TLEVELCREATORITEM_H


#include <QtQuick/qquickitem.h>


class Tlevel;


/**
 * This class is QML logic for "Level creator" dialog window
 */
class TlevelCreatorItem : public QQuickItem
{

  Q_OBJECT

public:
  TlevelCreatorItem(QQuickItem* parent = nullptr);
  ~TlevelCreatorItem() override;

private:
  Tlevel                    *m_level;
};

#endif // TLEVELCREATORITEM_H
