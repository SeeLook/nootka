/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include "tmainhelp.h"
#include "widgets/troundedlabel.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;


QString createLink(QString desc, QString href) {
  return "<a href=\"" + href + "\">" + desc + "</a>";
}

/** Returns text header with defined text size and background */
QString getHeader(QString text) {
	return "<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b>" + text + "</b></p></center>";
}

QString transRow (QString flag, QString lang, QString name, QString mailAndSite) {
  return QString("<tr valign=\"middle\" align=\"center\"><td> <img src=\"%1\">&nbsp;&nbsp;&nbsp;</td><td> %2 &nbsp;&nbsp;&nbsp;</td><td> <b>%3</b> </td><td>&nbsp;&nbsp;&nbsp; %4 </td></tr>").
      arg(gl->path + "picts/flags-" + flag + ".png").arg(lang).arg(name).arg(mailAndSite);
}




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
//     navList->setFlow(QListView::TopToBottom);
    abLay->addWidget(m_navList);

    m_stackLayout = new QStackedLayout;
    abLay->addLayout(m_stackLayout);

    mainLay->addLayout(abLay);

    m_okBut = new QPushButton("OK", this);
    mainLay->addWidget(m_okBut, 1, Qt::AlignCenter);

    setLayout(mainLay);

    m_navList->addItem(tr("About"));
    m_navList->item(0)->setIcon(QIcon(gl->path+"picts/nootka.png"));
    m_navList->item(0)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Help"));
    m_navList->item(1)->setIcon(QIcon(gl->path+"picts/help.png"));
    m_navList->item(1)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(authorsTxt());
    m_navList->item(2)->setIcon(QIcon(gl->path+"picts/author.png"));
    m_navList->item(2)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("License"));
    m_navList->item(3)->setIcon(QIcon(gl->path+"picts/license.png"));
    m_navList->item(3)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Support"));
    m_navList->item(4)->setIcon(QIcon(gl->path+"picts/support.png"));
    m_navList->item(4)->setTextAlignment(Qt::AlignCenter);
    m_navList->addItem(tr("Changes"));
    m_navList->item(5)->setIcon(QIcon(gl->path+"picts/chlog.png"));
    m_navList->item(5)->setTextAlignment(Qt::AlignCenter);

		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(moveScroll()));
		

    Tabout *m_about = new Tabout();
    TmainHelp *help = new TmainHelp(gl->path);
    QWidget *authorsPage = new QWidget();
		authorsPage->setContentsMargins(5, 5, 5, 5);
    QVBoxLayout *wiLLay = new QVBoxLayout;
  // AUTHORS
		QString authorStr = getHeader(tr("Code"));
		authorStr += "<b>Tomasz Bojczuk</b>    <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><br>";
		authorStr += getHeader(tr("Audio"));
		authorStr += tr("editing and/or recording of samples:") + "<br><b>Sergei Ivanov (tico-tico)</b><br>";
  // TRANSLATORS
    QString translStr = getHeader(tr("Translators"));
    translStr += "<table valign=\"middle\" align=\"center\">";
  // Czech
    translStr += transRow("cs", QString::fromUtf8("český"), "Pavel Fric",
                          "<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>");
  // German
    translStr += transRow("de", "deutsch", "JCW", "");
  // English
		translStr += transRow("en", "english", "Luster", 
                          "<a href=\"http://linuxmusicians.com\">http://linuxmusicians.com</a>");
  // French
    translStr += transRow("fr", QString::fromUtf8("français"), "Olivier Devineau", "");
  // Polish
    translStr += transRow("pl", "polski", "Tomasz Bojczuk", 
                          "<a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a>");
		translStr += transRow("ru", QString::fromUtf8("русский"), "Sergei Ivanov (tico-tico)", "");
    translStr += "</table>";
		QString otherStr = getHeader(tr("Other projects")) +
				tr("However this application could not exist without various open source projects.<br>Especially:") + 
       "<ul><li>" + createLink("Qt", "http://qt-project.org/") + " by Digia</li>" +
       "<li>" + createLink("FFTW", "http://www.fftw.org") + " by M. Frigo & S. G. Johnson</li>" +
       "<li>" + createLink("ogg vorbis", "http://vorbis.com") + " by XIPH</li>" +
       "<li>" + createLink("RtAudio & RtMidi", "http://www.music.mcgill.ca/~gary/") + " by G. P. Scavone</li>" +
       "<li>" + createLink("Tartini", "http://miracle.otago.ac.nz/tartini/index.html") + " by P. McLeod</li>" +
       "<li>" + createLink("SoundTouch", "http://www.surina.net/soundtouch/") + " by Olli Parviainen</li>";
#if defined (Q_OS_WIN)
		otherStr += "<li>" + createLink("NSIS", "http://nsis.sourceforge.net/Main_Page") + "</li>";
