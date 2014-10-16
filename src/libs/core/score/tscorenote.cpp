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

	createLines(m_mainDownLines, m_mainUpLines, m_mainMidLines);  
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
		setAmbitus(40, 1);
	else
		setAmbitus(34, 1);
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
}


TscoreNote::~TscoreNote() { // release work note and controls from destructing parent
	if (scoreScene()->right() &&
		(scoreScene()->workNote()->parentItem() == this || scoreScene()->right()->parentItem() == parentItem())) {
			scoreScene()->right()->setScoreNote(0);
			scoreScene()->left()->setScoreNote(0);
			setCursorParent(0);
	}
	delete m_note;
}

//##############################################################################
//#################### PUBLIC METHODS    #######################################
//##############################################################################

void TscoreNote::adjustSize() {
	m_height = staff()->height();
	createLines(m_mainDownLines, m_mainUpLines, m_mainMidLines);
	createLines(scoreScene()->downLines, scoreScene()->upLines, scoreScene()->midLines);
	setColor(m_mainColor);
}


void TscoreNote::setColor(QColor color) {
    m_mainColor = color;
    m_mainNote->setPen(Qt::NoPen);
    m_mainNote->setBrush(QBrush(m_mainColor, Qt::SolidPattern));
    m_mainAccid->setBrush(QBrush(m_mainColor));
    for (int i = 0; i < m_mainUpLines.size(); i++)
        m_mainUpLines[i]->setPen(QPen(color, 0.2));
    for (int i = 0; i < m_mainDownLines.size(); i++)
      m_mainDownLines[i]->setPen(QPen(color, 0.2));
		for (int i = 0; i < m_mainMidLines.size(); i++)
      m_mainMidLines[i]->setPen(QPen(color, 0.2));
    if (m_stringText)
        m_stringText->setBrush(QBrush(m_mainColor));
}


void TscoreNote::selectNote(bool sel) {
	m_selected = sel;
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
		checkLines(posY, m_mainDownLines, m_mainUpLines, m_mainMidLines);
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
}


void TscoreNote::hideNote() {
    m_mainNote->hide();
    m_mainAccid->hide();
    hideLines(m_mainUpLines);
    hideLines(m_mainDownLines);
		if (staff()->isPianoStaff())
			hideLines(m_mainMidLines);
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
    hideLines(scoreScene()->upLines);
    hideLines(scoreScene()->downLines);
		if (staff()->isPianoStaff())
			hideLines(scoreScene()->midLines);
    scoreScene()->setWorkPosY(0);
	}
}


void TscoreNote::markNote(QColor blurColor) {
	if (blurColor == -1) {
		m_mainNote->setPen(Qt::NoPen);
		m_mainNote->setGraphicsEffect(0);
	} else {
		m_mainNote->setPen(QPen(blurColor, 0.2));
		m_mainNote->setGraphicsEffect(new TdropShadowEffect(blurColor));
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
// 			qreal yy;
// 			if (notePos() < staff()->upperLinePos() + 4.0)
// 				yy = staff()->upperLinePos() + 7.5; // under staff
// 			else if (staff()->isPianoStaff() && notePos() > staff()->lowerLinePos()) // above lower staff
// 				yy = staff()->lowerLinePos() - m_nameText->boundingRect().height() * m_nameText->scale();
// 			else // above upper staff
// 				yy = staff()->upperLinePos() - m_nameText->boundingRect().height() * m_nameText->scale() + 1.0;
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
	m_ambitMin = qBound(1, lo, (int)m_height - 1); 
	m_ambitMax = qBound(1, hi, (int)m_height - 1);
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
		painter->setPen(QPen(emptyNoteColor, 0.6, Qt::SolidLine, Qt::RoundCap));
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
	m_popUpRect = new QGraphicsRectItem(0.0, 0.0, 0.1, 0.1 * (m_height / 7.0), this);
		m_popUpRect->setZValue(1);
		m_popUpRect->setPen(Qt::NoPen);
		m_popUpRect->setBrush(QBrush(qApp->palette().highlight().color()));
		m_popUpRect->setOpacity(0.2);
		m_popUpRect->setPos(3.45, m_height / 2.0 - 0.05 * (m_height / 7.0));
	m_popUpAnim = new TcombinedAnim(m_popUpRect);
		m_popUpAnim->setDuration(durTime);
		m_popUpAnim->setMoving(m_popUpRect->pos(), QPointF());
		m_popUpAnim->setScaling(70);
		m_popUpAnim->setFading(0.0, 0.6);
		connect(m_popUpAnim, SIGNAL(finished()), this, SLOT(popUpAnimFinished()));
		m_popUpAnim->startAnimations();
}



//#################################################################################################
//########################################## PROTECTED   ##########################################
//#################################################################################################

void TscoreNote::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverEnterEvent";
	if (staff()->controlledNotes()) {
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
  if ((event->pos().y() >= m_ambitMax) && (event->pos().y() <= m_ambitMin)) {
			scoreScene()->workNote()->show();
			scoreScene()->workAccid()->show();
  }
  emit statusTip(m_staticTip);
  TscoreItem::hoverEnterEvent(event);
}


void TscoreNote::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverLeaveEvent";
  hideWorkNote();
	if (staff()->controlledNotes()) {
			if (scoreScene()->right()->isEnabled() && !scoreScene()->right()->hasCursor())
					scoreScene()->right()->hideWithDelay();
			if (scoreScene()->left()->isEnabled() && !scoreScene()->left()->hasCursor())
					scoreScene()->left()->hideWithDelay();
	}
  TscoreItem::hoverLeaveEvent(event);
}


