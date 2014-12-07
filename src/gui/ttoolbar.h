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

#ifndef TTOOLBAR_H
#define TTOOLBAR_H

#include <QToolBar>
#include <QPointer>

class TmelMan;
class QMainWindow;
class QAction;


/** 
 * This is main tool bar in Nootka. 
 * It handles all actions in the app.
 * Due to all widgets in Nootka are squeezed into QGraphicsView as items
 * it has no parent, @p QMainWindow in constructor is used as an owner of the actions.
 * @p addMelodyButton(@class TmelMan) takes control over melody button icon size and its style.
 */
class TtoolBar : public QToolBar
{

	Q_OBJECT
	
public:
	explicit TtoolBar(QMainWindow* mainWindow);
	
			/** Main actions */
	QAction *settingsAct, *levelCreatorAct, *startExamAct, *aboutAct, *analyseAct;
	
			/** Exam related */
	QAction *prevQuestAct, *checkAct;
	QPointer<QAction> nextQuestAct, repeatSndAct, correctAct, tuneForkAct, attemptAct;
	
			/** Changes names and icons of actions suitable to normal mode.
			 * Deletes actions related to exam.	 */
	void actionsAfterExam();
	
			/** Changes names and icons of actions suitable to exam.
			 * Creates common actions used in exams/exercises.
			 * Other ones (like @p repeatSndAct) has to be called separately. */
	void actionsToExam();
	
	void createRepeatSoundAction(); /** Creates @p repeatSndAct. Don't add to tool bar */
	void createCorrectAction(); /** Creates @p correctAct. Don't add to tool bar */
	void createTuneForkAction(); /** Creates @p tuneForkAct. Don't add to tool bar */
	void createAttemptAction(); /** Creates @p attemptAct. Don't add to tool bar */
	
			/** Removes actions for previous and next question @p prevQuestAct & @p nextQuestAct.
			 * Adds check answer action (@p checkAct) 
			 * and @p repeatSndAct when @p TRUE and @p tuneForkAct when @p TRUE	 */
	void setForQuestion(bool repeatSound, bool tuneFork);
	void setAfterAnswer(); /** Removes actions required during answering */
	
	void addMelodyButton(TmelMan* melBut);
	void setMelodyButtonVisible(bool vis); /** Hides or shows melody button */
	
	void setBarIconStyle(Qt::ToolButtonStyle iconStyle, int iconS);
	
private:
	TmelMan					*m_melButton;
	
};

#endif // TTOOLBAR_H
