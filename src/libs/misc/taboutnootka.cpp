/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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
#include "tnootkalabel.h"
#include "tdonorswidget.h"
#include "help/tmainhelp.h"
#include <tinitcorelib.h>
#include <widgets/troundedlabel.h>
#include <QtWidgets/QtWidgets>

#define LI (QStringLiteral("<li>"))

QString createLink(const QString& desc, const QString& href) {
  return QStringLiteral("<a href=\"") + href + QStringLiteral("\">") + desc + QStringLiteral("</a>");
}


/** Returns text header with defined text size and background */
QString getHeader(const QString& text) {
	return QStringLiteral("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: x-large;\"><b>") + text + QStringLiteral("</b></p></center>");
}


QString transRow (const char* flag, const QString& lang, const QString& name, const QString& mailAndSite) {
  return QString("<tr valign=\"middle\" align=\"center\">"
                 "<td>&nbsp;&nbsp;<img src=\"%1\">&nbsp;&nbsp;</td>"
                 "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; %2 &nbsp;&nbsp;&nbsp;</td>"
                 "<td> <b>&nbsp; %3 &nbsp;</b> </td>"
                 "<td>&nbsp;&nbsp; %4 </td></tr>"
                 /*"<tr><td colspan=\"4\"><hr></td><tr/>"*/)
      .arg(Tcore::gl()->path + QLatin1String("picts/flags-") + QString(flag) + QLatin1String(".png"))
      .arg(lang)
      .arg(name)
      .arg(mailAndSite);
}



TaboutNootka::TaboutNootka(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("About Nootka"));
    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *abLay = new QHBoxLayout;
    m_navList = new QListWidget(this);
    int w = fontMetrics().boundingRect(QStringLiteral("Instrument")).width();
    m_navList->setIconSize(QSize(w, w));
    m_navList->setFixedWidth(w * 1.2);
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
    m_navList->item(0)->setIcon(QIcon(Tcore::gl()->path+"picts/nootka-frame.png"));
    m_navList->item(0)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Help"));
    m_navList->item(1)->setIcon(QIcon(Tcore::gl()->path+"picts/help-frame.png"));
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
    m_navList->addItem(tr("Donors", "Would be 'Sponsors' or even 'Backers' - translate as such as You fill, what sounds/looks better in Your language"));
    m_navList->item(5)->setIcon(QIcon(Tcore::gl()->path+"picts/donors.png"));
    m_navList->item(5)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Changes"));
    m_navList->item(6)->setIcon(QIcon(Tcore::gl()->path+"picts/chlog.png"));
    m_navList->item(6)->setTextAlignment(Qt::AlignCenter);

    QString br = QStringLiteral("<br>");
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(moveScroll()));
		

    Tabout *m_about = new Tabout();
    TmainHelp *help = new TmainHelp();
    QWidget *authorsPage = new QWidget();
		authorsPage->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *wiLLay = new QVBoxLayout;
// AUTHORS
		QString authorStr = getHeader(tr("Code"));
		authorStr += QStringLiteral("<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>");
		authorStr += getHeader(tr("Audio"));
		authorStr += tr("editing and/or recording of samples:") + QStringLiteral("<br><b>Sergei Ivanov (tico-tico)</b><br>");
// TRANSLATORS
    QString translStr = getHeader(tr("Translators"));
    translStr += QStringLiteral("<table valign=\"middle\" align=\"center\" cellpadding=\"5\">");
// Czech
    translStr += transRow("cs", QStringLiteral("český"), QStringLiteral("Pavel Fric"),
                          QStringLiteral("<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>"));
// German
    translStr += transRow("de", QStringLiteral("deutsch"), QStringLiteral("Johann C. Weihe"), QString());
// English
    translStr += transRow("en", QStringLiteral("english"), QStringLiteral("Luster"),
                          QStringLiteral("<a href=\"http://linuxmusicians.com\">http://linuxmusicians.com</a>"));
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
    translStr += "</table>";
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
    thankStr += QString::fromUtf8("<br><b>José Luis Marín</b> for patient and intensive tests under Linux and great feedback.<br>");
		thankStr += QLatin1String("<b>Aaron Wolf</b> <a href=\"http://blog.wolftune.com/\">http://blog.wolftune.com</a> for many warm words about Nootka in the web and helping clues.<br>");
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

// SPONSORS
    QWidget *donorsWidget = new TdonorsWidget(this);

// CHANGESLOG
    QTextEdit *chLogTxt = new QTextEdit();
    chLogTxt->setReadOnly(true);
    QFile chfile(Tcore::gl()->path + QLatin1String("changes"));
    if(chfile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&chfile);
        in.setCodec("UTF-8");
        QStringList htmlText = in.readAll().replace(QLatin1String("  "), QLatin1String("&nbsp;&nbsp;")).split("\n");
        for (int i = 0; i < htmlText.size(); i++) {
          if (htmlText[i].contains(QLatin1String("0.")) || htmlText[i].contains(QLatin1String("1.")))
            htmlText[i] = QLatin1String("<u><b>&nbsp;") + htmlText[i] + QLatin1String("</b></u>");
					else if (htmlText[i].contains(QLatin1String("======")))
						htmlText[i] = QLatin1String("<br><hr><b><big><center>Nootka ONE</big></b></center><hr>");
          else if (htmlText[i].contains(QLatin1String("BUG")))
            htmlText[i] = QLatin1String("&nbsp;&nbsp;<u>BUG FIXES</u>");
          else if (htmlText[i].contains(QLatin1String("Under the hood")))
            htmlText[i] = QLatin1String("&nbsp;&nbsp;<u>Under the hood</u>");
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
    m_stackLayout->addWidget(donorsWidget);
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




//######################### About ##########################################
Tabout::Tabout(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    TnootkaLabel *nootkaLab = new TnootkaLabel(Tcore::gl()->path + QLatin1String("picts/logo.png"), this, palette().highlight().color());
    lay->addWidget(nootkaLab);
    QString info = tr("Welcome on board.<br>Nootka is an open source application to help you in learning (and in teaching) classical score notation.<br>It is specially designed for guitarists but others will find many usable features as well.");
    QString betaInfo = tr("This is a beta version and may contain bugs or behave in unexpected ways. Also, it has unfinished features.<br>In spite of that, you are welcome to try it!");
    QString moreInfo = tr("See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and further releases.</p><p>Any bugs, suggestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p>with respects<br>Author");
		betaInfo = ""; // comment it to show info in beta releases
    m_aboutLab = new TroundedLabel("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: xx-large;\"><b>Nootka " + Tcore::gl()->version + "</b></p></center><p><br>" + info + "</p><p>" + betaInfo + "</p><p>" + moreInfo + "</p><br>"
#if defined(Q_OS_WIN32)
	  + tr("<br>To support this project and also to avoid infection of your PC<br><b>PLEASE, use the official project site to download updates, new releases and to see news.</b>")
#endif
							, this);
		m_aboutLab->setContentsMargins(10, 10, 10, 10);
    m_aboutLab->setWordWrap(true);
    m_aboutLab->setOpenExternalLinks(true);
    lay->addWidget(m_aboutLab);
    lay->addStretch();
    setLayout(lay);
}
