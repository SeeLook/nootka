/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  height: parent.height
  contentHeight: suppFlow.childrenRect.height + thText.height + Noo.fontSize() * 3
  contentWidth: width

//                        bugs    | donate   | record   | translate | MacOs   | vote     | report   | express |
  property var colors: [ "#ff5500", "#42DA06", "#930000", "#0000C0", "#888888", "#C000C0", /*"#FF0000",*/ activPal.text ]
  property var headers: [ "Test this version intensively", qsTr("Donate Nootka campaign"), "Record audio samples",
                          "Translate Nootka", "Mac needs feedback", Noo.isAndroid() ? "Rate this app" : "Vote on Nootka",
                          /*"Report an issue",*/ "Express your opinion" ]
  property var texts: [
    "Simply play with Nootka a lot to check all possible angles.<br><a href=\"https://sourceforge.net/p/nootka/bugs/milestone/Issues%20with%20beta/\">Create Ticket (call a bug)</a> when something doesn't work<br>or doesn't look good enough.",
    "<a href=\"https://nootka.sourceforge.io/index.php?C=donate\">" + qsTr("Through PayPal or a card") + "</a><br><a href=\"mailto:seelook.gmail.com\">" + qsTr("or send email for an account number") + "</a>",
    "Nootka uses natural sounds,<br>so audio samples of bandoneon and saxophones are needed.<br>Home made samples should be sufficient,<br>just <a href=\"mailto:seelook.gmail.com\">write message</a> for details.",
    "It does not require any programming skills.<br>Just read <a href=\"https://sourceforge.net/p/nootka/git/ci/master/tree/lang/how-to-translate.txt\">the instructions</a>,<br>translate and send your work.",
    "Mac Os version is a new thing.<br>Let us know does Nootka work there or not.",
    Noo.isAndroid() ?
    "Go to <a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>,<br>rate it nicely and put a comment in your native language." :
    "There are a lot of services. For example:<br><a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>, <a href=\"https://www.linux-apps.com/p/1127020/\">Linux Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a>",
//     "If you find any issue or a bug than request it through:<br><a href=\"https://sourceforge.net/p/nootka/bugs/\">bug tracker</a>",
    "Simply <a href=\"mailto:seelook.gmail.com\">send an email</a>"
  ]

  Flow {
    id: suppFlow
    width: parent.width - Noo.fontSize() * 2
    height: childrenRect.height
    anchors { margins: Noo.fontSize(); horizontalCenter: parent.horizontalCenter }
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
      Tile {
        visible: !Noo.isAndroid() || index !== 1
        width: Noo.isAndroid() ? suppFlow.width : (tt.width + Noo.fontSize() * 4)
        anchors.horizontalCenter: undefined
        bgBorder { color: colors[index]; width: 2 }
        bgColor: Qt.tint(colors[index], Qt.rgba(activPal.base.r, activPal.base.g, activPal.base.b, 0.9))
        LinkText {
          id: tt
          text: "<b><font size=\"5\" color=\"" + colors[index] +"\">" + headers[index] + "</font></b><br>" + texts[index]
          anchors.horizontalCenter: parent.horizontalCenter
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
