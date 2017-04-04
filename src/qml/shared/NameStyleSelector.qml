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
import QtQuick.Controls 2.1

import Nootka 1.0


Item {

  property bool seventhIsB: true
  property int style: 0

  width: childrenRect.width
  height: childrenRect.height

  ButtonGroup {
    id: styleRadios
    buttons: styleColumn.children
    onClicked: style = button.style
  }

  onStyleChanged: {
    for (var b = 0; b < 6; ++b) {
      if (styleRadios.buttons[b].style == style) {
        styleRadios.buttons[b].checked = true;
        break;
      }
    }
  }

  Column {
    Text {
      text: qsTr("Naming style")
      anchors.horizontalCenter: parent.horizontalCenter
      color: enabled ? activPal.text : disdPal.text
    }

    Frame {
      Column {
        id: styleColumn
        spacing: nootkaWindow.fontSize / 2
        RadioButton {
          default property int style: Nootka.Norsk_Hb
          text: qsTr("Scandinavian") + " (C, C#, Db ... Hb, H)"
          visible: !seventhIsB
        }
        RadioButton {
          default property int style: Nootka.Italiano_Si
          text: qsTr("Italian") + " (Do, Do#, Reb ... Sib, Si)"
        }
        RadioButton {
          default property int style: Nootka.Deutsch_His
          text: qsTr("German") + " (C, Cis, Des ... B, H)"
          visible: !seventhIsB
        }
        RadioButton {
          default property int style: Nootka.English_Bb
          text: qsTr("English") + " (C, C#, Db ... Bb, B)"
          visible: seventhIsB
        }
        RadioButton {
          default property int style: Nootka.Nederl_Bis
          text: qsTr("Dutch") + " (C, Cis, Des ... Bes, B)"
          visible: seventhIsB
        }
        RadioButton {
          default property int style: Nootka.Russian_Ci
          text: qsTr("Russian") + " (До, До# Реb ... Сиb, Си)"
        }
      }
    }
  }
}
