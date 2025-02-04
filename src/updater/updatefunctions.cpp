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

#include "updatefunctions.h"
#include <nootkaconfig.h>
#include <tglobals.h>

#include <QtCore/qsettings.h>

void getUpdateRules(TupdateRules &updateRules)
{
    GLOB->config->beginGroup(QLatin1String("Updates"));
    updateRules.enable = GLOB->config->value(QLatin1String("enableUpdates"), true).toBool();
    updateRules.period = static_cast<TupdateRules::Eperiod>(GLOB->config->value(QLatin1String("period"), 0).toInt());
    updateRules.checkForAll = GLOB->config->value(QLatin1String("checkForAll"), true).toBool();
    GLOB->config->endGroup();
}

void saveUpdateRules(TupdateRules &updateRules)
{
    GLOB->config->beginGroup(QLatin1String("Updates"));
    GLOB->config->setValue(QLatin1String("enableUpdates"), updateRules.enable);
    GLOB->config->setValue(QLatin1String("period"), static_cast<int>(updateRules.period));
    GLOB->config->setValue(QLatin1String("checkForAll"), updateRules.checkForAll);
    GLOB->config->endGroup();
}

bool isUpdateNecessary(TupdateRules &updateRules)
{
    auto recentDate = GLOB->config->value(QLatin1String("Updates/recentDate"), QDate(2012, 12, 31)).toDate();
    int passedDays = recentDate.daysTo(QDate::currentDate());
    bool necessariness = false;
    if (passedDays) {
        switch (updateRules.period) {
        case TupdateRules::e_daily:
            if (passedDays >= 1)
                necessariness = true;
            break;
        case TupdateRules::e_weekly:
            if (passedDays >= 7)
                necessariness = true;
            break;
        case TupdateRules::e_monthly:
            if (passedDays >= 30)
                necessariness = true;
            break;
        }
    }
    return necessariness;
}

bool isNewVersionStable(QString version)
{
    bool nonStable = false;
    nonStable = version.contains(QLatin1String("alpha"));
    nonStable = version.contains(QLatin1String("beta"));
    nonStable = version.contains(QLatin1String("rc"));
    return !nonStable;
}
