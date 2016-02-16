/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#ifndef TEXAMSUMMARY_H
#define TEXAMSUMMARY_H

#include <QtWidgets/qdialog.h>

class QVBoxLayout;
class QPushButton;
class Texam;

/** Global function to create and display an exam summary dialog.
 * If it returns false - user don't want to continue an exam
 * @p startExam is a pointer to know does user want to start exam on exercise level.
 * @class QMainWindow has to be put for proper size of analyze dialog. */
bool showExamSummary(QWidget* mainWindow, Texam* exam, bool cont, bool* startExam = 0);

/**
 * Dialog window with exam summary
 * When @param cont is true On button is shown text 'continue' 
 */
class TexamSummary : public QDialog
{
  Q_OBJECT
public:
  TexamSummary(Texam *exam, bool cont = false, QWidget *parent = 0);

  enum Eactions { e_continue, e_discard, e_startExam };
  Eactions doExec();

			/** By default it was created for exams. This method adjust the dialog for exercise context:
			* - changes window title, 
			* - continue button icon
			* - adds button to start exam on current exercise level */
	void setForExercise();

protected:
  void closeSlot();
  void analyseSlot();
  void continueSlot();
	void startExamSlot();
#if defined (Q_OS_ANDROID)
  void sendExamSlot();
#endif

private:
  Texam 						*m_exam;
  Eactions 					 m_state;
	QPushButton 			*m_closeButt, *m_okButt, *m_examButton;
#if defined (Q_OS_ANDROID)
  QPushButton       *m_sendButt;
#endif
	QVBoxLayout			  *m_leftLay;
	QWidget						*m_mainWIndow;
};

#endif // TEXAMSUMMARY_H
