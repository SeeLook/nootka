/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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


#include "tupdatesummary.h"
#include "tupdateruleswdg.h"
#include <nootkaconfig.h>
#include <tpath.h>
#include <qtr.h>

#include <QtCore/qversionnumber.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qscroller.h>

#include <QtCore/qdebug.h>


TupdateSummary::TupdateSummary(QString version, QString changes, TupdateRules* updateRules):
  QDialog(nullptr, Qt::WindowStaysOnTopHint),
  m_updateRules(updateRules)
{
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
  setWindowIcon(QIcon(Tpath::img("nootka")));
  setWindowTitle(tr("Updates"));
#endif
  auto mainLay = new QVBoxLayout;
  auto lab = new QLabel(this);
  mainLay->addWidget(lab);
  lab->setAlignment(Qt::AlignCenter);
  auto currVers = QVersionNumber::fromString(NOOTKA_VERSION);
  auto onlineVers = QVersionNumber::fromString(version);
  if (onlineVers > currVers) { //  new version is available, display what is new there
// #if defined (Q_OS_ANDROID)
      lab->setText(QLatin1String("<p style=\"font-size: large;\">")
      + tr("New Nootka %1 is available.").arg(version) + QLatin1String("<br>")
      + tr("To get it, visit %1 Nootka site").arg("<a href=\"https://nootka.sourceforge.io/index.php/download/\">") + QLatin1String("</a>.</p>"));
// #else
//       lab->setText(QLatin1String("<br><p style=\"font-size: xx-large;\">")
//       + tr("New Nootka %1 is available.").arg(version) + QLatin1String("<br>")
//       + tr("To get it, visit <a href=\"https://nootka.sourceforge.io/index.php/download/\">Nootka site</a>.") + QLatin1String("</p><br>"));
// #endif
      lab->setOpenExternalLinks(true);
      auto news = new QTextEdit(this);
#if defined (Q_OS_ANDROID)
      news->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      news->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      news->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
#endif
      QScroller::grabGesture(news->viewport(), QScroller::LeftMouseButtonGesture);
      news->setReadOnly(true);
      mainLay->addWidget(news);
      news->setText(tr("News:") + changes);
  } else if (currVers > onlineVers)
    lab->setText(QLatin1String("<br><p style=\"font-size: x-large;\">")
      + changes.replace(QLatin1String("."), QLatin1String("<br>")) + QLatin1String("</p><br>"));
  else
    lab->setText(QLatin1String("<br><p style=\"font-size: x-large;\">")
              + tr("No changes found.<br>This version is up to date.") + QLatin1String("</p><br>"));
  if (m_updateRules) {
    m_rulesWidget = new TupdateRulesWdg(m_updateRules, this);
    mainLay->addWidget(m_rulesWidget);
  }
  mainLay->addSpacing(10);
  m_okButton = new QPushButton(qTR("QDialogButtonBox", "Ok"), this);
  mainLay->addWidget(m_okButton, 0, Qt::AlignCenter);

  setLayout(mainLay);
  connect(m_okButton, &QPushButton::clicked, this, &TupdateSummary::okButtonSlot);
}



TupdateSummary::~TupdateSummary() {}


void TupdateSummary::okButtonSlot() {
  if (m_updateRules)
    m_rulesWidget->saveSettings();
  accept();
}
