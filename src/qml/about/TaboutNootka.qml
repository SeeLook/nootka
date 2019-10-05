/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


import "about"


Item {
  width: parent.width
  height: parent.height

  PagesDialog { id: pages }

  Component.onCompleted: {
    pages.addItem("about", qsTr("About"), "about/About")
    pages.addItem("help", Noo.TR("QShortcut", "Help"), "about/Help")
    pages.addItem("author", qsTr("Authors"), "about/Authors")
    pages.addItem("license", qsTr("License"), "about/License")
    pages.addItem("support", qsTr("Support"), "about/Support")
    pages.addItem("donors",
                  qsTr("Donors", "Would be 'Sponsors' or even 'Backers' - translate as such as You fill, what sounds/looks better in Your language"),
                  "about/Donors")
    pages.addItem("changes", qsTr("Changes"), "about/Changes")
    pages.addItem("qt", "Qt", "about/Qt")
    if (Noo.isAndroid()) {
        dialLoader.buttons = [DialogButtonBox.Close]
    } else {
        dialLoader.standardButtons = DialogButtonBox.Close
        dialLoader.title = qsTr("About Nootka")
    }
  }

  Connections {
    target: pages.mobileButt
    onClicked: dialLoader.dialogDrawer.open()
  }
}
