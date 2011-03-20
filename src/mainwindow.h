/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tscorewidget.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include "tnote.h"
//#include <QtGui>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
//    void whenEnableDblAccStateChanged(bool isEnabled);
    void createSettingsDialog();

    void noteWasClicked(int index, Tnote note);
    void noteNameWasChanged(Tnote note);
    void guitarWasClicked(Tnote note);

protected:
    void resizeEvent(QResizeEvent *);

private:
    TscoreWidget *m_score;
    TnoteName *m_noteName;
    TfingerBoard *m_guitar;

//    QMenu *m_actMenu, *m_scoreMenu;
//    QAction *showOherNotesAct, *useDblAccidAct;
    QAction *settingsAct;
    QToolBar *nootBar;

//    void createMenus();
    void createToolBar();
    void createActions();

};

#endif // MAINWINDOW_H
