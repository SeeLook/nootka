/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#include "tscorenote.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include "tnotecontrol.h"
#include <graphics/tdropshadoweffect.h>
#include <animations/tcrossfadetextanim.h>
#include <animations/tcombinedanim.h>
#include <music/tnote.h>
#include <tnoofont.h>
#include <tcolor.h>
#include <QEasingCurve>
#include <QGraphicsEffect>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QTimer>
#include <QTouchEvent>

#include <QDebug>

const int accCharTable[6] = { 0xe123, 0xe11a, 0x20, 0xe10e, 0xe125, 0xe116 };

/*static*/
QString TscoreNote::getAccid(int accNr) {
    QString str = QString(QChar(accCharTable[accNr + 2]));
    return str;
}


QGraphicsEllipseItem* TscoreNote::createNoteHead(QGraphicsItem* parentIt) {
  QGraphicsEllipseItem *noteHead = new QGraphicsEllipseItem();
	noteHead->setParentItem(parentIt);
  noteHead->setRect(0, 0, 3.5, 2);
  noteHead->hide();
  return noteHead;
}


/** To avoid creating many tips - one for every instance and waste RAM
 * this text exist as static variable 
 * and TscoreNote manages itself when status tip is necessary to be displayed. */
QString TscoreNote::m_staticTip = "";


//################################## CONSTRUCTOR ###################################

TscoreNote::TscoreNote(TscoreScene* scene, TscoreStaff* staff, int index) :
  TscoreItem(scene),
  m_mainPosY(0.0),
  m_accidental(0),
  m_index(index),
  m_stringText(0), m_stringNr(0),
  m_readOnly(false),
  m_nameText(0),
  m_ottava(0),
  m_bgColor(-1),
  m_noteAnim(0), m_popUpAnim(0), m_accidToKeyAnim(false),
  m_selected(false),
  m_touchedToMove(false)
{
  setStaff(staff);
	setParentItem(staff);
#if defined (Q_OS_ANDROID)
	enableTouchToMouse(false); // Touch events are re-implemented here
#endif
  m_height = staff->height();
  m_mainColor = qApp->palette().text().color();
	m_note = new Tnote(0, 0, 0);

	m_lines = new TscoreLines(this);
  m_mainNote = createNoteHead(this);
	
  m_mainAccid = new QGraphicsSimpleTextItem();
	m_mainAccid->setParentItem(m_mainNote);
	
  m_mainAccid->setFont(TnooFont(5));
	bool prepareScale = false;
	if (scoreScene()->accidScale() == -1.0) { // only when first TscoreNote is constructed
			m_staticTip = tr("Click to select a note, use mouse wheel to change accidentals.");
			m_mainAccid->setText(getAccid(1));
			scoreScene()->setAccidScale(6.0 / m_mainAccid->boundingRect().height());
			prepareScale = true;
	}
	m_emptyText = new QGraphicsSimpleTextItem(tr("enter note").toUpper().replace(" ", "\n"), this);
		m_emptyText->setZValue(1);
		QColor cc = qApp->palette().highlight().color();
		cc.setAlpha(80);
		m_emptyText->setBrush(Tcolor::merge(cc, qApp->palette().base().color()));
// 		m_emptyText->setBrush(qApp->palette().base().color());
		m_emptyText->setPen(QPen(qApp->palette().text().color(), 0.1));
		m_emptyText->setScale(8.0 / m_emptyText->boundingRect().width());
		m_emptyText->setTransformOriginPoint(m_emptyText->boundingRect().center().x() * m_emptyText->scale(), 
																				m_emptyText->boundingRect().center().y() * m_emptyText->scale());
		m_emptyText->setRotation(90);
		QRectF eRect = m_emptyText->mapToParent(m_emptyText->boundingRect()).boundingRect();
		m_emptyText->setPos((7.0 - eRect.width()) / 2 - eRect.x(), staff->upperLinePos() - eRect.y() + (staff->isPianoStaff() ? 8 : 0));
		m_emptyText->hide();
		
  m_mainAccid->setScale(scoreScene()->accidScale());
	if (prepareScale) {
			scoreScene()->setAccidYoffset(m_mainAccid->boundingRect().height() * scoreScene()->accidScale() * 0.34);
			m_mainAccid->setText("");
	}
	m_mainAccid->setPos(-3.0, - scoreScene()->accidYoffset());
	
	if (!scene->views().isEmpty() && scoreScene()->right() == 0)
			initNoteCursor();
	
  setColor(m_mainColor);
  m_mainNote->setZValue(34); // under
  m_mainAccid->setZValue(m_mainNote->zValue());
  if (staff->isPianoStaff())
		setAmbitus(40, 2);
	else
		setAmbitus(34, 2);
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
	checkEmptyText();
}


