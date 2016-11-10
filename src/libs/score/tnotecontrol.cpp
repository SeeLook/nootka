/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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


#include "tnotecontrol.h"
#include "tscorestaff.h"
#include "tscorenote.h"
#include "tscorescene.h"
#include <tnoofont.h>
#include <graphics/tdropshadoweffect.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qgraphicsview.h>


QLinearGradient TnoteControl::m_leftGrad;
QLinearGradient TnoteControl::m_rightGrad;


QGraphicsDropShadowEffect* ItemHighLight() {
	QGraphicsDropShadowEffect *hiBlur = new QGraphicsDropShadowEffect();
	hiBlur->setColor(qApp->palette().highlight().color());
	hiBlur->setOffset(1.0, 1.0);
	hiBlur->setBlurRadius(7.0);
	return hiBlur;
}


TnoteControl::TnoteControl(TscoreStaff* staff, TscoreScene* scene) :
	TscoreItem(scene),
	m_scoreNote(nullptr),
	m_isEnabled(true), m_entered(false),
	m_dblSharp(nullptr), m_sharp(nullptr), m_flat(nullptr), m_dblFlat(nullptr), m_accidGap(nullptr),
	m_underItem(nullptr),
	m_moveNote(false),
	m_currAccid(0), m_prevAccidIt(0),
	m_notesAdding(true),
	m_adding(false),
	m_delayTimer(new QTimer(this))
{
	setStaff(staff);
	setParentItem(staff);
	setZValue(60);
	hide();

	m_gradient = &m_rightGrad;
// 'name' glyph for editing note through its name
	m_name = new QGraphicsSimpleTextItem("c");
	m_name->setFont(TnooFont());
	m_name->setParentItem(this);
	m_name->setScale(boundingRect().width() / m_name->boundingRect().width());
	m_name->setBrush(scoreScene()->nameColor());
// 'x' for deleting notes
	m_cross = new QGraphicsSimpleTextItem("o");
	m_cross->setFont(TnooFont());
	m_cross->setParentItem(this);
	m_cross->setScale((boundingRect().width() / m_cross->boundingRect().width()));
	m_cross->setBrush(Qt::red);
	adjustSize();
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
	connect(m_delayTimer, SIGNAL(timeout()), this, SLOT(showDelayed()));

  setGraphicsEffect(new TdropShadowEffect);
}


TnoteControl::~TnoteControl()
{
}


void TnoteControl::adjustSize() {
	m_height = staff()->height();
	m_name->setBrush(scoreScene()->nameColor());
	qreal minusY = (staff()->isPianoStaff() ? staff()->lowerLinePos() : staff()->upperLinePos()) + 14.0;
	m_cross->setPos((WIDTH - m_cross->boundingRect().width() * m_cross->scale()) / 2.0, 
										minusY - m_cross->boundingRect().height() * m_cross->scale());
#if defined (Q_OS_ANDROID)
  qreal baseY = staff()->upperLinePos() - 6.0;
#else
  qreal baseY = staff()->upperLinePos() - 4.0;
#endif
	qreal dblSharpH = 0.0;
	if (m_dblSharp) {
		dblSharpH = m_dblSharp->boundingRect().height() * m_dblSharp->scale() + 2.0;
		m_dblSharp->setPos(centeredX(m_dblSharp), baseY + 2.0);
	} else
		dblSharpH = 3.0; // nice gap below 'plus'	button when no double accidentals
	if (m_sharp) {
		m_sharp->setPos(centeredX(m_sharp), baseY + dblSharpH);
    m_accidGap->setRect(0, 0, WIDTH, 2.0);
    m_accidGap->setPos(0, m_sharp->pos().y() + m_sharp->boundingRect().height() * m_sharp->scale());
		m_flat->setPos(centeredX(m_flat), m_accidGap->pos().y() + 2.0);
		m_accidLight->setRect(0.0, 0.0, WIDTH, m_sharp->boundingRect().height() * m_sharp->scale());
	}
	if (m_dblFlat)
		m_dblFlat->setPos(centeredX(m_dblFlat), m_flat->pos().y() + m_flat->boundingRect().height() * m_flat->scale());
	if (!m_sharp) { // right pane
		m_name->setPos(0.0, baseY + (m_name->boundingRect().height()) * m_name->scale());
	} else if (m_prevAccidIt) // restore accidental highlight
			m_accidLight->setPos(0.0, m_prevAccidIt->pos().y());

	m_leftGrad.setStart(WIDTH, 1.0);
	m_leftGrad.setFinalStop(0.0, 1.0);
	m_rightGrad.setStart(0.0, 1.0);
	m_rightGrad.setFinalStop(WIDTH, 1.0);
	QColor startC = qApp->palette().text().color(), endC = startC;
	startC.setAlpha(150);
	endC.setAlpha(200);
	m_leftGrad.setColorAt(0.5, startC);
	m_leftGrad.setColorAt(1.0, endC);
	m_rightGrad.setColorAt(0.5, startC);
	m_rightGrad.setColorAt(1.0, endC);
}


