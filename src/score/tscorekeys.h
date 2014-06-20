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

#ifndef TSCOREKEYS_H
#define TSCOREKEYS_H

#include <QObject>

class QShortcut;
class TmainScore;

/**
 * This class manages keyboard shortcuts for the score
 */
class TscoreKeys : public QObject
{
	
public:
	explicit TscoreKeys(TmainScore* sc);
	
	QShortcut* nextNote() { return m_nextNote; }
	QShortcut* prevNote() { return m_prevNote; }
	QShortcut* firstNote() { return m_firstNote; }
	QShortcut* lastNote() { return m_lastNote; }
	QShortcut* staffUp() { return m_staffUp; }
	QShortcut*	staffDown() { return m_staffDown; }
	QShortcut* clearScore() { return m_clearScore; }
	QShortcut* deleteCurrentNote() { return m_delCurrNote; }
	
	
protected:
	TmainScore* score() { return m_score; }
	
	
private:
	TmainScore									*m_score;
	QShortcut 									*m_nextNote, *m_prevNote;
	QShortcut 									*m_firstNote, *m_lastNote;
	QShortcut										*m_staffUp, *m_staffDown;
	QShortcut										*m_clearScore, *m_delCurrNote;

};

#endif // TSCOREKEYS_H
