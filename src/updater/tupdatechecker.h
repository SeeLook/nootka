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

#ifndef TUPDATECHECKER_H
#define TUPDATECHECKER_H

#include "updatefunctions.h"
#include <QtCore/qobject.h>
#include <QtNetwork/qnetworkreply.h>

class QNetworkAccessManager;

/**
 * Connects with nootka.sldc.pl to check updates.
 * Displays summary dialog after and console output.
 * Also emits signals with output communicates.
 */
class TupdateChecker : public QObject
{
    Q_OBJECT

public:
    TupdateChecker(QObject *parent = nullptr);
    virtual ~TupdateChecker();

    /**
     * Performs checking.
     * When @param checkRules = @p true it checks is it necessary in nootka config.
     * Doesn't display dialog when no new version was found.
     * otherwise forces connection and when versions are the same displays 'no update found'.
     */
    void check(bool checkRules = true);

signals:
    void updateMessage(QString);
    void updateSummary(const QString &version, const QString &changes, TupdateRules *rules);

protected:
    void replySlot(QNetworkReply *netReply);

    /**
     * Grabs connection errors, if any.
     */
    void errorSlot(QNetworkReply::NetworkError err);

    void showUpdateSummary(QString version, QString changes, TupdateRules *rules = nullptr);

private:
    QNetworkAccessManager *m_netManager;
    QNetworkReply *m_reply;
    QString m_curVersion;
    bool m_respectRules;
    bool m_success;
    TupdateRules m_updateRules;
};

#endif // TUPDATECHECKER_H
