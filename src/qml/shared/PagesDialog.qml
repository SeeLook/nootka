/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  anchors { fill: parent; leftMargin: NOO.isAndroid() ? 0 : NOO.factor() / 4 }

  function addItem(icon, text, page) {
    navList.pages.push("qrc:/" + page + "Page.qml")
    model.append({ "iconName": "pane/" + icon, "buttonText": text })
  }

  // private
  property real maxWidth: 0

  // navigation list on the left
  ListView {
    id: navList
    clip: true
    height: parent.height
    width: maxWidth
    topMargin: NOO.factor() / 2
    property var pages: []
    property PaneButton prevButt: null
    property int prevDelegate: -1

    model: ListModel { id: pageModel }

    spacing: NOO.factor() / 4

    highlightFollowsCurrentItem: false
    highlight: Rectangle {
      width: navList.prevButt.width; height: navList.prevButt.height + NOO.factor() / 4
      color: activPal.highlight
      y: navList.prevButt.y - NOO.factor() / 4
      Behavior on y { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1 }}
    }

    delegate: PaneButton {
      id: delegateButt
      name: buttonText
      pixmap: NOO.pix(iconName)
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

    Rectangle {
      color: NOO.isAndroid() ? "#000000" : "#ffffff"
      anchors.fill: parent
      z: -1
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

  // pages container on the right
  StackView {
    id: stack
    clip: true
    x: navList.width + (NOO.isAndroid() ? 0 : NOO.factor() / 4)
    width: parent.width - navList.width - (NOO.isAndroid() ? 0 : NOO.factor() / 4)
    height: parent.height
    replaceEnter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "x"; from: width; to: 0 }}
    replaceExit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "x"; from: 0; to: -width }}
  }
}

