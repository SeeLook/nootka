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


Dialog {
  visible: true
  width: nootkaWindow.width * 0.75; height: nootkaWindow.height * 0.75

  title: "Nootka - " + qsTr("application's settings")
//  width: pages.width; height: pages.height

  PagesDialog { id: pages }
  standardButtons: StandardButton.Apply | StandardButton.Cancel | StandardButton.RestoreDefaults | StandardButton.Help

  Component.onCompleted: {
    pages.addItem("global", qsTr("Common"), "Common")
    pages.addItem("scoreSettings", "Score", "Score")
    pages.addItem("guitarSettings", qsTr("Instrument"), "Instrument")
    pages.addItem("soundSettings", qsTr("Sound"), "Sound")
    pages.addItem("questionsSettings", qsTr("Exercises") + "\n& " + qsTr("Exam"), "Exam")
    pages.addItem("appearance", qsTr("Appearance"), "Appearance")
  }

}
