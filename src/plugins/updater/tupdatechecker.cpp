/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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
#include "tupdaterplugin.h"
#include <nootkaconfig.h>
#include <QtWidgets/qmessagebox.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>


TupdateChecker::TupdateChecker(QObject* parent, QWidget* parentWidget) :
  QObject(0),
  m_parentWidget(parentWidget),
  m_respectRules(false),
  m_reply(0),
  m_success(true)
{
  Q_UNUSED(parent)
  getUpdateRules(m_updateRules);

  m_netManager = new QNetworkAccessManager(qApp);
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible)
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replySlot(QNetworkReply*)));
}


void TupdateChecker::check(bool checkRules){
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible) {
    m_respectRules = checkRules;
    if (!m_respectRules)
      emit updateMessage(Torders::e_updaterChecking);
    if (!m_respectRules || (m_updateRules.enable && isUpdateNecessary(m_updateRules))) {
        QNetworkRequest request(QUrl(QString("http://nootka.sldc.pl/ch/version.php?v=%1").arg(QStringLiteral(NOOTKA_VERSION))));
        // This is additional hosting to improve updates system. It is much faster than sf.net
        //         QNetworkRequest request(QUrl("http://nootka.sourceforge.net/ch/version.php"));
  #if defined(Q_OS_WIN32)
          request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E)");
  #elif defined (Q_OS_ANDROID)
          request.setRawHeader("User-Agent" , "Mozilla/5.0 (Linux; Android 4.0.4; AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.133 Mobile Safari/535.19");
  #elif defined(Q_OS_LINUX)
          request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux i686 (x86_64); AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.133 Mobile Safari/535.19");
  #else
          request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_0) AppleWebKit/534.57.2 (KHTML, like Gecko) ");
  #endif
          m_reply = m_netManager->get(request);
          connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorSlot(QNetworkReply::NetworkError)));
    } else
        emit updateMessage(Torders::e_updaterNoNeed);
  } else
      emit updateMessage(Torders::e_updaterOffline);
}


TupdateChecker::~TupdateChecker()
{
  delete m_netManager;
}

void TupdateChecker::errorSlot(QNetworkReply::NetworkError err) {
  if (!m_respectRules)
      emit updateMessage(Torders::e_updaterError + err);
  m_success = false;
}


void TupdateChecker::replySlot(QNetworkReply* netReply) {
  if (m_success) {
      QString replyString(netReply->readAll());
      QStringList replyLines = replyString.split(QLatin1String(";"), QString::SkipEmptyParts);
      QString newVersion = replyLines.at(0);
      if (newVersion.contains(QLatin1String("Nootka:")))
        newVersion.replace(QLatin1String("Nootka:"), QString());
      else 
        m_success = false;
      if (m_success) {
          emit updateMessage(Torders::e_updaterSuccess);
          replyLines.removeFirst();
          QString changes = replyLines.join(QString());
          if (m_updateRules.curentVersion != newVersion) {
            showUpdateSummary(newVersion, changes, m_parentWidget, &m_updateRules);
          } else if (!m_respectRules) {
              showUpdateSummary(QString(), QString(), m_parentWidget, &m_updateRules);
          }
          m_updateRules.recentDate = QDate::currentDate();
          saveUpdateRules(m_updateRules);
      }
  }
  if (m_respectRules)
      emit updateMessage(Torders::e_updaterFinished);
  netReply->abort();
  netReply->close();
  netReply->deleteLater();
}






