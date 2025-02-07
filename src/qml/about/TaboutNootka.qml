/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import "about"

Item {
    property string mainHelpText: dialLoader.dialogObj.mainHelp()

    function showPage(pageId) {
        pages.selectPage(pageId);
    }

    width: parent.width
    height: parent.height
    Component.onCompleted: {
        pages.addItem("about", qsTr("About"), "about/About");
        pages.addItem("help", NOO.TR("QShortcut", "Help"), "about/Help");
        pages.addItem("author", qsTr("Authors"), "about/Authors");
        pages.addItem("license", qsTr("License"), "about/License");
        pages.addItem("support", qsTr("Support"), "about/Support");
        pages.addItem("donors", qsTr("Donors", "Would be 'Sponsors' or even 'Backers' - translate as such as You fill, what sounds/looks better in Your language"), "about/Donors");
        pages.addItem("changes", qsTr("Changes"), "about/Changes");
        pages.addItem("qt", "Qt", "about/Qt");
        dialLoader.standardButtons = DialogButtonBox.Close;
        dialLoader.title = qsTr("About Nootka");
    }

    PagesDialog {
        id: pages
    }

}
