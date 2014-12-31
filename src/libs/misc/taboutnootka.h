/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TABOUTNOOTKA_H
#define TABOUTNOOTKA_H

#include "nootkamiscglobal.h"
#include <QDialog>

class QTimer;
class QScrollArea;
class QListWidget;
class QStackedLayout;

class NOOTKAMISC_EXPORT TaboutNootka : public QDialog
{
    Q_OBJECT
public:
    explicit TaboutNootka(QWidget *parent = 0);

    static QString authorsTxt() { return tr("Authors"); }

protected slots:
		void moveScroll();
		void changeCurrentPage(int page);
			/** It is called after dialog constructor to grab and fix window size (avoid resizing) */
		void fixSize();

private:
    QListWidget 		*m_navList;
    QStackedLayout 	*m_stackLayout;
    QPushButton 		*m_okBut;
		QScrollArea 		*m_authorScroll;
		QTimer					*m_timer;
		

};

//######################### About ##########################################
class TroundedLabel;

class NOOTKAMISC_EXPORT Tabout : public QWidget
{
    Q_OBJECT
public:
    explicit Tabout(QWidget *parent = 0);


private:
    TroundedLabel *m_aboutLab;
};

#endif // TABOUTNOOTKA_H
