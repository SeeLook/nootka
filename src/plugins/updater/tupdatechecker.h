/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#ifndef TUPDATECHECKER_H
#define TUPDATECHECKER_H

#include <QObject>
#include <QNetworkReply>
#include "updatefunctions.h"

class QNetworkAccessManager;

/**
 * Connects with nootka.sf.net to check updates.
 * Displays summary dialog after and console output.
 * Also emits signals with output communicates.
 */
class TupdateChecker : public QObject
{

  Q_OBJECT

public:
  TupdateChecker(QObject *parent = 0, QWidget* parentWidget = 0);
  virtual ~TupdateChecker();

    /** Performs checking.
     * When @param checkRules = true it checks is it necessary in nootka config.
     * Doesn't display dialog when no new version was found.
     * otherwise forces connection and when versions are the sane displays 'no update found'. */
  void check(bool checkRules = true);

signals:
  void updateMessage(int); /**< int values of type @p Torders::Eupdater with updater states */

protected slots:
  void replySlot(QNetworkReply* netReply);

    /** Grabs connection errors if any. */
  void errorSlot(QNetworkReply::NetworkError err);

private:
  QWidget                   *m_parentWidget;
  QNetworkAccessManager     *m_netManager;
  QNetworkReply             *m_reply;
  QString                    m_curVersion;
  bool                       m_respectRules;
  bool                       m_success;
  TupdateRules               m_updateRules;
  
};

#endif // TUPDATECHECKER_H
