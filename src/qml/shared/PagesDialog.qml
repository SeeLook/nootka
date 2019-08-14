/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  property alias mobileButt: mobBut

  anchors { fill: parent; leftMargin: Noo.fontSize() / 4 }

  Rectangle { z: 2; color: activPal.base; x: navList.x; y: navList.y; width: navList.width; height: navList.height }
  Rectangle { // highlight
    id: butHigh;
    color: activPal.highlight; x: navList.x
    parent: navList.contentItem
    y: navList.prevButt ? navList.prevButt.y : 0
    Behavior on y { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1; duration: 500 }}
  }

  function addItem(icon, text, page) {
    navList.pages.push("qrc:/" + page + "Page.qml")
    model.append({ "iconName": icon, "buttonText": text })
  }

  Column {
    height: parent.height
    width: navList.width
    spacing: Noo.fontSize() / 4
    z: 3

    // navigation list on the left
    ListView {
      id: navList
      clip: true
      height: parent.height - (Noo.isAndroid() ? mobBut.height : 0)
      z: 3 // above stack
      property var pages: []
      property HeadButton prevButt: null
      property int prevDelegate: -1

      model: ListModel { id: pageModel }

      delegate: Component {
        HeadButton {
          id: delegateButt
          name: buttonText
          pixmap: Noo.pix(iconName)
          factor: Noo.fontSize() * (Noo.isAndroid() ? 0.5 : 0.7)
          fontSize: Noo.fontSize() * (Noo.isAndroid() ? 0.8 : 1)
          hiHover: false
          onClicked: {
            if (navList.prevButt !== delegateButt) {
              if (typeof(navList.pages[index]) === "string") {
                var c = Qt.createComponent(navList.pages[index])
                navList.pages[index] = c.createObject(stack)
              }
              navList.pages[index] = stack.replace(navList.pages[index])
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
            buttons.push(delegateButt)
            for (var i = 0; i < buttons.length; ++i) // keep buttons width the same
              buttons[i].width = w
            if (index === 0) {
              navList.prevButt = delegateButt
              textColor = activPal.highlightedText
              butHigh.width = delegateButt.width
              butHigh.height = delegateButt.height
              var c = Qt.createComponent(navList.pages[0])
              navList.pages[0] = stack.push(c.createObject(stack))
            }
          }
        }
      }

      ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

      function ensureVisible(yy, hh) {
        if (contentY >= yy)
          contentY = yy
        else if (contentY + height <= yy + hh)
          contentY = yy + hh - height
      }

      Behavior on contentY { enabled: GLOB.useAnimations; NumberAnimation { duration: 300; easing.type: Easing.OutBounce }}
    }

    Button {
      id: mobBut
      visible: Noo.isAndroid()
      width: navList.width
      height: Noo.fontSize() * 2
      background: Rectangle {
        anchors.fill: parent
        color: mobBut.down ? activPal.highlight : activPal.text
      }
      contentItem: Text {
        text: ". . ."
        font.bold: true
        color: mobBut.down ? activPal.highlightedText : activPal.base
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }
    }

  }

  // pages container on the right
  StackView {
    id: stack
    x: navList.width + Noo.fontSize() / 4
    z: -1 // below navigation list
    width: parent.width - navList.width
    height: parent.height
    // fade animations
    replaceEnter: Transition { enabled: GLOB.useAnimations; PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 500 }}
    replaceExit: Transition { enabled: GLOB.useAnimations; PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 500 }}
  }
}

