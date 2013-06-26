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


#include "tselectclef.h"
#include <tnotepixmap.h>
#include <QAction>
#include <QMenu>
#include <qtoolbar.h>

TselectClef::TselectClef(QMenu* parent) :
  QObject(parent),
  m_menu(parent),
  m_toolBar(0),
  m_parent(parent)
{
	createActions();
// 	m_menu->set
// 	menu->setStyleSheet(" QMenu { "
// "   icon-size: 32px; "
// " } " );
// 	QMenu::icon {
//   padding-left: 20px;
// }
}


TselectClef::TselectClef(QToolBar* parent) :
  QObject(parent),
  m_toolBar(parent),
  m_menu(0),
  m_parent(0)
{
	createActions();
	m_toolBar->setIconSize(QSize(40, 56));
}


TselectClef::~TselectClef()
{}


void TselectClef::selectClef(Tclef clef)
{

}


//######################################################################
//#################################### PRIVATE #########################
//######################################################################

void TselectClef::createActions() {
		m_trebleAct = getAction(Tclef(Tclef::e_treble_G));
		m_treble_8Act = getAction(Tclef(Tclef::e_treble_G_8down));
		m_bassAct = getAction(Tclef(Tclef::e_bass_F));
		m_bass_8Act = getAction(Tclef(Tclef::e_bass_F_8down));
		m_altoAct = getAction(Tclef(Tclef::e_alto_C));
		m_tenorAct = getAction(Tclef(Tclef::e_tenor_C));
		m_pianoAct = getAction(Tclef(Tclef::e_pianoStaff));
}


QAction* TselectClef::getAction(Tclef clef) {
		QIcon clefIcon(getNotePixmap(Tnote(0, 0, 0), clef.type()));
		QAction *action = new QAction(clefIcon, clef.name(), m_parent);
		action->setStatusTip("<b>" + clef.name() + "</b> (" + clef.desc() + ")");
		action->setCheckable(true);
		if (m_menu)
				m_menu->addAction(action);
		else
				m_toolBar->addAction(action);
		return action;
}




