/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;


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
    navList = new QListWidget(this);
    navList->setIconSize(QSize(80,80));
    navList->setFixedWidth(100);
    navList->setViewMode(QListView::IconMode);
    navList->setFlow(QListView::TopToBottom);
    abLay->addWidget(navList);

    stackLayout = new QStackedLayout;
    abLay->addLayout(stackLayout);

    mainLay->addLayout(abLay);

    okBut = new QPushButton("OK", this);
    mainLay->addWidget(okBut, 1, Qt::AlignCenter);

    setLayout(mainLay);

    navList->addItem(tr("About"));
    navList->item(0)->setIcon(QIcon(gl->path+"picts/nootka.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(authorsTxt());
    navList->item(1)->setIcon(QIcon(gl->path+"picts/author.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("License"));
    navList->item(2)->setIcon(QIcon(gl->path+"picts/license.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Support"));
    navList->item(3)->setIcon(QIcon(gl->path+"picts/support.png"));
    navList->item(3)->setTextAlignment(Qt::AlignCenter);



    Tabout *m_about = new Tabout();
    QWidget *wi = new QWidget();
    QVBoxLayout *wiLLay = new QVBoxLayout;
  // AUTHORS
    QString authorStr = "<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b>" + authorsTxt() + "</b></p></center><br>Tomasz Bojczuk    <a href=\"mailto:tomaszbojczuk.gmail.com\">tomaszbojczuk@gmail.com</a><br><br>";
  // TRANSLATORS
    QString translStr = "<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b>" + tr("Translators") + "</b></p></center><br>";
    translStr += "<table valign=\"middle\" align=\"center\">";
  // czech
    translStr += transRow("cs", QString::fromUtf8("český"), "Pavel Fric",
                          "<a href=\"http://fripohled.blogspot.com\">fripohled.blogspot.com</a>");
  // french
    translStr += transRow("fr", QString::fromUtf8("français"), "Olivier Devineau", "");
  // polish
    translStr += transRow("pl", "polski", "Tomasz Bojczuk", 
                          "<a href=\"mailto:tomaszbojczuk.gmail.com\">tomaszbojczuk@gmail.com</a>");
    translStr += "</table>";
    QLabel *authorsLab = new QLabel(authorStr + translStr + "<br><br>" + 
       tr("However this application could not exist without various open source projects.<br>Especially:") + "<ul><li><a href=\"http://qt-project.org/\">Qt</a></li><li><a href=\"http://www.fftw.org\">FFTW</a></li><li><a href=\"http://www.music.mcgill.ca/~gary/rtmidi/\">RtMidi</a></li><li><a href=\"http://miracle.otago.ac.nz/tartini/index.html\">Tartini</a></li></ul>");
    authorsLab->setOpenExternalLinks(true);
    wiLLay->addWidget(authorsLab);
    wiLLay->addStretch(1);
    wi->setLayout(wiLLay);
	
	QString trans = QApplication::translate("about translator", "translator", "Do not translate this, just put in 'translator comment field' Your data: Translator's' Name<br>Tramslator's' e-mail(optional)<br>Translator site(optional)");


    QTextEdit *licenseTxt = new QTextEdit();
    QFile file(gl->path + "gpl");
    if(file.open(QFile::ReadOnly | QFile::Text)) {
	      QTextStream in(&file);
		  in.setCodec("UTF-8");
	      licenseTxt->setPlainText(in.readAll());
    }

    TsupportNootka *support = new TsupportNootka(this);

    stackLayout->addWidget(m_about);

    stackLayout->addWidget(wi);
    stackLayout->addWidget(licenseTxt);
    stackLayout->addWidget(support);

    connect(okBut, SIGNAL(clicked()), this, SLOT(accept()));
    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));
}

//######################### About ##########################################
Tabout::Tabout(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    TnootkaLabel *nootkaLab = new TnootkaLabel(gl->path + "picts/logo.png", this, palette().highlight().color());
    lay->addWidget(nootkaLab);
    QString info = tr("Welcome on the board.<br> Nootka is open source application to help You learning (and teaching also) classical score notation.<br>It is specially dedicated for guitarists but others can find many usable features as well.");
    QString betaInfo = tr("This is beta version and can contain bugs or behave in unexpected way, also it has unfinished features.<br>Inspite of that You are welcome to try it!!");
    QString moreInfo = tr("See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and further releases.</p><p>Any bugs, sugestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p>with respects<br>Author");
    betaInfo = ""; // comment it to show info in beta releases
    m_aboutLab = new QLabel("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b>Nootka " + gl->version + "</b></p></center><p><br>" + info + "</p><p>" + betaInfo + "</p><p>" + moreInfo + "</p><br>"
#if defined(Q_OS_WIN32)
	  + tr("<br><br><br>To support project and also to avoid infection of Your PC<br><b>PLEASE, use project site to download updates, new releases and to see news.</b><br><br>")
#endif
							, this);
    m_aboutLab->setWordWrap(true);
    m_aboutLab->setOpenExternalLinks(true);
    lay->addWidget(m_aboutLab);
    lay->addStretch();
    setLayout(lay);
}
