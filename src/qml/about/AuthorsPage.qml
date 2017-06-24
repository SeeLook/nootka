/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Flickable {
  id: authorsPage

  clip: true
  contentHeight: aboutCont.height
  ScrollBar.vertical: ScrollBar {}

  Column {
    id: aboutCont
    spacing: ttt.font.pixelSize
    width: parent.width
    TextBackground { text: qsTranslate("TaboutNootka", "Code"); }
    Text {
      id: ttt
      anchors.horizontalCenter: parent.horizontalCenter; width: parent.width; color: activPal.text
      text: "<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>"
    }
    TextBackground { text: qsTranslate("TaboutNootka", "Audio"); }
    Text {
      anchors.horizontalCenter: parent.horizontalCenter; width: parent.width; color: activPal.text
      text: qsTranslate("TaboutNootka", "editing and/or recording of samples:") + "<br><b>Sergei Ivanov (tico-tico)</b><br>"
    }
    TextBackground { text: qsTranslate("TaboutNootka", "Translators"); }
    GridLayout {
      columns: 4
      Layout.alignment: Qt.AlignVCenter
      Image { sourceSize.height: ttt.font.pixelSize * 2; source: Tpath.pix("flags-cs"); }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;český "; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Pavel Fric</b> "; color: activPal.text }
      Text { text: " <a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>"; color: activPal.text }

      Image { sourceSize.height: ttt.font.pixelSize * 2; source: Tpath.pix("flags-de") }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;deutsch "; textFormat: Text.StyledText; color: activPal.text}
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Johann C. Weihe</b> "; font.bold: true; color: activPal.text }
      Item { width: 10; height: 10 }

      Image { sourceSize.height: ttt.font.pixelSize * 2; source: Tpath.pix("flags-es") }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;español "; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>José Luis Marín</b> "; font.bold: true; color: activPal.text }
      Text { text: " <a href=\"mailto:jsls@gmx.com\">jsls@gmx.com</a>"; color: activPal.text }

      Image { sourceSize.height: ttt.font.pixelSize * 2; source: Tpath.pix("flags-fr") }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;français "; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Olivier Devineau,<br>&nbsp;&nbsp;&nbsp;&nbsp;Jean-Marc Lartigue</b> "; color: activPal.text }
      Item { width: 10; height: 10 }

      Image { sourceSize.height: ttt.font.pixelSize * 2; source: Tpath.pix("flags-pl") }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;polski "; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Tomasz Bojczuk</b> "; color: activPal.text }
      Text { text: "<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>"; color: activPal.text }

      Image { sourceSize.height: ttt.font.pixelSize * 2; source: Tpath.pix("flags-ru") }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;русский "; textFormat: Text.StyledText; color: activPal.text }
      Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Sergei Ivanov (tico-tico)<br>&nbsp;&nbsp;&nbsp;&nbsp;Timur Artykov</b> "; color: activPal.text }
      Item { width: 10; height: 10 }
    }

    TextBackground { text: qsTranslate("TaboutNootka", "Other projects"); }
    Text {
      width: parent.width
      text: qsTranslate("TaboutNootka", "However this application could not exist without various open source projects.<br>Especially:") +
            "<ul>" + createLink("Qt", "http://qt-project.org/") + " by Digia" +
            createLink("FFTW", "http://www.fftw.org") + " by M. Frigo & S. G. Johnson" +
            createLink("ogg vorbis", "http://vorbis.com") + " by XIPH" +
            createLink("RtAudio & RtMidi", "http://www.music.mcgill.ca/~gary/") + " by G. P. Scavone" +
            createLink("Tartini", "http://miracle.otago.ac.nz/tartini/index.html") + " by P. McLeod" +
            createLink("SoundTouch", "http://www.surina.net/soundtouch/") + " by Olli Parviainen" +
            createLink("LilyPond emmentaler font", "http://lilypond.org/introduction.html") +
            createLink("Bravura SMuFL font", "http://www.smufl.org/fonts/") +
            "</ul>"
      color: activPal.text
      wrapMode: Text.WordWrap
    }
    TextBackground { text: qsTranslate("TaboutNootka", "Thanks"); }
    Text {
      width: parent.width
      text: "I would like to say <b>THANK YOU</b> for all people who helped me with developing Nootka.<br>
I will try to mention them in some random order:<br>
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
      color: activPal.text
      wrapMode: Text.WordWrap
    }
  }
  Timer {
    id: scrollTimer
    interval: 50; repeat: true; running: GLOB.useAnimations && authorsPage.visible
    onTriggered: {
      if (authorsPage.visible && authorsPage.contentY < authorsPage.contentHeight - authorsPage.height)
        authorsPage.contentY = authorsPage.contentY + 1.0
    }
  }

  function createLink(desc, href) {
    return "<li><a href=\"" + href + "\">" + desc + "</a></li>";
  }

  MouseArea {
    anchors.fill: parent
    onClicked: scrollTimer.running = !scrollTimer.running && GLOB.useAnimations
  }
}


