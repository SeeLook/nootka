/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import "../"


Item {
  property alias is7B: bButt.checked
  property int style: -1

  width: mainLay.width
  height: mainLay.height

  Grid {
    id: mainLay
    columns: 2
    spacing: NOO.factor()
    verticalItemAlignment: Grid.AlignVCenter

    ButtonGroup { buttons: butRow.children }
    Row {
      id: butRow
      y: (parent.height - height) / 2
      spacing: NOO.factor() / 2
      Text { text: qsTr("7th note is:"); color: enabled ? activPal.text : disdPal.text; anchors.verticalCenter: parent.verticalCenter }
      TradioButton { id: bButt; text: "B" }
      TradioButton { text: "H"; checked: !bButt.checked }
    }

    Text {
      id: preview
      font.pixelSize: NOO.factor() * 2
      color: activPal.text
    }
  }

  onStyleChanged: {
    if (style > -1) {
      var sp = ""
      for (var n = 1; n < 8; ++n)
        sp += NOO.noteName(NOO.note(n, 1, 0), style, false) + " "
      preview.text = sp
    }
  }
}

