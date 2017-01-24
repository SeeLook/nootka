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

import Score 1.0


Flickable {
  id: score

  property int clef: Tclef.Treble_G_8down
  property alias bgColor: bgRect.color
  property alias enableKeySign: staff0.enableKeySign

  function keySignature() { return enableKeySign ? staff0.keySignature.key : 0 }
  function setKeySignature(key) {
    if (staff0.enableKeySign && key !== staff0.keySignature.key)
      staff0.keySignature.key = key
  }

  width: parent.width

  contentWidth: score.width
  contentHeight: score.height

  Rectangle {
    id: bgRect
    anchors.fill: score.contentItem
    color: activPal.base
  }

  Column {
    Staff {
      id: staff0
      number: 0
      clef.type: score.clef
      enableKeySign: true
      clef.onTypeChanged: {
        // TODO: approve clef for all staves
      }
    }
  }

}
