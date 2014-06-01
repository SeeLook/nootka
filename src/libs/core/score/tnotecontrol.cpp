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
#include "tscorescene.h"
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
	m_isEnabled(true), m_entered(false),
	m_dblSharp(0), m_sharp(0), m_flat(0), m_dblFlat(0),
	m_underItem(0),
	m_moveNote(false),
	m_currAccid(0), m_prevAccidIt(0),
	m_notesAdding(true)
{
	setStaff(staff);
	setParentItem(staff);
	setZValue(60);
	enableTouchToMouse(false);
	hide();

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
	m_cross = new QGraphicsSimpleTextItem("x");
	m_cross->setParentItem(this);
	m_cross->setScale((boundingRect().width() / m_cross->boundingRect().width()) / 1.5);
	m_cross->setBrush(Qt::red);
	adjustSize();
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
}


TnoteControl::~TnoteControl()
{
// 	qDebug() << "\nTnoteControl was deleted\n";
}


void TnoteControl::adjustSize() {
	m_height = staff()->height();
	qreal minusY = (staff()->isPianoStaff() ? staff()->lowerLinePos() : staff()->upperLinePos()) + 10.0;
		m_cross->setPos((WIDTH - m_cross->boundingRect().width() * m_cross->scale()) / 2.0, minusY);
	m_plus->setPos(0.0, staff()->upperLinePos() - 4.0 - m_plus->boundingRect().height() * m_plus->scale());
	qreal dblSharpH = 0.0;
	if (m_dblSharp) {
		dblSharpH = m_dblSharp->boundingRect().height() * m_dblSharp->scale() + 1.0;
		m_dblSharp->setPos(centeredX(m_dblSharp), m_plus->pos().y() + 1.0 + m_plus->boundingRect().height() * m_plus->scale());
	} else
		dblSharpH = 3.0; // nice gap below 'plus'	button when no double accidentals
	if (m_sharp) {
		m_sharp->setPos(centeredX(m_sharp), m_plus->pos().y() + m_plus->boundingRect().height() * m_plus->scale() + dblSharpH);
		m_flat->setPos(centeredX(m_flat), m_sharp->pos().y() + 1.0 + m_sharp->boundingRect().height() * m_sharp->scale());
		m_accidHi->setRect(QRectF(0.0, 0.0, 
						WIDTH, m_sharp->boundingRect().height() * m_sharp->scale()));
	}
	if (m_dblFlat)
		m_dblFlat->setPos(centeredX(m_dblFlat), m_flat->pos().y() + 0.3 + m_flat->boundingRect().height() * m_flat->scale());
	if (!m_sharp) {
		m_name->setPos(0.0, m_plus->pos().y() + 2.0 + m_plus->boundingRect().height() * m_plus->scale() - 
					(m_name->boundingRect().height() / 3.5) * m_name->scale());
	} else if (m_prevAccidIt) // restore accidental highlight
			m_accidHi->setPos(0.0, m_prevAccidIt->pos().y());
}


void TnoteControl::hideWithDelay() {
	QTimer::singleShot(LEAVE_DELAY, this, SLOT(hideDelayed()));
}


