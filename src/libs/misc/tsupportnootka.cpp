/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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
#include <graphics/tnotepixmap.h>
#include <widgets/troundedlabel.h>
#include <tpath.h>
#include <QtWidgets/QtWidgets>


/** HTML @p color name (i.e. #00FF00) of given @p text */
QString coloredLi(const char* color, const QString& text) {
  return QString("<li><big><b style=\"color: %1;\">%2</b></big><br>").arg(QColor(color).name()).arg(text);
}


QString endLi = QStringLiteral("</li>");

TsupportNootka::TsupportNootka(QWidget *parent) :
  QScrollArea(parent)
{
  QString makinkBetter = tr("You also can help with making Nootka better.");
  QString littleEnglish = tr("It requires little bit English, so if you can read a text below there will be something you may get involved.");
  QString br = QStringLiteral("<br>");
  QLabel *headLab = new QLabel(QLatin1String("<big><b>") + makinkBetter + QLatin1String("</big></b><br>") + littleEnglish, this);
    headLab->setWordWrap(true);
    headLab->setAlignment(Qt::AlignCenter);
    headLab->setStyleSheet("background: palette(base)");
    headLab->setContentsMargins(5, 5, 5, 5);

//   QLabel *donateLab = new QLabel(this);
//     donateLab->setWordWrap(true);
//     donateLab->setOpenExternalLinks(true);
//     donateLab->setStyleSheet(QStringLiteral("background: #0F3F0A; color: #FFFFFF;"));
//     donateLab->setText(QLatin1String("<ul>") + coloredLi("#42DA06", tr("Donate Nootka campaign"))
//        + tr("Feature by feature and Nootka became big and complex project.<br>"
//           "To be able developing this application further,<br>"
//           "to finish version for Android (and other mobile systems later on),<br>"
//           "an equipment capable for that is needed.<br>"
//           "Simply, the PC I'm working on is to old for that…<br>"
//           "Donate Nootka, please.")
//        + QLatin1String("<p><a href=\"http://nootka.sourceforge.net/index.php?C=donate\">")
//        + tr("Through PayPal or a card") + QLatin1String("</a></p>")
//        + QLatin1String("<p align=\"center\"><a href=\"mailto:seelook.gmail.com\">")
//        + tr("or send email for an account number") + QLatin1String("</a></p>") + endLi + QLatin1String("</ul><br>"));
  QString supp;
  supp += QLatin1String("<ul>");
//   supp += QLatin1String("<li><big><b style=\"color: #FF8000;\">Make a donation</b></big><br>Nootka is free of charge but it requires some expenses.<br>To make donation use <a href=\"http://nootka.sourceforge.net\">Nootka site</a><br></li>");
  supp += coloredLi("#42DA06", tr("Donate Nootka campaign"))
       + QLatin1String("<a href=\"http://nootka.sourceforge.net/index.php?C=donate\">")
       + tr("Through PayPal or a card") + QLatin1String("</a>")
       + QLatin1String("<p align=\"center\"><a href=\"mailto:seelook.gmail.com\">")
       + tr("or send email for an account number") + QLatin1String("</a></p>") + endLi;
  supp += QLatin1String("<li><big><b style=\"color: #0000C0;\">Translate Nootka</b></big><br>It does not require any programming skills. Just read <a href=\"http://sourceforge.net/p/nootka/hg/ci/default/tree/lang/how-to-translate.txt\">the instructions</a>,<br>translate and send your work.<br></li>");
  supp += coloredLi("#E57300", "Update German translation") + QLatin1String("It seems to be abandoned, so continue translating, please.") + br + endLi;
  supp += QLatin1String("<li><big><b style=\"color: #FF0000;\">Report an issue</b></big><br>If you find any issue or a bug than request it through:<br><a href=\"http://sourceforge.net/p/nootka/bugs/\">bug tracker</a><br></li>");
  supp += QLatin1String("<li><big><b style=\"color: #C000C0;\">Vote on Nootka</b></big><br>There are a lot of services. For example:<br>"
                        "<a href=\"https://play.google.com/store/apps/details?id=net.sf.nootka\">Google Play</a>, "
                        "<a href=\"https://www.linux-apps.com/p/1127020/\">Linux Apps</a>, "
                        "<a href=\"http://www.softpedia.com/get/Others/Home-Education/Nootka.shtml\">Softpedia</a><br>"
                        "or add <b>like</b> or/and <b>g+</b> on <a href=\"http://nootka.sourceforge.net\">Nootka site</a><br></li>");
  supp += QLatin1String("<li><big><b>Express your opinion</b></big><br>Simply <a href=\"mailto:seelook.gmail.com\">send an email</a></li>");
  supp += QLatin1String("</ul><p>&nbsp;&nbsp;Thanks in advance<br>&nbsp;&nbsp;Author</p><br>");
  QLabel *textLab = new QLabel(supp, this);
  textLab->setWordWrap(true);
  textLab->setOpenExternalLinks(true);

  QVBoxLayout *lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
//     lay->addWidget(donateLab);
    lay->addWidget(headLab);
    lay->addWidget(textLab);
    lay->addStretch();
  QWidget *widget = new QWidget(this);
  widget->setLayout(lay);
  setWidget(widget);
  setWidgetResizable(true);
  setFrameShape(QFrame::NoFrame);

  setMinimumWidth(fontMetrics().boundingRect("w").width() * 60);
}



TsupportStandalone::TsupportStandalone(QWidget* parent) :
  QDialog(parent)
{
  setWindowTitle("Support Nootka");
  QLabel *leftLab = new QLabel(this);
  leftLab->setPixmap(QPixmap(Tpath::img("wizard-left")));

  QString dontWorry = QApplication::translate("TsupportNootka",
            "Don't worry. This window appears only once per Nootka release.<br>You can find it always in 'About Nootka' dialog");
  TsupportNootka *supportArea = new TsupportNootka(this);
  supportArea->setFrameShape(QFrame::QFrame::StyledPanel);
  TroundedLabel *neverLab = new TroundedLabel(dontWorry, this);
    neverLab->setAlignment(Qt::AlignCenter);
    neverLab->setBackroundColor(palette().base().color());
    neverLab->setStyleSheet("color: palette(highlightedText)");
    neverLab->setContentsMargins(5, 5, 5, 5);
  QPushButton *thanksButton = new QPushButton(QIcon(Tpath::img("support")), QLatin1String("  Thanks!"), this);
    thanksButton->setIconSize(QSize(64, 64));

// Layout
  QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(leftLab);
    lay->setSizeConstraint(QLayout::SetFixedSize);

  QVBoxLayout *supportLay = new QVBoxLayout();
    supportLay->addWidget(supportArea);
    QHBoxLayout *bottomLay = new QHBoxLayout;
      bottomLay->addWidget(neverLab);
      bottomLay->addWidget(thanksButton, 0, Qt::AlignRight);
    supportLay->addLayout(bottomLay);
  lay->addLayout(supportLay);
  setLayout(lay);

  connect(thanksButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
}
