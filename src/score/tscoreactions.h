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

class QIcon;
class QShortcut;
class TscoreKeys;
class TpushButton;
class QAction;
class TmainScore;

/** 
 * This class manages QActions in a score
 */
class TscoreActions : public QObject
{

	Q_OBJECT
	
public:
	explicit TscoreActions(TmainScore* sc);
	
	QAction* zoomIn() { return m_inZoom; }
	QAction* zoomOut() { return m_outZoom; }
	QAction* staffDown() { return m_staffDown; }
	QAction* staffUp() { return m_staffUp; }
	QAction* firstNote() { return m_firstNote; }
	QAction* lastNote() { return m_lastNote; }
	QAction* clearScore() { return m_clear; }
	
	TpushButton* extraAccids() { return m_accidsButt; }
	TpushButton* noteNames() { return m_namesButt; }
	
	void assignKeys(TscoreKeys *sKeys); /** Joins external keyboard shortcuts with those actions. */
	void disableActions(bool dis); /** Disables or enables all actions (theirs key shortcuts as well) */
	
private:
	QAction											*m_inZoom, *m_outZoom;
	QAction											*m_staffDown, *m_staffUp, *m_firstNote ,*m_lastNote;
	QAction											*m_clear;
	TpushButton									*m_accidsButt, *m_namesButt;
	TscoreKeys									*m_keys;
	QList<QAction*>							 m_actions; /** List of pointers to all actions */
	
private:
	void assocActionAndKey(QAction* act, QShortcut *key);
	QAction* createAction(const QString& ico, const QString& statTip);
	QAction* createAction(const QIcon& ico, const QString& statTip);
	void equipAcction(QAction* act, const QString& statTip); /** Common method during creation of an action. */
};

#endif // TSCOREACTIONS_H
