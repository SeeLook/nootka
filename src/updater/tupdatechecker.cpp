/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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
#include "tupdateitem.h"
#include <nootkaconfig.h>
#include <tnootkaqml.h>
#include <tglobals.h>

#include <QtCore/qsize.h>
#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qsettings.h>
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


void TupdateChecker::check(bool checkRules) {
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible) {
      m_respectRules = checkRules;
      if (!m_respectRules)
        emit updateMessage(tr("Checking for updates. Please wait..."));
      if (!m_respectRules || (m_updateRules.enable && isUpdateNecessary(m_updateRules))) {
          QString requestAddr = QStringLiteral("http");
  #if defined(Q_OS_ANDROID) || defined QT_NO_SSL
          bool hasSSL = false;
  #else
          bool hasSSL = QSslSocket::supportsSsl();
  #endif

          if (hasSSL)
            requestAddr += QStringLiteral("s");
          requestAddr += QString("://nootka.sldc.pl/ch/version.php?v=%1").arg(QLatin1String(NOOTKA_VERSION));
          QUrl requestUrl(requestAddr);
          QNetworkRequest request(requestUrl);
  #if defined(Q_OS_WIN)
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.74 Safari/537.36 Edg/79.0.309.43");
  #elif defined (Q_OS_ANDROID)
            request.setRawHeader("User-Agent" , "Mozilla/5.0 (Linux; Android 10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.66 Mobile Safari/537.36");
  #elif defined(Q_OS_LINUX)
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Linux x86_64; rv:87.0) Gecko/20100101 Firefox/87.0");
  #else
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_3) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15");
  #endif

  #ifndef QT_NO_SSL
          if (hasSSL)
            request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  #endif
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
            if (currVers < onlineVers) {
                if (m_respectRules)
                  showUpdateSummary(newVersionStr, changes, &m_updateRules);
                else
                  emit updateSummary(newVersionStr, changes, &m_updateRules);
            } else {
                auto newerMessage = QStringLiteral("Seems You have newer version than available online. Great! Happy testing!");
                if (!m_respectRules)
                  emit updateSummary(newVersionStr, newerMessage, &m_updateRules);
                else
                  qDebug() << "[TupdateChecker]" << newerMessage;
            }
        } else if (!m_respectRules)
            emit updateSummary(newVersionStr, changes, &m_updateRules);
        GLOB->config->setValue(QLatin1String("Updates/recentDate"), QDate::currentDate());
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


void TupdateChecker::showUpdateSummary(QString version, QString changes, TupdateRules* rules) {
  auto appEngine = qobject_cast<QQmlApplicationEngine*>(NOO->qmlEngine());

  if (appEngine && !appEngine->rootObjects().isEmpty()) {
    auto nootWin = appEngine->rootObjects().first();
    if (nootWin && QString(nootWin->metaObject()->className()).contains("MainWindow_QMLTYPE")) {
      QMetaObject::invokeMethod(nootWin, "showDialog", Q_ARG(QVariant, 9));
      auto dialogLoader = qvariant_cast<QObject*>(nootWin->property("dialogLoader"));
      if (dialogLoader) {
        auto updateContent = qvariant_cast<QObject*>(dialogLoader->property("currentDialog"));
        auto updateItem = qobject_cast<TupdateItem*>(updateContent);
        if (updateItem) {
          updateItem->setUpdateRules(rules, version);
          updateContent->setProperty("changes", QVariant::fromValue(changes));
        }
      }
    }
  }
}
