/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#include "tsimplescore.h"
#include <tscorescene.h>
#include <tscorestaff.h>
#include <tscorenote.h>
#include <tscorekeysignature.h>
#include <tscoreclef.h>
#include "tnotecontrol.h"
#include <music/tinstrument.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <QtWidgets/QtWidgets>
#include <tmtr.h>


#define TAP_TIME (200) //  ms
#define LONG_TAP_TIME (500) // ms


TsimpleScore::TsimpleScore(int notesNumber, QWidget* parent) :
  QGraphicsView(parent),
  m_bgGlyph(0),
  m_notesNr(notesNumber),
  m_prevBGglyph(-1),
  m_pinchZoomEmitted(false)
{
  if (TscoreNote::touchEnabled())
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
  else {
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    setMouseTracking(true);
  }
  m_wheelFree = true;
  m_wheelLockTimer = new QTimer(this);
  m_wheelLockTimer->setTimerType(Qt::PreciseTimer);
  m_wheelLockTimer->setInterval(150);
  m_wheelLockTimer->setSingleShot(true);
  connect(m_wheelLockTimer, &QTimer::timeout, this, &TsimpleScore::wheelLockSlot);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  
  m_scene = new TscoreScene(this);
  connect(m_scene, SIGNAL(statusTip(QString)), this, SLOT(statusTipChanged(QString)));
  setScene(m_scene);
	
  m_staff = new TscoreStaff(m_scene, m_notesNr);
	m_staff->enableToAddNotes(false);
	m_clefType = m_staff->scoreClef()->clef().type();
	connect(m_staff, SIGNAL(noteChanged(int)), this, SLOT(noteWasClicked(int)));
	connect(m_staff, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));

	setEnabledDblAccid(false);
	setAlignment(Qt::AlignLeft);
	resizeEvent(0);
}

TsimpleScore::~TsimpleScore() {}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

Tnote TsimpleScore::getNote(int index) {
	if (index >= 0 && index < m_notesNr)
		return *(staff()->getNote(index));
	else
		return Tnote();
}


void TsimpleScore::setNote(int index, const Tnote& note) {
		staff()->setNote(index, note);
// 		if (staff()->noteSegment(index)->pos().x() * transform().m11() > width() * 0.75)
// 				centerOn(staff()->noteSegment(index)->mapToScene(staff()->noteSegment(index)->pos()));
// It is necessary only when score will have linear view (single staff with horizontal scroll bar) - so far unused in Nootka
}


void TsimpleScore::clearNote(int index) {
	staff()->noteSegment(index)->markNote(-1);
	TsimpleScore::setNote(index, Tnote(0, 0, 0));
}


void TsimpleScore::setStringNumber(int index, int realNr) {
	if (index >= 0 && index < m_notesNr)
		staff()->noteSegment(index)->setString(realNr);
}


void TsimpleScore::clearStringNumber(int index) {
	if (index >= 0 && index < m_notesNr)
			staff()->noteSegment(index)->removeString();
}


void TsimpleScore::setClef(Tclef clef) {
	if (this->clef().type() != clef.type()) {
		staff()->onClefChanged(clef);
		m_clefType = clef.type();
	}
}


Tclef TsimpleScore::clef() {
	if (isPianoStaff())
			return Tclef(Tclef::e_pianoStaff);
	else if (staff()->scoreClef())
			return staff()->scoreClef()->clef();
	else
			return Tclef(Tclef::e_none);
}


void TsimpleScore::setClefDisabled(bool isDisabled) {
	if (staff()->scoreClef()) {
		if (isDisabled)
				staff()->scoreClef()->setReadOnly(true);
		else
			staff()->scoreClef()->setReadOnly(false);
	}
}


void TsimpleScore::setKeySignature(TkeySignature keySign) {
	if (staff()->scoreKey())
		staff()->scoreKey()->setKeySignature(keySign.value());
}


TkeySignature TsimpleScore::keySignature() {
	TkeySignature key(0);
	if (staff()->scoreKey())
		key = TkeySignature(staff()->scoreKey()->keySignature());
	return key;
}