TscoreNote::~TscoreNote() { // release work note and controls from destructing parent
	if (scoreScene()->right() && (scoreScene()->workNote()->parentItem() == this || scoreScene()->right()->parentItem() == parentItem()))
		scoreScene()->noteDeleted(this);
	delete m_note;
}

//##############################################################################
//#################### PUBLIC METHODS    #######################################
//##############################################################################

void TscoreNote::adjustSize() {
	m_height = staff()->height();
	m_lines->adjustLines(this);
	setColor(m_mainColor);
	if (staff()->isPianoStaff())
		m_emptyText->setPos(m_emptyText->x(), m_emptyText->y() + 6);
	else
		m_emptyText->setPos(m_emptyText->x(), m_emptyText->y() - 6);
}


void TscoreNote::setColor(const QColor& color) {
	m_mainColor = color;
	m_mainNote->setPen(Qt::NoPen);
	m_mainNote->setBrush(QBrush(m_mainColor, Qt::SolidPattern));
	m_mainAccid->setBrush(QBrush(m_mainColor));
	m_lines->setColor(color);
	if (m_stringText)
			m_stringText->setBrush(QBrush(m_mainColor));
}


void TscoreNote::selectNote(bool sel) {
	m_selected = sel;
	checkEmptyText();
}


void TscoreNote::moveNote(int posY) {
// 		if (posY == 0 || !(posY >= m_ambitMax - 1 && posY <= m_ambitMin)) {
		if (posY == 0 || !(posY >= 1 && posY <= m_height - 3)) {
				hideNote();
				m_mainAccid->setText(" ");
				m_accidental = 0;
				return;
    }
		if (!m_mainNote->isVisible()) {
        m_mainNote->show();
        m_mainAccid->show();
    }
		if (m_noteAnim) { // initialize animation
				m_noteAnim->setMoving(m_mainNote->pos(), QPointF(3.0, posY));
				m_noteAnim->startAnimations();
		} else { // just move a note
			m_mainNote->setPos(3.0, posY);
		}
		m_mainPosY = posY;
    int noteNr = (56 + staff()->notePosRelatedToClef(staff()->fixNotePos(posY))) % 7;
		QString newAccid = getAccid(m_accidental);
		if (staff()->accidInKeyArray[noteNr]) {
      if (m_accidental == 0) {
					newAccid = getAccid(3); // neutral
					m_mainAccid->hide();
					if (m_accidToKeyAnim)
							emit fromKeyAnim(newAccid, m_mainAccid->scenePos(), m_mainPosY);
			} else {
					if (staff()->accidInKeyArray[noteNr] == m_accidental) {
						if (m_accidToKeyAnim)
								emit toKeyAnim(newAccid, m_mainAccid->scenePos(), m_mainPosY);
						if (staff()->extraAccids()) // accidental from key signature in brackets
							newAccid = QString(QChar(accCharTable[m_accidental + 2] + 1));
						else
							newAccid = " "; // hide accidental
						
					}
      }
    }
    if (m_noteAnim) {
				m_accidAnim->startCrossFading(newAccid, m_mainColor);
    } else {
				m_mainAccid->setText(newAccid);
				m_mainAccid->show();
    }

    setStringPos();
		m_lines->checkLines(posY);
}


void TscoreNote::setNote(int notePos, int accNr, const Tnote& n) {
	m_accidental = accNr;
	*m_note = n;
	moveNote(notePos);
	if (m_mainPosY == 0)
			*m_note = Tnote(); // set note to null if beyond the score possibilities
	if (m_nameText)
			showNoteName();
	update();
	checkEmptyText();
}


void TscoreNote::hideNote() {
	m_mainNote->hide();
	m_mainAccid->hide();
	m_lines->hideAllLines();
	m_mainPosY = 0;
	m_accidental = 0;
	m_mainNote->setPos(3.0, 0);
}


void TscoreNote::moveWorkNote(const QPointF& newPos) {
	QGraphicsSceneHoverEvent moveEvent(QEvent::GraphicsSceneHoverMove);
	moveEvent.setPos(newPos);
	hoverMoveEvent(&moveEvent);
}


void TscoreNote::hideWorkNote() {
	if (scoreScene()->workNote() && scoreScene()->workNote()->isVisible()) {
    scoreScene()->workNote()->hide();
    scoreScene()->workAccid()->hide();
    scoreScene()->workLines()->hideAllLines();
    scoreScene()->setWorkPosY(0);
	}
}


