/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Tflickable {
  property string helpText: dialLoader.dialogObj.mainHelp()
  contentHeight: text.height;

  Rectangle {
      anchors.fill: parent
      color: activPal.base
  }

  LinkText {
    id: text
    width: parent.width
    padding: Noo.fontSize()
    wrapMode: TextEdit.Wrap; textFormat: Text.RichText
    text: helpText
  }
}


