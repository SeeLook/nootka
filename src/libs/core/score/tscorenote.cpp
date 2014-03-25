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
#include <graphics/tdropshadoweffect.h>
#include <animations/tcrossfadetextanim.h>
#include <animations/tcombinedanim.h>
#include <QGraphicsEffect>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include <QTimer>
#include <qtimeline.h>

#include <QDebug>


/*static*/
QString TscoreNote::getAccid(int accNr) {
    const int accCharTable[6] = { 0xe123, 0xe11a, 0x20, 0xe10e, 0xe125, 0xe116 };
    QString str = QString(QChar(accCharTable[accNr+2]));
    return str;
}


QFont TscoreNote::getAccidFont() {
  QFont font(QFont("nootka"));
  font.setPixelSize(5);
  return font;
}

qreal TscoreNote::m_accidYoffset = 0.0;
qreal TscoreNote::m_accidScale = 1.0;

//################################## CONSTRUCTOR ###################################
TscoreNote::TscoreNote(TscoreScene* scene, TscoreStaff* staff, int index) :
  TscoreItem(scene),
  m_workPosY(0.0), m_mainPosY(0.0),
  m_curentAccid(0), m_accidental(0),
  m_index(index),
  m_readOnly(false),
  m_stringText(0), m_stringNr(0),
//   m_noteNr(100),
  m_ottava(0),
  m_bgColor(-1),
  m_noteAnim(0), m_accidToKeyAnim(false)
{
  setStaff(staff);
	setParentItem(staff);
  m_height = staff->height();
  m_workColor = qApp->palette().highlight().color();
  m_workColor.setAlpha(200);
  m_mainColor = qApp->palette().text().color();
  
  int i = staff->upperLinePos() - 2;
  while (i > 0) {
		m_mainUpLines << createNoteLine(i);
    m_upLines << createNoteLine(i);
    i -= 2;
  }
  i = staff->upperLinePos() + 10;
  while (i < m_height) {
		m_mainDownLines << createNoteLine(i);
    m_downLines << createNoteLine(i);    
    i += 2;
  }
  
  m_workNote = createNoteHead();
// 	m_workNote->setParentItem(0);
// 	m_workNote->setScale(5);
//   QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
//   blur->setBlurRadius(0.5);
  m_workNote->setGraphicsEffect(new TdropShadowEffect(m_workColor));
// 	m_workNote->setParentItem(this);
// 	m_workNote->setScale(1);
//   scoreScene()->addBlur(m_workNote, 3.0);
  
  m_mainNote = createNoteHead();
	
  m_mainAccid = new QGraphicsSimpleTextItem();
	m_mainAccid->setParentItem(m_mainNote);
	
  
//   QGraphicsBlurEffect *accidBlur = new QGraphicsBlurEffect;
//   accidBlur->setBlurRadius(blur->blurRadius());
  m_workAccid = new QGraphicsSimpleTextItem();
  m_workAccid->setBrush(QBrush(m_workColor));
//   m_workAccid->setGraphicsEffect(accidBlur);
	m_workAccid->setParentItem(m_workNote);
  m_workAccid->hide();
  
  QFont font(getAccidFont());
  m_workAccid->setFont(font);
  m_mainAccid->setFont(font);
  m_workAccid->hide();
  m_workAccid->setText(getAccid(1));
  m_accidScale = 6.0 / m_workAccid->boundingRect().height();
  m_workAccid->setScale(m_accidScale);
  m_mainAccid->setScale(m_accidScale);
	m_accidYoffset = m_workAccid->boundingRect().height() * m_accidScale * 0.34;
	m_workAccid->setPos(-3.0, -m_accidYoffset);
	m_mainAccid->setPos(-3.0, -m_accidYoffset);
	m_workAccid->setText(" ");
  
  setColor(m_mainColor);
  setPointedColor(m_workColor);
  m_workNote->setZValue(35);
  m_workAccid->setZValue(m_workNote->zValue());
  m_mainNote->setZValue(34); // under
  m_mainAccid->setZValue(m_mainNote->zValue());
  if (staff->kindOfStaff() == TscoreStaff::e_normal)
      setAmbitus(37, 1);
  else if (staff->kindOfStaff() == TscoreStaff::e_upper)
        setAmbitus(m_height - 2, 2);
	else if (staff->kindOfStaff() == TscoreStaff::e_lower)
				setAmbitus(m_height - 2, 1);
  
  setStatusTip(tr("Click to select a note, use mouse wheel to change accidentals."));
}


TscoreNote::~TscoreNote() {}

//##############################################################################
//#################### PUBLIC METHODS    #######################################
//##############################################################################

void TscoreNote::setColor(QColor color) {
    m_mainColor = color;
    m_mainNote->setPen(QPen(m_mainColor, 0.2));
// 		m_mainNote->setPen(Qt::NoPen);
    m_mainNote->setBrush(QBrush(m_mainColor, Qt::SolidPattern));
    m_mainAccid->setBrush(QBrush(m_mainColor));
    for (int i = 0; i < m_mainUpLines.size(); i++)
        m_mainUpLines[i]->setPen(QPen(color, 0.2));
    for (int i = 0; i < m_mainDownLines.size(); i++)
      m_mainDownLines[i]->setPen(QPen(color, 0.2));
    if (m_stringText)
        m_stringText->setBrush(QBrush(m_mainColor));
}


