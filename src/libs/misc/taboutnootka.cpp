/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
#include "tnootkalabel.h"
#include "help/tmainhelp.h"
#include <tinitcorelib.h>
#include <widgets/troundedlabel.h>
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
  return QString("<tr valign=\"middle\" align=\"center\"><td>&nbsp;&nbsp;<img src=\"%1\">&nbsp;&nbsp;</td><td>&nbsp; %2 &nbsp;&nbsp;&nbsp;</td><td> <b>&nbsp; %3 &nbsp;</b> </td><td>&nbsp;&nbsp; %4 </td></tr>").
      arg(Tcore::gl()->path + QLatin1String("picts/flags-") + QString(flag) + QLatin1String(".png")).
      arg(lang).
      arg(name).
      arg(mailAndSite);
}

//#################################################################################################
//###################                TaboutNootka      ############################################
//#################################################################################################

TaboutNootka::TaboutNootka(QWidget *parent) :
  QDialog(parent)
{
    setWindowTitle(tr("About Nootka"));
    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *abLay = new QHBoxLayout;
    m_navList = new QListWidget(this);
    m_navList->setIconSize(QSize(80, 80));
    m_navList->setFixedWidth(110);
    m_navList->setViewMode(QListView::IconMode);
		m_navList->setMovement(QListView::Static);
    m_navList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    abLay->addWidget(m_navList);

    m_stackLayout = new QStackedLayout;
    abLay->addLayout(m_stackLayout);

    mainLay->addLayout(abLay);

    m_okBut = new QPushButton("OK", this);
    mainLay->addWidget(m_okBut, 1, Qt::AlignCenter);

    setLayout(mainLay);

    m_navList->addItem(tr("About"));
    m_navList->item(0)->setIcon(QIcon(Tcore::gl()->path+"picts/nootka.png"));
    m_navList->item(0)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Help"));
    m_navList->item(1)->setIcon(QIcon(Tcore::gl()->path+"picts/help.png"));
    m_navList->item(1)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(authorsTxt());
    m_navList->item(2)->setIcon(QIcon(Tcore::gl()->path+"picts/author.png"));
    m_navList->item(2)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("License"));
    m_navList->item(3)->setIcon(QIcon(Tcore::gl()->path+"picts/license.png"));
    m_navList->item(3)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Support"));
    m_navList->item(4)->setIcon(QIcon(Tcore::gl()->path+"picts/support.png"));
    m_navList->item(4)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Changes"));
    m_navList->item(5)->setIcon(QIcon(Tcore::gl()->path+"picts/chlog.png"));
    m_navList->item(5)->setTextAlignment(Qt::AlignCenter);

		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(moveScroll()));

    Tabout *m_about = new Tabout();
    TmainHelp *help = new TmainHelp();
    QWidget *authorsPage = new QWidget();
		authorsPage->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *wiLLay = new QVBoxLayout;
// AUTHORS
		QString authorStr = getHeader(tr("Code"));
		authorStr += QLatin1String("<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>");
		authorStr += getHeader(tr("Audio"));
		authorStr += tr("editing and/or recording of samples:") + QLatin1String("<br><b>Sergei Ivanov (tico-tico)</b><br>");
// TRANSLATORS
    QString translStr = getHeader(tr("Translators"));
    translStr += QLatin1String("<table valign=\"middle\" align=\"center\">");
// Czech
    translStr += transRow("cs", QStringLiteral("český"), QStringLiteral("Pavel Fric"),
                          QStringLiteral("<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>"));
// German
    translStr += transRow("de", QStringLiteral("deutsch"), "Johann C. Weihe", QString());
// English
		translStr += transRow("en", QStringLiteral("english"), QStringLiteral("Luster"),
                          QStringLiteral("<a href=\"http://linuxmusicians.com\">http://linuxmusicians.com</a>"));
// French
    translStr += transRow("fr", QStringLiteral("français"), QStringLiteral("Olivier Devineau,<br>&nbsp;&nbsp;Jean-Marc Lartigue"), QString());
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
       LI + createLink("RtAudio & RtMidi", "http://www.music.mcgill.ca/~gary/") + QLatin1String(" by G. P. Scavone</li>") +
       LI + createLink("Tartini", "http://miracle.otago.ac.nz/tartini/index.html") + QLatin1String(" by P. McLeod</li>") +
       LI + createLink("SoundTouch", "http://www.surina.net/soundtouch/") + QLatin1String(" by Olli Parviainen</li>") +
       LI + createLink("LilyPond emmentaler font", "http://lilypond.org/introduction.html");
#if defined (Q_OS_WIN)
		otherStr += LI + createLink("NSIS", "http://nsis.sourceforge.net/Main_Page") + QLatin1String("</li>");
