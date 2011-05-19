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


#ifndef TEXAMEXECUTOR_H
#define TEXAMEXECUTOR_H

#include "texamlevel.h"
#include "tqaunit.h"
//#include <QtGui>
#include <QList>

class MainWindow;
class QAction;

    /** This class manages of exam executing*/
class TexamExecutor : public QObject
{
    Q_OBJECT

public:
    explicit TexamExecutor(MainWindow *mainW);

protected:
    MainWindow *mW;
    QAction *nextQuestAct, *checkAct;

protected slots:
    void askQuestion();
    void checkAnswer();

private:
    void createQuestionsList();
    Tnote determineAccid(Tnote n);

    void prepareToExam();
    void restoreAfterExam();
        /** Disables score, noteName and guitar*/
    void disableWidgets();
    void clearWidgets();



    TexamLevel m_level;
    QList<TQAunit::TQAgroup> m_questList;
    QList<TQAunit> m_answList;
//    TQAunit m_curQ;
        /** Previous accidentals used*/
    Tnote::Eacidentals m_prevAccid;
        /** intervals between asking about double accidentals.
        * By default every forth question is with double adccid.*/
    int m_dblAccidsCntr;
    Tnote m_note2;

};

#endif // TEXAMEXECUTOR_H
