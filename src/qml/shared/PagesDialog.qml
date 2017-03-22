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
import QtQuick.Window 2.0


/**
 * Implements a dialog with navigation list on the left
 * Model has following fields:
 * 'icon', 'text' and 'page' - with path to qml file
 */
Item {

  property alias stack: stack
  property alias model: navList.model
  property alias pages: navList.pages
  property alias currentPage: stack.currentItem

  anchors.fill: parent

  Rectangle { color: activPal.base; x: navList.x; y: navList.y; width: navList.width; height: navList.height }
  Rectangle { // highlight
    id: butHigh;
    color: activPal.highlight; x: navList.x
    parent: navList.contentItem
    Behavior on y { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1; duration: 500 }}
  }

  function addItem(icon, text, page) {
    navList.pages.push("qrc:/" + page + "Page.qml")
    model.append({ "iconName": icon, "buttonText": text })
  }

  Row {
    anchors { fill: parent }
    spacing: Screen.pixelDensity * 2

    // navigation list on the left
    ListView {
      id: navList
      clip: true
      height: parent.height
      property var buttons: []
      property var pages: []
      property HeadButton prevButt: null
      property int prevDelegate: -1

      model: ListModel { id: pageModel }

      delegate: Component {
        HeadButton {
          id: delegateButt
          name: buttonText
          icon: Tpath.pix(iconName)
          factor: Screen.pixelDensity * (Noo.isAndroid() ? 1.2 : 1.6)
          fontSize: nootkaWindow.fontSize * (Noo.isAndroid() ? 0.8 : 1.0)
          onPressed: {
            if (stack.currentItem !== navList.pages[index]) {
              stack.push(navList.pages[index])
              navList.pages[index] = stack.currentItem
              butHigh.y = y
              butHigh.height = height
              navList.ensureVisible(y, height)
              navList.prevButt.textColor = activPal.text
              textColor = activPal.highlightedText
              navList.prevButt = delegateButt
            }
          }
          Component.onCompleted: {
            if (index === navList.prevDelegate) // workaround to avoid loading delegate twice
              return
            navList.prevDelegate = index
            var w = Math.max(navList.width, width)
            navList.width = w
            butHigh.width = w
            navList.buttons.push(delegateButt)
            for (var i = 0; i < navList.buttons.length; ++i) // keep buttons width the same
              navList.buttons[i].width = w
            if (index === 0) {
              navList.prevButt = delegateButt
              textColor = activPal.highlightedText
              butHigh.y =  delegateButt.y
              butHigh.width = delegateButt.width
              butHigh.height = delegateButt.height
              stack.push(navList.pages[0])
              navList.pages[0] = stack.currentItem
            }
          }
        }
      }

      ScrollBar.vertical: ScrollBar { active: true }

      function ensureVisible(yy, hh) {
        if (contentY >= yy)
            contentY = yy;
        else if (contentY + height <= yy + hh)
            contentY = yy + hh - height;
      }

      Behavior on contentY { enabled: GLOB.useAnimations; NumberAnimation { duration: 300; easing.type: Easing.OutBounce }}
    }

    // pages container on the right
    StackView {
      id: stack
      width: parent.width - navList.width - Screen.pixelDensity * 2
      height: parent.height
      // fade animations
      pushEnter: Transition { enabled: GLOB.useAnimations; PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }}
      pushExit: Transition { enabled: GLOB.useAnimations; PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 300 }}
    }
  }
}
 
