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


#ifndef TLEVELSUMMARYWDG_H
#define TLEVELSUMMARYWDG_H

#include <QtGui>
#include "texamlevel.h"

class TlevelSummaryWdg : public QWidget
{
    Q_OBJECT
public:
//    explicit TlevelSummaryWdg(TexamLevel *level, QWidget *parent = 0);
    explicit TlevelSummaryWdg(QWidget *parent = 0);

    void setLevel(const TexamLevel &lev);

signals:

public slots:

private:
    QLabel *summLab;

};

#endif // TLEVELSUMMARYWDG_H
