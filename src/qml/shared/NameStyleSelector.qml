/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Item {
  id: root

  property bool seventhIsB: true
  property int style: -1

  width: childrenRect.width
  height: childrenRect.height
  antialiasing: true

  ButtonGroup {
    id: styleGr
    buttons: styleColumn.children
    onClicked: style = button.style
  }

  onStyleChanged: {
    for (var b = 0; b < 6; ++b) {
      if (styleGr.buttons[b].style == style) {
        styleGr.buttons[b].checked = true;
        break;
      }
    }
  }

  onSeventhIsBChanged: {
    if (seventhIsB) {
        if (styleGr.buttons[0].checked)
          root.style = Nootka.English_Bb
        else if (styleGr.buttons[1].checked)
          root.style = Nootka.Nederl_Bis
    } else {
        if (styleGr.buttons[3].checked)
          root.style = Nootka.Norsk_Hb
        else if (styleGr.buttons[4].checked)
          root.style = Nootka.Deutsch_His
    }
  }

  Column {
    Text {
      text: qsTr("Naming style")
      anchors.horizontalCenter: parent.horizontalCenter
      color: enabled ? activPal.text : disdPal.text
    }

    Tframe {
      width: NOO.factor() * 25
      Column {
        id: styleColumn
        spacing: NOO.factor() / 2
        TradioButton { // 0
          default property int style: Nootka.Norsk_Hb
          text: qsTr("Scandinavian") + " (C, C#, Db ... Hb, H)"
          visible: !seventhIsB
        }
        TradioButton { // 1
          default property int style: Nootka.Deutsch_His
          text: qsTr("German") + " (C, Cis, Des ... B, H)"
          visible: !seventhIsB
        }
        TradioButton { // 2
          default property int style: Nootka.Italiano_Si
            text: qsTr("Italian") + " (Do, Do#, Reb ... Sib, Si)"
        }
        TradioButton { // 3
          default property int style: Nootka.English_Bb
          text: qsTr("English") + " (C, C#, Db ... Bb, B)"
          visible: seventhIsB
        }
        TradioButton { // 4
          default property int style: Nootka.Nederl_Bis
          text: qsTr("Dutch") + " (C, Cis, Des ... Bes, B)"
          visible: seventhIsB
        }
        TradioButton { // 5
          default property int style: Nootka.Russian_Ci
          text: qsTr("Russian") + " (До, До# Реb ... Сиb, Си)"
        }
      }
    }
  }
}
