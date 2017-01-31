/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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
#include <tinitcorelib.h>



void getUpdateRules(TupdateRules& updateRules) {

  Tcore::gl()->config->beginGroup("Updates");
    updateRules.enable = Tcore::gl()->config->value("enableUpdates", true).toBool();
    updateRules.recentDate = Tcore::gl()->config->value("recentDate", QDate(2012, 12, 31)).toDate();
    updateRules.period = EupdatePeriod(Tcore::gl()->config->value("period", 0).toInt());
    updateRules.checkForAll = Tcore::gl()->config->value("checkForAll", true).toBool();
  Tcore::gl()->config->endGroup();
  updateRules.curentVersion = NOOTKA_VERSION;
}

void saveUpdateRules(TupdateRules& updateRules) {
  Tcore::gl()->config->beginGroup("Updates");
    Tcore::gl()->config->setValue("enableUpdates", updateRules.enable);
    Tcore::gl()->config->setValue("recentDate", updateRules.recentDate);
    Tcore::gl()->config->setValue("period", (int)updateRules.period);
    Tcore::gl()->config->setValue("checkForAll", updateRules.checkForAll);
  Tcore::gl()->config->endGroup();
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
  nonStable = version.contains("alpha");
  nonStable = version.contains("beta");
  nonStable = version.contains("rc");
  return !nonStable;
}


void showUpdateSummary(QString version, QString changes, QWidget* parent, TupdateRules* rules) {
  TupdateSummary *sumaryDlg = new TupdateSummary(version, changes, rules, parent);
  sumaryDlg->exec();
  delete sumaryDlg;
}







