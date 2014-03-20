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

#include <QWidget>
#include "tlevelselector.h"
#include "tlevel.h"

    /** This widget contain general level settings and belongs to
    @class examsettings dialog.*/
class levelSettings : public QWidget
{
    Q_OBJECT
public:
    explicit levelSettings(QString &imgPath, QWidget *parent = 0);

    static QString moreLevelLinkTxt() { return tr("Get more levels <a href=\"%1\">from Nootka home page</a>")
            .arg("http://www.nootka.sf.net/index.php?C=down#levels"); }
    TlevelSelector *levelSelector;
    QPushButton *saveBut, *startExamBut;

};

#endif // LEVELSETTINGS_H
