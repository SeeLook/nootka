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


#include <QSettings>
#include "updatefunctions.h"
#include "tupdatesummary.h"
#include <QDebug>


void getUpdateRules(TupdateRules& updateRules) {
#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings settings;
#endif
    settings.beginGroup("Updates");
      updateRules.enable = settings.value("enableUpdates", true).toBool();
      updateRules.recentDate = settings.value("recentDate", QDate(2012, 12, 31)).toDate();
      updateRules.period = EupdatePeriod(settings.value("period", 0).toInt());
      updateRules.checkForAll = settings.value("checkForAll", true).toBool();
    settings.endGroup();
    settings.beginGroup("General");
      updateRules.curentVersion = settings.value("version", "").toString();
    settings.endGroup();
}

void saveUpdateRules(TupdateRules& updateRules) {
#if defined(Q_OS_WIN32) // I hate mess in Win registry
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Nootka", "Nootka");
#else
    QSettings settings;
#endif
    settings.beginGroup("Updates");
      settings.setValue("enableUpdates", updateRules.enable);
      settings.setValue("recentDate", updateRules.recentDate);
      settings.setValue("period", (int)updateRules.period);
      settings.setValue("checkForAll", updateRules.checkForAll);
    settings.endGroup();
}


bool isUpdateNecessary(TupdateRules& updateRules) {
  int passedDays = updateRules.recentDate.daysTo(QDate::currentDate());
  qDebug() << passedDays;
  bool necessariness = false;
  if (passedDays) {
    switch(updateRules.period) {
      case e_daily:
        if (passedDays >= 1) necessariness = true;
        break;
      case e_weekly:
        if (passedDays >= 7) necessariness = true;
        break;
      case e_monthly:
        if (passedDays >= 30) necessariness = true;
        break;
    }
  }
  return necessariness;
}

bool isNewVersionStable(QString version) {
  bool nonStable = false;
  nonStable = version.contains("alpha");
  nonStable = version.contains("beta");
  nonStable = version.contains("rc");
  return !nonStable;
}

void showUpdateSummary(QString version, QString changes, TupdateRules* rules) {
    TupdateSummary *sumaryDlg = new TupdateSummary(version, changes, rules);
    sumaryDlg->exec();
    delete sumaryDlg;
}







