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

#include <QtCore/qsettings.h>
#include "updatefunctions.h"
#include "tupdatesummary.h"
#include <nootkaconfig.h>
#include <tglobals.h>



void getUpdateRules(TupdateRules& updateRules) {

  GLOB->config->beginGroup(QLatin1String("Updates"));
  updateRules.enable = GLOB->config->value(QLatin1String("enableUpdates"), true).toBool();
  updateRules.recentDate = GLOB->config->value(QLatin1String("recentDate"), QDate(2012, 12, 31)).toDate();
  updateRules.period = EupdatePeriod(GLOB->config->value(QLatin1String("period"), 0).toInt());
  updateRules.checkForAll = GLOB->config->value(QLatin1String("checkForAll"), true).toBool();
  GLOB->config->endGroup();
  updateRules.curentVersion = NOOTKA_VERSION;
}

void saveUpdateRules(TupdateRules& updateRules) {
  GLOB->config->beginGroup(QLatin1String("Updates"));
  GLOB->config->setValue(QLatin1String("enableUpdates"), updateRules.enable);
  GLOB->config->setValue(QLatin1String("recentDate"), updateRules.recentDate);
  GLOB->config->setValue(QLatin1String("period"), (int)updateRules.period);
  GLOB->config->setValue(QLatin1String("checkForAll"), updateRules.checkForAll);
  GLOB->config->endGroup();
}


bool isUpdateNecessary(TupdateRules& updateRules) {
  int passedDays = updateRules.recentDate.daysTo(QDate::currentDate());
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
  nonStable = version.contains(QLatin1String("alpha"));
  nonStable = version.contains(QLatin1String("beta"));
  nonStable = version.contains(QLatin1String("rc"));
  return !nonStable;
}


void showUpdateSummary(QString version, QString changes, TupdateRules* rules) {
  auto sumaryDlg = new TupdateSummary(version, changes, rules);
  sumaryDlg->exec();
  delete sumaryDlg;
}







