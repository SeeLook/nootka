/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                  				   *
 *   tomaszbojczuk@gmail.com   						                                 *
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


#include "tsettingsdialogbase.h"
#include <QtGui>

TsettingsDialogBase::TsettingsDialogBase(QWidget *parent) :
        QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *contLay = new QHBoxLayout;
    navList = new QListWidget(this);
    navList->setIconSize(QSize(80, 80));
    navList->setFixedWidth(100);
    navList->setViewMode(QListView::IconMode);
		navList->setMovement(QListView::Static);

    contLay->addWidget(navList);

    QVBoxLayout *aLay = new QVBoxLayout;
    stackLayout = new QStackedLayout;
    
    aLay->addLayout(stackLayout);
    hint = new QLabel(this);
    aLay->addWidget(hint);
    hint->setFixedHeight(70);
    hint->setWordWrap(true);
#if defined(Q_OS_WIN32)
    QColor bg = palette().window().color().lighter(101);
#else
    QColor bg = palette().window().color().lighter(105);
#endif
    hint->setStyleSheet(QString("border-radius: 10px; background-color: %1;").arg(bg.name()));
    contLay->addLayout(aLay);

    mainLay->addLayout(contLay);

    QHBoxLayout *butLay = new QHBoxLayout();
    defaultBut = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_BrowserReload)), tr("Default"), this);
    okBut = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_DialogApplyButton)), tr("Accept"), this);
    butLay->addStretch(1);
    butLay->addWidget(defaultBut);
    butLay->addStretch(3);
    defaultBut->hide();
    butLay->addWidget(okBut);
    butLay->addStretch(1);
    cancelBut = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_DialogDiscardButton)), tr("Discard"), this);
    butLay->addWidget(cancelBut);
//     butLay->addStretch(1);
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
