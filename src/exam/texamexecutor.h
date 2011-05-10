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
//#include "mainwindow.h"
//#include <QtGui>
#include <QList>

class MainWindow;

    /** This class manages of exam executing*/
class TexamExecutor : public QObject
{
    Q_OBJECT

public:
    explicit TexamExecutor(MainWindow *mainW);

protected:
    MainWindow *mW;

private:
    void createQuestionsList();
    void prepareToExam();
    void restoreAfterExam();


    TexamLevel m_level;
    QList<TQAunit::TQAgroup> m_questList;
};

#endif // TEXAMEXECUTOR_H
