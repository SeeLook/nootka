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

import QtQuick 2.7
import QtGraphicalEffects 1.0

import Instruments 1.0


GuitarBackground {
  id: guitar

  Rectangle {
    id: finger
    color: Qt.rgba(1, 0, 0.5, 0.78)
    width: guitar.fretWidth / 1.6
    height: guitar.stringsGap * 0.7
    radius: height / 2
    x: guitar.fingerPos.x; y: guitar.fingerPos.y
    visible: false // guitar.active && guitar.fingerPos.x > 0 
//     Behavior on x { enabled: GLOB.useAnimations; NumberAnimation { duration: 200 }}
//     Behavior on y { enabled: GLOB.useAnimations; NumberAnimation { duration: 200 }}
  }

  DropShadow {
    id: fingerShadow
    anchors.fill: finger
    horizontalOffset: guitar.stringsGap / 7
    verticalOffset: guitar.stringsGap / 7
    color: activPal.shadow
    radius: 8.0
    source: finger
    visible: guitar.active && guitar.fingerPos.x > 0
  }

  Rectangle {
    id: string
    color: finger.color
    width: guitar.width - guitar.stringsGap - 2
    height: guitar.string < 6 ? guitar.strWidth(guitar.string) * 1.5 : 0
    x: 1
    y: guitar.string < 6 ? 10 + guitar.stringsGap / 2 + guitar.string * guitar.stringsGap - height / 3 : 0
    visible: guitar.active && guitar.string < 6 && guitar.fingerPos.x == 0
  }
}
