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


#ifndef TSETTINGSDIALOGBASE_H
#define TSETTINGSDIALOGBASE_H

#include <QtGui>

class TsettingsDialogBase : public QDialog
{
    Q_OBJECT
public:
    explicit TsettingsDialogBase(QWidget *parent = 0);

signals:

public slots:

protected:
    bool event(QEvent *event);

    QListWidget *navList;
    QStackedLayout *stackLayout;
    QPushButton *cancelBut, *okBut;
    QLabel *hint;


};

#endif // TSETTINGSDIALOGBASE_H
