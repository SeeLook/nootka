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


#ifndef TABOUTNOOTKA_H
#define TABOUTNOOTKA_H

#include <QDialog>

class QListWidget;
class QStackedLayout;

class TaboutNootka : public QDialog
{
    Q_OBJECT
public:
    explicit TaboutNootka(QWidget *parent = 0);

signals:

public slots:

private:
    QListWidget *navList;
    QStackedLayout *stackLayout;
    QPushButton *cancelBut, *okBut;

};

//######################### About ##########################################
class QLabel;
class Tabout : public QWidget
{
    Q_OBJECT
public:
    explicit Tabout(QWidget *parent = 0);


private:
    QLabel *m_aboutLab;
};

#endif // TABOUTNOOTKA_H
