/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TSTARTEXAMDLG_H
#define TSTARTEXAMDLG_H

#include <QDialog>


class TlevelSelector;
class TexamLevel;
class QRadioButton;
class QGroupBox;
class QLineEdit;
class QLabel;
class QComboBox;

class TstartExamDlg : public QDialog
{
    Q_OBJECT
public:
      /** QString @param &nick is student Name  */
    explicit TstartExamDlg(QString &nick, QString &path, QWidget *parent = 0);

        /** Describes actions commited by user.
        * @param e_none - dialog discarded,
        * @param e_continue - exam to continue,
        * @param e_newLevel - new level selected.
        * @param e_levelCreator - open Level creator.
	*/
    enum Eactions { e_none, e_continue, e_newLevel, e_levelCreator };
        /** This method calls dialog window,
        * takes txt reference and puts there eighter user name
        * or exam file path, depends on returned @param Eactions,
        * and returns secected level. */
    Eactions showDialog(QString &txt, TexamLevel &lev);
        /** exam file extension and its description */
    static const QString examFilterTxt() { return tr("Exams' results")  + "(*.noo)" ; }
	/** Returns system user name (login name)  */
    static QString systemUserName();


signals:

public slots:

private:
    bool event(QEvent *event);

    QGroupBox *examGr, *levelGr;
    TlevelSelector *levelsView;
    QLineEdit *nameEdit;
    QPushButton *m_createBut, *m_loadExamBut, *m_cancelBut, *m_newExamBut, *m_contExamButt;
    QLabel *hint;
    QComboBox *examCombo;
    QStringList recentExams;
    Eactions m_Acction;

private slots:
    void levelOrExamChanged();
    void levelToLoad();
        /** occurs when user clicks Accept button*/
    void startAccepted();
    void loadExam();
        /** occurs when user clicks create Level Button */
    Eactions createLevel();
};

#endif // TSTARTEXAMDLG_H