void TscoreNote::markNote(QColor blurColor) {
	if (blurColor == -1) {
		m_mainNote->setPen(Qt::NoPen);
		m_mainNote->setGraphicsEffect(0);
	} else {
		m_mainNote->setPen(QPen(blurColor, 0.2));
		QGraphicsDropShadowEffect *bluredPen = new QGraphicsDropShadowEffect();
		bluredPen->setBlurRadius(10);
		bluredPen->setColor(QColor(blurColor.name()));
		bluredPen->setOffset(0.5, 0.5);
		m_mainNote->setGraphicsEffect(bluredPen /*TdropShadowEffect(blurColor)*/);
	}
	update();
}


void TscoreNote::setString(int realNr) {
	if (!m_stringText) {
        m_stringText = new QGraphicsSimpleTextItem();
				m_stringText->setFont(TnooFont(5));
        m_stringText->setBrush(QBrush(m_mainColor));
        m_stringText->setParentItem(this);
        m_stringText->setZValue(-1);
    }
    m_stringText->setText(QString("%1").arg(realNr));
		m_stringText->setScale(5.0 / m_stringText->boundingRect().width());
		m_stringNr = realNr;
    setStringPos();
}


void TscoreNote::removeString() {
		if (m_stringText) {
        delete m_stringText;
        m_stringText = 0;
    }
    m_stringNr = 0;
}


void TscoreNote::setReadOnly(bool ro) {
		setAcceptHoverEvents(!ro);
		m_readOnly = ro;
}

/** If @dropShadowColor is not set and m_nameText has already existed,
 * previously color remained. */
void TscoreNote::showNoteName(const QColor& dropShadowColor) {
	bool setColor = false;
	if (!m_nameText) {
		m_nameText = new QGraphicsTextItem();
		m_nameText->setDefaultTextColor(m_mainColor);
		m_nameText->setParentItem(this);
		m_nameText->setZValue(10);
		m_nameText->setAcceptHoverEvents(false);
		setColor = true;
	}
	if (dropShadowColor != -1)
		setColor = true;
	if (setColor) {
		QGraphicsDropShadowEffect *dropEff = new QGraphicsDropShadowEffect();
		if (dropShadowColor == -1)
			dropEff->setColor(scoreScene()->nameColor());
		else
			dropEff->setColor(dropShadowColor);
		dropEff->setOffset(0.7, 0.7);
		dropEff->setBlurRadius(5.0);
		m_nameText->setGraphicsEffect(dropEff);
	}
	if (m_note->note) {
			m_nameText->setHtml(m_note->toRichText());
			m_nameText->setScale(8.0 / m_nameText->boundingRect().height());
			if (m_nameText->boundingRect().width() * m_nameText->scale() > boundingRect().width())
					m_nameText->setScale(boundingRect().width() / (m_nameText->boundingRect().width()));
			m_nameText->setPos((8.0 - m_nameText->boundingRect().width() * m_nameText->scale()) * 0.75, /*yy);*/
							notePos() > staff()->upperLinePos() ? 
										notePos() - (m_nameText->boundingRect().height() + 2.0) * m_nameText->scale(): // above note
										notePos() + m_mainNote->boundingRect().height()); // below note
			m_nameText->show();
	} else
			m_nameText->hide();
}


void TscoreNote::removeNoteName() {
	delete m_nameText;
	m_nameText = 0;
}


void TscoreNote::enableNoteAnim(bool enable, int duration) {
	if (enable) {
			if (!m_noteAnim) {
					m_noteAnim = new TcombinedAnim(m_mainNote, this);
					m_noteAnim->setDuration(duration);
					m_noteAnim->setMoving(mainNote()->pos(), mainNote()->pos());
					m_noteAnim->moving()->setEasingCurveType(QEasingCurve::InExpo);
					m_noteAnim->setScaling(1.0, 1.5);
					m_noteAnim->scaling()->setEasingCurveType(QEasingCurve::OutQuint);
					m_accidAnim = new TcrossFadeTextAnim(m_mainAccid, this);
			}
			m_accidAnim->setDuration(duration);
	} else {
			if (m_noteAnim) {
				delete m_noteAnim;
				m_noteAnim = 0;
				delete m_accidAnim;
				m_accidAnim = 0;
			}
	}
}


void TscoreNote::setAmbitus(int lo, int hi) { 
	m_ambitMin = qBound(2, lo, (int)m_height - 1); 
	m_ambitMax = qBound(2, hi, (int)m_height - 1);
}


QRectF TscoreNote::boundingRect() const{
    return QRectF(0, 0, 7.0, m_height);
}


