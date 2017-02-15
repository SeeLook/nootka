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


Flickable {
  id: flick
  clip: true
  ScrollBar.vertical: ScrollBar {}
  contentHeight: text.paintedHeight; contentWidth: width

  Rectangle {
      anchors.fill: parent
      color: activPal.base
  }

  TextEdit {
      id: text
      width: flick.width
      textMargin: font.pixelSize
      wrapMode: TextEdit.Wrap
      readOnly: true
      textFormat: TextEdit.RichText
      color: activPal.text
      text: Noo.getChanges();
  }
}
 
