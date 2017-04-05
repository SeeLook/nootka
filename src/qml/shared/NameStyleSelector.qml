/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

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
