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

import Score 1.0


Flickable {
  id: score

  TscoreObject {
    id: scoreObj
    width: score.width / scale
    onStaffCreate: {
      var c = Qt.createComponent("qrc:/Staff.qml")
      var prevStaffId = staves.length - 1
      var lastStaff = c.createObject(score.contentItem, { "number": prevStaffId + 1, "clef.type": score.clef })
      staves.push(lastStaff)
      lastStaff.enableKeySignature(enableKeySign)
      score.contentY = score.contentHeight - score.height
      lastStaff.keySignature.onKeySignatureChanged.connect(setKeySignature)
      lastStaff.onDestroing.connect(removeLastStaff)
      if (enableKeySign)
        lastStaff.keySignature.key = keySignature()
    }
    onStavesHeightChanged: score.contentHeight = Math.max(stavesHeight, score.height)
    function removeLastStaff() { staves.pop() } // remove last staff from staves[] list
  }

  property alias scale: staff0.scale
  property int clef: Tclef.Treble_G_8down
  property alias meter: scoreObj.meter
  property alias bgColor: bgRect.color
  property bool enableKeySign: false
  property bool showKeyName: true
  property var staves: []

  function keySignature() { return enableKeySign ? staff0.keySignature.key : 0 }

  function setKeySignature(key) {
    if (enableKeySign) {
      for (var s = 0; s < staves.length; ++s)
        if (key !== staves[s].keySignature.key)
          staves[s].keySignature.key = key
    }
  }

  clip: true
  width: parent.width

  contentWidth: score.width
//   contentHeight: Math.max(scoreObj.stavesHeight, score.height)

  Rectangle {
    id: bgRect
    anchors.fill: score.contentItem
    color: activPal.base
  }

  Staff {
    id: staff0
    number: 0
    clef.type: score.clef
    meter: Meter { parent: staff0 }
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
    Component.onCompleted: staves.push(staff0)
  }

  onEnableKeySignChanged: {
    staff0.enableKeySignature(enableKeySign)
    if (enableKeySign)
      staff0.keySignature.onKeySignatureChanged.connect(setKeySignature)
    for (var s = 1; s < staves.length; ++s) {
      staves[s].enableKeySignature(enableKeySign)
      if (enableKeySign)
        staff0.keySignature.onKeySignatureChanged.connect(setKeySignature)
    }
  }

  function addNote(n) { scoreObj.addNote(n) }
  function setNote(staff, nr, n) { scoreObj.setNote(staff, nr, n) }
}
