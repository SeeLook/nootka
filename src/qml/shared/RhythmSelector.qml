/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0


TrtmSelectorItem {
  id: rtmSelItem

  implicitWidth: Noo.fontSize() * 12 + settCol.width
  implicitHeight: Noo.fontSize() * 20

  Row {
    spacing: Noo.fontSize()
    ListView {
      clip: true
      height: rtmSelItem.height - 2; width: Noo.fontSize() * 10 - 2; x: 1; y: 1
      spacing: Noo.fontSize() / 10
      ScrollBar.vertical: ScrollBar { active: true; visible: active }
      model: 35
      delegate: Component {
        Rectangle {
          height: Noo.fontSize() * 2; width: Noo.fontSize() * 10 - 2; x: 1
          color: index % 2 ? activPal.alternateBase : activPal.base
          MouseArea {
            anchors.fill: parent
            onClicked: groupChanged(index, !chBox.checked)
          }
          Row {
            TcheckBox {
              id: chBox
              property int pow: Math.pow(2, index < 21 ? index : index - 21)
              checked: (index < 21 && basicMask & pow) || (index > 20 && dotsMask & pow)
              onClicked: groupChanged(index, checked)
            }
            Text {
              font { pixelSize: Noo.fontSize() * 1.5; family: "Scorek" }
              y: -font.pixelSize
              text: getGroupText(index + 1)
              color: activPal.text
            }
          }
        }
      }
    }

    Column {
      id: settCol
      anchors.verticalCenter: parent.verticalCenter
      TcheckBox {
        text: qsTr("basic")
        checked: basicMask
        onClicked: basicMask = checked ? 2097151 : 0
      }
      TcheckBox {
        text: qsTr("dots")
        checked: dotsMask
        onClicked: dotsMask = checked ? 16383 : 0
      }
      TcheckBox {
        text: qsTr("rests")
      }
      TcheckBox {
        text: qsTr("ties")
      }
    }
  }
  
}
