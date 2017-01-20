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
import TtickColors 1.0
import QtGraphicalEffects 1.0


Item {
  id: intoBar

  property real pitch: 0.1

  onWidthChanged: tc.resize(intoBar.width - noteText.width * 3)

  TtickColors { id: tc }

  Repeater {
    id: iRepLeft
    model: (intoBar.width - noteText.width * 3) / 2 / (pitchView.tickGap + pitchView.tickWidth) - 1
    Rectangle {
      color: activPal.text
//      color: {
//        var tickNr = iRep.model * index / 100
//        tickNr <= volBar.volume * 100 ? tc.colorAt(tickNr) : disdPal.text
//      }
      width: pitchView.tickWidth
      radius: pitchView.tickWidth / 2
      height: pitchView.tickWidth * 1.5 + ((intoBar.height - pitchView.tickWidth * 4) / iRepLeft.model) * (iRepLeft.model - index)
      y: (parent.height - height) / 2
      x: (index * pitchView.tickGap) + (index + 2) * pitchView.tickWidth
    }
  }

  Text {
    id: noteText
    anchors.top: parent.Top
    x: (intoBar.width - width) / 2
    anchors.verticalCenter: parent.verticalCenter
    font.family: "Nootka"
    font.pixelSize: intoBar.height * 0.8
    text: "n"
  }

  Repeater {
    id: iRepRight
    model: (intoBar.width - noteText.width * 3) / 2 / (pitchView.tickGap + pitchView.tickWidth) - 1
    Rectangle {
      color: activPal.text
//      color: {
//        var tickNr = iRep.model * index / 100
//        tickNr <= volBar.volume * 100 ? tc.colorAt(tickNr) : disdPal.text
//      }
      width: pitchView.tickWidth
      radius: pitchView.tickWidth / 2
      height: pitchView.tickWidth * 1.5 + ((intoBar.height - pitchView.tickWidth * 4) / iRepRight.model) * index
      y: (parent.height - height) / 2
      x: noteText.x + noteText.width * 2 + (index * pitchView.tickGap) + (index + 2) * pitchView.tickWidth
    }
  }

}
