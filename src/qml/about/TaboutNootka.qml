/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import "fakeTrans.js" as Fake


Item {
  width: parent.width
  height: parent.height - (Noo.isAndroid() ? 0 : nootkaWindow.fontSize * 3)

  PagesDialog { id: pages }

  Component.onCompleted: {
    pages.addItem("nootka-frame", qsTr("About"), "About")
    pages.addItem("help-frame", Fake.tr("QShortcut", "Help"), "Help")
    pages.addItem("author", qsTr("Authors"), "Authors")
    pages.addItem("license", qsTr("License"), "License")
    pages.addItem("support", qsTr("Support"), "Support")
    pages.addItem("donors", qsTr("Donors", "Would be 'Sponsors' or even 'Backers' - translate as such as You fill, what sounds/looks better in Your language"), "Donors")
    pages.addItem("chlog", qsTr("Changes"), "Changes")
    pages.addItem("qt", "Qt", "Qt")
  }

  function accepted() {} // fake
}
