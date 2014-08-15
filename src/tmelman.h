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

#ifndef TMELMAN_H
#define TMELMAN_H

#include <QObject>
#include <QIcon>
#include <QKeySequence>

class QAction;
class QMainWindow;
class QWidgetAction;
class QMenu;
class QToolButton;
class TmainScore;


/** 
 * Melody manager - graphically represented by QToolButton 
 * in Nootka tool bar with actions to manage melodies.
 */
class TmelMan : public QObject
{
	Q_OBJECT
	
public:
	explicit TmelMan(TmainScore* score, const QString& path);
	
	QWidgetAction* melodyAction() { return m_melAct; }
	QToolButton* button() { return m_button; }
	
public slots:
	void playMelodySlot();
	void recordMelodySlot();
	void randomizeMelodySlot();
	
private:
	QAction* createAction(const QString& t, const char* slot, const QKeySequence& k = QKeySequence(), const QIcon& i = QIcon());
	
private:
	TmainScore						*m_score;
	QWidgetAction					*m_melAct;
	QToolButton						*m_button;
	QMenu									*m_menu;
	QAction								*m_playMelAct, *m_recMelAct;
	QString								 m_path;
};

#endif // TMELMAN_H
