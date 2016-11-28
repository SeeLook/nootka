/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "taboutnootka.h"
#include "tsupportnootka.h"
#include "tabout.h"
#include "tdonorswidget.h"
#include "tnootkalabel.h"
#include "help/tmainhelp.h"
#include <help/thelpdialogbase.h>
#include <tpath.h>
#include <qtr.h>
#include <widgets/troundedlabel.h>
#include <touch/ttoucharea.h>
#include <touch/ttouchproxy.h>
#include <QtWidgets/QtWidgets>

#define LI (QLatin1String("<li>"))

QString createLink(const QString& desc, const QString& href) {
  return QLatin1String("<a href=\"") + href + QLatin1String("\">") + desc + QLatin1String("</a>");
}


/** Returns text header with defined text size and background */
QString getHeader(const QString& text) {
	return QLatin1String("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: x-large;\"><b>") + text + QLatin1String("</b></p></center>");
}


QString transRow (const char* flag, const QString& lang, const QString& name, const QString& mailAndSite) {
  return QString("<tr valign=\"middle\" align=\"center\">"
                 "<td>&nbsp;&nbsp;<img src=\"%1\">&nbsp;&nbsp;</td>"
                 "<td>&nbsp;&nbsp;&nbsp; %2 &nbsp;&nbsp;&nbsp;</td>"
                 "<td> <b>&nbsp; %3 &nbsp;</b> </td>"
                 "<td>&nbsp;&nbsp; %4 </td></tr>"
                 /*"<tr><td colspan=\"4\"><hr></td><tr/>"*/)
      .arg(Tpath::main + QLatin1String("picts/flags-") + QString(flag) + QLatin1String(".png"))
      .arg(lang)
      .arg(name)
      .arg(mailAndSite);
}

//#################################################################################################
//###################                TaboutNootka      ############################################
//#################################################################################################

TaboutNootka::TaboutNootka(QWidget *parent) :
  TsettingsDialogBase(parent)
{
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
  setWindowTitle(tr("About Nootka"));
#endif

  addItem(tr("About"), Tpath::img("nootka-frame"));                               // 0
  addItem(qTR("QShortcut", "Help"), Tpath::img("help-frame"));// 1
  addItem(authorsTxt(), Tpath::img("author"));                                    // 2
  addItem(tr("License"), Tpath::img("license"));                                  // 3
  addItem(tr("Support"), Tpath::img("support"));                                  // 4
  addItem(tr("Donors",
             "Would be 'Sponsors' or even 'Backers' - translate as such as You fill, what sounds/looks better in Your language"),
          Tpath::img("donors"));                                                  // 5
  addItem(tr("Changes"), Tpath::img("chlog"));                                    // 6
  addItem(QStringLiteral("Qt"), Tpath::img("qt"));                                // 7
#if !defined (Q_OS_ANDROID)
  addItem(qTR("QShortcut", "Close"), Tpath::img("exit"));                         // 8
#endif

  QString br = QStringLiteral("<br>");
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(moveScroll()));
// ABOUT
  auto aboutPage = new Tabout(this);
// HELP
  auto helpPage = new QTabWidget(this);
    helpPage->setTabBarAutoHide(true);
  auto helpWidget = new TmainHelp(this);
    helpPage->addTab(helpWidget, QStringLiteral("Nootka"));
  if (TtouchProxy::touchEnabled()) {
    auto touchEdit = new QTextEdit(this);
      touchEdit->setReadOnly(true);
      QScroller::grabGesture(touchEdit->viewport(), QScroller::LeftMouseButtonGesture);
      QString touchText = TtouchProxy::touchScoreHelp();
      touchText += QLatin1String("<br>") + TtouchProxy::touchClefHelp();
      touchText += ThelpDialogBase::onlineDocP("getting-started");
#if defined (Q_OS_ANDROID) // zoomed fret occurs only under mobile
      touchText += QLatin1String("<br><hr><br>") + TtouchProxy::touchGuitarHelp();
#endif
      touchEdit->setHtml(touchText);
    helpPage->addTab(touchEdit, qTR("TscoreActions", "Score"));
  }

  auto authorsPage = new QWidget(this);
  authorsPage->setContentsMargins(5, 5, 5, 5);