void TnoteControl::addAccidentals() {
	if (!m_sharp) {
		m_sharp = createNootkaTextItem("#");
		m_flat = createNootkaTextItem("b");
		m_accidHi = new QGraphicsRectItem(this);
		m_accidHi->hide();
		m_accidHi->setPen(Qt::NoPen);
		m_accidHi->setBrush(QBrush(qApp->palette().highlight().color()));
		m_accidHi->setZValue(8); // below accidentals
	}
	if (scoreScene()->doubleAccidsFuse() == 2) { // double accidentals
		if (!m_dblSharp) {
			m_dblSharp = createNootkaTextItem("x");
			m_dblFlat = createNootkaTextItem("B");
		}
	} else {
		if (m_dblSharp) {
			delete m_dblSharp;
			m_dblSharp = 0;
			delete m_dblFlat;
			m_dblFlat = 0;
		}
	}
	m_name->hide();
	m_cross->hide();
	adjustSize();
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
			qreal lowest = (staff()->isPianoStaff() ? staff()->lowerLinePos(): staff()->upperLinePos()) + 18.0;
			painter->drawRoundedRect(0.0, staff()->upperLinePos() - 6.0, WIDTH, lowest - staff()->upperLinePos(), 0.75, 0.75);
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
#if !defined (Q_OS_ANDROID)
			QTimer::singleShot(300, this, SLOT(showDelayed()));
			if (notesAddingEnabled()) {
					if (staff()->number() == 0 && staff()->count() < 2)
							m_cross->hide(); // prevent deleting only one note
					else
							m_cross->show();
			}
#else
			m_minus->hide();
			show();
#endif
			if (pos().x() < m_scoreNote->pos().x()) // hide name for left control
				m_name->hide();
// 			else
// 				m_cross->hide();
	} else {
			hide();
	}
}


void TnoteControl::setAccidental(int acc) {
		m_currAccid = acc;
		QGraphicsSimpleTextItem* it = 0;
		if (acc == -2)
			it = m_dblFlat;
		else if (acc == -1)
			it = m_flat;
		else if (acc == 1)
			it = m_sharp;
		else if (acc == 2)
			it = m_dblSharp;
		markItemText(it);
}


void TnoteControl::enableToAddNotes(bool addEnabled) {
	m_notesAdding = addEnabled;
	if (notesAddingEnabled()) {
		m_plus->show();
		if (staff()->number() == 0 && staff()->count() < 2)
				m_cross->hide(); // prevent deleting only one note
		else
				m_cross->show();
	} else {
		m_plus->hide();
		m_cross->hide();
	}
}

//##########################################################################################################
//####################################         PROTECTED    ################################################
//##########################################################################################################


void TnoteControl::hideDelayed() {
	if (m_scoreNote->isCursorVisible())
		return;
	if (hasCursor())
		hideWithDelay();
	else
		hide();
}


void TnoteControl::hoverEnterDelayed() {
	if (hasCursor()) {
		m_entered = true;
	}
}


void TnoteControl::showDelayed() {
	show();
}


void TnoteControl::itemSelected(const QPointF& cPos) {
	QGraphicsItem *it = scene()->itemAt(mapToScene(cPos), scene()->views()[0]->transform());
	if (it == 0 || it->parentItem() != this)
		return;
	if (it == m_name) {
		hoverLeaveEvent(0);
		emit nameMenu(m_scoreNote);
	} else if (it == m_plus) {
		if (pos().x() > m_scoreNote->pos().x()) { // right control - append a note
				staff()->insertNote(m_scoreNote->index() + 1);
		} else { // left control - preppend a note
				staff()->insertNote(m_scoreNote->index() - 1);
		}
	} else if (it == m_cross) {
		staff()->removeNote(m_scoreNote->index());
		hoverLeaveEvent(0);
	} else // accidentals remained
			accidChanged(it);
	
}

#if !defined (Q_OS_ANDROID)
void TnoteControl::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	TscoreItem::hoverEnterEvent(0);
	QTimer::singleShot(300, this, SLOT(hoverEnterDelayed()));
}