void TnoteControl::hideWithDelay(int delay) {
  if (delay) {
    if (delay == -1)
      delay = touchEnabled() ? 1000: 300;
    QTimer::singleShot(delay, this, SLOT(hideDelayed()));
  } else
    hideDelayed();
}


void TnoteControl::addAccidentals() {
	if (!m_sharp) {
		m_sharp = createNootkaTextItem("#");
    m_sharp->setScale(m_sharp->scale() * 1.2); // it looks and works better
		m_flat = createNootkaTextItem("b");
    m_flat->setScale(m_flat->scale() * 1.2);
		m_accidLight = new QGraphicsRectItem(this);
		m_accidLight->hide();
		m_accidLight->setPen(Qt::NoPen);
		m_accidLight->setBrush(QBrush(qApp->palette().highlight().color()));
		m_accidLight->setZValue(8); // below accidentals
  // gap item between sharp and flat to avoid blinking
    m_accidGap = new QGraphicsRectItem();
    m_accidGap->setParentItem(this);
    m_accidGap->setPen(Qt::NoPen);
    m_accidGap->setZValue(12);
	}
	if (scoreScene()->doubleAccidsFuse() == 2) { // double accidentals
		if (!m_dblSharp) {
			m_dblSharp = createNootkaTextItem("x");
			m_dblFlat = createNootkaTextItem("B");
		}
	} else {
		if (m_dblSharp) {
			delete m_dblSharp;
			m_dblSharp = nullptr;
			delete m_dblFlat;
			m_dblFlat = nullptr;
		}
	}
	if (isLeftPane())
		m_gradient = &m_leftGrad;
	else 
		m_gradient = &m_rightGrad;
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
#if defined (Q_OS_ANDROID)
    qreal baseY = staff()->upperLinePos() - 8.0;
#else
    qreal baseY = staff()->upperLinePos() - 6.0;
#endif
    if (m_entered && m_adding)
      painter->setBrush(QBrush(*m_gradient));
    else {
      QColor bc = qApp->palette().base().color();
      bc.setAlpha(240);
      painter->setBrush(QBrush(bc));
    }
    painter->setPen(Qt::NoPen);
#if defined (Q_OS_ANDROID)
    qreal lowest = (staff()->isPianoStaff() ? staff()->lowerLinePos(): staff()->upperLinePos()) + 26.0;
#else
    qreal lowest = (staff()->isPianoStaff() ? staff()->lowerLinePos(): staff()->upperLinePos()) + 20.0;
#endif
    painter->drawRoundedRect(QRectF(0.0, baseY, WIDTH, lowest - staff()->upperLinePos()), 0.25, 0.25);
    if ((touchEnabled() && notesAddingEnabled()) || (m_entered && m_adding)) { // 'plus' symbol
      if (touchEnabled())
        painter->setPen(QPen(qApp->palette().text().color(), 0.4, Qt::SolidLine, Qt::RoundCap));
      else
        painter->setPen(QPen(qApp->palette().base().color(), 0.4, Qt::SolidLine, Qt::RoundCap));
      qreal plusW = (WIDTH - 2.0) / 2.0;
      painter->drawLine(QLineF(plusW, baseY + 2.0, WIDTH - plusW, baseY + 2.0)); // horizontal 'plus' line
      painter->drawLine(QLineF(WIDTH / 2.0, baseY + 1.0, WIDTH / 2.0, baseY + 3.0)); // vertical 'plus' line
    }
	}
}


void TnoteControl::setScoreNote(TscoreNote* sn) {
	m_scoreNote = sn;
	if (sn) {
			if (sn->parentItem() && parentItem() != sn->parentItem()) {
				if (parentItem())
						parentItem()->setZValue(10);
				setParentItem(sn->parentItem());
				setStaff(sn->staff());
				parentItem()->setZValue(11);
			}
			if (notesAddingEnabled()) {
					if (staff()->number() == 0 && staff()->count() < 2)
							m_cross->hide(); // prevent deleting only one note
					else
							m_cross->show();
			}
			if (isLeftPane()) { // hide 'name' and 'cross' for left control
				m_name->hide();
				m_cross->hide();
			}
	} else {
			hide();
	}
}


void TnoteControl::setAccidental(int acc) {
  m_currAccid = acc;
  QGraphicsSimpleTextItem* it = nullptr;
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
		if (staff()->number() == 0 && staff()->count() < 2)
				m_cross->hide(); // prevent deleting only one note
		else
				m_cross->show();
	} else {
		m_cross->hide();
	}
}


void TnoteControl::hide() {
	m_adding = false;
	QGraphicsItem::hide();
}

//##########################################################################################################
//####################################         PROTECTED    ################################################
//##########################################################################################################


void TnoteControl::hideDelayed() {
	if (scoreScene()->isCursorVisible())
		return;
	if (hasCursor())
		hideWithDelay();
	else
		hide();
}


