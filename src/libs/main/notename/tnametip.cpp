/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tnametip.h"
#include "tnotename.h"
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>


TnameTip::TnameTip(TnoteName* noteName) :
	TgraphicsTextTip(QString(), qApp->palette().text().color()),
	m_proxy(0),
	m_name(noteName),
	m_cursorEntered(false)
{
	m_widget = new QWidget();
	m_widget->setObjectName(QStringLiteral("TnameTip"));
	m_widget->setStyleSheet(QStringLiteral("QWidget#TnameTip { background: transparent }"));

	m_lay = new QVBoxLayout;
	wrapNoteName();
	m_widget->setLayout(m_lay);
#if defined (Q_OS_MAC) // fix spacing under Mac
  m_lay->setSpacing(0);
  m_lay->setContentsMargins(5, 0, 5, 0);
#endif
	m_proxy = new QGraphicsProxyWidget(this);
	m_proxy->setWidget(m_widget);
	m_proxy->setParentItem(this);

	m_hideTimer = new QTimer(this);
	m_hideTimer->setSingleShot(true);

	connect(this, SIGNAL(entered()), this, SLOT(enteredSlot()));
	connect(this, SIGNAL(leaved()), this, SLOT(hide()));
	connect(m_hideTimer, SIGNAL(timeout()), this, SLOT(showTimeExpired()));
}


TnameTip::~TnameTip()
{
	m_proxy->clearFocus();
	delete m_proxy;
}


void TnameTip::wrapNoteName() {
	m_lay->addWidget(m_name);
}


void TnameTip::unwrapNoteName() {
	m_lay->removeWidget(m_name);
}


void TnameTip::adjustSize() {
	m_widget->adjustSize();
	m_proxy->adjustSize();
	QGraphicsTextItem::adjustSize();
}


QRectF TnameTip::boundingRect() const {
  return QRect(0, 0, m_proxy->widget()->sizeHint().width(), m_proxy->widget()->sizeHint().height());
}



void TnameTip::show() {
	m_hideTimer->stop();
	m_hideTimer->start(2000); // if user didn't put mouse over the tip after 2 sec. - hide it
	QGraphicsTextItem::show();
}


void TnameTip::showTimeExpired() {
	if (!m_cursorEntered)
		hide();
}





