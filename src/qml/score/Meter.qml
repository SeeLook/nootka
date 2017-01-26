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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0


Text {
  property bool readOnly: false

  visible: staff.number === 0
  font { family: "Scorek"; pixelSize: 8 }
  color: activPal.text
  text: staff.number === 0 ? Noo.meter(score.meter).symbol() : ""

  Drawer { // meter menu
      id: meterDrawer
      width: nootkaWindow.width / 4; height: nootkaWindow.height

      Grid {
        width: parent.width
        anchors.margins: nootkaWindow.font.pixelSize / 2
        columns: 2
        spacing: nootkaWindow.font.pixelSize / 2

        Repeater {
          model: 12
          Button {
            height: nootkaWindow.font.pixelSize * 5
            width: parent.width / 2.1
            Text {
                id: buttText
                property int meter: Noo.meter(Math.pow(2, index)).meter()

                anchors.horizontalCenter: parent.horizontalCenter
                y: -nootkaWindow.font.pixelSize * 3.5
                font { family: "Scorek"; pixelSize: nootkaWindow.font.pixelSize * 4 }
                text: Noo.meter(meter).symbol()
                color: activPal.text
            }
            onClicked: {
                score.meter = buttText.meter
                meterDrawer.close()
            }
          }
        }
      }
  }

  MouseArea {
      anchors.fill: parent
      enabled: !readOnly
      onClicked: meterDrawer.open()
  }
}
