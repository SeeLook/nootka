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
    spacing: Noo.fontSize()
    padding: Noo.fontSize() / 2
    width: parent.width
    TextBackground { text: qsTranslate("TaboutNootka", "Code"); }
    LinkText {
      anchors.horizontalCenter: parent.horizontalCenter
      text: "<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>"
    }
    TextBackground { text: qsTranslate("TaboutNootka", "Audio"); }
    Text {
      anchors.horizontalCenter: parent.horizontalCenter; color: activPal.text
      text: qsTranslate("TaboutNootka", "editing and/or recording of samples:") + "<br><b>Sergei Ivanov (tico-tico)</b><br>"
    }
    TextBackground { text: qsTranslate("TaboutNootka", "Translators"); }

    ListModel {
      id: trModel
      ListElement { flag:"cs"; lang: "český";       trr: "Pavel Fric";      web: "<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>" }
      ListElement { flag:"de"; lang: "deutsch";     trr: "Johann C. Weihe,<br>Renato Reinau"; web: "" }
      ListElement { flag:"es"; lang: "español";     trr: "José Luis Marín"; web: "<a href=\"mailto:jsls@gmx.com\">jsls@gmx.com</a>" }
      ListElement { flag:"fr"; lang: "français";    trr: "Olivier Devineau,<br>Jean-Marc Lartigue"; web: "" }
      ListElement { flag:"hu"; lang: "magyar";      trr: "Gábor Kertész";   web: "<a href=\"mailto:kergab@gmail.com\">kergab@gmail.com</a>"  }
      ListElement { flag:"it"; lang: "italiano";    trr: "Gianluca 'Kununna' Fiorentino"; web: "" }
      ListElement { flag:"pl"; lang: "polski";      trr: "Tomasz Bojczuk";  web: "<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>" }
      ListElement { flag:"ru"; lang: "русский";     trr: "Sergei Ivanov (tico-tico),<br>Timur Artykov"; web: "" }
      ListElement { flag:"sl"; lang: "slovenščina"; trr: "Grega Trček";     web: "" }
    }

    Column {
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize() / 2
      Repeater {
        model: trModel
        Row {
          spacing: Noo.fontSize() * (Noo.isAndroid() ? 0.25 : 1)
          Image {
            height: Noo.fontSize() * 3.5; width: height
            source: Noo.pix("flags/" + flag)
            anchors.verticalCenter: parent.verticalCenter
          }
          Text {
            text: lang; textFormat: Text.PlainText; color: activPal.text
            width: Noo.fontSize() * (Noo.isAndroid() ? 5 : 8)
            font.pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 0.8 : 1)
            anchors.verticalCenter: parent.verticalCenter
          }
          Text {
            text: trr; color: activPal.text; font.bold: true;
            textFormat: Text.StyledText; width: Noo.fontSize() * (web === "" ? 24 : 16)
            fontSizeMode: Text.Fit; minimumPixelSize: Noo.fontSize() / 2;  minimumPointSize: minimumPixelSize
            anchors.verticalCenter: parent.verticalCenter
          }
          LinkText {
            text: web; anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 0.8 : 1)
          }
        }
      }
    }

    TextBackground { text: qsTranslate("TaboutNootka", "Other projects"); }
    LinkText {
      width: parent.width
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
    TextBackground { text: qsTranslate("TaboutNootka", "Thanks"); }
    LinkText {
      width: parent.width
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


