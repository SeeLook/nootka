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


#include "tupdateitem.h"
#include <nootkaconfig.h>
#include <tpath.h>
#include <qtr.h>

#include <QtCore/qversionnumber.h>
#include <QtCore/qdebug.h>
#include <QtGui/qguiapplication.h>


TupdateItem* TupdateItem::m_instance = nullptr;


TupdateItem::TupdateItem(QQuickItem* parent) :
  QQuickItem(parent)
{
  if (m_instance) {
    qDebug() << "[TupdateItem] instance already exists! FIXME!";
    return;
  }
  m_instance = this;
}


void TupdateItem::setVersion(const QString& v) {
  m_version = v;
  emit versionChanged();
  emit onlineIsNewerChanged();
  emit changesChanged();
}


void TupdateItem::setRules(const QSize& r) {
  m_rules.period = static_cast<TupdateRules::Eperiod>(qBound(0, r.width(), 2));
  m_rules.checkForAll = r.height() == 0;
  emit rulesChanged();
}


QString TupdateItem::changes() {
  if (onlineIsNewer())
    return tr("News:") + m_changes.replace(QLatin1String("\n"), QLatin1String("<br>"));
  else if (QVersionNumber::fromString(m_version) < QVersionNumber::fromString(NOOTKA_VERSION))
    return QLatin1String("<br><font size=\"5\">") + m_changes.replace(QLatin1String("."), QLatin1String(".<br>")) + QLatin1String("</font>");
  else
    return QLatin1String("<br><font size=\"5\">")
          + QGuiApplication::translate("TupdateSummary", "No changes found.<br>This version is up to date.")
          + QLatin1String("</font>");
}


void TupdateItem::setChanges(const QString& ch) {
  m_changes = ch;
  emit changesChanged();
}


void TupdateItem::setUpdateCheck(bool uc) {
  m_rules.enable = uc;
  emit updateCheckChanged();
}


void TupdateItem::setUpdateRules(TupdateRules* r, const QString& vers) {
  setVersion(vers);
  setUpdateCheck(r->enable);
  setRules(QSize(static_cast<int>(r->period), (r->checkForAll ? 0 : 1)));
}


bool TupdateItem::onlineIsNewer() const {
  return QVersionNumber::fromString(m_version) > QVersionNumber::fromString(NOOTKA_VERSION);
}


TupdateItem::~TupdateItem() {
  saveUpdateRules(m_rules);
  m_instance = nullptr;
}

