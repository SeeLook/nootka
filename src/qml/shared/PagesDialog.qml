/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


/**
 * Implements a dialog with navigation list on the left
 * Model has following fields:
 * 'icon', 'text' and 'page' - with path to QML file
 */
Item {
  property alias stack: stack
  property alias model: navList.model
  property alias pages: navList.pages
  property alias currentPage: stack.currentItem
  property var buttons: []

  anchors { fill: parent; leftMargin: Noo.isAndroid() ? 0 : Noo.fontSize() / 4 }

  Rectangle {
    color: Noo.isAndroid() ? "#000000" : "#ffffff"
    z: 2; x: navList.x; y: navList.y; width: navList.width; height: navList.height
  }

  function addItem(icon, text, page) {
    navList.pages.push("qrc:/" + page + "Page.qml")
    model.append({ "iconName": "pane/" + icon, "buttonText": text })
  }

  // private
  property real maxWidth: 0

  Column {
    height: parent.height
    width: navList.width
    spacing: Noo.fontSize() / 4
    z: 3

    // navigation list on the left
    ListView {
      id: navList
      clip: true
      height: parent.height
      width: maxWidth
      z: 3 // above stack
      topMargin: Noo.fontSize() / 2
      property var pages: []
      property PaneButton prevButt: null
      property int prevDelegate: -1

      model: ListModel { id: pageModel }

      spacing: Noo.fontSize() / 4

      highlightFollowsCurrentItem: false
      highlight: Component {
        Rectangle {
          width: navList.prevButt.width; height: navList.prevButt.height + Noo.fontSize() / 4
          color: activPal.highlight
          y: navList.prevButt.y - Noo.fontSize() / 4
          Behavior on y { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1 }}
        }
      }

      delegate: Component {
        PaneButton {
          id: delegateButt
          name: buttonText
          pixmap: Noo.pix(iconName)
          onClicked: {
            if (navList.prevButt !== delegateButt) {
              if (typeof(navList.pages[index]) === "string") {
                var c = Qt.createComponent(navList.pages[index])
                navList.pages[index] = c.createObject(stack)
              }
              navList.pages[index] = stack.replace(navList.pages[index])
              navList.ensureVisible(y, height)
              navList.prevButt = delegateButt
            }
          }
          Component.onCompleted: {
            if (index === navList.prevDelegate) // workaround to avoid loading delegate twice
              return
            navList.prevDelegate = index
            maxWidth = Math.max(navList.width, width)
            buttons.push(delegateButt)
            for (var i = 0; i < buttons.length; ++i) // keep buttons width the same
              buttons[i].width = maxWidth
            if (index === 0) {
              navList.prevButt = delegateButt
              var c = Qt.createComponent(navList.pages[0])
              navList.pages[0] = stack.push(c.createObject(stack))
            }
          }
        }
      }

      ScrollBar.vertical: ScrollBar { active: false }

      function ensureVisible(yy, hh) {
        if (contentY >= yy)
          contentY = yy
        else if (contentY + height <= yy + hh)
          contentY = yy + hh - height
      }

      Behavior on contentY { enabled: GLOB.useAnimations; NumberAnimation { duration: 300; easing.type: Easing.OutBounce }}
    }

  }

  // pages container on the right
  StackView {
    id: stack
    x: navList.width + (Noo.isAndroid() ? 0 : Noo.fontSize() / 4)
    z: -1 // below navigation list
    width: parent.width - navList.width - (Noo.isAndroid() ? 0 : Noo.fontSize() / 4)
    height: parent.height
    // fade animations
    replaceEnter: Transition { enabled: GLOB.useAnimations; PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 500 }}
    replaceExit: Transition { enabled: GLOB.useAnimations; PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 500 }}
  }
}