void TscoreNote::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
//   if ((event->pos().y() >= m_ambitMax) && (event->pos().y() <= m_ambitMin)) {
	if ((event->pos().y() >= 1) && (event->pos().y() <= m_height - 3.0)) {
		if (staff()->isPianoStaff() && 
			(event->pos().y() >= staff()->upperLinePos() + 10.6) && (event->pos().y() <= staff()->lowerLinePos() - 2.4)) {
				hideWorkNote();
				return;
		}
    if (event->pos().y() != scoreScene()->workPosY()) {
      scoreScene()->setWorkPosY(event->pos().y() - 0.6);
      scoreScene()->workNote()->setPos(3.0, scoreScene()->workPosY());
      if (!scoreScene()->workNote()->isVisible()) {
        scoreScene()->workNote()->show();
        scoreScene()->workAccid()->show();
      }
      checkLines(scoreScene()->workPosY(), scoreScene()->downLines, scoreScene()->upLines, scoreScene()->midLines);
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
				if (staff()->selectableNotes() || staff()->controlledNotes()) {
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

QGraphicsLineItem* TscoreNote::createNoteLine(int yPos) {
	QGraphicsLineItem *line = new QGraphicsLineItem();
	line->hide();
	registryItem(line);
	line->setZValue(7);
	line->setLine(2.5, yPos, boundingRect().width(), yPos);
	return line;
}


void TscoreNote::hideLines(TaddLines& linesList) {
	for (int i=0; i < linesList.size(); i++)
		linesList[i]->hide();
}


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
	createLines(scoreScene()->downLines, scoreScene()->upLines, scoreScene()->midLines);
  scoreScene()->initNoteCursor(this);
}


void TscoreNote::checkLines(int curPos, TaddLines& low, TaddLines& upp, TaddLines& mid) {
	for (int i = 0; i < upp.size(); i++) {
		if (curPos < upp[i]->line().y1())
			upp[i]->show();
		else 
			upp[i]->hide();
	}
	if (staff()->isPianoStaff()) {
		if (curPos == mid[0]->line().y1() - 1)
			mid[0]->show();
		else
			mid[0]->hide();
		if (curPos == mid[1]->line().y1() - 1)
			mid[1]->show();
		else
			mid[1]->hide();
	}
	for (int i = 0; i < low.size(); i++) {
		if (curPos > low[i]->line().y1() - 2) 
			low[i]->show();
		else 
			low[i]->hide();
	}
}


void TscoreNote::setCursorParent(TscoreItem* item) {
	scoreScene()->workNote()->setParentItem(item);
	for (int i = 0; i < scoreScene()->downLines.size(); i++)
		scoreScene()->downLines[i]->setParentItem(item);
	for (int i = 0; i < scoreScene()->midLines.size(); i++)
		scoreScene()->midLines[i]->setParentItem(item);
	for (int i = 0; i < scoreScene()->upLines.size(); i++)
		scoreScene()->upLines[i]->setParentItem(item);
}


void TscoreNote::createLines(TaddLines& low, TaddLines& upp, TaddLines& mid) {
	deleteLines(upp);
	deleteLines(mid);
	deleteLines(low);
  int i = staff()->upperLinePos() - 2;
  while (i > 0) { // upper lines
		upp << createNoteLine(i);
    i -= 2;
  }
  i = staff()->upperLinePos() + 10.0; // distance between upper and lower (or lower grand staff) staff line
  if (staff()->isPianoStaff()) {
		i = staff()->lowerLinePos() + 10.0;
		mid << createNoteLine(staff()->upperLinePos() + 10);
		mid << createNoteLine(staff()->lowerLinePos() - 2);
	}
  while (i < m_height - 2) {
		low << createNoteLine(i);
    i += 2;
  }
}


void TscoreNote::deleteLines(TaddLines& linesList) {
	for (int i = 0; i < linesList.size(); i++)
		delete linesList[i];
	linesList.clear();
}


void TscoreNote::popUpAnimFinished() {
	delete m_popUpRect;
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