void TscoreNote::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	if (m_bgColor != -1) {
// 			paintBackground(painter, m_bgColor);
		QPointF center(3.5, m_mainPosY + 1.0);
		if (m_mainPosY == 0)
			center.setY(staff()->upperLinePos() + 4.0);
		QRadialGradient gr(center, 10.0);
		QColor c1 = m_bgColor;
		c1.setAlpha(40);
		QColor c2 = m_bgColor;
		c2.setAlpha(80);
		gr.setColorAt(0.0, c1);
		gr.setColorAt(0.9, c1);
		gr.setColorAt(0.95, c2);
		gr.setColorAt(1.0, Qt::transparent);
		painter->setBrush(gr);
		painter->setPen(Qt::NoPen);
		painter->drawRect(0.0, qMax(center.y() - 10.0, 0.0), 7.0, qMin(center.y() + 10.0, m_height));
	}
	if (!m_selected && m_mainPosY == 0 && scoreScene()->right() && scoreScene()->right()->notesAddingEnabled()) {
		QColor emptyNoteColor;
		if (m_mainNote->pen().style() == Qt::NoPen)
			emptyNoteColor = qApp->palette().highlight().color();
		else
			emptyNoteColor = m_mainNote->pen().color();
		emptyNoteColor.setAlpha(120);
		painter->setPen(QPen(emptyNoteColor, 0.4, Qt::SolidLine, Qt::RoundCap));
		painter->drawLine(QLineF(0.5, staff()->upperLinePos() - 1.0, 6.5, staff()->upperLinePos() - 2.0));
		qreal loLine = staff()->isPianoStaff() ? staff()->lowerLinePos() : staff()->upperLinePos();
		painter->drawLine(QLineF(0.5, loLine + 10.0, 6.5, loLine + 9.0));
	}
}


void TscoreNote::keyAnimFinished() {
	if (!m_readOnly)
			m_mainAccid->show();
}


void TscoreNote::popUpAnim(int durTime) {
	if (m_popUpAnim)
		return;
	m_popUpAnim = new TcombinedAnim(m_emptyText);
		m_popUpAnim->setDuration(durTime);
		m_popUpAnim->setMoving(QPointF(m_emptyText->x(), -10), m_emptyText->pos());
// 		m_popUpAnim->setScaling(m_emptyText->scale(), m_emptyText->scale() * 1.6);
// 		m_popUpAnim->setFading(0.2, 1.0);
		connect(m_popUpAnim, SIGNAL(finished()), this, SLOT(popUpAnimFinished()));
		m_popUpAnim->startAnimations();
}



//#################################################################################################
//########################################## PROTECTED   ##########################################
//#################################################################################################

void TscoreNote::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverEnterEvent";
	scoreScene()->noteEntered(this);
  emit statusTip(m_staticTip);
  TscoreItem::hoverEnterEvent(event);
}


void TscoreNote::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverLeaveEvent";
  hideWorkNote();
	scoreScene()->noteLeaved(this);
  TscoreItem::hoverLeaveEvent(event);
}


void TscoreNote::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
  if ((event->pos().y() >= m_ambitMax) && (event->pos().y() <= m_ambitMin)) {
// 	if ((event->pos().y() >= 2.0) && (event->pos().y() <= m_height - 3.0)) {
		if (staff()->isPianoStaff() && // dead space between staves
			(event->pos().y() >= staff()->upperLinePos() + 10.6) && (event->pos().y() <= staff()->lowerLinePos() - 2.4)) {
				hideWorkNote();
				return;
		}
    if (event->pos().y() != scoreScene()->workPosY()) {
			scoreScene()->noteMoved(this, event->pos().y() - 0.6);
    }
  }
}


void TscoreNote::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (scoreScene()->workPosY()) { // edit mode
		if (event->button() == Qt::LeftButton) {
				m_accidental = scoreScene()->currentAccid();
        moveNote(scoreScene()->workPosY());
        emit noteWasClicked(m_index);
				if (m_nameText)
					showNoteName();
				update();
    } else if (event->button() == Qt::RightButton) {
				if (staff()->selectableNotes() || scoreScene()->controlledNotes()) {
						setBackgroundColor(qApp->palette().highlight().color());
						emit noteWasSelected(m_index);
						update();
				}
    }
	} else { // read only mode
		if (event->button() == Qt::LeftButton)
			emit roNoteClicked(this);
		else if (event->button() == Qt::RightButton)
			emit roNoteSelected(this);
	}
}


void TscoreNote::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) {
	if (scoreScene()->workPosY()) // edit mode
		emit noteWasSelected(m_index);
	else // read only mode
		emit roNoteSelected(this);
}


