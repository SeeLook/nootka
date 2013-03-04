/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tupdatechecker.h"
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <iostream>

TupdateChecker::TupdateChecker(QObject* parent) :
  QObject(),
  m_respectRules(false),
  m_reply(0),
  m_success(true)
{
    getUpdateRules(m_updateRules);

    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replySlot(QNetworkReply*)));
}


void TupdateChecker::check(bool checkRules){
  m_respectRules = checkRules;
  if (!m_respectRules)
    std::cout << tr("Checking for updates. Please wait...").toStdString() << std::endl;
  if (!m_respectRules || (m_updateRules.enable && isUpdateNecessary(m_updateRules))) {
        QNetworkRequest request(QUrl("http://nootka.sourceforge.net/ch/version.php"));
#if defined(Q_OS_WIN32)
        request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E)");
#elif defined(Q_OS_LINUX)
        request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; U; Linux i686 (x86_64); ");
#else
        request.setRawHeader("User-Agent", "");
#endif
        m_reply = m_netManager->get(request);
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorSlot(QNetworkReply::NetworkError)));
  } else {
    std::cout << "No need for update" << std::endl;
    exit(0);
  }
}


TupdateChecker::~TupdateChecker()
{
  delete m_reply;  
}

void TupdateChecker::errorSlot(QNetworkReply::NetworkError err) {
  if (!m_respectRules)
    std::cout << "An error occured: " << (int)err << std::endl;
  m_success = false;
}


void TupdateChecker::replySlot(QNetworkReply* netReply) {
  if (m_success) {
      QString replyString(netReply->readAll());
      netReply->abort();
      netReply->close();
      netReply->deleteLater();
      QStringList replyLines = replyString.split(";", QString::SkipEmptyParts);
      QString newVersion = replyLines.at(0);
      if (newVersion.contains("Nootka:"))
        newVersion.replace("Nootka:", "");
      else 
        m_success = false;
      if (m_success) {
          if (!m_respectRules)
            std::cout << "success" << std::endl;
          replyLines.removeFirst();
          QString changes = replyLines.join("");
          if (m_updateRules.curentVersion != newVersion) {
            showUpdateSummary(newVersion, changes, &m_updateRules);
          } else if (!m_respectRules) {
              showUpdateSummary("", "", &m_updateRules);
          }
          m_updateRules.recentDate = QDate::currentDate();
          saveUpdateRules(m_updateRules);
          if (!m_respectRules)
            std::cout << " " << std::endl;
      }
  }
  exit(0);
}




