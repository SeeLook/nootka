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
#include <QHBoxLayout>
// #include <QTextEdit>
#include <QLabel>
#include <QScrollArea>

TsupportNootka::TsupportNootka(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout;
    QScrollArea *scrollArea = new QScrollArea();
    QLabel *lab = new QLabel(this);
//     QTextEdit *sup = new QTextEdit(this);
    lab->setText("<b>&nbsp;&nbsp;You also can help with makeing Notka better</b><br><ul><li><b>Translate Nootka</b><br>It does not require any programming skills. Just read <a href=\"\">the instructions</a>, tramslate and send Your work.<br></li><li><b>Test Nootka intensively</b><br>When You will produce a big exam file, let's say 500 or more questions, it doesn't matter how many mistakes You did, send me the file please. It will help to improve analyse of exams.<br></li><li><b>Maintain Nootka package</b><br>If You can build Nootka from sources share Your package with others<br></li><li><b>Make a donation</b><br>Nootka is for free but developing costs. To make donation:<br> use <a href=\"http://sourceforge.net/donate/index.php?group_id=515420\">this site</a><br>or see more on <a href=\"http://nootka.sourceforge.net/index.php?L=en&C=sup\">on Nootka site</a><br></li><li><b>Vote on Nootka</b><br>There are a lot of services. For example:<br><a href=\"http://kde-apps.org/content/show.php/Nootka?content=143598\">Kde-Apps</a>, <a href=\"http://qt-apps.org/content/show.php/Nootka?content=148705\">Qt-Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a><br></li><li><b>Express Your opinion</b><br>Simply <a href=\"mailto:seelook.gmail.com\">send an email</a></li></ul><p>&nbsp;&nbsp;Thanks in advance<br>&nbsp;&nbsp;Author</p><br>");
    lab->setWordWrap(true);
    lab->setOpenExternalLinks(true);
//     sup->setReadOnly(true);
    
//     lay->addWidget(sup);
    scrollArea->setWidget(lab);
    lay->addStretch(1);
    lay->addSpacing(5);
    lay->addWidget(scrollArea);
    lay->addStretch(1);
    lay->addSpacing(5);

    setLayout(lay);

}
