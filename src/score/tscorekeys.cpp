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

#include "tscorekeys.h"
#include "tmainscore.h"
#include <QShortcut>

TscoreKeys::TscoreKeys(TmainScore* sc) :
	QObject(sc),
	m_score(sc)
{
	m_nextNote = new QShortcut(QKeySequence(Qt::Key_Right), score());
		connect(m_nextNote, SIGNAL(activated()), sc, SLOT(moveSelectedNote()));
	m_prevNote = new QShortcut(QKeySequence(Qt::Key_Left), score());
		connect(m_prevNote, SIGNAL(activated()), sc, SLOT(moveSelectedNote()));
	m_lastNote = new QShortcut(QKeySequence(Qt::Key_End), score());
// 		connect(m_lastNote, SIGNAL(activated()), sc, SLOT(moveSelectedNote()));
	m_firstNote = new QShortcut(QKeySequence(Qt::Key_Home), score());
// 		connect(m_firstNote, SIGNAL(activated()), sc, SLOT(moveSelectedNote()));
	m_staffUp = new QShortcut(QKeySequence(Qt::Key_PageUp), score());
// 		connect(m_staffUp, SIGNAL(activated()), sc, SLOT(moveSelectedNote()));
	m_staffDown = new QShortcut(QKeySequence(Qt::Key_PageDown), score());
// 		connect(m_staffDown, SIGNAL(activated()), sc, SLOT(moveSelectedNote()));
	m_clearScore = new QShortcut(QKeySequence("Shift+DEL"), score());
}






