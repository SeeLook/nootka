/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Flickable {
  width: parent.width; height: parent.height
  clip: true
  contentHeight: suppFlow.childrenRect.height + thText.height + Noo.fontSize() * 3
  contentWidth: Math.max(width, Noo.fontSize() * 35)

  ScrollBar.vertical: ScrollBar {}
//                     | donate   | translate |  vote   | report   | express |
  property var colors: [ "#42DA06", "#0000C0", "#C000C0", "#FF0000", activPal.text ]
  property var headers: [ qsTr("Donate Nootka campaign"), "Translate Nootka", Noo.isAndroid() ? "Rate this app" : "Vote on Nootka", "Report an issue", "Express your opinion" ]
  property var texts: [ 
   "<a href=\"http://nootka.sourceforge.net/index.php?C=donate\">" + qsTr("Through PayPal or a card") + "</a><br><a href=\"mailto:seelook.gmail.com\">" + qsTr("or send email for an account number") + "</a>",
    "It does not require any programming skills. Just read <a href=\"http://sourceforge.net/p/nootka/hg/ci/default/tree/lang/how-to-translate.txt\">the instructions</a>,<br>translate and send your work.",
    Noo.isAndroid() ?
    "Go to <a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>,<br>rate it nicely and put a comment in your native language." :
    "There are a lot of services. For example:<br><a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>, <a href=\"https://www.linux-apps.com/p/1127020/\">Linux Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a><br>or add <b>like</b> or/and <b>g+</b> on <a href=\"http://nootka.sourceforge.net\">Nootka site</a>",
    "If you find any issue or a bug than request it through:<br><a href=\"http://sourceforge.net/p/nootka/bugs/\">bug tracker</a>",
    "Simply <a href=\"mailto:seelook.gmail.com\">send an email</a>"
  ]

  Flow {
    id: suppFlow
    width: parent.width
    height: childrenRect.height
    anchors.margins: Noo.fontSize()
    spacing: Noo.fontSize() / 2

    Tile {
      width: parent.width
      anchors.horizontalCenter: undefined
      bgColor: activPal.highlight
      Text {
        text: qsTr("You also can help with making Nootka better.")
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        font { pixelSize: Noo.fontSize() * 2; bold: true }
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.highlightedText
      }
    }
    Tile {
      anchors.horizontalCenter: undefined
      bgBorder { color: activPal.highlight; width: 2 }
      Text {
        width: parent.width * 0.96
        text: qsTr("It requires little bit English, so if you can read a text below there will be something you may get involved.")
        font { pixelSize: Noo.fontSize() * 1.1 }
        wrapMode: Text.WordWrap
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text
      }
    }

    Repeater {
      model: colors.length
      delegate: Component {
        Tile {
          visible: !Noo.isAndroid() || index != 0
          width: tt.width + Noo.fontSize() * 4
          anchors.horizontalCenter: undefined
          bgBorder { color: colors[index]; width: 2 }
          bgColor: Qt.tint(colors[index], Qt.rgba(activPal.base.r, activPal.base.g, activPal.base.b, 0.9))
          Text {
            id: tt
            text: "<b><font size=\"5\" color=\"" + colors[index] +"\">" + headers[index] + "</font></b><br>" + texts[index]
            anchors.horizontalCenter: parent.horizontalCenter
            onLinkActivated: Qt.openUrlExternally(link)
            color: activPal.text
            MouseArea {
              anchors.fill: parent
              acceptedButtons: Qt.NoButton
              cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
          }
        }
      }
    }

  }

  Tile {
    width: Noo.fontSize() * 20
    anchors { horizontalCenter: parent.horizontalCenter; top: suppFlow.bottom }
    bgColor: activPal.highlight
    bgBorder { color: activPal.highlightedText; width: 2 }
    Text {
      id: thText
      text: "Thanks in advance!<br>Author"
      font { pixelSize: Noo.fontSize() * 1.7 }
      horizontalAlignment: Text.AlignRight
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.highlightedText
    }
  }
}
