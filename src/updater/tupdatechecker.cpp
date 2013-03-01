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
#include <QDebug>

TupdateChecker::TupdateChecker(QObject* parent) :
  QObject(),
  m_respectRules(false),
  m_reply(0)
{
    getUpdateRules(m_updateRules);

    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replySlot(QNetworkReply*)));
}


void TupdateChecker::check(bool checkRules){ 
  m_respectRules = checkRules;
  if (!m_respectRules || (m_updateRules.enable && isUpdateNecessary(m_updateRules))) {
        m_reply = m_netManager->get(QNetworkRequest(QUrl("http://nootka.sourceforge.net/ch/version")));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorSlot(QNetworkReply::NetworkError)));
  } else {
    qDebug("No need for update");
    exit(0);
  }
}


TupdateChecker::~TupdateChecker()
{
  delete m_reply;  
}

void TupdateChecker::errorSlot(QNetworkReply::NetworkError err) {
    qDebug() << "Error:" << err;
}


void TupdateChecker::replySlot(QNetworkReply* netReply) {
  QString replyString(netReply->readAll());
  netReply->abort();
  netReply->close();
  netReply->deleteLater();
  QStringList replyLines = replyString.split(";", QString::SkipEmptyParts);
  QString newVersion = replyLines.at(0);
  bool success = true;
  if (newVersion.contains("Nootka:"))
    newVersion.replace("Nootka:", "");
  else 
    success = false;
  if (success) {
      replyLines.removeFirst();
      QString changes = replyLines.join("");
      qDebug() << newVersion;
      qDebug() << changes;
      if (m_updateRules.curentVersion != newVersion) {
        if (m_respectRules) {
          if (m_updateRules.checkForAll || isNewVersionStable(newVersion)) {
            showUpdateSummary(newVersion, changes, &m_updateRules);
          }
        } else
          showUpdateSummary(newVersion, changes, &m_updateRules);
      } else if (!m_respectRules)
          showUpdateSummary("", "", &m_updateRules);
//       m_updateRules.recentDate = QDate::currentDate();
      saveUpdateRules(m_updateRules);
  }
  exit(0);
}



QString TupdateChecker::getVersion() {

}


void TupdateChecker::showUpdateDialog(QString version, QString changes) {

}