#endif
    otherStr += QLatin1String("</ul>");

		QString thankStr = getHeader(tr("Thanks"));
		thankStr += QLatin1String("I would like to say <b>THANK YOU</b> for all people who helped me with developing Nootka.<br>I will try to mention them in some random order:<br>");
		thankStr += QLatin1String("<br><b>Aaron Wolf</b> <a href=\"http://blog.wolftune.com/\">http://blog.wolftune.com</a> for many warm words about Nootka in the web and helping clues.<br>");
		thankStr += QLatin1String("<b>falkTX</b> from <a href=\"http://kxstudio.sourceforge.net/\">http://kxstudio.sourceforge.net</a> for building *.deb and testing and for many clues.<br>");
		thankStr += QLatin1String("<b>Users</b> of <a href=\"http://www.linuxmusicians.com/\">http://www.linuxmusicians.com</a> forum for testing and comments.<br>");
		thankStr += QLatin1String("<b>Olli Parviainen</b> <a href=\"http://www.surina.net/soundtouch/\">http://www.surina.net/soundtouch</a> for help with his SoundTouch library.<br>");
		thankStr += QLatin1String("<b>Sergei Ivanov</b> for testing Nootka intensively, bug hunting and many valuable notices.<br>");
		thankStr += QLatin1String("<b>Translators (Pavel, Olivier, Sergei and Johann)</b> for many, maaaany clues and comments.<br>");
		thankStr += QLatin1String("<b>Project16 @ KVR</b> <a href=\"http://www.kvraudio.com/\">http://www.kvraudio.com</a> for the bass guitar samples<br>");
		thankStr += QLatin1String("<b>And all others that helped.</b><br>");
    QLabel *authorsLab = new QLabel(authorStr + translStr + otherStr + thankStr);
    authorsLab->setOpenExternalLinks(true);
		authorsLab->setWordWrap(true);
    wiLLay->addWidget(authorsLab);
    wiLLay->addStretch(1);
    authorsPage->setLayout(wiLLay);
		m_authorScroll = new QScrollArea();
		m_authorScroll->setAlignment(Qt::AlignCenter);
// 		m_authorScroll->setFixedWidth(authorsLab->width());
		m_authorScroll->setWidget(authorsPage);
	
	QString trans = QApplication::translate("about translator", "translator", "Do not translate this, just put in 'translator comment field' your data: Translator's' Name<br>Tramslator's' e-mail(optional)<br>Translator site(optional)");

// LICENSE GPL or Copyright for Debian based
    QTextEdit *licenseTxt = new QTextEdit();
    licenseTxt->setReadOnly(true);
    QFile file(Tcore::gl()->path + QLatin1String("gpl"));
		if (!file.exists()) { // Debian based
				QDir d(Tcore::gl()->path);
				d.cdUp();
				file.setFileName(d.path() + QLatin1String("/doc/nootka/copyright"));
		}
    if(file.open(QFile::ReadOnly | QFile::Text)) {
	      QTextStream in(&file);
        in.setCodec("UTF-8");
	      licenseTxt->setPlainText(in.readAll());
    } 
    file.close();
    TsupportNootka *support = new TsupportNootka(this);

// CHANGESLOG
    QTextEdit *chLogTxt = new QTextEdit();
    chLogTxt->setReadOnly(true);
    QFile chfile(Tcore::gl()->path + QLatin1String("changes"));
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
        chLogTxt->setHtml(htmlText.join(QString()));
    }
    chfile.close();

    m_stackLayout->addWidget(m_about);

    m_stackLayout->addWidget(help);
    m_stackLayout->addWidget(m_authorScroll);
    m_stackLayout->addWidget(licenseTxt);
    m_stackLayout->addWidget(support);
    m_stackLayout->addWidget(chLogTxt);

    connect(m_okBut, SIGNAL(clicked()), this, SLOT(accept()));
		connect(m_navList, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentPage(int)));
		
		QTimer::singleShot(500, this, SLOT(fixSize()));
}


void TaboutNootka::changeCurrentPage(int page) {
		m_stackLayout->setCurrentIndex(page);
		if (page == 2) {
				m_timer->start(100);
		} else
				m_timer->stop();
}


void TaboutNootka::moveScroll() {
  m_authorScroll->verticalScrollBar()->setValue(m_authorScroll->verticalScrollBar()->value() + 1);
}


void TaboutNootka::fixSize() {
  m_navList->setFixedWidth(m_navList->sizeHintForColumn(0) + 2 * m_navList->frameWidth() +
          (m_navList->verticalScrollBar()->isVisible() ? m_navList->verticalScrollBar()->width() : 0));
  adjustSize();
  setFixedSize(size());
}



