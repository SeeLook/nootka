/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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

class QGraphicsPixmapItem;
class QAction;
class QMainWindow;
class QWidgetAction;
class Tmenu;
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
	explicit TmelMan(TmainScore* score);
  virtual ~TmelMan();
	
	QWidgetAction* melodyAction() { return m_melAct; }
	QToolButton* button() { return m_button; }

	QAction* generateAction() { return m_genMelodyAct; }
	QAction* playAction() { return m_playMelAct; }
	QAction* recordAction() { return m_recMelAct; }
	
public slots:
	void playMelodySlot();
	void recordMelodySlot();
	void randomizeMelodySlot();
	void loadMelodySlot();
	void saveMelodySlot();
	
private:
  enum EscoreState { e_playing, e_recording, e_none };
	QAction* createAction(const QString& t, const char* slot, const QKeySequence& k = QKeySequence(), const QIcon& i = QIcon());
  void showAudioMark(EscoreState ss);
	
private:
	TmainScore						*m_score;
	QWidgetAction					*m_melAct;
	QToolButton						*m_button;
	Tmenu									*m_menu;
	QAction								*m_playMelAct, *m_recMelAct, *m_genMelodyAct;
  QGraphicsPixmapItem   *m_audioMark;
};

#endif // TMELMAN_H
