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


#include "taboutnootka.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

TaboutNootka::TaboutNootka(QWidget *parent) :
    QDialog(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;
    navList = new QListWidget(this);
    navList->setIconSize(QSize(80,80));
    navList->setFixedWidth(100);
    navList->setViewMode(QListView::IconMode);
    navList->setFlow(QListView::TopToBottom);
    mainLay->addWidget(navList);

    stackLayout = new QStackedLayout;
    mainLay->addLayout(stackLayout);

    setLayout(mainLay);

    navList->addItem(tr("About"));
    navList->item(0)->setIcon(QIcon(gl->path+"picts/nootka.svg"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Authors"));
    navList->item(1)->setIcon(QIcon(gl->path+"picts/author.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);

    Tabout *m_about = new Tabout();

    stackLayout->addWidget(m_about);
}

//######################### About ##########################################
Tabout::Tabout(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;
    m_aboutLab = new QLabel("<center><b>Nootka " + gl->version + tr("</b></center><p>This is developers preview of Nootka. It works quite stable, but has less functioinality yet.</p><p>See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and furter relaces.</p><p>Any bugs, sugestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p style=\"text-align: right;\">with respects<br>Author</p>"), this);
    m_aboutLab->setWordWrap(true);
    lay->addWidget(m_aboutLab);

    setLayout(lay);
}
