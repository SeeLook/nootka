/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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
#include <tpath.h>
#include <qtr.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qscroller.h>


TupdateSummary::TupdateSummary(QString version, QString changes, TupdateRules* updateRules, QWidget* parent): 
  QDialog(parent, Qt::WindowStaysOnTopHint),
  m_updateRules(updateRules)
{
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
	setWindowIcon(QIcon(Tpath::img("nootka")));
	setWindowTitle(tr("Updates"));
#endif
  QVBoxLayout *mainLay = new QVBoxLayout;
  QLabel *lab = new QLabel(this);
  mainLay->addWidget(lab);
  lab->setAlignment(Qt::AlignCenter);
  if (!version.isEmpty()) {
#if defined (Q_OS_ANDROID)
    lab->setText(QStringLiteral("<p style=\"font-size: large;\">") +
      tr("New Nootka %1 is available.").arg(version) + QStringLiteral("<br>") +
      tr("To get it, visit <a href=\"https://nootka.sourceforge.io/index.php?C=down\">Nootka site</a>.")
                 + QStringLiteral("</p>"));
#else
    lab->setText(QStringLiteral("<br><p style=\"font-size: xx-large;\">") +
      tr("New Nootka %1 is available.").arg(version) + QStringLiteral("<br>") +
      tr("To get it, visit <a href=\"https://nootka.sourceforge.io/index.php?C=down\">Nootka site</a>.")
                 + QStringLiteral("</p><br>"));
#endif
    lab->setOpenExternalLinks(true);
    QTextEdit *news = new QTextEdit(this);
#if defined (Q_OS_ANDROID)
    news->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    news->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    news->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
#endif
    QScroller::grabGesture(news->viewport(), QScroller::LeftMouseButtonGesture);
    news->setReadOnly(true);
    mainLay->addWidget(news);
    news->setText(tr("News:") + changes);
  } else {
    lab->setText(QStringLiteral("<br><p style=\"font-size: xx-large;\">")
                 + tr("No changes found.<br>This version is up to date.") + QStringLiteral("</p><br>"));
  }
  if (m_updateRules) {
    m_rulesWidget = new TupdateRulesWdg(m_updateRules, this);
    mainLay->addWidget(m_rulesWidget);
  }
  mainLay->addSpacing(10);
  m_okButton = new QPushButton(qTR("QDialogButtonBox", "Ok"), this);
  mainLay->addWidget(m_okButton, 0, Qt::AlignCenter);
  
  setLayout(mainLay);
  
  connect(m_okButton, SIGNAL(clicked()), this, SLOT(okButtonSlot()));
}



TupdateSummary::~TupdateSummary() {}


void TupdateSummary::okButtonSlot() {
  if (m_updateRules)
    m_rulesWidget->saveSettings();
  accept();
}
