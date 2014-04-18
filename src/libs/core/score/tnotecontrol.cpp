/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tnotecontrol.h"
#include "tscorestaff.h"
#include "tscorenote.h"
#include <tnoofont.h>
#include <QPainter>
#include <QPalette>
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QGraphicsSceneHoverEvent>

#include <QDebug>


TnoteControl::TnoteControl(TscoreStaff* staff, TscoreScene* scene) :
	TscoreItem(scene),
	m_scoreNote(0),
	m_hasMouse(false)
{
	setStaff(staff);
	setParentItem(staff);
	m_height = staff->height();
	hide();
	setStatusTip(tr("Click <big><b>+</b></big> to add new note or<br><big><b>-</b></big> remove it."));
	setZValue(60);
// 	painter->setPen(Qt::NoPen);
// 	QColor bc = qApp->palette().text().color();
// 	bc.setAlpha(20);
	m_plus = new QGraphicsSimpleTextItem("+");
	m_plus->setParentItem(this);
	m_plus->setScale(boundingRect().width() / m_plus->boundingRect().width());
	m_plus->setBrush(QBrush(Qt::green));
	m_plus->setPos(0.0, 0.2);
	m_minus = new QGraphicsSimpleTextItem("-");
	m_minus->setParentItem(this);
	m_minus->setScale(boundingRect().width() / m_minus->boundingRect().width());
	m_minus->setBrush(QBrush(Qt::red));
	m_minus->setPos(0.0, m_height - 6.0);
	m_name = new QGraphicsSimpleTextItem("c");
	m_name->setFont(TnooFont());
	m_name->setParentItem(this);
	m_name->setScale(boundingRect().width() / m_name->boundingRect().width());
	m_name->setBrush(Qt::darkCyan);
	m_name->setPos(0.0, 4.0);
}


TnoteControl::~TnoteControl()
{
	qDebug() << "\nTnoteControl was deleted\n";
}


void TnoteControl::adjustSize() {
	m_height = staff()->height();
	m_minus->setPos(0.0, m_height - 6.0);
}


void TnoteControl::hideWithDelay(int delay) {
	QTimer::singleShot(delay, this, SLOT(hideDelayed()));
}



QRectF TnoteControl::boundingRect() const {
	return QRectF(0.0, 0.0, 3.0, m_height);
}


void TnoteControl::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setPen(Qt::NoPen);
	QColor bc = qApp->palette().text().color();
	bc.setAlpha(15);
	painter->setBrush(QBrush(bc));
	painter->drawRect(boundingRect());
}


void TnoteControl::setScoreNote(TscoreNote* sn) {
	m_scoreNote = sn;
	if (sn) {
			if (parentItem() != sn->parentItem()) {
				parentItem()->setZValue(10);
				setParentItem(sn->parentItem());
				setStaff(sn->staff());
				parentItem()->setZValue(11);
			}
			show();
			if (staff()->count() < 2)
					m_minus->hide();
			else
				m_minus->show();
			if (pos().x() < m_scoreNote->pos().x()) // hide name for left control
				m_name->hide();
	} else {
			hide();
	}
}


void TnoteControl::hideDelayed() {
	if (m_scoreNote->isCursorVisible())
		return;
	if (hasMouseCursor())
		hideWithDelay(300);
	else
		hide();
}


void TnoteControl::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	m_hasMouse = true;
	TscoreItem::hoverEnterEvent(event);
}


void TnoteControl::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
	m_hasMouse = false;
	hide();
	TscoreItem::hoverLeaveEvent(event);
}


void TnoteControl::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (event->pos().y() < 3.0) { // add a note
		if (pos().x() > m_scoreNote->pos().x()) { // right control - append a note
				staff()->insertNote(m_scoreNote->index() + 1);
		} else { // left control - preppend a note
				staff()->insertNote(m_scoreNote->index() - 1);
		}
	} else if (m_minus->isVisible() && event->pos().y() > staff()->height() - 7.0) {
			staff()->removeNote(m_scoreNote->index());
			m_hasMouse = false;
			hide();
	}
}










