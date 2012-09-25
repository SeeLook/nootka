/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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



#include "tsupportnootka.h"
#include <QtGui>

TsupportNootka::TsupportNootka(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    QScrollArea *scrollArea = new QScrollArea();
    QLabel *headLab = new QLabel("<b style=\"font-size: 20px\">" + QObject::tr("You also can help with makeing Notka better.") +
      "</b><br>" + QObject::tr("It requires little bit english, so if You can read a text below there will be something You may get involved."), this);
    headLab->setWordWrap(true);
    headLab->setAlignment(Qt::AlignCenter);
    headLab->setStyleSheet("border-radius: 5px 5px; background-color: palette(base); color: palette(highlightedText)");
    lay->addWidget(headLab);
    QLabel *lab = new QLabel(this);
    QString supp;
    supp += "<ul>";
    supp += "<li><b>Translate Nootka</b><br>It does not require any programming skills. Just read <a href=\"http://code.google.com/p/nootka/source/browse/lang/how-to-translate.txt\">the instructions</a>,<br>tramslate and send Your work.<br></li>";
    supp += "<li><b>Check this version</b><br>If You used any previous version of Nootka, put attention please, Are continued exams such as You expected. There were many changes in this realm ant it has to be tested.<br></li>";
    supp += "<li><b>Test Nootka intensively</b><br>When You will produce a big exam file, let's say 200 or more questions, it doesn't matter how many mistakes You did, send me the file please. It will help to improve analyse of exams.<br></li>";
    supp += "<li><b>Maintain Nootka package</b><br>Windows, MacOs and various Linux distibutions 32 or 64 bits.<br>Life is too short...<br>If You know how to compile Nootka from sources You would be keep Nootka version for Your platform.<br></li>";
    supp += "<li><b>Make a donation</b><br>Nootka is free of charge but it requires some expenses. To make donation by PayPal:<br> use <a href=\"http://nootka.sourceforge.net/index.php?L=en&C=sup\"> Nootka site</a><br></li>";
    supp += "<li><b>Vote on Nootka</b><br>There are a lot of services. For example:<br><a href=\"http://kde-apps.org/content/show.php/Nootka?content=143598\">Kde-Apps</a>, <a href=\"http://qt-apps.org/content/show.php/Nootka?content=148705\">Qt-Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a><br></li>";
    supp += "<li><b>Express Your opinion</b><br>Simply <a href=\"mailto:seelook.gmail.com\">send an email</a></li>";
    supp += "</ul><p>&nbsp;&nbsp;Thanks in advance<br>&nbsp;&nbsp;Author</p><br>";
    lab->setText(supp);
    lab->setWordWrap(true);
    lab->setOpenExternalLinks(true);
    
    scrollArea->setWidget(lab);
    
    scrollArea->setContentsMargins(5, 5, 5, 5);
    lay->addWidget(scrollArea);
    setLayout(lay);
}

TsupportStandalone::TsupportStandalone(QString& path, QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle("Support Nootka");
    QVBoxLayout *lay = new QVBoxLayout();
//     QLabel *headLab = new QLabel(QString("<img src=\"%1\">").arg(path + "picts/logo.png"), this);
//     lay->addWidget(headLab, 1, Qt::AlignCenter);
    TsupportNootka *suppWdg = new TsupportNootka(this);
    lay->addWidget(suppWdg);
    QLabel *neverLab = new QLabel(QObject::tr("Don't worry. This window appears only once per Nootka release.<br>You can find it always in 'About Nootka' dialog"), this);
    neverLab->setAlignment(Qt::AlignCenter);
    neverLab->setStyleSheet("border-radius: 5px 5px; background-color: palette(base); color: palette(highlightedText)");
//     neverLab->setWordWrap(true);
    lay->addWidget(neverLab, 1, Qt::AlignCenter);
    QPushButton *butt = new QPushButton(QIcon(path + "picts/nootka.png"), "  Thanks", this);
    butt->setIconSize(QSize(48, 48));
    lay->addStretch(1);
    lay->addWidget(butt, 1, Qt::AlignCenter);
    
    setLayout(lay);
    
    connect(butt, SIGNAL(clicked(bool)), this, SLOT(accept()));
}
