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

#ifndef UPDATEFUNCTIONS_H
#define UPDATEFUNCTIONS_H

#include <QtCore/qdatetime.h>

class TupdateRules
{
public:
    enum Eperiod { e_daily = 0, e_weekly = 1, e_monthly = 2 };

    bool enable; /**< is updating enabled */
    Eperiod period; /**< how often checking has to be perform */
    bool checkForAll; /**< if true check for all versions (alpha, beta, rc) */
};

/**
 * Fulfills @p updateRules with configuration file content.
 */
void getUpdateRules(TupdateRules &updateRules);

/**
 * Compares date of recent checking, current date, update period
 * and determine is update necessary.
 */
bool isUpdateNecessary(TupdateRules &updateRules);

bool isNewVersionStable(QString version);

/**
 * Stores rules in Nootka configuration file
 */
void saveUpdateRules(TupdateRules &updateRules);

#endif // UPDATEFUNCTIONS_H
