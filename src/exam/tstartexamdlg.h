/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com   						                                 *
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


#ifndef TSTARTEXAMDLG_H
#define TSTARTEXAMDLG_H

#include <QDialog>
#include <exam/tlevel.h>

class QMenu;
class TroundedLabel;
class TexamParams;
class TlevelSelector;
class QRadioButton;
class QGroupBox;
class QLineEdit;
class QComboBox;

class TstartExamDlg : public QDialog
{
    Q_OBJECT
public:
      /** QString @param &nick is student Name  */
    explicit TstartExamDlg(const QString &nick, const QString &path, TexamParams *examParams, QWidget *parent = 0);

        /** Describes actions committed by user.
        * @param e_none - dialog discarded,
        * @param e_contExam - exam to continue,
        * @param e_newExam - exam mode on new level selected.
				* @param e_runExercise - start exercise mode.
        * @param e_levelCreator - open Level creator.	*/
    enum Eactions { e_none, e_contExam, e_newExam, e_levelCreator, e_runExercise };
		
        /** This method calls dialog window,
        * takes txt reference and puts there either user name
        * or exam file path, depends on returned @param Eactions,
        * and returns selected level. */
    Eactions showDialog(QString &txt, Tlevel &lev);
    
        /** Returns system user name (log-in name)  */
    static QString systemUserName();


private:
    bool event(QEvent *event);
		bool isAnyLevelSelected();
				/** All buttons on widget have 48x48 icons. */
		void setIconSize(QPushButton *button);
				/** Updates status tip text of new exercise/exam buttons when user selected a level. */
		void updateButtonStatusText(const QString &levelName);
		

    QGroupBox 					*examGr, *levelGr;
    TlevelSelector 			*m_levelsView;
    QLineEdit 					*m_nameEdit;
    QPushButton 				*m_createLevelButt, *m_loadExamBut;
		QPushButton					*m_exerciseButt;
		QPushButton					*m_newExamButt, *m_contExamButt, *m_lastExamButt;
		QPushButton					*m_helpButt, *m_cancelBut;
    TroundedLabel				*m_hintLabel;
    QStringList 				m_recentExams;
    Eactions 						m_Acction;
    TexamParams 				*m_examParams;
		QString							m_path;
		QMenu 							*m_examMenu;
		QString							m_selectedExamFile;
		Tlevel							m_prevExerciseLevel; /** Level grabbed from exercise.noo file */

private slots:
    void levelToLoad();
    void startAccepted(); /** occurs when user clicks Accept button*/
    void examFromFileDialog();
    Eactions createLevel(); /** occurs when user clicks create Level Button */
		void continuePrevExam(); /** when some action with exam file path was clicked. */
		void continueTheLast(); /** Continue last exam button click slot */
		void examToContSelected(QString eFile);
    void levelWasSelected(Tlevel level);
		void helpSelected();
};

#endif // TSTARTEXAMDLG_H

