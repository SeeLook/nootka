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
  title: "Nootka - " + qsTr("application's settings")
  visible: true
//  width: pages.width; height: pages.height

  width: nootkaWindow.width * 0.75; height: nootkaWindow.height * 0.75

  PagesDialog {
    id: pages
    anchors.fill: parent
    model: ListModel { id: pageModel }
    Component.onCompleted: {
      pageModel.append({"iconName": "global", "buttonText": qsTr("Common"), "page": "Common"})
      pageModel.append({"iconName": "scoreSettings", "buttonText": "Score", "page": "Score"})
      pageModel.append({"iconName": "guitarSettings", "buttonText": qsTr("Instrument"), "page": "Instrument"})
      pageModel.append({"iconName": "soundSettings", "buttonText": qsTr("Sound"), "page": "Sound"})
      pageModel.append({"iconName": "questionsSettings", "buttonText": qsTr("Exercises") + "\n& " + qsTr("Exam"), "page": "Exam"})
      pageModel.append({"iconName": "appearance", "buttonText": qsTr("Appearance"), "page": "Appearance"})
    }
  }
  standardButtons: StandardButton.Apply | StandardButton.Cancel | StandardButton.RestoreDefaults | StandardButton.Help

}