// AUTHORS
  QString authorStr = getHeader(tr("Code"));
  authorStr += QLatin1String("<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>");
  authorStr += getHeader(tr("Audio"));
  authorStr += tr("editing and/or recording of samples:") + QLatin1String("<br><b>Sergei Ivanov (tico-tico)</b><br>");
// TRANSLATORS
    QString translStr = getHeader(tr("Translators"));
    translStr += QStringLiteral("<table valign=\"middle\" align=\"center\" cellpadding=\"5\">");
// Czech
  translStr += transRow("cs", QStringLiteral("český"), QStringLiteral("Pavel Fric"),
                        QStringLiteral("<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>"));
// German
  translStr += transRow("de", QStringLiteral("deutsch"), QStringLiteral("Johann C. Weihe"), QString());
// English
  translStr += transRow("en", QStringLiteral("english"), QStringLiteral("Luster"), QString());
// Spanish
  translStr += transRow("es", QStringLiteral("español"), QStringLiteral("José Luis Marín"),
                          QStringLiteral("<a href=\"mailto:jsls@gmx.com\">jsls@gmx.com</a>"));
// French
  translStr += transRow("fr", QStringLiteral("français"), QStringLiteral("Olivier Devineau,<br>&nbsp;&nbsp;Jean-Marc Lartigue"),
                        QString());
// Polish
  translStr += transRow("pl", QStringLiteral("polski"), QStringLiteral("Tomasz Bojczuk"),
                        QStringLiteral("<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>"));
  translStr += transRow("ru", QStringLiteral("русский"),
                        QStringLiteral("Sergei Ivanov (tico-tico),<br>&nbsp;&nbsp;Timur Artykov"), QString());
  translStr += QLatin1String("</table>");
  QString otherStr = getHeader(tr("Other projects")) +
      tr("However this application could not exist without various open source projects.<br>Especially:") +
      QLatin1String("<ul><li>") + createLink("Qt", "http://qt-project.org/") + QLatin1String(" by Digia</li>") +
      LI + createLink("FFTW", "http://www.fftw.org") + QLatin1String(" by M. Frigo & S. G. Johnson</li>") +
      LI + createLink("ogg vorbis", "http://vorbis.com") + QLatin1String(" by XIPH</li>") +
    #if !defined (Q_OS_ANDROID)
      LI + createLink("RtAudio & RtMidi", "http://www.music.mcgill.ca/~gary/") + QLatin1String(" by G. P. Scavone</li>") +
    #endif
      LI + createLink("Tartini", "http://miracle.otago.ac.nz/tartini/index.html") + QLatin1String(" by P. McLeod</li>") +
      LI + createLink("SoundTouch", "http://www.surina.net/soundtouch/") + QLatin1String(" by Olli Parviainen</li>") +
      LI + createLink("LilyPond emmentaler font", "http://lilypond.org/introduction.html");
#if defined (Q_OS_WIN)
  otherStr += LI + createLink("NSIS", "http://nsis.sourceforge.net/Main_Page") + QLatin1String("</li>");
