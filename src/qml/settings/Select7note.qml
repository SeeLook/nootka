/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.1


Item {
  property alias is7B: bButt.checked
  property int style: -1

  width: mainLay.width
  height: mainLay.height

  Grid {
    id: mainLay
    columns: 2
    spacing: Noo.fontSize()

    ButtonGroup { buttons: butRow.children }
    Row {
      id: butRow
      y: (parent.height - height) / 2
      spacing: Noo.fontSize() / 2
      Text { text: qsTr("7th note is:"); color: enabled ? activPal.text : disdPal.text; anchors.verticalCenter: parent.verticalCenter }
      RadioButton { id: bButt; text: "B" }
      RadioButton { text: "H"; checked: !bButt.checked }
    }

    Text {
      id: preview
      font.pixelSize: Noo.fontSize() * 2
    }
  }

  onStyleChanged: {
    if (style > -1) {
      var sp = ""
      for (var n = 1; n < 8; ++n)
        sp += Noo.noteName(Noo.note(n, 1, 0), style, false) + " "
      preview.text = sp
    }
  }
}

