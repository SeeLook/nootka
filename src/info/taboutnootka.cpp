/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "taboutnootka.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

TaboutNootka::TaboutNootka(QWidget *parent) :
    QDialog(parent)
{
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


    Tabout *m_about = new Tabout();
    QWidget *wi = new QWidget();
    QVBoxLayout *wiLLay = new QVBoxLayout;
    QLabel *authorsLab = new QLabel(("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b><br>" + authorsTxt() + "<br></b></p></center><br><br>Tomasz Bojczuk<br><a href=\"mailto:tomaszbojczuk.gmail.com\">tomaszbojczuk@gmail.com</a><br><br><br>" +
       tr("Althougth this application could not exist without various open source projects.<br>Especially:") + "<ul><li><a href=\"http://qt.nokia.com\">Qt</a></li><li><a href=\"http://portaudio.com\">Portaudio</a></li></ul>"));
    authorsLab->setOpenExternalLinks(true);
    wiLLay->addWidget(authorsLab);
    wiLLay->addStretch(1);
    wi->setLayout(wiLLay);


    QTextEdit *licenseTxt = new QTextEdit();
    QFile file(gl->path + "LICENSE");
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
	      QTextStream in(&file);
	      licenseTxt->setPlainText(in.readAll());
        }        


    stackLayout->addWidget(m_about);

    stackLayout->addWidget(wi);
    stackLayout->addWidget(licenseTxt);

    connect(okBut, SIGNAL(clicked()), this, SLOT(accept()));
    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));
}

//######################### About ##########################################
Tabout::Tabout(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    m_aboutLab = new QLabel(QString("<center><img src=\"%1\"><br><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: 20px;\"><b>Nootka " + gl->version + "</b></p></center>").arg(gl->path+"picts/logo.png") + tr("<p> Welcome on the board.<br> Nootka is open source application to help You learning (and teaching also) classical score notation. It is specially dedicated for guitarists but others can find something usable as well.<br>This is beta version and can contain bugs or behave in unexpected way, also it has unfinished features. Inspite of that You are welcome to try it !!</p><p>See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and further releaces.</p><p>Any bugs, sugestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p>with respects<br>Author</p><br>"), this);
    m_aboutLab->setWordWrap(true);
    m_aboutLab->setOpenExternalLinks(true);
    lay->addWidget(m_aboutLab);

    setLayout(lay);
}
