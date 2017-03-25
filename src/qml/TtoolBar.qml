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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0


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
