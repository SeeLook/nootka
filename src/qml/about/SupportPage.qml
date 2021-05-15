/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  height: parent.height
  contentHeight: suppFlow.childrenRect.height + thText.height + NOO.factor() * 3
  contentWidth: width

  Flow {
    id: suppFlow
    width: parent.width - NOO.factor() * 2
    height: childrenRect.height
    anchors { margins: NOO.factor(); horizontalCenter: parent.horizontalCenter }
    spacing: NOO.factor() / 2

    Tile {
      width: parent.width
      anchors.horizontalCenter: undefined
      bgColor: activPal.highlight
      Text {
        text: qsTr("You also can help with making Nootka better.")
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        font { pixelSize: NOO.factor() * 2; bold: true }
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
        font { pixelSize: NOO.factor() * 1.1 }
        wrapMode: Text.WordWrap
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text
      }
    }

    ListModel {
      id: suppModel
      //           desktop | Android
      //ListElement qsTr("Donate Nootka campaign") : onCompleted | D 0
      ListElement { // D 1 | A 0
        accent: "#930000"
        header: "Record audio samples"
        message: "Nootka uses natural sounds,<br>so audio samples of bandoneon and saxophones are needed.<br>Also new ones of electric and bass guitar will be needed soon.<br>Home made samples should be sufficient.<br><a href=\"https://www.opencode.net/seelook/nootka/blob/master/CONTRIBUTING.md#record\">Take a look here</a> or just <a href=\"mailto:seelook.gmail.com\">write message</a> for details."
      }
      ListElement { // D 2 | A 1
        accent: "#0000C0"
        header: "Translate Nootka"
        message: "It does not require any programming skills.<br>Just read <a href=\"https://www.opencode.net/seelook/nootka/blob/master/lang/how-to-translate.md\">the instructions</a>,<br>translate and send your work."
      }
      ListElement { // D 3 | A 2
        accent: "#888888"
        header: "Mac needs feedback"
        message: "Mac Os version is a new thing.<br>Let us know does Nootka work there or not."
      }
      //ListElement { NOO.isAndroid() ? "Rate this app" : "Vote on Nootka" // D 4 | A 3
      ListElement { // D 5 | A 4
        accent: "teal"
        header: "Create a tutorial"
        message: "Take some use case and make video of it or write it down with a few screenshots.<br>Send it somewhere (YouTube, some blog) or here, to Nootka.<br>It may help others a lot."
      }
      ListElement { // D 6 | A 5
        accent: "#fff"
        header: "$> Hacking Nootka code"
        message: "<font color=\"#fff\">If You know QML or C++ You could give a hand.<br>
                <a href=\"https://www.opencode.net/seelook/nootka/blob/master/CONTRIBUTING.md#record\">Visit this link for further instructions.</a></font>"
      }
      ListElement { // D 7 | A 6
        accent: "#FF0000"
        header: "Report an issue"
        message: "If you find any issue or a bug than request it through:<br><a href=\"https://sourceforge.net/p/nootka/bugs/\">bug tracker</a>"
      }
      //ListElement "Express your opinion" D 8 | A 7

      Component.onCompleted: {
        // HACK: ListElement can handle only static data, but not any function like qsTr or so.
        // So insert/append such a data with script here
        if (!NOO.isAndroid()) {
          insert(0, {
            "accent": "#42DA06", "header": qsTr("Donate Nootka campaign"),
            "message": "<a href=\"https://nootka.sourceforge.io/index.php?C=donate\">" + qsTr("Through PayPal or a card") + "</a><br><a href=\"mailto:seelook.gmail.com\">" + qsTr("or send email for an account number") + "</a>"
          })
        }
        insert(NOO.isAndroid() ? 2 : 3, {
          "accent": "#C000C0", "header": NOO.isAndroid() ? "Rate this app" : "Vote on Nootka",
          "message": NOO.isAndroid() ?
          "Go to <a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>,<br>rate it nicely and put a comment in your native language." :
          "There are a lot of services. For example:<br><a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>, <a href=\"https://www.linux-apps.com/p/1127020/\">Linux Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a>"
        })
        append({ "accent": "#000", "header": "Express your opinion", "message": "Simply <a href=\"mailto:seelook.gmail.com\">send an email</a>" })
        get(count - 1).accent = activPal.text
      }
    }

    Repeater {
      model: suppModel
      Tile {
        width: tt.width + NOO.factor() * 4
        anchors.horizontalCenter: undefined
        bgBorder { color: accent; width: 2 }
        bgColor: Qt.tint(accent, Qt.rgba(activPal.base.r, activPal.base.g, activPal.base.b, 0.9))
        LinkText {
          id: tt
          text: "<b><font size=\"5\" color=\"" + accent + "\">" + header + "</font></b><br>" + message
          anchors.horizontalCenter: parent.horizontalCenter
        }
        Component.onCompleted: {
          if (accent === "#fff")
            bgColor = "#000" // hacking bg color is black
        }
      }
    }

  }

  Tile {
    width: NOO.factor() * 20
    anchors { horizontalCenter: parent.horizontalCenter; top: suppFlow.bottom }
    bgColor: activPal.highlight
    bgBorder { color: activPal.highlightedText; width: 2 }
    Text {
      id: thText
      text: "Thanks in advance!<br>Author"
      font { pixelSize: NOO.factor() * 1.7 }
      horizontalAlignment: Text.AlignRight
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.highlightedText
    }
  }
}
