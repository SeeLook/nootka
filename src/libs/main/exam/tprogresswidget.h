/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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


#ifndef TPROGRESSWIDGET_H
#define TPROGRESSWIDGET_H

#include <QtWidgets/qwidget.h>

class Texam;
class QProgressBar;
class QLabel;

#define   PROGRESS    TprogressWidget::instance()

/**
 * Displays progress bar of exam and some additional data about it.
 * It should be activated by @p activate(),
 * then it is updated by @p progress() after every answer
 * and terminated with @p terminate() at the end..
 * If exam becomes passed (finished), invoke @p setFinished() to update.
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p PROGRESS
 */
class TprogressWidget : public QWidget
{

  Q_OBJECT

public:
  TprogressWidget(QWidget *parent = 0);
  virtual ~TprogressWidget();

  static TprogressWidget* instance() { return m_instance; }

  static QString progressExamTxt() { return tr("Progress of the exam"); } // Progress of the exam
  static QString examFinishedTxt() { return tr("Exam was finished"); } // Exam was finished

  void activate(Texam* exam, int totalNr);
  void progress();
  void terminate();
  void setFinished();

  void resize(int fontSize);

protected:
  QString zeroLabTxt();
  void updateLabels();

private:
  int 								      m_totalNr;
  QLabel 						       *m_answLab, *m_totalLab;
  QProgressBar 			       *m_bar;
  Texam							       *m_exam;
  static TprogressWidget   *m_instance;
};

#endif // TPROGRESSWIDGET_H
