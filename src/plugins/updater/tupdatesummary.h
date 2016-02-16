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


#ifndef TUPDATESUMMARY_H
#define TUPDATESUMMARY_H

#include <QDialog>

class QPushButton;
class TupdateRulesWdg;
class TupdateRules;



/** Displays dialog with summary of nootka-updater. 
 * If @param version is emty - displays no updates found.
 * When TupdateRules is given - it shows config widget inside. 
 */
class TupdateSummary : public QDialog
{

  Q_OBJECT
  
public:
    TupdateSummary(QString version, QString changes, TupdateRules *updateRules = 0, QWidget *parent = 0);
    virtual ~TupdateSummary();
    
protected slots:
    void okButtonSlot();
    
private:
    TupdateRules *m_updateRules;
    TupdateRulesWdg *m_rulesWidget;
    QPushButton *m_okButton;
};

#endif // TUPDATESUMMARY_H
