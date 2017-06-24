/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2


ToolBar {
  id: toolBar
  height: settAct.height

  property alias scoreAct: scoreAct

  Rectangle { // background
    anchors.fill: parent
    color: activPal.window
  }

  RowLayout {
    HeadButton { id: settAct; action: nootkaWindow.settingsAct }
    HeadButton { action: nootkaWindow.levelAct }
//     HeadButton {
//       id: chartAct
//       name: qsTr("Analyze", "could be Chart as well")
//       icon: Tpath.pix("charts")
//       tip: qsTr("Analysis of exam results")
//     }
    HeadButton { id: scoreAct; action: nootkaWindow.scoreAct }
    HeadButton { action: nootkaWindow.examAct }

    Item { Layout.fillWidth: true }
  }
  NootkaLabel {
    anchors.right: parent.right
    height: toolBar.height
    onClicked: nootkaWindow.aboutAct.trigger()
  }

}
