/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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

#ifndef TTOOLBAR_H
#define TTOOLBAR_H

#include <QtWidgets/qtoolbar.h>
#include <QtCore/qpointer.h>

class TscoreActions;
class TnootkaLabel;
class TmelMan;
class QMainWindow;
class QAction;

#define   TOOLBAR     TtoolBar::instance()

/**
 * This is main tool bar in Nootka.
 * It handles all actions in the app.
 * Due to all widgets in Nootka are squeezed into QGraphicsView as items
 * it has no parent, @p QMainWindow in constructor is used as an owner of the actions.
 * @p addMelodyButton(@class TmelMan) takes control over melody button icon size and its style.
 * Under Mobile it is just container for actions.
 * Overridden @p addAction() makes action visible,
 * so created on demand menus can know that it is available.
 * @p removeAction() makes an action not visible,
 * (and of course takes it from the tool bar on desktop)
 * WARING! @p actions() list is not valid under mobile
 * as long as @p addAction() doesn't add it to this tool bar.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p TOOLBAR
 */
class TtoolBar : public QToolBar
{

	Q_OBJECT

public:
	explicit TtoolBar(const QString& version, QMainWindow* mainWindow);

  virtual ~TtoolBar();

  static TtoolBar* instance() { return m_instance; }

			/** Main actions */
	QAction *settingsAct, *levelCreatorAct, *startExamAct, *analyseAct;
  QAction *aboutAct, *aboutSimpleAct; /**< Button with Nootka logo and simple text & icon QAction */

			/** Exam related */
	QPointer<QAction> prevQuestAct, checkAct;
	QPointer<QAction> nextQuestAct, repeatSndAct, correctAct, tuneForkAct, attemptAct;

      /** TmelMan and TscoreActions has to be set before call above actions!
       * @p addScoreActions() and @p addMelodyButton() have to be invoked before! */
  QAction* scoreZoomIn();
  QAction* scoreZoomOut();
  QAction* scoreExtraAccids();
  QAction* scoreShowNames();
  QAction* scoreDeleteAll();
  QAction* generateMelody();
  QAction* recordMelody();
  QAction* playMelody();

			/** Changes names and icons of actions suitable to normal mode.
			 * Deletes actions related to exam.	 */
	void actionsAfterExam();

    /** It inserts any given action before Nootka label which is always on the right side (the last). */
	void addAction(QAction* a);

      /** Ramoves given QAction from tool bar. Makes it not visible. */
  void removeAction(QAction* a);
	
			/** Changes names and icons of actions suitable to exam.
			 * Creates common actions used in exams/exercises.
			 * Other ones (like @p repeatSndAct) has to be called separately. */
	void actionsToExam();

	void createRepeatSoundAction(); /**< Creates @p repeatSndAct. Don't add to tool bar */
	void createCorrectAction(); /**< Creates @p correctAct. Don't add to tool bar */
	void createTuneForkAction(); /**< Creates @p tuneForkAct. Don't add to tool bar */
	void createAttemptAction(); /**< Creates @p attemptAct. Don't add to tool bar */

			/** Removes actions for previous and next question @p prevQuestAct & @p nextQuestAct.
			 * Adds check answer action (@p checkAct) 
			 * and @p repeatSndAct when @p TRUE and @p tuneForkAct when @p TRUE	 */
	void setForQuestion(bool repeatSound, bool tuneFork);
	void setAfterAnswer(); /**< Removes actions required during answering */

	void addMelodyButton(TmelMan* melBut);
	void setMelodyButtonVisible(bool vis); /**< Hides or shows melody button */

  void addScoreActions(TscoreActions* scoreBut); /**< Adds button with associated menu with actions to manage the score. */

	void setBarIconStyle(Qt::ToolButtonStyle iconStyle, int iconS);

	void setProxy(QGraphicsProxyWidget* proxy);

#if defined (Q_OS_ANDROID)
      /** Initializes pointer to quick actions list (owned by @class TmelodyItem - see more there) */
  void setFlyingActions(QList<QAction*> *fly) { m_flyActions = fly; }
  QList<QAction*>* flyActions() { return m_flyActions; }
#endif

protected:
	virtual void resizeEvent(QResizeEvent* event);

private:
	TmelMan													*m_melButton;
  TscoreActions                   *m_scoreActs;
	TnootkaLabel 										*m_nootLabel;
	QGraphicsProxyWidget						*m_proxy;
	QWidget 												*m_spacer;
#if defined (Q_OS_ANDROID)
  QList<QAction*>                 *m_flyActions;
#endif
  static TtoolBar                 *m_instance;
};

#endif // TTOOLBAR_H
