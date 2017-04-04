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

import Nootka 1.0


Column {
  property alias sufix: sufixField.text
  property int noteOne: 3
  property int alterOne: 0
  property int noteTwo: 7
  property int alterTwo: -1
  property int nameStyle: 0

  Text {
    wrapMode: Text.NoWrap
    text: qsTranslate("TscoreSettings", "in the major keys:")
    anchors.horizontalCenter: parent.horizontalCenter
    color: enabled ? activPal.text : disdPal.text
  }
  TextField  {
    id: sufixField
    maximumLength: 10
    horizontalAlignment: TextInput.AlignHCenter
  }
  Text {
    text: qsTranslate("TscoreSettings", "for example")
    anchors.horizontalCenter: parent.horizontalCenter
    color: enabled ? activPal.text : disdPal.text
  }
  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    color: enabled ? activPal.text : disdPal.text
    font { bold: true; pixelSize: nootkaWindow.fontSize * 1.5 }
    text: Noo.noteName(Noo.note(noteOne, 0, alterOne), nameStyle, false) + (sufix === "" || sufix === " " ? "" : "-" + sufixField.text) + "\n"
        + Noo.noteName(Noo.note(noteTwo, 0, alterTwo), nameStyle, false) + (sufix === "" || sufix === " " ? "" : "-" + sufixField.text)
  }
}
