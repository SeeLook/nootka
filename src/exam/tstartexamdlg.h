/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include "texamlevel.h"

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
    explicit TstartExamDlg(QString &nick, QString &path, TexamParams *examParams, QWidget *parent = 0);

        /** Describes actions committed by user.
        * @param e_none - dialog discarded,
        * @param e_continue - exam to continue,
        * @param e_newLevel - exam mode on new level selected.
				* @param e_practice - start in practice mode.
        * @param e_levelCreator - open Level creator.	*/
    enum Eactions { e_none, e_continue, e_newLevel, e_levelCreator, e_practice };
        /** This method calls dialog window,
        * takes txt reference and puts there either user name
        * or exam file path, depends on returned @param Eactions,
        * and returns secected level. */
    Eactions showDialog(QString &txt, TexamLevel &lev);
        /** exam file extension and its description */
    static const QString examFilterTxt() { return tr("Exam results")  + " (*.noo)" ; }
    static const QString loadExamFileTxt() { return tr("Load an exam file"); }
        /** Returns system user name (login name)  */
    static QString systemUserName();


signals:

public slots:

private:
    bool event(QEvent *event);

    QGroupBox 					*examGr, *levelGr;
    TlevelSelector 			*m_levelsView;
    QLineEdit 					*m_nameEdit;
    QPushButton 				*m_createBut, *m_loadExamBut, *m_cancelBut, *m_newExamBut, *m_contExamButt;
		QPushButton					*m_practiceBut;
    TroundedLabel				*m_hintLabel;
    QComboBox 					*m_examCombo;
    QStringList 				m_recentExams;
    Eactions 						m_Acction;
    TexamParams 				*m_examParams;

private slots:
    void levelToLoad();
        /** occurs when user clicks Accept button*/
    void startAccepted();
    void loadExam();
        /** occurs when user clicks create Level Button */
    Eactions createLevel();
    void prevExamSelected(int index);
    void levelWasSelected(TexamLevel level);
				/** occurs when user clicks exercises Button */
		void practiceSelected();
};

#endif // TSTARTEXAMDLG_H
