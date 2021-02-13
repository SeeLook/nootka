/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0


TrtmSelectorItem {
  implicitWidth: Noo.factor() * 10
  implicitHeight: Noo.factor() * 20

  Component {
    id: moreComp
    Rectangle {
      height: Noo.factor() * 2; width: Noo.factor() * 10
      color: enabled ? (ma.containsMouse ? activPal.highlight : activPal.button ): Qt.darker(disdPal.window, 1.2)
      Text {
        anchors.verticalCenter: parent.verticalCenter; horizontalAlignment: Text.AlignHCenter
        width: Noo.factor() * 10; fontSizeMode: Text.HorizontalFit; font { bold: true; pixelSize: Noo.factor() }
        text: qsTr("rhythmic groups") + "  ⋮"; textFormat: Text.StyledText
        color: enabled ? (ma.containsMouse ? activPal.highlightedText : activPal.buttonText) : disdPal.buttonText
      }
      MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
          morePop.y = parent.y > 100 ? rtmList.height - morePop.height : rtmList.y
          morePop.open()
        }
      }
    }
  }

  ListView {
    id: rtmList
    clip: true
    height: parent.height; width: parent.width
    spacing: Noo.factor() / 10
    ScrollBar.vertical: ScrollBar { active: enabled; visible: active }
    model: 35
    delegate: Component {
      Rectangle {
        height: Noo.factor() * 2.5; width: parent ? parent.width : 0
        color: enabled ? (index % 2 ? activPal.alternateBase : activPal.base) : Qt.darker(disdPal.window, 1.2)
        MouseArea {
          anchors.fill: parent
          onClicked: userChangedGroup(index, !chBox.checked)
        }
        Row {
          TcheckBox {
            id: chBox
            property int pow: Math.pow(2, index < 22 ? index : index - 22)
            checked: (index < 22 && basicMask & pow) || (index > 21 && dotsMask & pow)
            onClicked: userChangedGroup(index, checked)
          }
          Text {
            font { pixelSize: Noo.factor() * 1.5; family: "Scorek" }
            y: -font.pixelSize * 0.8
            text: getGroupText(index + 1)
            color: enabled ? activPal.text : disdPal.text
          }
        }
      }
    }
    header: moreComp
    footer: moreComp
  }

  Popup {
    id: morePop
    x: rtmList.width

    margins: Noo.factor()
    background: GlowRect { color: activPal.window; shadowRadius: Noo.factor() / 2 }

    scale: GLOB.useAnimations ? 0 : 1.0
    enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
    exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0.0 }}

    Column {
      spacing: Noo.factor() / 2
      Text { text: qsTr("Filters") + ":"; color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
      TcheckBox {
        text: qsTr("basic rhythms")
        checked: basicMask
        onClicked: {
          basicMask = checked ? 4194303 : 0
          rtmList.positionViewAtIndex(0, ListView.Beginning)
          basicGroupChanged()
        }
      }
      TcheckBox {
        text: qsTr("rhythms with dots")
        checked: dotsMask
        onClicked: {
          dotsMask = checked ? 16383 : 0
          rtmList.positionViewAtIndex(22, ListView.Beginning)
          dotsGroupChanged()
        }
      }
    }
  }
}
