/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0
import "../"


Column {
  property alias sufix: sufixField.text
  property int noteOne: 3
  property int alterOne: 0
  property int noteTwo: 7
  property int alterTwo: -1
  property int nameStyle: 0

  spacing: Noo.fontSize() / 2

  Text {
    wrapMode: Text.NoWrap
    horizontalAlignment: Text.AlignHCenter
    text: qsTranslate("TscoreSettings", "in the major keys:")
    anchors.horizontalCenter: parent.horizontalCenter
    color: enabled ? activPal.text : disdPal.text
  }
  TtextField  {
    width: Noo.fontSize() * 8
    id: sufixField
    maximumLength: 10
    horizontalAlignment: TextInput.AlignHCenter
    anchors.horizontalCenter: parent.horizontalCenter
  }
  Item { height: Noo.fontSize() / 2; width: 1 }
  Text {
    text: qsTranslate("TscoreSettings", "for example") + ":"
    anchors.horizontalCenter: parent.horizontalCenter
    color: enabled ? activPal.text : disdPal.text
  }
  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    horizontalAlignment: Text.AlignHCenter
    color: enabled ? activPal.text : disdPal.text
    font { bold: true; pixelSize: Noo.fontSize() * 1.5 }
    text: Noo.noteName(Noo.note(noteOne, 0, alterOne), nameStyle, false) + (sufix === "" || sufix === " " ? "" : "-" + sufixField.text) + "\n"
        + Noo.noteName(Noo.note(noteTwo, 0, alterTwo), nameStyle, false) + (sufix === "" || sufix === " " ? "" : "-" + sufixField.text)
  }
}
