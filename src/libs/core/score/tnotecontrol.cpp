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
#include <QGraphicsScene>
#include <QGraphicsEffect>
#include <QGraphicsView>

// #include <QDebug>

#if defined (Q_OS_ANDROID)
  #define WIDTH (5.0)
  #define LEAVE_DELAY (1000)
#else
  #define WIDTH (3.0)
  #define LEAVE_DELAY (300)
#endif


QGraphicsDropShadowEffect* ItemHighLight() {
	QGraphicsDropShadowEffect *hiBlur = new QGraphicsDropShadowEffect();
	hiBlur->setColor(qApp->palette().highlight().color());
	hiBlur->setOffset(1.0, 1.0);
	hiBlur->setBlurRadius(7.0);
	return hiBlur;
}

TnoteControl::TnoteControl(TscoreStaff* staff, TscoreScene* scene) :
	TscoreItem(scene),
	m_scoreNote(0),
	m_hasMouse(false), m_entered(false),
	m_underItem(0)
{
	setStaff(staff);
	setParentItem(staff);
	setZValue(60);
// 	m_height = staff->height();
	hide();
	setStatusTip(tr("Click <big><b>+</b></big> to add new note or <big><b>-</b></big> to remove it.<br>Click %1 to edit note name").arg(TnooFont::span("c", qApp->fontMetrics().boundingRect("A").height() * 1.5, "color: #008080"))); // #008080 - dark Cyan
	
// '+' for adding notes
	m_plus = new QGraphicsSimpleTextItem("+");
	m_plus->setParentItem(this);
	m_plus->setScale(boundingRect().width() / m_plus->boundingRect().width());
	m_plus->setBrush(QBrush(Qt::green));
// 'name' glyph for editing note throught its name
	m_name = new QGraphicsSimpleTextItem("c");
	m_name->setFont(TnooFont());
	m_name->setParentItem(this);
	m_name->setScale(boundingRect().width() / m_name->boundingRect().width());
	m_name->setBrush(Qt::darkCyan);
// '-' for deleting notes
	m_minus = new QGraphicsLineItem();
	m_minus->setParentItem(this);
	m_minus->setPen(QPen(Qt::red, 0.5));
	adjustSize();
}


TnoteControl::~TnoteControl()
{
// 	qDebug() << "\nTnoteControl was deleted\n";
}


void TnoteControl::adjustSize() {
	m_height = staff()->height();
	m_plus->setPos(0.0, staff()->upperLinePos() - m_plus->boundingRect().height() * m_plus->scale());
	m_name->setPos(0.0, m_plus->pos().y() + m_plus->boundingRect().height() * m_plus->scale() - 
																					(m_name->boundingRect().height() / 3.5) * m_name->scale());
	qreal minusY = (staff()->isPianoStaff() ? staff()->lowerLinePos() : staff()->upperLinePos()) + 11.0;
	m_minus->setLine(WIDTH / 4.0, minusY, WIDTH - WIDTH / 4.0, minusY);
}


void TnoteControl::hideWithDelay(int delay) {
	Q_UNUSED(delay)
	QTimer::singleShot(LEAVE_DELAY, this, SLOT(hideDelayed()));
}



QRectF TnoteControl::boundingRect() const {
        return QRectF(0.0, 0.0, WIDTH, m_height);
}


void TnoteControl::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
	if (parentItem()) {
			painter->setPen(Qt::NoPen);
			QColor bc = qApp->palette().base().color(); // Qt::lightGray;
			bc.setAlpha(220);
			painter->setBrush(QBrush(bc));
			qreal lowest = (staff()->isPianoStaff() ? staff()->lowerLinePos(): staff()->upperLinePos()) + 16.0;
			painter->drawRoundedRect(0.0, staff()->upperLinePos() - 4.0, WIDTH, lowest - staff()->upperLinePos(), 0.75, 0.75);
	}
}


void TnoteControl::setScoreNote(TscoreNote* sn) {
	m_scoreNote = sn;
	if (sn) {
			if (parentItem() != sn->parentItem()) {
				if (parentItem())
						parentItem()->setZValue(10);
				setParentItem(sn->parentItem());
				setStaff(sn->staff());
				parentItem()->setZValue(11);
			}
			QTimer::singleShot(300, this, SLOT(showDelayed()));
			if (staff()->number() == 0 && staff()->count() < 2)
					m_minus->hide(); // prevent deleting only one note
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
		hideWithDelay(LEAVE_DELAY);
	else
		hide();
}


void TnoteControl::hoverEnterDelayed() {
	if (m_hasMouse) {
		TscoreItem::hoverEnterEvent(0);
		m_entered = true;
	}
}


void TnoteControl::showDelayed() {
	show();
}


void TnoteControl::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	QTimer::singleShot(300, this, SLOT(hoverEnterDelayed()));
	m_hasMouse = true;
}


void TnoteControl::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	QGraphicsItem *it = scene()->itemAt(mapToScene(event->pos()), scene()->views()[0]->transform());
	if (it != this) {
		if (it != m_underItem) {
			it->setGraphicsEffect(ItemHighLight());
			if (m_underItem)
				m_underItem->setGraphicsEffect(0);
			m_underItem = it;
		}
	} else if (m_underItem) {
			m_underItem->setGraphicsEffect(0);
			m_underItem = 0;
	}
}


void TnoteControl::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
	m_hasMouse = false;
// 	hide();
	if (m_underItem) {
			m_underItem->setGraphicsEffect(0);
			m_underItem = 0;
	}
	hideWithDelay(LEAVE_DELAY);
	if (m_entered)
		TscoreItem::hoverLeaveEvent(event);
	m_entered = false;
}


void TnoteControl::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (event->pos().y() < m_name->pos().y()) { // add a note
		if (pos().x() > m_scoreNote->pos().x()) { // right control - append a note
				staff()->insertNote(m_scoreNote->index() + 1);
		} else { // left control - preppend a note
				staff()->insertNote(m_scoreNote->index() - 1);
		}
	} else if (event->pos().y() < m_name->pos().y() + m_name->boundingRect().height() * m_name->scale()) { // edit note name
			hoverLeaveEvent(0);
			emit nameMenu(m_scoreNote);
	} else if (m_minus->isVisible() && event->pos().y() > m_minus->line().y1() - 1.0) {
			staff()->removeNote(m_scoreNote->index());
			hoverLeaveEvent(0);
	}
}










