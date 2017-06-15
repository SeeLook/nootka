/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0


Flickable {
  clip: true
  ScrollBar.vertical: ScrollBar {}
  contentHeight: text.paintedHeight; contentWidth: width

  TextEdit {
      id: text
      width: parent.width
      textMargin: font.pixelSize
      wrapMode: TextEdit.Wrap
      readOnly: true
      textFormat: TextEdit.RichText
      color: activPal.text
      text: Noo.aboutQt()
  }
}
 