void TnoteControl::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	QGraphicsItem *it = scene()->itemAt(mapToScene(event->pos()), scene()->views()[0]->transform());
	if (it && it->parentItem() == this) {
		if (it != m_underItem) {
			it->setGraphicsEffect(ItemHighLight());
			if (it == m_dblSharp)
				emit statusTip(tr("<b>double sharp</b> - raises a note by two semitones (whole tone).<br>On the guitar it is two frets up."));
			else if (it == m_sharp)
				emit statusTip(tr("<b>sharp</b> - raises a note by a half tone (semitone).<br>On the guitar it is one fret up."));
			else if (it == m_flat)
				emit statusTip(tr("<b>flat</b> - lowers a note by a half tone (semitone).<br>On the guitar it is one fret down."));
			else if (it == m_dblFlat)
				emit statusTip(tr("<b>double flat</b> - lowers a note by two semitones (whole tone).<br>On the guitar it is two frets down."));
			else if (it == m_plus)
				emit statusTip(tr("Click <big><b>+</b></big> to add a new note"));
			else if (it == m_cross)
				emit statusTip(tr("Click <big><b>x</b></big> to remove a note"));
			else if (it == m_name)
				emit statusTip(tr("Click %1 to edit note name")
					.arg(TnooFont::span("c", qApp->fontMetrics().boundingRect("A").height() * 1.5, "color: #008080"))); // #008080 - dark Cyan
			else
				emit statusTip("");
			if (m_underItem)
				m_underItem->setGraphicsEffect(0);
			m_underItem = it;
		}
	} else if (m_underItem) {
			m_underItem->setGraphicsEffect(0);
			emit statusTip("");
			m_underItem = 0;
	}
}


void TnoteControl::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
// 	hide();
	if (m_underItem) {
			m_underItem->setGraphicsEffect(0);
			m_underItem = 0;
	}
	hideWithDelay();
// 	if (m_entered)
		TscoreItem::hoverLeaveEvent(event);
	m_entered = false;
}


void TnoteControl::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	itemSelected(event->pos());
}
#endif

#if defined (Q_OS_ANDROID)
void TnoteControl::touched(const QPointF& cPos) {
	TscoreItem::touched(cPos); // keep hasCursor() working
	m_moveNote = true;
}


void TnoteControl::untouched(const QPointF& cPos) {
	TscoreItem::untouched(cPos);
	m_moveNote = false;
}


void TnoteControl::touchMove(const QPointF& cPos) {
  if (m_moveNote && m_scoreNote) {
		m_scoreNote->moveWorkNote(cPos);
  }
}


void TnoteControl::shortTap(const QPointF& cPos) {
	if (m_scoreNote && m_scoreNote->isCursorVisible())
		m_scoreNote->hideWorkNote();
	itemSelected(cPos);
}
#endif

//##########################################################################################################
//####################################         PRIVATE     #################################################
//##########################################################################################################

void TnoteControl::accidChanged(QGraphicsItem* accidIt) {
	int acc = 0;
	if (accidIt == m_dblSharp)
		acc = 2;
	else if (accidIt == m_sharp)
		acc = 1;
	else if (accidIt == m_flat)
		acc = -1;
	else if (accidIt == m_dblFlat)
		acc = -2;
	if (m_prevAccidIt == accidIt)
		acc = 0;
	setAccidental(acc);
	scoreScene()->setCurrentAccid(m_currAccid);
}


QGraphicsSimpleTextItem* TnoteControl::createNootkaTextItem(const QString& aText) {
	QGraphicsSimpleTextItem *nooItem = new QGraphicsSimpleTextItem(aText, this);
	nooItem->setFont(TnooFont());
	nooItem->setBrush(QBrush(qApp->palette().text().color()));
	nooItem->setScale(WIDTH / nooItem->boundingRect().height());
	nooItem->setZValue(10);
	return nooItem;
}


inline qreal TnoteControl::centeredX(QGraphicsItem* it) {
	return (WIDTH - it->boundingRect().width() * it->scale()) / 2.0;
}


void TnoteControl::markItemText(QGraphicsSimpleTextItem* textItem) {
	if (m_prevAccidIt)
		m_prevAccidIt->setBrush(QBrush(qApp->palette().text().color()));
	m_prevAccidIt = textItem;
	if (m_currAccid) {
		m_prevAccidIt->setBrush(QBrush(qApp->palette().highlightedText().color()));
		m_accidHi->setPos(0.0, m_prevAccidIt->pos().y());
		m_accidHi->show();
	} else
		m_accidHi->hide();
}





