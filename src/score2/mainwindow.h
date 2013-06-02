/*
 *                                                                        *
 * Copyright (C) 2013 by Tomasz Bojczuk                                   *
 * tomaszbojczuk@gmail.com                                                *
 *                                                                        *
 * This program is free software; you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation; either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                        *
 * 
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tnote.h"

class QCheckBox;
class TmainScore;
class QStatusBar;
class TsimpleScore;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    
protected slots:
   void updateStatusTip(QString status);
	 void keySignBoxChanged(bool enable);
	 void pianoBoxChanged(bool enable);
	 void scoreChangedNote(int index, Tnote note);

private:
    TsimpleScore *m_simpleScore;
		TmainScore *m_mainScore;
    QStatusBar *m_bar;
		QCheckBox *m_keyBox, *m_pianBox;

};

#endif // MAINWINDOW_H
