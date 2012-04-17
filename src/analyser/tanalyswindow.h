/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#ifndef TANALYSWINDOW_H
#define TANALYSWINDOW_H

#include <QMainWindow>

class Texam;
class TanalysWdg;
class QLabel;
class QComboBox;

class TanalysWindow : public QMainWindow
{
	Q_OBJECT
	
public:
    explicit TanalysWindow();
	virtual ~TanalysWindow();
  
    void setExam(QString &examFile);

private:
  QToolBar *m_toolBar;
  QAction *m_openExamAct, *m_closeAct;
  QLabel *m_levelLab, *m_userLab;
  QComboBox *m_chartListComo;
  
  TanalysWdg *m_analyseWdg;
  Texam *m_exam;
  
private:
  void createActions();
  void loadExamSlot();
	
};

#endif // TANALYSWINDOW_H
