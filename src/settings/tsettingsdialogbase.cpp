/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                  				   *
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
#include <widgets/troundedlabel.h>
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

    m_aLay = new QVBoxLayout;
    stackLayout = new QStackedLayout;
    
    m_widget = new QWidget(this);
    m_scrollArea = new QScrollArea(this);
		m_scrollArea->hide();
    
    m_widget->setLayout(stackLayout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_aLay->addWidget(m_widget);
    hint = new TroundedLabel(this);
    m_aLay->addWidget(hint);
    hint->setFixedHeight(70);
    hint->setWordWrap(true);
    contLay->addLayout(m_aLay);

    mainLay->addLayout(contLay);

		buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
		mainLay->addWidget(buttonBox);
    setLayout(mainLay);
    
    fitSize();
}


bool TsettingsDialogBase::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        hint->setText("<center>"+se->tip()+"</center>");
    }
    return QDialog::event(event);
}


void TsettingsDialogBase::fitSize() {
  if (qApp->desktop()->availableGeometry().height() <= 600) {
			showMaximized();
      hint->hide();
			m_aLay->removeWidget(m_widget);
      m_scrollArea->setWidget(m_widget);
			m_aLay->insertWidget(0, m_scrollArea);
      m_scrollArea->show();
      convertStatusTips();
      connect(stackLayout, SIGNAL(currentChanged(int)), this, SLOT(convertStatusTips()));
  }
}


void TsettingsDialogBase::convertStatusTips() {
	QList<QWidget*> allWidgets = findChildren<QWidget*>();
	foreach(QWidget *w, allWidgets) {
		if (w->statusTip() != "") {
			w->setToolTip(w->statusTip());
			w->setStatusTip("");
		}
	}
}