#if defined(Q_OS_ANDROID)
void TscoreNote::touched(const QPointF& cPos) {
	if (m_readOnly)
			return;
	TscoreItem::touched(cPos);			
}


void TscoreNote::untouched(const QPointF& cPos) {
	if (m_readOnly)
			return;
	TscoreItem::untouched(cPos);
	m_touchedToMove = false;
	if (staff()->controlledNotes()) {
			if (scoreScene()->right()->isEnabled() && !scoreScene()->right()->hasCursor())
					scoreScene()->right()->hideWithDelay();
			if (scoreScene()->left()->isEnabled() && !scoreScene()->left()->hasCursor())
					scoreScene()->left()->hideWithDelay();
			if (isCursorVisible()) {
					QTimer::singleShot(950, this, SLOT(hideWorkNote()));
			}
	}		
}


void TscoreNote::touchMove(const QPointF& cPos) {
	if (m_readOnly)
			return;
  if (m_touchedToMove && isCursorVisible())
		moveWorkNote(cPos);
}


void TscoreNote::shortTap(const QPointF& cPos) {
	if (m_readOnly)
			return;
	if (isCursorVisible()) {
			if (cPos.y() >= scoreScene()->workPosY() - 4 && cPos.y() <= scoreScene()->workPosY() + 4) {
				if (cPos.x() >= 0.0 && cPos.x() <= 7.0) { // if finger taken over note - it was selected
						QGraphicsSceneMouseEvent me(QEvent::MouseButtonPress);
						me.setPos(cPos);
						me.setButton(Qt::LeftButton);
						mousePressEvent(&me);
				}
			}
			hideWorkNote();
			scoreScene()->right()->hide();
			scoreScene()->left()->hide();
	} else 
			emit noteWasSelected(m_index);
}


void TscoreNote::longTap(const QPointF& cPos) {
	if (m_readOnly)
			return;
	if (staff()->controlledNotes()) {
			m_touchedToMove = true;
			if (scoreScene()->right()->isEnabled()) {
			scoreScene()->right()->setPos(pos().x() + boundingRect().width(), 0.0);
			scoreScene()->right()->setScoreNote(this);
		}
		if (scoreScene()->left()->isEnabled()) {
			scoreScene()->left()->setPos(pos().x() - scoreScene()->left()->boundingRect().width(), 0.0);
			scoreScene()->left()->setScoreNote(this);
		}
	}
	if (scoreScene()->workNote()->parentItem() != this)
			setCursorParent(this);
  if ((cPos.y() >= m_ambitMax) && (cPos.y() <= m_ambitMin)) {
			scoreScene()->workNote()->show();
			scoreScene()->workAccid()->show();
  }
}
#endif

//##########################################################################################################
//####################################         PRIVATE     #################################################
//##########################################################################################################

void TscoreNote::setStringPos() {
	if (m_stringText) {
			qreal yy = staff()->upperLinePos() + 9.0; // below the staff
			if (m_mainPosY > staff()->upperLinePos() + 4.0) 
					yy = staff()->upperLinePos() - 7.0; // above the staff 
			m_stringText->setPos(6.5 - m_stringText->boundingRect().width() * m_stringText->scale(), yy);
			// 6.5 is right position of note head
	}
}


void TscoreNote::initNoteCursor() {
  scoreScene()->initNoteCursor(this);
	hideWorkNote();
}


void TscoreNote::checkEmptyText() {
	if (!m_selected && m_mainPosY == 0 && scoreScene()->right() && scoreScene()->right()->notesAddingEnabled())
		m_emptyText->show();
	else
		m_emptyText->hide();
}


void TscoreNote::popUpAnimFinished() {
// 	delete m_emptyText;
	m_popUpAnim->deleteLater();
	m_popUpAnim = 0;
}



/*
void TscoreNote::checkOctavation() {
	bool notPossible = false;
			if (pos < m_ambitMax) {
				m_ottava = -1;
				pos += 7;
				if (pos < m_ambitMax) {
					m_ottava = -2;
					pos += 7;
					if (pos < m_ambitMax)
						notPossible = true;
				}
			} else {
				m_ottava = 1;
				pos -= 7;
				if (pos > m_ambitMin) {
					m_ottava = 2;
					pos -= 7;
					if (pos > m_ambitMin)
						notPossible = true;
				}
			}
			if (notPossible) {
				m_ottava = 0;
				m_mainPosY = 0;
				hideNote();
				return;
			}
			qDebug() << "octavation required" << (int)m_ottava;
} */




