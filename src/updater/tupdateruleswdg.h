/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#ifndef TUPDATERULESWDG_H
#define TUPDATERULESWDG_H

#include <QWidget>

class QGroupBox;
struct TupdateRules;
class QButtonGroup;
class QRadioButton;

class TupdateRulesWdg : public QWidget
{

  Q_OBJECT
  
public:
    TupdateRulesWdg(TupdateRules *updateRules, QWidget *parent = 0);
    virtual ~TupdateRulesWdg();
        /** Stores settnigs to Nootka config file. */
    void saveSettings();
    
    
private:
    QGroupBox *m_updatesEnableGr;
    QRadioButton *m_dailyRadio, *m_weeklyRadio, *m_monthlyRadio, *m_stableRadio, *m_allRadio;
    QButtonGroup *m_periodGroup, *m_stableAllGroup;
    TupdateRules *m_updateRules;
};

#endif // TUPDATERULESWDG_H
