/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Item {
  id: nootkaLabel
  property color bgColor: "white"
  property alias active: mouseArea.hoverEnabled

  signal clicked()
  signal entered()
  signal exited()

  width: logo.width

  Rectangle {
    id: bgRect
    height: parent.height
    width: logo.width
    color: bgColor
  }

  Image {
    id: logo
    mipmap: true
    anchors.top: parent.Top
    source: NOO.pix("logo")
    height: nootkaLabel.height; width: height * (sourceSize.width / sourceSize.height)
  }

  Text {
    y: nootkaLabel.height * 0.72
    anchors.horizontalCenter: logo.horizontalCenter
    font.pixelSize: nootkaLabel.height * 0.2
    color: bgColor
    text: NOO.version()
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: !NOO.isAndroid()
    onEntered: {
      bgColor = NOO.randomColor()
      nootkaLabel.entered()
    }
    onExited: {
      bgColor = "white"
      nootkaLabel.exited()
    }
    onClicked: nootkaLabel.clicked()
  }
}