#endif
  otherStr += QLatin1String("</ul>");

  QString thankStr = getHeader(tr("Thanks"));
  thankStr += QLatin1String("I would like to say <b>THANK YOU</b> for all people who helped me with developing Nootka.<br>I will try to mention them in some random order:<br>");
  thankStr += QString::fromUtf8("<br><b>José Luis Marín</b> for patient and intensive tests under Linux and great feedback.<br>");
  thankStr += QLatin1String("<b>Aaron Wolf</b> <a href=\"http://blog.wolftune.com/\">http://blog.wolftune.com</a> for many warm words about Nootka in the web and helping clues.<br>");
  thankStr += QLatin1String("<b>falkTX</b> from <a href=\"http://kxstudio.sourceforge.net/\">http://kxstudio.sourceforge.net</a> for building *.deb and testing and for many clues.<br>");
  thankStr += QLatin1String("<b>Users</b> of <a href=\"http://www.linuxmusicians.com/\">http://www.linuxmusicians.com</a> forum for testing and comments.<br>");
  thankStr += QLatin1String("<b>Olli Parviainen</b> <a href=\"http://www.surina.net/soundtouch/\">http://www.surina.net/soundtouch</a> for help with his SoundTouch library.<br>");
  thankStr += QLatin1String("<b>Sergei Ivanov</b> for testing Nootka intensively, bug hunting and many valuable notices.<br>");
  thankStr += QLatin1String("<b>Translators (Pavel, Olivier, Sergei and Johann)</b> for many, maaaany clues and comments.<br>");
  thankStr += QLatin1String("<b>Project16 @ KVR</b> <a href=\"http://www.kvraudio.com/\">http://www.kvraudio.com</a> for the bass guitar samples<br>");
  thankStr += QLatin1String("<b>And all others that helped.</b><br>");
  auto authorsLab = new QLabel(authorStr + translStr + otherStr + thankStr, authorsPage);
    authorsLab->setOpenExternalLinks(true);
    authorsLab->setWordWrap(true);
  auto wiLLay = new QVBoxLayout;
    wiLLay->addWidget(authorsLab);
    wiLLay->addStretch(1);
  authorsPage->setLayout(wiLLay);

  m_authorScroll = new TtouchArea(this);
  m_authorScroll->setAlignment(Qt::AlignCenter);
  m_authorScroll->setWidget(authorsPage);

  QString trans = QApplication::translate("about translator", "translator", "Do not translate this, just put in 'translator comment field' your data: Translator's' Name<br>Tramslator's' e-mail(optional)<br>Translator site(optional)");
  Q_UNUSED(trans);

// LICENSE GPL or Copyright for Debian based
  auto licensePage = new QTextEdit(this);
  licensePage->setReadOnly(true);
  QScroller::grabGesture(licensePage->viewport(), QScroller::LeftMouseButtonGesture);
#if defined (Q_OS_ANDROID)
  licensePage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  licensePage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
  QFile file(Tpath::main + QLatin1String("gpl"));
  if (!file.exists()) { // Debian based
      QDir d(Tpath::main);
      d.cdUp();
      file.setFileName(d.path() + QLatin1String("/doc/nootka/copyright"));
  }
  if(file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      licensePage->setPlainText(in.readAll());
  }
  file.close();

  auto supportPage = new TsupportNootka(this);

// SPONSORS
  auto donorsWidget = new TdonorsWidget(this);

// CHANGESLOG
  auto chLogPage = new QTextEdit(this);
  QScroller::grabGesture(chLogPage->viewport(), QScroller::LeftMouseButtonGesture);
