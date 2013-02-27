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


#include "tupdatesummary.h"
#include "tupdateruleswdg.h"
#include <QVBoxLayout>
#include <QLabel>

TupdateSummary::TupdateSummary(QString version, QString changes, TupdateRules* updateRules, QWidget* parent): 
  QDialog(parent),
  m_updateRules(updateRules)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *lab = new QLabel(this);
    mainLay->addWidget(lab);
    if (version != "") {
      changes.replace("\n", "<br>");
      lab->setText(tr("New Nootka %1 is available.").arg(version) + "<br><br><b>" + tr("News:") + 
        "</b>" + changes);
    } else {
      lab->setText(tr("No changes found.<br>This version is up to date."));
    }
    if (m_updateRules) {
      m_rulesWidget = new TupdateRulesWdg(m_updateRules, this);
      mainLay->addWidget(m_rulesWidget);
    }
    
    setLayout(mainLay);
}



TupdateSummary::~TupdateSummary() {}

void TupdateSummary::okButtonSlot() {

}
