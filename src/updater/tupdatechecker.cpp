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


#include "tupdatechecker.h"
#include <nootkaconfig.h>
#include <QtWidgets/qmessagebox.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtCore/qversionnumber.h>

#include <QtCore/qdebug.h>


TupdateChecker::TupdateChecker(QObject* parent) :
  QObject(parent),
  m_reply(nullptr),
  m_respectRules(false),
  m_success(true)
{
  getUpdateRules(m_updateRules);

  m_netManager = new QNetworkAccessManager();
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible)
    connect(m_netManager, &QNetworkAccessManager::finished, this, &TupdateChecker::replySlot);
}


void TupdateChecker::check(bool checkRules){
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible) {
      m_respectRules = checkRules;
      if (!m_respectRules)
        emit updateMessage(tr("Checking for updates. Please wait..."));
      if (!m_respectRules || (m_updateRules.enable && isUpdateNecessary(m_updateRules))) {
          QString requestAddr = QStringLiteral("http");
  #if defined(Q_OS_ANDROID)
          bool hasSSL = false;
  #else
          bool hasSSL = QSslSocket::supportsSsl();
  #endif
          if (hasSSL) {
            requestAddr += QStringLiteral("s");
//             qDebug() << "[TupdateChecker] SSL is supported, using https protocol to check updates";
          }
          requestAddr += QString("://nootka.sldc.pl/ch/version.php?v=%1").arg(QLatin1String(NOOTKA_VERSION));
          QUrl requestUrl(requestAddr);
          QNetworkRequest request(requestUrl);
  #if defined(Q_OS_WIN32)
            request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E)");
  #elif defined (Q_OS_ANDROID)
            request.setRawHeader("User-Agent" , "Mozilla/5.0 (Linux; Android 4.0.4; AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.133 Mobile Safari/535.19");
  #elif defined(Q_OS_LINUX)
            request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux i686 (x86_64); AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.133 Mobile Safari/535.19");
  #else
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_0) AppleWebKit/534.57.2 (KHTML, like Gecko) ");
  #endif
          if (hasSSL)
            request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
          m_reply = m_netManager->get(request);
          connect(m_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &TupdateChecker::errorSlot);
    } else
        emit updateMessage(QStringLiteral("No need for updates"));
  } else
      emit updateMessage(QStringLiteral("offline"));
}


TupdateChecker::~TupdateChecker()
{
  if (m_reply) {
    qDebug() << "[TupdateChecker] Update checking doesn't finish, trying to abort...";
    disconnect(m_netManager, &QNetworkAccessManager::finished, this, &TupdateChecker::replySlot);
    disconnect(m_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &TupdateChecker::errorSlot);
    m_reply->abort();
    m_reply->close();
    m_reply->deleteLater();
  }
  delete m_netManager;
}


void TupdateChecker::errorSlot(QNetworkReply::NetworkError err) {
  if (!m_respectRules)
    emit updateMessage(QString("An error occurred: %1").arg(QVariant::fromValue(err).toString()));
  m_success = false;
}


void TupdateChecker::replySlot(QNetworkReply* netReply) {
  if (m_success) {
    QString replyString(netReply->readAll());
    QStringList replyLines = replyString.split(QLatin1String(";"), QString::SkipEmptyParts);
    QString newVersionStr = replyLines.at(0);
    if (newVersionStr.contains(QLatin1String("Nootka:")))
      newVersionStr.replace(QLatin1String("Nootka:"), QString());
    else
      m_success = false;
    if (m_success) {
        auto currVers = QVersionNumber::fromString(QLatin1String(NOOTKA_VERSION));
        auto onlineVers = QVersionNumber::fromString(newVersionStr);
        emit updateMessage(QStringLiteral("success"));
        replyLines.removeFirst();
        QString changes = replyLines.join(QString());
        if (currVers != onlineVers) {
            if (currVers < onlineVers)
                showUpdateSummary(newVersionStr, changes, &m_updateRules);
            else {
                auto newerMessage = QStringLiteral("Seems You have newer version than available online. Great! Happy testing!");
                if (!m_respectRules)
                  showUpdateSummary(newVersionStr, newerMessage, &m_updateRules);
                else
                  qDebug() << "[TupdateChecker]" << newerMessage;
            }
        } else if (!m_respectRules)
            showUpdateSummary(newVersionStr, QString(), &m_updateRules);
        m_updateRules.recentDate = QDate::currentDate();
        saveUpdateRules(m_updateRules);
        emit updateMessage(QString());
    }
  }
  if (m_respectRules)
    emit updateMessage(QString());

  netReply->abort();
  netReply->close();
  netReply->deleteLater();
  m_reply = nullptr;
}