#if defined (Q_OS_ANDROID)
  chLogPage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  chLogPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
  chLogPage->setReadOnly(true);
  QFile chfile(Tpath::main + QLatin1String("changes"));
  if(chfile.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&chfile);
      in.setCodec("UTF-8");
      QStringList htmlText = in.readAll().replace(QLatin1String("  "), QLatin1String("&nbsp;&nbsp;")).split(QLatin1String("\n"));
      for (int i = 0; i < htmlText.size(); i++) {
        if (htmlText[i].contains(QLatin1String("0.")) || htmlText[i].contains(QLatin1String("1.")))
          htmlText[i] = QLatin1String("<u><b>&nbsp;") + htmlText[i] + QLatin1String("</b></u>");
        else if (htmlText[i].contains(QLatin1String("======")))
          htmlText[i] = QStringLiteral("<br><hr><b><big><center>Nootka ONE</big></b></center><hr>");
        else if (htmlText[i].contains(QLatin1String("BUG")))
          htmlText[i] = QStringLiteral("&nbsp;&nbsp;<u>BUG FIXES</u>");
        else if (htmlText[i].contains(QLatin1String("Under the hood")))
          htmlText[i] = QStringLiteral("&nbsp;&nbsp;<u>Under the hood</u>");
        else if (!htmlText[i].contains(QLatin1String("&nbsp;&nbsp; - ")))
          htmlText[i] = QLatin1String("<b>") + htmlText[i] + QLatin1String("</b>");
        htmlText[i].append(QLatin1String("<br>"));
      }
      chLogPage->setHtml(htmlText.join(QString()));
  }
  chfile.close();

  auto qtAboutPage = new TtouchArea(this);
  auto qtLabel = new QLabel(qTR("QMessageBox", "<h3>About Qt</h3><p>This program uses Qt version %1.</p>").arg(qVersion()) +
    qTR("QMessageBox",
    "<p>Qt is a C++ toolkit for cross-platform application "
      "development.</p>"
      "<p>Qt provides single-source portability across all major desktop "
      "operating systems. It is also available for embedded Linux and other "
      "embedded and mobile operating systems.</p>"
      "<p>Qt is available under three different licensing options designed "
      "to accommodate the needs of our various users.</p>"
      "<p>Qt licensed under our commercial license agreement is appropriate "
      "for development of proprietary/commercial software where you do not "
      "want to share any source code with third parties or otherwise cannot "
      "comply with the terms of the GNU LGPL version 3 or GNU LGPL version 2.1.</p>"
      "<p>Qt licensed under the GNU LGPL version 3 is appropriate for the "
      "development of Qt&nbsp;applications provided you can comply with the terms "
      "and conditions of the GNU LGPL version 3.</p>"
      "<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
      "development of Qt&nbsp;applications provided you can comply with the terms "
      "and conditions of the GNU LGPL version 2.1.</p>"
      "<p>Please see <a href=\"http://%2/\">%2</a> "
      "for an overview of Qt licensing.</p>"
      "<p>Copyright (C) %1 The Qt Company Ltd and other "
      "contributors.</p>"
      "<p>Qt and the Qt logo are trademarks of The Qt Company Ltd.</p>"
      "<p>Qt is The Qt Company Ltd product developed as an open source "
      "project. See <a href=\"http://%3/\">%3</a> for more information.</p>"
      ).arg(QStringLiteral("2015"),
            QStringLiteral("qt.io/licensing"),
            QStringLiteral("qt.io")), this);
  qtLabel->setWordWrap(true);
  auto qtLay = new QVBoxLayout;
    qtLay->addWidget(qtLabel);
  qtAboutPage->setLayout(qtLay);

  hint->hide();

  addPage(aboutPage);
  addPage(helpPage);
  addPage(m_authorScroll);
  addPage(licensePage);
  addPage(supportPage);
  addPage(donorsWidget);
  addPage(chLogPage);
  addPage(qtAboutPage);

  connect(navList, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentPage(int)));
#if defined (Q_OS_ANDROID)
  cancelBut = buttonBox->addButton(QDialogButtonBox::Close);
  cancelBut->setIcon(QIcon(Tpath::img("exit")));
  connect(cancelBut, &QPushButton::clicked, this, &TaboutNootka::accept);
#else
  layout()->setSizeConstraint(QLayout::SetFixedSize);
#endif
}


void TaboutNootka::changeCurrentPage(int page) {
  if (page < stackLayout->count())
    stackLayout->setCurrentIndex(page);
  if (page == 2) { // scroll authors page
#if defined (Q_OS_ANDROID)
      m_timer->start(60);
#else
      m_timer->start(100);
#endif
  } else {
      m_timer->stop();
#if !defined (Q_OS_ANDROID)
      if (page == navList->count() - 1)
          close();
#endif
  }
}


void TaboutNootka::moveScroll() {
  m_authorScroll->verticalScrollBar()->setValue(m_authorScroll->verticalScrollBar()->value() + 1);
}





