/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  id: authorsPage

  contentHeight: aboutCont.height
  contentWidth: width

  MouseArea {
    anchors.fill: parent
    onClicked: scrollTimer.running = !scrollTimer.running && GLOB.useAnimations
  }

  Column {
    id: aboutCont
    spacing: NOO.factor() / 4
    padding: NOO.factor() / 2
    width: parent.width

    Tile {
      bgColor: activPal.base
      Column {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TextBackground { text: qsTranslate("TaboutNootka", "Code") }
        LinkText {
          anchors.horizontalCenter: parent.horizontalCenter
          text: "<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br><b>"
            + ("<a style=\"text-decoration: none; color: %1\" href=\"https://www.opencode.net/seelook/nootka\">https://www.opencode.net/seelook/nootka</a></b>").arg(activPal.text)
          textFormat: Text.RichText; horizontalAlignment: Text.AlignHCenter
        }
      }
    }

    Tile {
      bgColor: activPal.alternateBase
      Column {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TextBackground { text: qsTranslate("TaboutNootka", "Audio") }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter; color: activPal.text
          text: qsTranslate("TaboutNootka", "editing and/or recording of samples:") + "<br><b>Sergei Ivanov (tico-tico)</b><br>"
        }
      }
    }

    Tile {
      bgColor: activPal.base
      Column {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TextBackground { text: qsTranslate("TaboutNootka", "Translators") }

        ListModel {
          id: trModel
          ListElement { flag:"cs"; lang: "Český";       trr: "Pavel Fric";      web: "<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>" }
          ListElement { flag:"de"; lang: "Deutsch";     trr: "Johann C. Weihe,<br>Renato Reinau"; web: "" }
          ListElement { flag:"es"; lang: "Español";     trr: "José Luis Marín"; web: "<a href=\"mailto:jsls@gmx.com\">jsls@gmx.com</a>" }
          ListElement { flag:"fr"; lang: "Français";    trr: "Olivier Devineau,<br>Jean-Marc Lartigue"; web: "" }
          ListElement { flag:"hu"; lang: "Magyar";      trr: "Gábor Kertész";   web: "<a href=\"mailto:kergab@gmail.com\">kergab@gmail.com</a>"  }
          ListElement { flag:"it"; lang: "Italiano";    trr: "Gianluca 'Kununna' Fiorentino"; web: "" }
          ListElement { flag:"pl"; lang: "polski";      trr: "Tomasz Bojczuk";  web: "<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>" }
          ListElement { flag:"ru"; lang: "Русский";     trr: "Sergei Ivanov (tico-tico),<br>Timur Artykov"; web: "" }
          ListElement { flag:"sl"; lang: "Slovenščina"; trr: "Grega Trček";     web: "" }
        }

        Column {
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: NOO.factor() / 2
          Repeater {
            model: trModel
            Row {
              spacing: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1)
              Image {
                height: NOO.factor() * 3.5; width: height
                source: NOO.pix("flags/" + flag)
                anchors.verticalCenter: parent.verticalCenter
              }
              Text {
                text: lang; textFormat: Text.PlainText; color: activPal.text
                width: NOO.factor() * (NOO.isAndroid() ? 5 : 8)
                font.pixelSize: NOO.factor() * (NOO.isAndroid() ? 0.8 : 1)
                anchors.verticalCenter: parent.verticalCenter
              }
              Text {
                text: trr; color: activPal.text; font.bold: true;
                textFormat: Text.StyledText; width: NOO.factor() * (web === "" ? 24 : 16)
                fontSizeMode: Text.Fit; minimumPixelSize: NOO.factor() / 2
                anchors.verticalCenter: parent.verticalCenter
              }
              LinkText {
                text: web; anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: NOO.factor() * (NOO.isAndroid() ? 0.8 : 1)
              }
            }
          }
        }
      }
    }

    Tile {
      bgColor: activPal.alternateBase
      Column {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TextBackground { text: qsTranslate("TaboutNootka", "Other projects") }
        LinkText {
          x: NOO.factor() * 1.5
          width: aboutCont.width  - NOO.factor() * 3
          text: qsTranslate("TaboutNootka", "However this application could not exist without various open source projects.<br>Especially:") +
                "<ul>" + createLink("Qt", "https://www.qt.io/developers/") + " by Qt Company" +
                createLink("FFTW", "http://www.fftw.org") + " by M. Frigo & S. G. Johnson" +
                createLink("ogg vorbis", "http://vorbis.com") + " by XIPH" +
                createLink("RtAudio"/* + " & RtMidi"*/, "https://www.music.mcgill.ca/~gary/rtaudio/index.html") + " by G. P. Scavone" +
                createLink("Tartini", "http://miracle.otago.ac.nz/tartini/index.html") + " by P. McLeod" +
                createLink("SoundTouch " + SOUND.soundTouchVersion(), "http://www.surina.net/soundtouch/") + " by Olli Parviainen" +
                createLink("LilyPond emmentaler font", "http://lilypond.org/introduction.html") +
                createLink("Bravura SMuFL font", "https://www.smufl.org/fonts/") +
                createLink("Country flags images", "https://github.com/joielechong/iso-country-flags-svg-collection") +
                "</ul>"
          wrapMode: Text.WordWrap
        }
      }
    }

    Tile {
      bgColor: activPal.base
      Column {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TextBackground { text: qsTranslate("TaboutNootka", "Thanks") }
        LinkText {
          x: NOO.factor() * 1.5
          width: aboutCont.width  - NOO.factor() * 3
          text: "I would like to say <b>THANK YOU</b> for all people who helped with developing Nootka.<br>
    Let's try to mention them in some random order:<br>
    <br><b>José Luis Marín</b> for patient and intensive tests under Linux and great feedback.<br>
    <b>Aaron Wolf</b> <a href=\"http://blog.wolftune.com/\">http://blog.wolftune.com</a> for many warm words about Nootka in the web and helping clues.<br>
    <b>falkTX</b> from <a href=\"http://kxstudio.sourceforge.net/\">http://kxstudio.sourceforge.net</a> for building *.deb and testing and for many clues.<br>
    <b>Users</b> of <a href=\"http://www.linuxmusicians.com/\">http://www.linuxmusicians.com</a> forum for testing and comments.<br>
    <b>Olli Parviainen</b> <a href=\"http://www.surina.net/soundtouch/\">http://www.surina.net/soundtouch</a> for help with his SoundTouch library.<br>
    <b>Sergei Ivanov</b> for testing Nootka intensively, bug hunting and many valuable notices.<br>
    <b>Translators (Pavel, Olivier, Sergei, Jean-Marc, José Luis and Johann)</b> for many, maaaany clues and comments.<br>
    <b>Project16 @ KVR</b> <a href=\"http://www.kvraudio.com/\">http://www.kvraudio.com</a> for the bass guitar samples<br>
    <b>And all others that helped.</b><br>"
          textFormat: Text.StyledText
          wrapMode: Text.WordWrap
        }
      }
    }

  } // Column aboutCont

  Timer {
    id: scrollTimer
    interval: 20; repeat: true; running: GLOB.useAnimations && authorsPage.visible
    onTriggered: {
      if (authorsPage.visible && authorsPage.contentY < authorsPage.contentHeight - authorsPage.height)
        authorsPage.contentY++
    }
  }

  function createLink(desc, href) {
    return "<li><a href=\"" + href + "\">" + desc + "</a></li>";
  }
}


