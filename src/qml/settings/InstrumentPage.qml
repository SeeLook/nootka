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
import QtQuick.Controls 2.0

import Score 1.0


Flickable {
  clip: true
  anchors.fill: parent
  contentHeight: instrCol.height

  property var instrGlyphs: ["v", "h", "j", "i"]

  Column {
    id: instrCol
    width: parent.width
    spacing: nootkaWindow.fontSize / 2
    Text {
      text: qsTr("Instrument")
      anchors.horizontalCenter: parent.horizontalCenter
    }
    Row {
      spacing: nootkaWindow.fontSize
      anchors.horizontalCenter: parent.horizontalCenter
      Repeater {
        model: 4
        Button {
          contentItem: Text {
            font {family: "nootka"; pixelSize: 50 }
            text: instrGlyphs[index]
            anchors.horizontalCenter: parent.horizontalCenter
          }
        }
      }
    }

    Row {
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: nootkaWindow.fontSize
      Text { text: qsTr("number of frets:"); anchors.verticalCenter: parent.verticalCenter }
      SpinBox {}
      Text { text: qsTr("number of strings:"); anchors.verticalCenter: parent.verticalCenter }
      SpinBox {}
    }

    Tile {
      width: parent.width
      Column {
        spacing: nootkaWindow.fontSize / 4
        width: parent.width
        Row {
          spacing: nootkaWindow.fontSize
          anchors.horizontalCenter: parent.horizontalCenter
          Text { text: qsTr("tuning of the guitar"); anchors.verticalCenter: parent.verticalCenter }
          ComboBox {
            model: []
          }
        }
        Score {
          width: parent.width * 0.9
          height: nootkaWindow.fontSize * 20
          anchors.horizontalCenter: parent.horizontalCenter
        }
      }
      description: qsTr("Select appropriate tuning from the list or prepare your own.") + "<br>" + 
                    qsTr("Remember to select the appropriate clef in Score settings.")
    }

    Tile {
      width: parent.width
      CheckBox {
        text: qsTr("show all possibilities of a note")
        anchors.horizontalCenter: parent.horizontalCenter
      }
      description: qsTr("As you know, the same note can be played in several places on the fingerboard.<br>If checked, all of them will be shown.")
    }

    Tile {
      Row {
        spacing: nootkaWindow.fontSize
        anchors.horizontalCenter: parent.horizontalCenter
        Text { text: qsTr("marked frets", "or frets with dots/marks"); anchors.verticalCenter: parent.verticalCenter }
        TextField {
          maximumLength: 25
          validator: RegExpValidator { regExp: /([1-2]{0,1}[0-9]{1,2}!{0,1},){0,7}/ }
        }
      }
      description: qsTr("Put numbers of frets marked with dot. Separate the numbers with comma. Add ! (exclamation mark) after a number to paint a dot twice.")
    }
  }
}
