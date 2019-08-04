/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
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
    Grid {
      columns: 4
      spacing: Noo.fontSize() / 2
      columnSpacing: Noo.fontSize() * 2; rowSpacing: Noo.fontSize() / 2
      verticalItemAlignment: Grid.AlignVCenter
      anchors.horizontalCenter: parent.horizontalCenter

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-cs"); }
      Text { text: "český"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "Pavel Fric "; color: activPal.text; font.bold: true }
      LinkText { text: "<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>" }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-de") }
      Text { text: "deutsch"; textFormat: Text.StyledText; color: activPal.text}
      Text { text: "Johann C. Weihe,<br>Renato Reinau"; font.bold: true; color: activPal.text }
      Item { width: 10; height: 10 }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-es") }
      Text { text: "español"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "José Luis Marín"; font.bold: true; color: activPal.text }
      LinkText { text: "<a href=\"mailto:jsls@gmx.com\">jsls@gmx.com</a>" }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-fr") }
      Text { text: "français"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "Olivier Devineau,<br>Jean-Marc Lartigue"; font.bold: true; color: activPal.text }
      Item { width: 10; height: 10 }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-hu") }
      Text { text: "magyar"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "Gábor Kertész"; font.bold: true; color: activPal.text }
      LinkText { text: "<a href=\"mailto:kergab@gmail.com\">kergab@gmail.com</a>" }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-pl") }
      Text { text: "polski"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "Tomasz Bojczuk"; font.bold: true; color: activPal.text }
      LinkText { text: "<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>" }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-ru") }
      Text { text: "русский"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "Sergei Ivanov (tico-tico),<br>Timur Artykov"; font.bold: true; color: activPal.text }
      Item { width: 10; height: 10 }

      Image { sourceSize.height: Noo.fontSize() * 3; source: Noo.pix("flags-sl") }
      Text { text: "slovenščina"; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "Grega Trček"; font.bold: true; color: activPal.text }
      Item { width: 10; height: 10 }
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
            createLink("SoundTouch", "http://www.surina.net/soundtouch/") + " by Olli Parviainen" +
            createLink("LilyPond emmentaler font", "http://lilypond.org/introduction.html") +
            createLink("Bravura SMuFL font", "https://www.smufl.org/fonts/") +
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


