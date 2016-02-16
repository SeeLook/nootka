/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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



#ifndef TSCOREACTIONS_H
#define TSCOREACTIONS_H

#include <QObject>
#include <QIcon>
#include <QKeySequence>


class QIcon;
class QWidgetAction;
class Tmenu;
class QToolButton;
class QShortcut;
class TscoreKeys;
class TpushButton;
class QAction;
class TmainScore;

/** 
 * This class manages QActions in a score.
 * It has a tool bar button @p button() to display drop down menu.
 * Its action is available through @p mainAction().
 */
class TscoreActions : public QObject
{

	Q_OBJECT
	
public:
	explicit TscoreActions(TmainScore* score);
  virtual ~TscoreActions();
  
  QToolButton* button() { return m_button; } /** Button displaying score icon and drop down menu with score actions */
  QWidgetAction* mainAction() { return m_mainAction; } /** This represents score button action. */
	
	QAction* zoomIn() { return m_inZoom; }
	QAction* zoomOut() { return m_outZoom; }
	
	QAction* nextNote() { return m_nextNote; }
  QAction* prevNote() { return m_prevNote; }
	QAction* staffDown() { return m_staffDown; }
	QAction* staffUp() { return m_staffUp; }
	QAction* firstNote() { return m_firstNote; }
	QAction* lastNote() { return m_lastNote; }
	
	QAction* deleteCurrentNote() { return m_delCurrNote; }
	QAction* clearScore() { return m_clear; }
	
	QAction* extraAccids() { return m_accidsAct; }
	QAction* noteNames() { return m_namesAct; }
	
	void disableActions(bool dis); /** Disables or enables all actions (theirs key shortcuts as well) */
  void setForExam(bool isExam); /** Disables actions unused during exams or enables them back. */
	
private:
  TmainScore                  *m_score;
  QToolButton                 *m_button;
  Tmenu                       *m_menu;
  QWidgetAction               *m_mainAction;
	QAction											*m_inZoom, *m_outZoom;
	QAction											*m_staffDown, *m_staffUp, *m_firstNote ,*m_lastNote, *m_nextNote, *m_prevNote;
	QAction											*m_clear, *m_delCurrNote;
	QAction   									*m_accidsAct, *m_namesAct;
	TscoreKeys									*m_keys;
	QList<QAction*>							 m_actions; /** List of pointers to all actions */
	
private:
  QAction* createAction(const QString& t, const char* slot, const QKeySequence& k = QKeySequence(), const QIcon& i = QIcon());
  
};

#endif // TSCOREACTIONS_H
