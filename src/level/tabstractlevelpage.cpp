/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tabstractlevelpage.h"
#include "tlevelcreatordlg.h"
#include <exam/tlevel.h>

Tlevel* 								TabstractLevelPage::m_workLevel = 0;
int 										TabstractLevelPage::m_cnt = 0;

TabstractLevelPage::TabstractLevelPage(TlevelCreatorDlg* creator) :
	QWidget(0)
{
	if (!m_workLevel)
		m_workLevel = new Tlevel();
	m_cnt++;
	connect(this, SIGNAL(levelChanged()), creator, SLOT(levelWasChanged()));
}


TabstractLevelPage::~TabstractLevelPage()
{
	m_cnt--;
	if (m_cnt == 0) {
		delete m_workLevel;
		m_workLevel = 0;
	}		
}


void TabstractLevelPage::changedLocal() {
	saveLevel(m_workLevel);
	emit levelChanged();
}










