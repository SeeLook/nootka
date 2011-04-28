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


#ifndef TQUESTIONASWDG_H
#define TQUESTIONASWDG_H

#include <QtGui>
#include "tqatype.h"


class TquestionAsWdg : public QGroupBox
{
    Q_OBJECT
public:
    explicit TquestionAsWdg(QWidget *parent = 0);

    static QString questionTxt, questionsTxt, answerTxt, answersTxt;
    static QString asNoteTxt, asNameTxt, asFretPosTxt, asSoundTxt;

    bool answerAsNote() { return asNoteChB->isChecked(); }
    bool answerAsName() { return asNameChB->isChecked(); }
    bool answerAsPos() { return asFretPosChB->isChecked(); }
    bool answerAsSound() { return asSoundChB->isChecked(); }

    void setAnswers(TQAtype types);
    TQAtype getAnswers();

signals:
        /** This signal is emited when any QCheckBox changes his state. */
    void answerStateChanged();


private:
    QCheckBox *asNoteChB, *asNameChB, *asFretPosChB, *asSoundChB;

private slots:
    void buttonClicked();
    void groupChecked();

};

#endif // TQUESTIONASWDG_H