void TscoreNote::setPointedColor(QColor color) {
    m_workColor = color;
    m_workNote->setPen(QPen(m_workColor, 0.2));
// 		m_workNote->setPen(Qt::NoPen);
    m_workNote->setBrush(QBrush(m_workColor, Qt::SolidPattern));
    m_workAccid->setBrush(QBrush(m_workColor));
    for (int i = 0; i < m_upLines.size(); i++)
        m_upLines[i]->setPen(QPen(color, 0.2));
    for (int i = 0; i < m_downLines.size(); i++)
      m_downLines[i]->setPen(QPen(color, 0.2));
}


void TscoreNote::setWorkAccid(int accNr) {
		m_curentAccid = accNr;
		m_workAccid->setText(getAccid(accNr));
}


void TscoreNote::moveNote(int posY) {
		if (posY == 0 || !(posY >= m_ambitMax - 1 && posY <= m_ambitMin)) {
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
    int noteNr = (56 + staff()->notePosRelatedToClef(posY)) % 7;
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
    for (int i = 0; i < m_mainUpLines.size(); i++) {
      if (posY < m_mainUpLines[i]->line().y1())
        m_mainUpLines[i]->show();
      else 
        m_mainUpLines[i]->hide();
    }
    for (int i = 0; i < m_mainDownLines.size(); i++) {
      if (posY > m_mainDownLines[i]->line().y1() - 2)
        m_mainDownLines[i]->show();
      else 
        m_mainDownLines[i]->hide();
    }
}


void TscoreNote::setNote(int notePos, int accNr) {
	m_accidental = accNr;
	moveNote(notePos);
}


void TscoreNote::hideNote() {
    m_mainNote->hide();
    m_mainAccid->hide();
    hideLines(m_mainUpLines);
    hideLines(m_mainDownLines);
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
	if (m_workNote->isVisible()) {
    m_workNote->hide();
    m_workAccid->hide();
    hideLines(m_upLines);
    hideLines(m_downLines);
    m_workPosY = 0.0;
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
}


void TscoreNote::setString(int realNr) {
	if (!m_stringText) {
        m_stringText = new QGraphicsSimpleTextItem();
				m_stringText->setFont(QFont("nootka", 5, QFont::Normal));
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
			paintBackground(painter, m_bgColor);
	}
}


void TscoreNote::keyAnimFinished() {
	if (!m_readOnly)
			m_mainAccid->show();
}


//#################################################################################################
//########################################## PROTECTED   ##########################################
//#################################################################################################

void TscoreNote::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverEnterEvent";
  if ((event->pos().y() >= m_ambitMax) && (event->pos().y() <= m_ambitMin)) {
      m_workNote->show();
      m_workAccid->show();
  }
  TscoreItem::hoverEnterEvent(event);
}


void TscoreNote::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverLeaveEvent";
  hideWorkNote();
  TscoreItem::hoverLeaveEvent(event);
}


void TscoreNote::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
// 	qDebug() << "hoverMoveEvent" << event->pos();
  if ((event->pos().y() >= m_ambitMax) && (event->pos().y() <= m_ambitMin)) {
    if (event->pos().y() != m_workPosY) {
			if (m_curentAccid != scoreScene()->currentAccid()) { // update accidental symbol
					m_curentAccid = scoreScene()->currentAccid();
					m_workAccid->setText(getAccid(m_curentAccid));
			}
      m_workPosY = event->pos().y() - 0.6;
      m_workNote->setPos(3.0, m_workPosY);
      if (!m_workNote->isVisible()) {
        m_workNote->show();
        m_workAccid->show();
      }
      for (int i = 0; i < m_upLines.size(); i++) {
        if (m_workPosY < m_upLines[i]->line().y1())
          m_upLines[i]->show();
        else 
          m_upLines[i]->hide();
      }
      for (int i = 0; i < m_downLines.size(); i++) {
        if (m_workPosY > m_downLines[i]->line().y1() - 2) 
          m_downLines[i]->show();
        else 
          m_downLines[i]->hide();
      }
    }
  }
}


void TscoreNote::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (event->button() == Qt::LeftButton && m_workPosY) {
        m_accidental = m_curentAccid;
        moveNote(m_workPosY);
        emit noteWasClicked(m_index);
    }
}


void TscoreNote::wheelEvent(QGraphicsSceneWheelEvent* event) {
		if (m_readOnly)
			return;
    int prevAcc = m_curentAccid;
    if (event->delta() < 0) {
        m_curentAccid--;
        if (m_curentAccid < (-scoreScene()->doubleAccidsFuse()))
          m_curentAccid = -(scoreScene()->doubleAccidsFuse());
    } else {
        m_curentAccid++;
        if (m_curentAccid > scoreScene()->doubleAccidsFuse())
          m_curentAccid = scoreScene()->doubleAccidsFuse();
    }
    if (prevAcc != m_curentAccid) {
				setWorkAccid(m_curentAccid);
        scoreScene()->setCurrentAccid(m_curentAccid);
        emit accidWasChanged(m_curentAccid);
    }
}

//##########################################################################################################
//####################################         PRIVATE     #################################################
//##########################################################################################################

QGraphicsEllipseItem* TscoreNote::createNoteHead() {
  QGraphicsEllipseItem *noteHead = new QGraphicsEllipseItem();
//   registryItem(noteHead);
	noteHead->setParentItem(this);
  noteHead->setRect(0, 0, 3.5, 2);
  noteHead->hide();
  return noteHead;
}


QGraphicsLineItem* TscoreNote::createNoteLine(int yPos) {
    QGraphicsLineItem *line = new QGraphicsLineItem();
    line->hide();
    registryItem(line);
    line->setLine(2.5, yPos, boundingRect().width(), yPos);
    return line;
}


void TscoreNote::hideLines(QList< QGraphicsLineItem* >& linesList) {
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




