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


#ifndef TUPDATECHECKER_H
#define TUPDATECHECKER_H

#include <QtCore/QObject>
#include <QNetworkReply>
#include "updatefunctions.h"

class QNetworkReply;
class QNetworkAccessManager;


class TupdateChecker : public QObject
{
  Q_OBJECT
  
public:
  TupdateChecker(bool hasRules = true, QObject *parent = 0);
  virtual ~TupdateChecker();
  
  static QString getVersion();
  static void showUpdateDialog(QString version, QString changes);
  
protected slots:
  void replySlot(QNetworkReply* netReply);
  void errorSlot(QNetworkReply::NetworkError err);
  
private:
  QNetworkAccessManager *m_netManager;
  QString m_curVersion;
  bool m_hasRules;
  TupdateRules m_updateRules;
};

#endif // TUPDATECHECKER_H