void TsimpleScore::setEnableKeySign(bool isEnabled) {
  if (isEnabled != isKeySignEnabled()) {
		scoreScene()->mouseEntersOnKey(false);
		staff()->setEnableKeySign(isEnabled);
		if (isEnabled)
				staff()->scoreKey()->showKeyName(true);
		resizeEvent(0);
	}
}


bool TsimpleScore::isKeySignEnabled() {
  return static_cast<bool>(staff()->scoreKey());
}


void TsimpleScore::setEnabledDblAccid(bool isEnabled) {
	m_scene->setDoubleAccidsEnabled(isEnabled);
	if (staff()->noteSegment(0) && scoreScene()->left())
		scoreScene()->left()->addAccidentals();
}


void TsimpleScore::setNoteDisabled(int index, bool isDisabled) {
	staff()->noteSegment(index)->setReadOnly(isDisabled);
}


bool TsimpleScore::isNoteDisabled(int index) {
	return staff()->noteSegment(index)->isReadOnly();
}


void TsimpleScore::setScoreDisabled(bool disabled) {
	staff()->setDisabled(disabled);
//   setAttribute(Qt::WA_TransparentForMouseEvents, disabled);
}


void TsimpleScore::setNoteNameEnabled(bool nameEnabled) {
	scoreScene()->right()->enableNoteName(nameEnabled);
}


void TsimpleScore::setControllersEnabled(bool leftCtrl, bool rightCtrl) {
	scoreScene()->left()->setEnabled(leftCtrl);
	scoreScene()->right()->setEnabled(rightCtrl);
}


void TsimpleScore::setAmbitus(int index, Tnote lo, Tnote hi) {
	if (index >= 0 && index < m_notesNr)
			staff()->noteSegment(index)->setAmbitus((staff()->noteToPos(lo)) + 1, (staff()->noteToPos(hi)) + 1);
}


void TsimpleScore::setAmbitus(Tnote lo, Tnote hi) {
		for (int i = 0; i < m_notesNr; i++)
			setAmbitus(i, lo, hi);
}

/** !!!!All values are hard coded */
Tnote TsimpleScore::lowestNote() {
	if (staff()->isPianoStaff())
      return Tnote(1, -1);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G)
			return Tnote(7, -1);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G_8down)
			return Tnote(7, -2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F)
			return Tnote(7, -2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F_8down)
			return Tnote(2, -3);
	if (staff()->scoreClef()->clef().type() == Tclef::e_alto_C)
			return Tnote(1, -1);
	if (staff()->scoreClef()->clef().type() == Tclef::e_tenor_C)
			return Tnote(6, -2);
	qDebug() << "lowestNote() riches end of method without result";
	return Tnote(6, -2); // It should never happened
}


Tnote TsimpleScore::highestNote() {
	if (staff()->isPianoStaff())
		return Tnote(1, 4);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G)
		return Tnote(4, 4);
	if (staff()->scoreClef()->clef().type() == Tclef::e_treble_G_8down)
		return Tnote(4, 3);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F)
		return Tnote(6, 2);
	if (staff()->scoreClef()->clef().type() == Tclef::e_bass_F_8down)
		return Tnote(6, 1);
	if (staff()->scoreClef()->clef().type() == Tclef::e_alto_C)
		return Tnote(5, 3);
	if (staff()->scoreClef()->clef().type() == Tclef::e_tenor_C)
		return Tnote(3, 3);
	qDebug() << "highestNote() riches end of method without result";
	return Tnote(4, 4);
}


void TsimpleScore::addBGglyph(int instr) {
	if (instr < 0 || instr > 3)
			return;
	m_prevBGglyph = instr;
	if (m_bgGlyph)
		delete m_bgGlyph;
	m_bgGlyph = new QGraphicsSimpleTextItem(instrumentToGlyph(Einstrument(instr)));
	m_bgGlyph->setParentItem(staff());
	m_bgGlyph->setFont(TnooFont());
	QColor bgColor = palette().highlight().color();
	bgColor.setAlpha(50);
	m_bgGlyph->setBrush(bgColor);
	qreal factor = (staff()->height() / m_bgGlyph->boundingRect().height());
	m_bgGlyph->setScale(factor);
	m_bgGlyph->setPos((staff()->width() - m_bgGlyph->boundingRect().width() * factor) / 2 , 
									(staff()->height() - m_bgGlyph->boundingRect().height() * factor) / 2);
	m_bgGlyph->setZValue(1);
	
}


