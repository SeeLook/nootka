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


#include "tsettingsdialogbase.h"
#include <QtGui>

TsettingsDialogBase::TsettingsDialogBase(QWidget *parent) :
        QDialog(parent,Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
//    setWindowFlags(Qt::Dialog | Qt::Window);
//    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *contLay = new QHBoxLayout;
    navList = new QListWidget(this);
    navList->setIconSize(QSize(80,80));
    navList->setFixedWidth(105);
    navList->setViewMode(QListView::IconMode);
//    navList->setFlow(QListView::TopToBottom);
//    navList->setWrapping(false);
    navList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    contLay->addWidget(navList);

    QVBoxLayout *aLay = new QVBoxLayout;
    stackLayout = new QStackedLayout;
    aLay->addLayout(stackLayout);

    QGroupBox *hGr = new QGroupBox(this);
    QVBoxLayout *hLay = new QVBoxLayout;
    hint = new QLabel(this);
    hLay->addWidget(hint);
    hGr->setLayout(hLay);
    aLay->addWidget(hGr);
    hint->setFixedHeight(70);
    hint->setWordWrap(true);
    contLay->addLayout(aLay);

    mainLay->addLayout(contLay);

    QHBoxLayout *butLay = new QHBoxLayout();
    okBut = new QPushButton(tr("Accept"),this);
    butLay->addStretch(1);
    butLay->addWidget(okBut);
    butLay->addStretch(1);
    cancelBut = new QPushButton(tr("Discard"),this);
    butLay->addWidget(cancelBut);
    butLay->addStretch(1);
    mainLay->addLayout(butLay);

    setLayout(mainLay);

    connect(cancelBut, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okBut, SIGNAL(clicked()), this, SLOT(accept()));
}

bool TsettingsDialogBase::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        hint->setText("<center>"+se->tip()+"</center>");
    }
    return QDialog::event(event);
}
