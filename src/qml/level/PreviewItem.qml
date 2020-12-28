/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  property int index: aboveItem ? aboveItem.index + (aboveItem.visible ? 1 : 0) : 0
  property var aboveItem: null
  property bool layHorizontal: true /** layout direction, horizontal by default */
  property bool handleWidth: false /** When width is maximum value of all similar items */
  property alias text: txt.text /** label */
  property alias text2: txt2.text // value
  property alias textItem: txt
  property alias textItem2: txt2
  property alias showBottomLine: bottomLine.visible

  anchors.horizontalCenter: parent.horizontalCenter

  implicitWidth: Math.max(layHorizontal ? txt.width + txt2.width : Math.max(txt.width, txt2.width))  + Noo.fontSize() * 2
  implicitHeight: (layHorizontal ? Math.max(txt.height, txt2.height) : txt.height + txt2.height) + Noo.fontSize() / 2
  color: index ? Noo.alpha(index % 2 === 1 ? activPal.alternateBase : activPal.base, 150) : "transparent"
  visible: text2 !== ""
  width: handleWidth ? maxLabelWidth + maxValueWidth + Noo.fontSize() * 2 : undefined

  onImplicitWidthChanged: {
    if (handleWidth) {
      maxLabelWidth = Math.max(maxLabelWidth, txt.implicitWidth + Noo.fontSize())
      maxValueWidth = Math.max(maxValueWidth, txt2.implicitWidth + Noo.fontSize())
    }
  }

  Text {
    id: txt
    x: Noo.fontSize(); y: layHorizontal ? (parent.height - height) / 2 : 0
    textFormat: Text.StyledText
    color: activPal.text
    font.pixelSize: Noo.fontSize()
  }
  Text {
    id: txt2
    x: handleWidth ? maxLabelWidth + Noo.fontSize() + (maxValueWidth - width) / 2 : (layHorizontal ? 0 : (parent.width - width) / 2)
    y: (parent.height - height) / 2
    horizontalAlignment: handleWidth ? Text.AlignHCenter : undefined
    textFormat: Text.StyledText
    color: activPal.text
    font.pixelSize: Noo.fontSize()
  }

  Rectangle {
    visible: handleWidth; color: activPal.text
    width: Noo.fontSize() / 12;
    height: parent.height
    x: maxLabelWidth + Noo.fontSize() * 0.5
  }
  Rectangle {
    id: bottomLine
    visible: handleWidth; color: activPal.text
    height: Noo.fontSize() / 12;
    width: parent.width
    y: parent.height - height / 2
  }
}