void TsimpleScore::noteWasClicked(int index) {
	Tnote note = *(staff()->getNote(index));
	emit noteWasChanged(index, note);
}


QSize TsimpleScore::sizeHint() const {
	return QWidget::sizeHint();
}


QSize TsimpleScore::minimumSizeHint() const {
	return QWidget::minimumSizeHint();
}


bool TsimpleScore::isPianoStaff() {
	return staff()->isPianoStaff(); 
}



void TsimpleScore::setBGcolor(QColor bgColor) {
  bgColor.setAlpha(230);
  viewport()->setStyleSheet(Tcolor::bgTag(bgColor));
}


void TsimpleScore::setMaxKeySign(int maxK) {
  if (staff()->scoreKey())
    staff()->scoreKey()->setMaxKey(maxK);
}


void TsimpleScore::setMinKeySign(int minK) {
  if (staff()->scoreKey())
    staff()->scoreKey()->setMinKey(minK);
}


//##########################################################################################################
//##########################################   EVENTS    ###################################################
//##########################################################################################################

void TsimpleScore::resizeEvent(QResizeEvent* event) {
	int hh = height(), ww = width();
	if (event) {
		hh = event->size().height();
		ww = event->size().width();
	}
	if (hh == 0)
		return;
	qreal staffOff = 1.0;
  if (staff()->isPianoStaff())
    staffOff = 2.0;
  qreal factor = ((qreal)hh / (staff()->height()))/* / transform().m11()*/;
  qreal yy = 0.0;
  if ((staff()->width() + staffOff + 1.0) * factor > width()) {
    factor = width() / ((staff()->width() + staffOff + 1.0));
    yy = 1.0;
  }
  QTransform matrix;
  matrix.scale(factor, factor);
  setTransform(matrix);
	if (width() > (staff()->width() + staffOff + 1.0) * transform().m11())
		staffOff = ((ww / transform().m11() - (staff()->width()))  / 2.0);
	scene()->setSceneRect(0.0, 0.0, ww / transform().m11(), hh / transform().m11());
  if (yy)
    yy = (scene()->sceneRect().height() - staff()->height()) / 2.0;
	staff()->setPos(staffOff, yy);
}


/**
 * Touch events are processed here and @p TscoreItem gets only @p touched() @p touchMove() and @p untouched()
 * with latest touch position in scene coordinates. To use them, TscoreItem subclass has to map it first.
 * When second finger touches screen, last touch event is canceled by putting null point as a parameter.
 * So far, only: @class TscoreNote, @class TscoreKeySignature and @class TscoreClef handles those methods.
 * Two fingers touch is used to scroll score and to zoom score in/out
 */