#endif
    otherStr += "</ul>";
		
		QString thankStr = getHeader(tr("Thanks"));
		thankStr += "I would like to say <b>THANK YOU</b> for all people who helped me with developing Nootka.<br>I will try to mention them in some random order:<br>";
		thankStr += "<br><b>Aaron Wolf</b> <a href=\"http://blog.wolftune.com/\">http://blog.wolftune.com</a> for many warm words about Nootka in the web and helping clues.<br>";
		thankStr += "<b>falkTX</b> from <a href=\"http://kxstudio.sourceforge.net/\">http://kxstudio.sourceforge.net</a> for building *.deb and testing and for many clues.<br>";
		thankStr += "<b>Users</b> of <a href=\"http://www.linuxmusicians.com/\">http://www.linuxmusicians.com</a> forum for testing and comments.<br>";
		thankStr += "<b>Olli Parviainen</b> <a href=\"http://www.surina.net/soundtouch/\">http://www.surina.net/soundtouch</a> for help with his SoundTouch library.<br>";
		thankStr += "<b>Sergei Ivanov</b> for testing Nootka intensively, bug hunting and many valuable notices.<br>";
		thankStr += "<b>Translators (Pavel, Olivier, Sergei)</b> for many, maaaany clues and comments.<br>";
		thankStr += "<b>Project16 @ KVR</b> <a href=\"http://www.kvraudio.com/\">http://www.kvraudio.com</a> for the bass guitar samples<br>";
		thankStr += "<b>And all others that helped.</b><br>";
    QLabel *authorsLab = new QLabel(authorStr + translStr + otherStr + thankStr);
    authorsLab->setOpenExternalLinks(true);
		authorsLab->setWordWrap(true);
    wiLLay->addWidget(authorsLab);
    wiLLay->addStretch(1);
    authorsPage->setLayout(wiLLay);
		m_authorScroll = new QScrollArea();
		m_authorScroll->setAlignment(Qt::AlignCenter);
		m_authorScroll->setFixedWidth(authorsLab->width());
		m_authorScroll->setWidget(authorsPage);
	
	QString trans = QApplication::translate("about translator", "translator", "Do not translate this, just put in 'translator comment field' your data: Translator's' Name<br>Tramslator's' e-mail(optional)<br>Translator site(optional)");

// LICENSE GPL
    QTextEdit *licenseTxt = new QTextEdit();
    licenseTxt->setReadOnly(true);
    QFile file(gl->path + "gpl");
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
    QFile chfile(gl->path + "changes");
    if(chfile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&chfile);
        in.setCodec("UTF-8");
        QStringList htmlText = in.readAll().replace("  ", "&nbsp;&nbsp;").split("\n");
        for (int i = 0; i < htmlText.size(); i++) {
          if (htmlText[i].contains("0."))
            htmlText[i] = "<u><b>&nbsp;" + htmlText[i] + "</b></u>";
          else if (htmlText[i].contains("BUG"))
            htmlText[i] = "&nbsp;&nbsp;<u>BUG FIXES</u>";
          else if (htmlText[i].contains("Under the hood"))
            htmlText[i] = "&nbsp;&nbsp;<u>Under the hood</u>";
          else if (!htmlText[i].contains("&nbsp;&nbsp; - "))
            htmlText[i] = "<b>" + htmlText[i] + "</b>";
//           if (htmlText[i].isEmpty())
//             htmlText[i].append("<hr>");
//           else
          htmlText[i].append("<br>");
        }
//         htmlText.prepend(QString("<div style=\"color: %1; background-color: %2;\">").
//             arg(palette().highlightedText().color().name()).arg(palette().highlight().color().name()));
//         htmlText.append("</div>");
        chLogTxt->setHtml(htmlText.join(""));
//         chLogTxt->setPlainText(htmlText.join(""));
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
		
		QTimer::singleShot(20, this, SLOT(fixSize()));
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
		setFixedSize(size());
}




//######################### About ##########################################
Tabout::Tabout(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    TnootkaLabel *nootkaLab = new TnootkaLabel(gl->path + "picts/logo.png", this, palette().highlight().color());
    lay->addWidget(nootkaLab);
    QString info = tr("Welcome on board.<br>Nootka is an open source application to help you in learning (and in teaching) classical score notation.<br>It is specially designed for guitarists but others will find many usable features as well.");
    QString betaInfo = tr("This is a beta version and may contain bugs or behave in unexpected ways. Also, it has unfinished features.<br>In spite of that, you are welcome to try it!");
    QString moreInfo = tr("See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and further releases.</p><p>Any bugs, suggestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p>with respects<br>Author");
		betaInfo = ""; // comment it to show info in beta releases
    m_aboutLab = new TroundedLabel("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b>Nootka " + gl->version + "</b></p></center><p><br>" + info + "</p><p>" + betaInfo + "</p><p>" + moreInfo + "</p><br>"
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
