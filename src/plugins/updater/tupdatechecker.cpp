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
  getUpdateRules(m_updateRules);

  m_netManager = new QNetworkAccessManager(qApp);
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible)
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replySlot(QNetworkReply*)));
//   connect(this, &TupdateChecker::communicate, this, &TupdateChecker::communicateSlot);
}


void TupdateChecker::check(bool checkRules){
  if (m_netManager->networkAccessible() == QNetworkAccessManager::Accessible) {
    m_respectRules = checkRules;
    if (!m_respectRules)
      emit communicate(tr("Checking for updates. Please wait..."));
    if (!m_respectRules || (m_updateRules.enable && isUpdateNecessary(m_updateRules))) {
        QNetworkRequest request(QUrl(QString("http://nootka.sldc.pl/ch/version.php?v=%1").arg(QStringLiteral(NOOTKA_VERSION))));
        // This is additional hosting to improve updates system. It is much faster than sf.net
        //         QNetworkRequest request(QUrl("http://nootka.sourceforge.net/ch/version.php"));
  #if defined(Q_OS_WIN32)
          request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C; .NET4.0E)");
  #elif defined(Q_OS_LINUX)
          request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux i686 (x86_64); ");
  #else
          request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_6_8) AppleWebKit/534.57.2 (KHTML, like Gecko) ");
  #endif
          m_reply = m_netManager->get(request);
          connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorSlot(QNetworkReply::NetworkError)));
    } else
        emit communicate("No need for updates");
  } else
      emit communicate(QLatin1String("offline"));
}


TupdateChecker::~TupdateChecker()
{
	delete m_netManager;
}

void TupdateChecker::errorSlot(QNetworkReply::NetworkError err) {
  if (!m_respectRules)
    emit communicate(QString("An error occurred: %1").arg((int)err));
  m_success = false;
}


void TupdateChecker::replySlot(QNetworkReply* netReply) {
  if (m_success) {
      QString replyString(netReply->readAll());
      QStringList replyLines = replyString.split(";", QString::SkipEmptyParts);
      QString newVersion = replyLines.at(0);
      if (newVersion.contains("Nootka:"))
        newVersion.replace("Nootka:", "");
      else 
        m_success = false;
      if (m_success) {
          emit communicate("success");
          replyLines.removeFirst();
          QString changes = replyLines.join("");
          if (m_updateRules.curentVersion != newVersion) {
            showUpdateSummary(newVersion, changes, m_parentWidget, &m_updateRules);
          } else if (!m_respectRules) {
              showUpdateSummary("", "", m_parentWidget, &m_updateRules);
          }
          m_updateRules.recentDate = QDate::currentDate();
          saveUpdateRules(m_updateRules);
      }
  }
  if (m_respectRules)
		emit communicate("checking finished");
  netReply->abort();
  netReply->close();
  netReply->deleteLater();
}


void TupdateChecker::communicateSlot(const QString& message) {
  qDebug() << message;
}




