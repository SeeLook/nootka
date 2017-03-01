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
    visible: guitar.active && guitar.fingerPos.x > 0 //false
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
}
