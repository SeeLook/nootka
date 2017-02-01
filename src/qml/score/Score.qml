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

  TscoreObject { id: scoreObj; /*parent: score*/ }

  property int clef: Tclef.Treble_G_8down
  property int meter: Tmeter.Meter_4_4
  property alias bgColor: bgRect.color
  property bool enableKeySign: false
  property bool showKeyName: true

  function keySignature() { return enableKeySign ? staff0.keySignature.key : 0 }
  function setKeySignature(key) {
    if (enableKeySign && key !== staff0.keySignature.key)
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
        clef.onTypeChanged: {
          // TODO: approve clef for all staves
        }
        Text { // key name
          visible: showKeyName && enableKeySign
          x: 4.5
          y: 5
          color: activPal.text
          font.pointSize: 1.5
          text: staff0.keySignature ? Noo.majorKeyName(staff0.keySignature.key) + "<br>" + Noo.minorKeyName(staff0.keySignature.key) : ""
        }
      }
  }

  onEnableKeySignChanged: {
    staff0.enableKeySignature(enableKeySign)
  }

  function addNote(n) { scoreObj.addNote(n) }
  function setNote(staff, nr, n) { scoreObj.setNote(staff, nr, n) }
}
