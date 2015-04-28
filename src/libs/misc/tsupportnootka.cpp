/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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
#include <widgets/troundedlabel.h>
#include <QtWidgets>

TsupportNootka::TsupportNootka(QWidget *parent) :
  QWidget(parent)
{
  QVBoxLayout *lay = new QVBoxLayout;
  QScrollArea *scrollArea = new QScrollArea();
  TroundedLabel *headLab = new TroundedLabel("<big><b>" + QObject::tr("You also can help with making Nootka better.") + "</big></b><br>" +
    QObject::tr("It requires little bit English, so if you can read a text below there will be something you may get involved."), this);
  headLab->setWordWrap(true);
  headLab->setAlignment(Qt::AlignCenter);
  headLab->setBackroundColor(palette().base().color());
  headLab->setStyleSheet("color: palette(highlightedText)");
  headLab->setContentsMargins(5, 5, 5, 5);
  lay->addWidget(headLab);
  QLabel *lab = new QLabel(this);
  QString supp;
  supp += "<ul>";
// 		supp += "<li><b style=\"color: #008000;\">Many thanks</b><br>For all who has helped already!<br></li>";
  supp += "<li><big><b style=\"color: #0000C0;\">Translate Nootka</b></big><br>It does not require any programming skills. Just read <a href=\"http://sourceforge.net/p/nootka/hg/ci/default/tree/lang/how-to-translate.txt\">the instructions</a>,<br>translate and send your work.<br></li>";
//     supp += "<li><big><b>Maintain Nootka package</b></big><br>Windows, MacOs and various Linux distributions 32 or 64 bits.<br>Life is too short...<br>If you know how to compile Nootka from sources you could keep Nootka version for your platform.<br></li>";
  supp += "<li><big><b style=\"color: #FF8000;\">Make a donation</b></big><br>Nootka is free of charge but it requires some expenses.<br>To make donation use <a href=\"http://nootka.sourceforge.net\">Nootka site</a><br></li>";
  supp += "<li><big><b style=\"color: #FF0000;\">Report an issue</b></big><br>If you find any issue or a bug request it through:<br><a href=\"http://sourceforge.net/p/nootka/bugs/\">bug tracker</a><br></li>";
  supp += "<li><big><b style=\"color: #C000C0;\">Vote on Nootka</b></big><br>There are a lot of services. For example:<br><a href=\"http://kde-apps.org/content/show.php/Nootka?content=143598\">Kde-Apps</a>, <a href=\"http://qt-apps.org/content/show.php/Nootka?content=148705\">Qt-Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a><br>or add <b>like</b> or/and <b>g+</b> on <a href=\"http://nootka.sourceforge.net\">Nootka site</a><br></li>";
  supp += "<li><big><b>Express your opinion</b></big><br>Simply <a href=\"mailto:seelook.gmail.com\">send an email</a></li>";
  supp += "</ul><p>&nbsp;&nbsp;Thanks in advance<br>&nbsp;&nbsp;Author</p><br>";
  lab->setText(supp);
  lab->setWordWrap(true);
  lab->setOpenExternalLinks(true);

  scrollArea->setWidget(lab);

  scrollArea->setContentsMargins(5, 5, 5, 5);
  lay->addWidget(scrollArea);
  setLayout(lay);
  setMinimumWidth(fontMetrics().boundingRect("w").width() * 60);
}



TsupportStandalone::TsupportStandalone(QString& path, QWidget* parent) :
  QDialog(parent)
{
  setWindowTitle("Support Nootka");
  QVBoxLayout *lay = new QVBoxLayout();
  TsupportNootka *suppWdg = new TsupportNootka(this);
  lay->addWidget(suppWdg);
  TroundedLabel *neverLab = new TroundedLabel(QObject::tr("Don't worry. This window appears only once per Nootka release.<br>You can find it always in 'About Nootka' dialog"), this);
  neverLab->setAlignment(Qt::AlignCenter);
  neverLab->setBackroundColor(palette().base().color());
  neverLab->setStyleSheet("color: palette(highlightedText)");
  neverLab->setContentsMargins(5, 5, 5, 5);
  lay->addWidget(neverLab, 1, Qt::AlignCenter);
  QPushButton *butt = new QPushButton(QIcon(path + "picts/nootka.png"), "  Thanks", this);
  butt->setIconSize(QSize(48, 48));
  lay->addStretch(1);
  lay->addWidget(butt, 1, Qt::AlignCenter);

  setLayout(lay);

  connect(butt, SIGNAL(clicked(bool)), this, SLOT(accept()));
}
