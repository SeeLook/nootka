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

class Tchart;
class Texam;
class QLabel;
class QComboBox;
class TexamLevel;
class QToolBar;
class QVBoxLayout;



class TanalysDialog : public QDialog
{
    Q_OBJECT
    
public:
    TanalysDialog(Texam *exam = 0, QWidget *parent = 0);
    virtual ~TanalysDialog();
    
    void loadExam(QString &examFile); // loads exam from given file name
    void setExam(Texam *exam);  // loads exam from variable
    
protected slots:
  void testSlot();
  void loadExamSlot();
  void zoomInSlot();
  void zoomOutSlot();

private:
  QToolBar *m_toolBar;
  QAction *m_openExamAct, *m_closeAct, *m_zoomInAct, *m_zoomOutAct;
  QLabel *m_levelLab, *m_userLab;
  QComboBox *m_chartListCombo;
  
  Tchart *m_chart;
  Texam *m_exam;
  TexamLevel *m_level;
  QVBoxLayout *m_plotLay;
  
  
private:
  void createActions();

     
    
    
};

#endif // TANALYSDIALOG_H
