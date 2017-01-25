/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3


import "fakeTrans.js" as Fake

Dialog {
  title: qsTr("About Nootka")
  visible: true
//  width: pages.width; height: pages.height

  width: nootkaWindow.width * 0.75; height: nootkaWindow.height * 0.75

  PagesDialog {
    id: pages
    anchors.fill: parent
    model: ListModel { id: pageModel }
    Component.onCompleted: {
      pageModel.append({"iconName": "nootka-frame", "buttonText": qsTr("About")})
      pageModel.append({"iconName": "help-frame", "buttonText": Fake.tr("QShortcut", "Help")})
      pageModel.append({"iconName": "author", "buttonText": qsTr("Authors")})
      pageModel.append({"iconName": "license", "buttonText": qsTr("License")})
      pageModel.append({"iconName": "support", "buttonText": qsTr("Support")})
      pageModel.append({"iconName": "donors", "buttonText": qsTr("Donors", "Would be 'Sponsors' or even 'Backers' - translate as such as You fill, what sounds/looks better in Your language")})
      pageModel.append({"iconName": "chlog", "buttonText": qsTr("Changes")})
      pageModel.append({"iconName": "qt", "buttonText": "Qt"})
    }

    Flickable {
      id: licensePage
      parent: pages.stack
    }

    Flickable {
      id: authorsPage
      parent: pages.stack
      width: parent.width
      Layout.fillHeight: true
      contentHeight: aboutCont.height
      ScrollBar.vertical: ScrollBar {}

      Column {
        id: aboutCont
        width: parent.width
        TextBackground { text: qsTr("Code"); }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter; width: parent.width; color: activPal.text
          text: "<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>"
        }
        TextBackground { text: qsTr("Audio"); }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter; width: parent.width; color: activPal.text
          text: qsTr("editing and/or recording of samples:") + "<br><b>Sergei Ivanov (tico-tico)</b><br>"
        }
        TextBackground { text: qsTr("Translators"); }
        GridLayout {
          columns: 4
          Layout.alignment: Qt.AlignVCenter
          Image { source: Tpath.pix("flags-cs"); }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;český "; textFormat: Text.RichText; color: activPal.text }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Pavel Fric</b> "; color: activPal.text }
          Text { text: " <a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>"; color: activPal.text }

          Image { source: Tpath.pix("flags-de") }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;deutsch "; textFormat: Text.RichText; color: activPal.text}
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Johann C. Weihe</b> "; font.bold: true; color: activPal.text }
          Text { text: " " }

          Image { source: Tpath.pix("flags-es") }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;español "; textFormat: Text.RichText; color: activPal.text }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>José Luis Marín</b> "; font.bold: true; color: activPal.text }
          Text { text: " <a href=\"mailto:jsls@gmx.com\">jsls@gmx.com</a>"; color: activPal.text }

          Image { source: Tpath.pix("flags-fr") }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;français "; textFormat: Text.RichText; color: activPal.text }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Olivier Devineau,<br>&nbsp;&nbsp;&nbsp;&nbsp;Jean-Marc Lartigue</b> "; color: activPal.text }
          Text { text: " " }

          Image { source: Tpath.pix("flags-pl") }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;polski "; textFormat: Text.RichText; color: activPal.text }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Tomasz Bojczuk</b> "; color: activPal.text }
          Text { text: "<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>"; color: activPal.text }

          Image { source: Tpath.pix("flags-ru") }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;русский "; textFormat: Text.RichText; color: activPal.text }
          Text { text: "&nbsp;&nbsp;&nbsp;&nbsp;<b>Sergei Ivanov (tico-tico)<br>&nbsp;&nbsp;&nbsp;&nbsp;Timur Artykov</b> "; color: activPal.text }
          Text { text: " " }
        }

        TextBackground { text: qsTr("Other projects"); }
        Text {
          text: qsTr("However this application could not exist without various open source projects.<br>Especially:") +
                "<ul><li>" + createLink("Qt", "http://qt-project.org/") + " by Digia</li>" +
                "<li>" + createLink("FFTW", "http://www.fftw.org") + " by M. Frigo & S. G. Johnson</li>" +
                "<li>" + createLink("ogg vorbis", "http://vorbis.com") + " by XIPH</li>" +
                "<li>" + createLink("RtAudio & RtMidi", "http://www.music.mcgill.ca/~gary/") + " by G. P. Scavone</li>" +
                "<li>" + createLink("Tartini", "http://miracle.otago.ac.nz/tartini/index.html") + " by P. McLeod</li>" +
                "<li>" + createLink("SoundTouch", "http://www.surina.net/soundtouch/") + " by Olli Parviainen</li>" +
                "<li>" + createLink("LilyPond emmentaler font", "http://lilypond.org/introduction.html</li>") +
                "</ul>"
          color: activPal.text
        }
        TextBackground { text: qsTr("Thanks"); }
        Text {
          text: "I would like to say <b>THANK YOU</b> for all people who helped me with developing Nootka.<br>
I will try to mention them in some random order:<br>
<br><b>José Luis Marín</b> for patient and intensive tests under Linux and great feedback.<br>
<b>Aaron Wolf</b> <a href=\"http://blog.wolftune.com/\">http://blog.wolftune.com</a> for many warm words about Nootka in the web and helping clues.<br>
<b>falkTX</b> from <a href=\"http://kxstudio.sourceforge.net/\">http://kxstudio.sourceforge.net</a> for building *.deb and testing and for many clues.<br>
<b>Users</b> of <a href=\"http://www.linuxmusicians.com/\">http://www.linuxmusicians.com</a> forum for testing and comments.<br>
<b>Olli Parviainen</b> <a href=\"http://www.surina.net/soundtouch/\">http://www.surina.net/soundtouch</a> for help with his SoundTouch library.<br>
<b>Sergei Ivanov</b> for testing Nootka intensively, bug hunting and many valuable notices.<br>
<b>Translators (Pavel, Olivier, Sergei, José Luis and Johann)</b> for many, maaaany clues and comments.<br>
<b>Project16 @ KVR</b> <a href=\"http://www.kvraudio.com/\">http://www.kvraudio.com</a> for the bass guitar samples<br>
<b>And all others that helped.</b><br>"
          color: activPal.text
        }
      }
      Timer {
        interval: 50; repeat: true; running: GLOB.useAnimations && authorsPage.visible
        onTriggered: {
          if (authorsPage.visible && authorsPage.contentY < authorsPage.contentHeight - authorsPage.height)
            authorsPage.contentY = authorsPage.contentY + 1.0
        }
      }
//      Component.onCompleted: pages.stack.currentIndex = 2
    }

    HelpPage {
      id: helpPage
      parent: pages.stack
    }
    AboutPage {
      id: aboutPage
      parent: pages.stack
    }
  }

  function createLink(desc, href) {
    return "<a href=\"" + href + "\">" + desc + "</a>";
  }

}