bool TsimpleScore::viewportEvent(QEvent* event) {
  if (TscoreNote::touchEnabled()) {
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
      QTouchEvent *te = static_cast<QTouchEvent*>(event);
      if (te->touchPoints().count() == 1) {
        // Touch event is sent here with view coordinates points - their has to be mapped to score scene
        QPointF touchScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
        switch(te->touchPoints().first().state()) {
          case Qt::TouchPointPressed: {
            TscoreItem *it = castItem(scene()->itemAt(touchScenePos, transform()));
            if (it) {
              it->touched(touchScenePos);
              m_currentIt = it;
            }
            break;
          }
          case Qt::TouchPointMoved: {
            if (m_currentIt)
              m_currentIt->touchMove(touchScenePos);
            break;
          }
          case Qt::TouchPointReleased: {
            m_pinchZoomEmitted = false;
            if (m_currentIt) {
              m_currentIt->untouched(touchScenePos);
              m_currentIt = 0;
            }
            break;
          }
          default:
            break;
        }
        return m_currentIt ? true : false;
      } else if (te->touchPoints().count() == 2) {
        if (m_currentIt) { // cancel the last touch event
          m_currentIt->untouched(QPointF(0, 0));
          m_currentIt = 0;
        }
        QLineF l1(te->touchPoints()[0].startPos(), te->touchPoints()[1].startPos()); // initial double-touch position
        QLineF l2(te->touchPoints()[0].pos(), te->touchPoints()[1].pos()); // final position
        qreal distance = l1.length() - l2.length();
        if (distance < -2.0 * Tmtr::fingerPixels()) { // zoom out
          if (!m_pinchZoomEmitted) {
            emit pinchZoom(1);
            m_pinchZoomEmitted = true;
          }
        } else if (distance > 2.0 * Tmtr::fingerPixels()) { // zoom in
          if (!m_pinchZoomEmitted) {
            emit pinchZoom(-1);
            m_pinchZoomEmitted = true;
          }
        } else if (distance < Tmtr::fingerPixels()) // just scroll
          verticalScrollBar()->setValue(verticalScrollBar()->value() + int(te->touchPoints()[0].lastPos().y() - te->touchPoints()[0].pos().y()));
        return true;
      }
    }
  }
  return QGraphicsView::viewportEvent(event);
}


void TsimpleScore::wheelEvent(QWheelEvent* event) {
	bool propagate = true;
	if (event->angleDelta().y()) {
		if (staff()->scoreKey() && !staff()->scoreKey()->readOnly()) {
			QPointF pp = staff()->mapFromScene(mapToScene(event->pos()));
			if (pp.x() > staff()->scoreKey()->pos().x() && pp.x() < staff()->scoreKey()->pos().x() + staff()->scoreKey()->boundingRect().width() - 2.0) {
				if (m_wheelFree) {
					if (event->angleDelta().y() < 0 && keySignature().value() > -7) {
							setKeySignature(keySignature().value() - 1);
							m_wheelFree = false;
							m_wheelLockTimer->start();
					} else if (event->angleDelta().y() > 0 && keySignature().value() < 7) {
							setKeySignature(keySignature().value() + 1);
							m_wheelFree = false;
							m_wheelLockTimer->start();
					}
				}
				propagate = false;
			}
		}
	} else {
			if (event->angleDelta().x() && m_scene->isCursorVisible()) {
				if (m_wheelFree) {
					if (event->angleDelta().x() < -1) {
						m_scene->setCurrentAccid(m_scene->currentAccid() + 1);
						m_wheelFree = false;
						m_wheelLockTimer->start();
					} else if (event->angleDelta().x() > 1) {
						m_scene->setCurrentAccid(m_scene->currentAccid() - 1);
						m_wheelFree = false;
						m_wheelLockTimer->start();
					}
				}
			}
			propagate = false;
	}
  if (propagate)
    QAbstractScrollArea::wheelEvent(event);
}


void TsimpleScore::wheelLockSlot() {
  m_wheelFree = true;
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TsimpleScore::statusTipChanged(QString status) {
	QStatusTipEvent *tipEvent = new QStatusTipEvent(status);
	qApp->postEvent(window(), tipEvent);
	emit statusTip(status);
}


void TsimpleScore::onClefChanged(Tclef clef) {
	if (isPianoStaff())
		emit clefChanged(Tclef(Tclef::e_pianoStaff));
	else
		emit clefChanged(staff()->scoreClef()->clef());
	if ((m_clefType == Tclef::e_pianoStaff && clef.type() != Tclef::e_pianoStaff) ||
			(m_clefType != Tclef::e_pianoStaff && clef.type() == Tclef::e_pianoStaff)	)
					resizeEvent(0);
	m_clefType = clef.type();
}


TscoreItem* TsimpleScore::castItem(QGraphicsItem* it) {
	if (it) {
		for (int i = 0; i < 3; ++i) {
			if (it->type() == TscoreItem::ScoreItemType)
				return static_cast<TscoreItem*>(it);
			if (it->parentItem())
				it = it->parentItem();
			else
				break;
		}
	}
	return 0;
}




