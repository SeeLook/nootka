/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import "fakeTrans.js" as Fake


Dialog {
  visible: true
  title: qsTr("About Nootka")
//  width: pages.width; height: pages.height

  width: nootkaWindow.width * 0.75; height: nootkaWindow.height * 0.75

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

}
