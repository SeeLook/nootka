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


#ifndef TUPDATESUMMARY_H
#define TUPDATESUMMARY_H


#include "updatefunctions.h"
#include <QtQuick/qquickitem.h>


/**
 * Displays dialog with summary of Nootka-updater.
 * Version are determined and compared from strings by @p QVersionNumber.
 * When @p TupdateRules is given - it shows configuration widget inside.
 */
class TupdateItem : public QQuickItem
{

  Q_OBJECT
  Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
  Q_PROPERTY(QString changes READ changes WRITE setChanges NOTIFY changesChanged)
  Q_PROPERTY(QSize rules READ rules WRITE setRules NOTIFY rulesChanged)
  Q_PROPERTY(bool updateCheck READ updateCheck WRITE setUpdateCheck NOTIFY updateCheckChanged)
  Q_PROPERTY(bool onlineIsNewer READ onlineIsNewer NOTIFY onlineIsNewerChanged)

public:
  TupdateItem(QQuickItem *parent = nullptr);
  ~TupdateItem() override;

  static TupdateItem* instance() { return m_instance; }

      /**
       * This is version obatined from remote.
       */
  QString version() const { return m_version; }
  void setVersion(const QString& v);

      /**
       * Changed from remote or infromation that all is up to date.
       */
  QString changes();
  void setChanges(const QString& ch);

      /**
       * Store update chech rules in @p QSize.
       * @p width is period of checking (0 - daily, 1 - weekly, 2 - monthly)
       * @p height 0 is for all checks (alpha, beta, rc) and 1 is for stable releases only
       */
  QSize rules() const { return QSize(static_cast<int>(m_rules.period), m_rules.checkForAll ? 0 : 1); }
  void setRules(const QSize& r);

  bool updateCheck() const { return m_rules.enable; }
  void setUpdateCheck(bool uc);

  void setUpdateRules(TupdateRules* r, const QString& vers);

  bool onlineIsNewer() const;

signals:
  void versionChanged();
  void changesChanged();
  void rulesChanged();
  void updateCheckChanged();
  void onlineIsNewerChanged();

private:
  TupdateRules            m_rules;
  QString                 m_changes;
  QString                 m_version;

  static TupdateItem     *m_instance;
};

#endif // TUPDATESUMMARY_H