void TnoteControl::showDelayed() {
	m_delayTimer->stop();
	if (hasCursor()) {
		m_entered = true;
		if (m_adding)
			update();
	}
}


void TnoteControl::itemSelected(const QPointF& cPos) {
  if (touchEnabled() && notesAddingEnabled()) {
    if (cPos.y() < staff()->upperLinePos() - 3.0) {
      if (pos().x() > m_scoreNote->pos().x()) { // right control - append a note
          m_scoreNote->hideWorkNote();
          if (m_scoreNote->index() == staff()->count() - 1)
            QTimer::singleShot(50, [=]{ hide(); }); // hide right pane when note is added at staff end to edit new one easily (otherwise pane covers it)
          staff()->insertNote(m_scoreNote->index() + 1);
          return;
      } else { // left control - preppend a note
          m_scoreNote->hideWorkNote();
          staff()->insertNote(m_scoreNote->index());
          return;
      }
    }
  } else {
    if (m_adding) {
      if (pos().x() > m_scoreNote->pos().x()) { // right control - append a note
          if (m_scoreNote->index() == staff()->count() - 1)
            QTimer::singleShot(50, [=]{ hide(); }); // hide right pane when note is added at staff end to edit new one easily (otherwise pane covers it)
          staff()->insertNote(m_scoreNote->index() + 1);
      } else { // left control - preppend a note
          staff()->insertNote(m_scoreNote->index());
      }
      return;
    }
  }
	auto it = scene()->itemAt(mapToScene(cPos), scene()->views()[0]->transform());
	if (it == nullptr || it->parentItem() != this || it == m_accidGap)
    return;
	if (it == m_name) {
		hoverLeaveEvent(nullptr);
    QTimer::singleShot(5, [=]{ emit nameMenu(m_scoreNote); });
	} else if (it == m_cross) {
		staff()->removeNote(m_scoreNote->index());
		hoverLeaveEvent(nullptr);
	} else // accidentals remained
			accidChanged(it);
	
}


void TnoteControl::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	TscoreItem::hoverEnterEvent(0);
	m_delayTimer->start(150);
}


void TnoteControl::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	scoreScene()->controlMoved();
  auto it = scene()->itemAt(mapToScene(event->pos()), scene()->views()[0]->transform());
	if (m_notesAdding) {
		if (it == this) {
      if (!m_adding)
        emit statusTip(tr("Click to add a new note"));
			m_adding = true;
		} else
			m_adding = false;
		if (it != m_underItem)
			update();
	}
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
			else if (it == m_cross)
				emit statusTip(tr("Click %1 to remove a note")
					.arg(TnooFont::span(QLatin1String("o"), qApp->fontMetrics().boundingRect(QLatin1String("A")).height() * 2.0, QLatin1String("color: #ff0000")))); // red
			else if (it == m_name)
				emit statusTip(tr("Click %1 to edit note name")
					.arg(TnooFont::span(QLatin1String("c"), qApp->fontMetrics().boundingRect(QLatin1String("A")).height() * 1.5,
															QLatin1String("color: ") + scoreScene()->nameColor().name())));
			else if (!m_adding)
				emit statusTip(QString());
			if (m_underItem)
				m_underItem->setGraphicsEffect(nullptr);
			m_underItem = it;
		}
	} else if (m_underItem) {
			m_underItem->setGraphicsEffect(nullptr);
      if (!m_adding)
        emit statusTip(QString());
			m_underItem = nullptr;
	}
}


void TnoteControl::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
	if (m_adding) {
		m_adding = false;
		update();
	}
	if (m_underItem) {
		m_underItem->setGraphicsEffect(nullptr);
		m_underItem = nullptr;
	}
	hideWithDelay();
	TscoreItem::hoverLeaveEvent(event);
	m_entered = false;
	scoreScene()->controlLeaved(scoreNote());
}


void TnoteControl::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	itemSelected(event->pos());
}


void TnoteControl::touched(const QPointF& scenePos) {
	TscoreItem::hoverEnterEvent(0);
  QGraphicsSceneHoverEvent he(QEvent::GraphicsSceneHoverMove);
  he.setPos(mapFromScene(scenePos));
  hoverMoveEvent(&he);
  m_entered = true;
}


void TnoteControl::untouched(const QPointF& scenePos) {
// 	TscoreItem::untouched(scenePos);
//   if (m_scoreNote && m_scoreNote->hasCursor())
//     m_scoreNote->hideWorkNote();
  itemSelected(mapFromScene(scenePos));
  hoverLeaveEvent(0);
//   m_entered = false;
}

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
	auto nooItem = new QGraphicsSimpleTextItem(aText, this);
	nooItem->setFont(TnooFont());
	nooItem->setBrush(QBrush(qApp->palette().text().color()));
	nooItem->setScale((WIDTH / nooItem->boundingRect().height()) * 1.12);
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
		m_accidLight->setPos(0.0, m_prevAccidIt->pos().y());
		m_accidLight->show();
	} else
		m_accidLight->hide();
}





