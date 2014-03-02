/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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
#include <QPointer>
#include "tchart.h"

class TgraphicsTextTip;
class Texam;
class QLabel;
class QComboBox;
class Tlevel;
class QToolBar;
class QVBoxLayout;
class QToolButton;



class TanalysDialog : public QDialog
{
    Q_OBJECT
    
public:
  TanalysDialog(Texam *exam = 0, QWidget *parent = 0);
  virtual ~TanalysDialog();
  
  void loadExam(QString &examFile); // loads exam from given file name
  void setExam(Texam *exam);  // loads exam from variable
	
	
	static QString analyseExamWinTitle() { return tr("Analysis of exam results"); }
	static QString analyseExerciseWinTitle() { return tr("Analysis of exercise"); }
    
protected:
  void resizeEvent(QResizeEvent* event);
    
protected slots:
  void testSlot();
  void loadExamSlot();
  void zoomInSlot();
  void zoomOutSlot();
  void wrongSeparateSlot();
  void includeWrongSlot();
  void analyseChanged(int index);
  void openRecentExam();
	void openRecentExercise();
  void moreLevelInfo();
  void maximizeWindow();
  void chartTypeChanged();
  void linkOnTipClicked();
	void showTuningPreview();

private:
  QToolBar 											*m_toolBar;
  QAction 											*m_closeAct, *m_zoomInAct, *m_zoomOutAct, *m_inclWrongAct, *m_wrongSeparateAct, *m_maximizeAct;
  QAction 											*m_linearAct, *m_barAct;
  QLabel 												*m_levelLab, *m_userLab, *m_effectLab, *m_questNrLab, *m_tuningLab;
  QComboBox 										*m_chartListCombo;
  QToolButton 									*m_settButt, *m_openButton;
  QPushButton 									*m_moreButton, *m_tuningButton;
  
  Tchart 												*m_chart;
  Texam 												*m_exam;
  Tlevel 												*m_level;
  QVBoxLayout 									*m_plotLay;
	
		/** true if exam instance was created by TanalysDialog, false when exam was given as param */
  bool 													m_wasExamCreated; 
  Tchart::Tsettings 						m_chartSetts;
  bool 													m_isMaximized;
	QPointer<TgraphicsTextTip> 		m_tunTip;
  
  
private:
  void createActions();
  void createChart(Tchart::Tsettings &chartSett);
  void enableComboItem(int index, bool enable);
    
};

#endif // TANALYSDIALOG_H
