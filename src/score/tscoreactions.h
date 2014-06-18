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



#ifndef TSCOREACTIONS_H
#define TSCOREACTIONS_H

#include <QObject>

class QShortcut;
class TscoreKeys;
class TpushButton;
class QAction;
class TmainScore;

/** 
 * This class manages Qions in a score
 */
class TscoreActions : public QObject
{

public:
	explicit TscoreActions(TmainScore* sc, const QString& path);
	
	QAction* zoomIn() { return m_inZoom; }
	QAction* zoomOut() { return m_outZoom; }
	QAction* staffDown() { return m_staffDown; }
	QAction* staffUp() { return m_staffUp; }
	QAction* firstNote() { return m_firstNote; }
	QAction* lastNote() { return m_lastNote; }
	QAction* clearScore() { return m_clear; }
	
	TpushButton* extraAccids() { return m_accidsButt; }
	TpushButton* noteNames() { return m_namesButt; }
	
			/** Joins external keyboard shortcuts with those actions. */
	void assignKeys(TscoreKeys *sKeys);
	
private:
	QAction											*m_inZoom, *m_outZoom;
	QAction											*m_staffDown, *m_staffUp, *m_firstNote ,*m_lastNote;
	QAction											*m_clear;
	TpushButton									*m_accidsButt, *m_namesButt;
	TscoreKeys									*m_keys;
	
private:
	void assocActionAndKey(QAction* act, QShortcut *key);
};

#endif // TSCOREACTIONS_H
