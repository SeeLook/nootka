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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TANALYSDIALOG_H
#define TANALYSDIALOG_H

#include <QDialog>

class TbaseChart;
class Texam;
class TanalysWdg;
class QLabel;
class QComboBox;
class TexamLevel;
class QToolBar;
class QVBoxLayout;



class TanalysDialog : public QDialog
{
    Q_OBJECT
    
public:
    TanalysDialog(QWidget *parent = 0);
    virtual ~TanalysDialog();
    
    void loadExam(QString &examFile);
    
protected slots:
  
  void loadExamSlot();

private:
  QToolBar *m_toolBar;
  QAction *m_openExamAct, *m_closeAct;
  QLabel *m_levelLab, *m_userLab;
  QComboBox *m_chartListComo;
  
  TbaseChart *m_chart;
  Texam *m_exam;
  TexamLevel *m_level;
  QVBoxLayout *m_lay;
  
  
private:
  void createActions();
     
    
    
};

#endif // TANALYSDIALOG_H
