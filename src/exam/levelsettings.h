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


#ifndef LEVELSETTINGS_H
#define LEVELSETTINGS_H

#include <QtGui>
#include "tlevelsummarywdg.h"

    /** This widget contain general level settings and belongs to
    @class examsettings dialog.*/
class levelSettings : public QWidget
{
    Q_OBJECT
public:
    explicit levelSettings(QWidget *parent = 0);

signals:

public slots:

private:
    QPushButton *saveBut, *loadBut;
    QListWidget *levelsList;
    TlevelSummaryWdg *summWdg;

};

#endif // LEVELSETTINGS_H
