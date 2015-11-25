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
#include <graphics/tnotepixmap.h>
#include <tpath.h>
#include <QtWidgets/QtWidgets>


TsupportNootka::TsupportNootka(QWidget *parent) :
  TtouchArea(parent)
{
  auto headLab = new TroundedLabel("<big><b>" + QObject::tr("You also can help with making Nootka better.") + "</big></b><br>" +
    QObject::tr("It requires little bit English, so if you can read a text below there will be something you may get involved."), this);
    headLab->setWordWrap(true);
    headLab->setAlignment(Qt::AlignCenter);
    headLab->setBackroundColor(palette().base().color());
//     headLab->setStyleSheet("color: palette(highlightedText)");
    headLab->setContentsMargins(5, 5, 5, 5);
  auto textLab = new QLabel(this);
  QString supp;
  supp += "<ul>";
  supp += "<li><big><b style=\"color: #0000C0;\">Translate Nootka</b></big><br>It does not require any programming skills. Just read <a href=\"http://sourceforge.net/p/nootka/hg/ci/default/tree/lang/how-to-translate.txt\">the instructions</a>,<br>translate and send your work.<br></li>";
//     supp += "<li><big><b>Maintain Nootka package</b></big><br>Windows, MacOs and various Linux distributions 32 or 64 bits.<br>Life is too short...<br>If you know how to compile Nootka from sources you could keep Nootka version for your platform.<br></li>";
  supp += "<li><big><b style=\"color: #FF8000;\">Make a donation</b></big><br>Nootka is free of charge but it requires some expenses.<br>To make donation use <a href=\"http://nootka.sourceforge.net\">Nootka site</a><br></li>";
  supp += "<li><big><b style=\"color: #FF0000;\">Report an issue</b></big><br>If you find any issue or a bug than request it through:<br><a href=\"http://sourceforge.net/p/nootka/bugs/\">bug tracker</a><br></li>";
  supp += "<li><big><b style=\"color: #C000C0;\">Vote on Nootka</b></big><br>There are a lot of services. For example:<br><a href=\"http://kde-apps.org/content/show.php/Nootka?content=143598\">Kde-Apps</a>, <a href=\"http://qt-apps.org/content/show.php/Nootka?content=148705\">Qt-Apps</a>, <a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a><br>or add <b>like</b> or/and <b>g+</b> on <a href=\"http://nootka.sourceforge.net\">Nootka site</a><br></li>";
  supp += "<li><big><b>Express your opinion</b></big><br>Simply <a href=\"mailto:seelook.gmail.com\">send an email</a></li>";
  supp += "</ul><p>&nbsp;&nbsp;Thanks in advance<br>&nbsp;&nbsp;Author</p>";
  textLab->setText(supp);
  textLab->setWordWrap(true);
  textLab->setOpenExternalLinks(true);

  auto lay = new QVBoxLayout;
    lay->addWidget(headLab);
    lay->addWidget(textLab);
  setLayout(lay);

#if !defined (Q_OS_ANDROID)
  setMinimumWidth(fontMetrics().boundingRect("w").width() * 60);
#endif
}



TsupportStandalone::TsupportStandalone(QWidget* parent) :
  QDialog(parent)
{
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
  setWindowTitle("Support Nootka");
#endif
  auto leftLab = new QLabel(pixToHtml(Tpath::img("wizard-left")), this);
  auto supportArea = new TsupportNootka(this);
  supportArea->setFrameShape(QFrame::QFrame::StyledPanel);
  auto neverLab = new TroundedLabel(QObject::tr("Don't worry. This window appears only once per Nootka release.<br>You can find it always in 'About Nootka' dialog"), this);
    neverLab->setAlignment(Qt::AlignCenter);
    neverLab->setBackroundColor(palette().base().color());
//     neverLab->setStyleSheet("color: palette(highlightedText)");
    neverLab->setContentsMargins(5, 5, 5, 5);
  auto thanksButton = new QPushButton(QIcon(Tpath::img("nootka")), QLatin1String("  Thanks"), this);
    thanksButton->setIconSize(QSize(48, 48));
// Layout
  auto lay = new QHBoxLayout;
    lay->addWidget(leftLab);
  auto supportLay = new QVBoxLayout();
    supportLay->addWidget(supportArea);
    auto bottomLay = new QHBoxLayout;
      bottomLay->addWidget(neverLab);
      bottomLay->addWidget(thanksButton);
    supportLay->addLayout(bottomLay);
  lay->addLayout(supportLay);
  setLayout(lay);

  connect(thanksButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
}
