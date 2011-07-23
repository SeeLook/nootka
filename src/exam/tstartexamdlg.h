/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
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
 *  You should have received a copy of the GNU General Public License	   *
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
    explicit TstartExamDlg(QWidget *parent = 0);

        /** Describes actions commited by user.
        * @param e_none - dialog discarded,
        * @param e_continue - exam to continue,
        * @param e_newLevel - new level selected.*/
    enum Eactions { e_none, e_continue, e_newLevel };
        /** This method calls dialog window,
        * takes txt reference and puts there eighter user name
        * or exam file path, depends on returned @param Eactions,
        * and returns secected level. */
    Eactions showDialog(QString &txt, TexamLevel &lev);
        /** exam file extension and its description */
    static const QString examFilterTxt;


signals:

public slots:

private:
    bool event(QEvent *event);


    QRadioButton *contRadio, *levelRadio;
    QGroupBox *examGr, *levelGr;
    TlevelSelector *levelsView;
    QLineEdit *nameEdit;
    QPushButton /**createBut,*/ *loadExamBut, *startBut, *cancelBut;
    QLabel *hint;
    QComboBox *examCombo;
    QStringList recentExams;

private slots:
    void levelOrExamChanged();
    void levelToLoad();
        /** occurs when user click Accept button*/
    void startAccepted();
    void loadExam();
};

#endif // TSTARTEXAMDLG_H
