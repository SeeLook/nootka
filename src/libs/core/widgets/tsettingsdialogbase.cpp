/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                  				   *
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
#include <QtWidgets>


TsettingsDialogBase::TsettingsDialogBase(QWidget *parent) :
        QDialog(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *contLay = new QHBoxLayout;
    navList = new QListWidget(this);
    int w = fontMetrics().boundingRect(QStringLiteral("Instrument")).width();
    navList->setIconSize(QSize(w, w));
    navList->setFixedWidth(w * 1.2);
    navList->setViewMode(QListView::IconMode);
    navList->setMovement(QListView::Static);
    navList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
    hint->setFixedHeight(fontMetrics().boundingRect("A").height() * 4);
		hint->setMinimumWidth(fontMetrics().boundingRect("w").width() * 70);
    hint->setWordWrap(true);
    contLay->addLayout(m_aLay);

    mainLay->addLayout(contLay);

		buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
		mainLay->addWidget(buttonBox);
    setLayout(mainLay);
		
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


bool TsettingsDialogBase::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        hint->setText("<center>"+se->tip()+"</center>");
    } else if (event->type() == QEvent::Resize)
      QTimer::singleShot(20, this, SLOT(fitSize()));
    return QDialog::event(event);
}


void TsettingsDialogBase::fitSize() {
  if (qApp->desktop()->availableGeometry().height() <= 600) {
#if defined (Q_OS_WIN)
      setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
      setMinimumSize(qApp->desktop()->availableGeometry().width() - 100,
                     qApp->desktop()->availableGeometry().height() - 70);
#endif
      showMaximized();
      hint->hide();
			m_aLay->removeWidget(m_widget);
      m_scrollArea->setWidget(m_widget);
			m_aLay->insertWidget(0, m_scrollArea);
      m_scrollArea->show();
      convertStatusTips();
      connect(stackLayout, SIGNAL(currentChanged(int)), this, SLOT(convertStatusTips()));
  }
  navList->setFixedWidth(navList->sizeHintForColumn(0) + 2 * navList->frameWidth() +
          (navList->verticalScrollBar()->isVisible() ? navList->verticalScrollBar()->width() : 0));
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


void TsettingsDialogBase::openHelpLink(const QString& hash) {
  QDesktopServices::openUrl(QUrl(QString("http://nootka.sourceforge.net/index.php?L=%1&C=doc#" + hash).
    arg(QString(qgetenv("LANG")).left(2).toLower()), QUrl::TolerantMode));
}







