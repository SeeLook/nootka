/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#include "tfingerboard.h"
#include "tglobals.h"
#include "ttune.h"
#include <tgraphicstexttip.h>
#include <animations/tstrikedoutitem.h>
#include <animations/tcombinedanim.h>
#include <QtGui>
#include <QDebug>

extern Tglobals *gl;


TfingerBoard::TfingerBoard(QWidget *parent) :
    QGraphicsView(parent),
    m_isCursorOverGuitar(false),
    m_movingItem(0),
    m_noteName(0),
    m_corrStyle(Tnote::defaultStyle)
{
    if (gl->GfingerColor == -1) {
        gl->GfingerColor = gl->invertColor(palette().highlight().color());
        gl->GfingerColor.setAlpha(200);
    }
    if (gl->GselectedColor == -1) {
        gl->GselectedColor = palette().highlight().color();
    }

    m_scene = new QGraphicsScene(this);

    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setScene(m_scene);
    setMouseTracking(true);
    setStatusTip(tr("Select a string or fret and click to see it on the staff."));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGraphicsBlurEffect *blur[6];
    for (int i = 0; i < 6; i++) {
        m_strings[i] = new QGraphicsLineItem();
        m_strings[i]->hide();
        m_scene->addItem(m_strings[i]);
        m_strings[i]->setZValue(50);

        m_fingers[i] = new QGraphicsEllipseItem();
        m_fingers[i]->hide();
        m_fingers[i]->setPen(QPen(gl->GselectedColor));
        m_fingers[i]->setBrush(QBrush(gl->GselectedColor, Qt::SolidPattern));
        m_scene->addItem(m_fingers[i]);
        m_fingers[i]->setZValue(50);

        m_workStrings[i] = new QGraphicsLineItem();
        m_workStrings[i]->hide();
        m_scene->addItem(m_workStrings[i]);
        m_workStrings[i]->setZValue(112);
        blur[i] = new QGraphicsBlurEffect();
        blur[i]->setBlurRadius(3);
        m_workStrings[i]->setGraphicsEffect(blur[i]);
    }

    m_workFinger = new QGraphicsEllipseItem();
    m_workFinger->hide();
    QGraphicsBlurEffect *workBlur = new QGraphicsBlurEffect();
    workBlur->setBlurRadius(3);
    m_workFinger->setPen(QPen(gl->GfingerColor, 2));
    m_workFinger->setBrush(QBrush(gl->GfingerColor, Qt::SolidPattern));
    m_workFinger->setGraphicsEffect(workBlur);
    m_scene->addItem(m_workFinger);
    m_workFinger->setZValue(112);

    m_curStr = 7; // none
    m_curFret = 99; // none
    m_selNote = Tnote(0,0,0);
    m_questFinger = 0;
    m_questString = 0;
    m_questMark = 0;
    m_rangeBox1 = 0;
    m_rangeBox2 = 0;
    m_highString = 0;
    m_isDisabled = false;
		m_beyondTip = 0;
		m_fingerPos.setPos(1, 30);
		
		setTune();
		
}

//################################################################################################
//################################################   PUBLIC    ###################################
//################################################################################################

void TfingerBoard::acceptSettings() {
    for (int i = 0; i < 6; i++) {
        m_fingers[i]->setPen(QPen(gl->GselectedColor));
        m_fingers[i]->setBrush(QBrush(gl->GselectedColor, Qt::SolidPattern));
    }
    m_workFinger->setBrush(QBrush(gl->GfingerColor, Qt::SolidPattern));
		m_workFinger->setPen(QPen(gl->GfingerColor, 2));
		setTune();
// 		paint();
		resizeEvent(0);
    setFinger(m_selNote);
}


void TfingerBoard::setFinger(Tnote note) {
    if (note.note) {
				short noteNr = note.getChromaticNrOfNote();
        bool doShow = true;
				bool foundPos = false;
        for(int i = 0; i < gl->Gtune()->stringNr(); i++) { // looking for pos to show
            int diff = noteNr - gl->Gtune()->str(gl->strOrder(i) + 1).getChromaticNrOfNote();
            if (doShow && diff >= 0 && diff <= gl->GfretsNumber) { // found
								foundPos = true;
// 								deleteBeyondTip();
                if (diff == 0) { // open string
                    m_fingers[gl->strOrder(i)]->hide();
                    m_strings[gl->strOrder(i)]->show();
                } else { // some fret
                    m_strings[gl->strOrder(i)]->hide();
                    paintFinger(m_fingers[gl->strOrder(i)], gl->strOrder(i), diff);
										m_fingerPos.setPos(gl->strOrder(i) + i, diff);
                    m_fingers[gl->strOrder(i)]->show();
                }
                if (!gl->GshowOtherPos) {
                    doShow = false;
                }
            } else { // not found on this string or no need to show
                m_fingers[gl->strOrder(i)]->hide();
                m_strings[gl->strOrder(i)]->hide();
            }
        }
        if (foundPos)
					deleteBeyondTip();
				else if (!m_beyondTip) {
						m_beyondTip = new TgraphicsTextTip(QString("<span style=\"font-size: %1px; color: %2;\"><br><b> ").
														arg(height() / 7).arg(gl->EquestionColor.name()) +
														tr("This note is beyond the scale of the guitar!") + " </b></span><br>", palette().text().color());
						if (!gl->GisRightHanded) {
							QTransform trans;
							trans.translate(width() / 2, 0);
							trans.scale(-1, 1);
							m_beyondTip->setTransform(trans);
						}
						m_scene->addItem(m_beyondTip);
						m_beyondTip->setZValue(150);
						m_beyondTip->setPos((m_scene->width() - m_beyondTip->boundingRect().width()) / 2,
												(m_scene->height() - m_beyondTip->boundingRect().height()) / 2);
				}
    } else { // hide highlighted fingers/string if no note
        for (int i = 0; i < gl->Gtune()->stringNr(); i++) {
            m_fingers[i]->hide();
            m_strings[i]->hide();
        }
    }
    m_selNote = note;
}


void TfingerBoard::setFinger(TfingerPos pos) {
    for(int i = 0; i < gl->Gtune()->stringNr(); i++) {
        if (i != pos.str() - 1) { //hide
            m_fingers[i]->hide();
            m_strings[i]->hide();
        }
        else { //show
            if (pos.fret()) { // some fret
                paintFinger(m_fingers[i], i, pos.fret());
                m_fingers[i]->show();
            }
            else { // open string
                m_fingers[i]->hide();
                m_strings[i]->show();
            }
            m_selNote = posToNote(pos.str() - 1, pos.fret());
        }
    }
    m_fingerPos = pos;
}


void TfingerBoard::clearFingerBoard() {
  // Clear marks
    if (m_curFret != 99) {
      if (m_curFret) {
        m_fingers[gl->strOrder(m_curStr)]->setPen(Qt::NoPen);
        m_fingers[gl->strOrder(m_curStr)]->setGraphicsEffect(0);
      }
      else
        if (m_curStr != 7) {
          m_strings[gl->strOrder(m_curStr)]->setPen(QPen(gl->GselectedColor, m_strWidth[m_curStr]));
          m_strings[gl->strOrder(m_curStr)]->setGraphicsEffect(0);
        }
    } // done
    if (m_questFinger) {
        delete m_questFinger;
        m_questFinger = 0;
    }
    if (m_questString) {
        delete m_questString;
        m_questString = 0;
    }
    if (m_questMark) {
        delete m_questMark;
        m_questMark = 0;
    }
    if (m_rangeBox1)
        m_rangeBox1->hide();
    if (m_rangeBox2)
        m_rangeBox2->hide();
    setFinger(Tnote(0,0,0));
		m_fingerPos.setPos(6, 39);
    clearHighLight();
		deleteBeyondTip();
		deleteNoteName();
		m_nameInCorrection = false;
}


void TfingerBoard::setGuitarDisabled(bool disabled) {
  if (disabled) {
    setMouseTracking(false);
    m_isDisabled = true;
    m_workFinger->hide();
    for(int i = 0; i < gl->Gtune()->stringNr(); i++)
      m_workStrings[i]->hide();
  } else {
    setMouseTracking(true);
    m_isDisabled = false;
    if (isCursorOverGuitar()) {
      m_curStr = 7;
      m_curFret = 99;
      paintFingerAtPoint(mapFromGlobal(cursor().pos()));
    }
  }
}


void TfingerBoard::clearHighLight() {
  if (m_highString) {
      delete m_highString;
      m_highString = 0;
  }
}

int TfingerBoard::posX12fret() {
     return m_fretsPos[qMin(11, 12 - (19 - gl->GfretsNumber))];
}

//################################################################################################
//######################### EXAM RELATED #########################################################
//################################################################################################

void TfingerBoard::askQuestion(TfingerPos pos) {
    m_questPos = pos;
    QColor qC = gl->EquestionColor;
    qC.setAlpha(200); //it is too much opaque
    if (pos.fret()) { // some fret
        if (!m_questFinger) {
            m_questFinger = new QGraphicsEllipseItem();
            m_questFinger->setPen(QPen(qC, 2));
            m_questFinger->setBrush(QBrush(qC, Qt::SolidPattern));
            m_scene->addItem(m_questFinger);
            m_questFinger->setZValue(110);
            m_questFinger->setRect(0, 0, m_fretWidth / 1.6, qRound(0.7 * m_strGap));
            paintFinger(m_questFinger, pos.str() - 1, pos.fret());
        }
    } else { // open string
        if (!m_questString) {
            m_questString = new QGraphicsLineItem();
            m_questString->setPen(QPen(qC, m_strings[pos.str() - 1]->pen().width() + 2, Qt::SolidLine));
            m_scene->addItem(m_questString);
            m_questString->setZValue(110);
            m_questString->setLine(m_strings[pos.str() - 1]->line());
        }
    }
    paintQuestMark();
}


void TfingerBoard::markAnswer(QColor blurColor) {
  if (m_fingerPos.fret() != 39 && m_fingerPos.str() != 7) {
    if (m_fingerPos.fret()) {
			m_fingers[m_fingerPos.str() - 1]->setPen(QPen(QColor(blurColor.name()), 3));
// 			m_fingers[m_curStr]->setGraphicsEffect(new QGraphicsBlurEffect());
      m_fingers[gl->strOrder(m_curStr)]->setGraphicsEffect(new QGraphicsBlurEffect());
    }
    else
      if (m_fingerPos.str() != 7) {
				m_strings[m_fingerPos.str() - 1]->setPen(QPen(QColor(blurColor.name()), 5));
// 				QGraphicsLineItem *strHighLight = new QGraphicsLineItem;
//         m_strings[gl->strOrder(m_curStr)]->setPen(QPen(blurColor, 5));
//         m_strings[gl->strOrder(m_curStr)]->setGraphicsEffect(new QGraphicsBlurEffect());
      }
  }
}


void TfingerBoard::markQuestion(QColor blurColor) {
    if (m_questFinger)
      m_questFinger->setPen(QPen(blurColor, 3));
    if (m_questString)
      m_questString->setPen(QPen(QColor(blurColor.name()), m_questString->pen().width()));
}


void TfingerBoard::showName(Tnote::EnameStyle st, Tnote& note, const QColor& textColor) {
	m_nameInCorrection = false;
	m_corrStyle = st;
	QGraphicsEllipseItem *qFinger = 0;
	QGraphicsLineItem *qString = 0;
	if (note.note && (m_questFinger || m_questString)) { // Question was on the guitar
			m_noteName = new TgraphicsTextTip(note.toRichText(st));
			if (m_questFinger)
				qFinger = m_questFinger;
			else
				qString = m_questString;
	} else { // try to take note from an answer
			if (m_fingerPos.fret() != 39 && m_fingerPos.str() != 7 && m_selNote.note) {
					m_noteName = new TgraphicsTextTip(m_selNote.toRichText(st));
					if (m_fingerPos.fret())
						qFinger = m_fingers[m_fingerPos.str() - 1];
					else
						if (m_fingerPos.str() != 7)
							qString = m_strings[m_fingerPos.str() - 1];
			}
	}
	if (!qFinger && !qString)
			return;
	m_noteName->setZValue(200);
	m_noteName->setScale((fbRect().height() / 2.2) / m_noteName->boundingRect().height());
	m_noteName->setDefaultTextColor(QColor(textColor.lighter().name()));
	scene()->addItem(m_noteName);
	if (!gl->GisRightHanded) {
		QTransform trans;
		trans.scale(-1.0, 1.0);
		m_noteName->setTransform(trans, true);
	}
	QPointF tPos;
	qreal yPos, nameScale = m_noteName->transform().m11() * m_noteName->scale();
	if (qFinger) {
		tPos.setX(qFinger->pos().x() + (qFinger->boundingRect().width() - m_noteName->boundingRect().width() * nameScale) /  2);
		yPos = qFinger->pos().y();
	} else if (qString) {
			m_noteName->setScale(m_noteName->scale() * 1.5);
			tPos.setX((width() - m_noteName->boundingRect().width()) / 2);
			yPos = qString->line().p1().y();
	}
	if (yPos < height() / 2.0) {
				if (qFinger)
					yPos += qFinger->boundingRect().height();
				else
					yPos += qString->pen().widthF();
        tPos.setY(yPos);
	} else
				tPos.setY(yPos - m_noteName->boundingRect().height() * m_noteName->scale());
	m_noteName->setPos(tPos);
	m_nameInCorrection = true;
}


void TfingerBoard::deleteNoteName() {
	if (m_noteName) {
		delete m_noteName;
		m_noteName = 0;
	}
}


void TfingerBoard::createRangeBox(char loFret, char hiFret) {
    m_loFret = loFret;
    m_hiFret = hiFret;
    if (!m_rangeBox1) {
        m_rangeBox1 = new QGraphicsRectItem();
        m_rangeBox1->setGraphicsEffect(new QGraphicsBlurEffect());
        m_scene->addItem(m_rangeBox1);
        m_rangeBox1->setBrush(QBrush(Qt::NoBrush));
    }
    if (!m_rangeBox2 && m_loFret == 0 && m_hiFret > 0 && m_hiFret < gl->GfretsNumber) {
        m_rangeBox2 = new QGraphicsRectItem();
        m_rangeBox2->setGraphicsEffect(new QGraphicsBlurEffect());
        m_scene->addItem(m_rangeBox2);
        m_rangeBox2->setBrush(QBrush(Qt::NoBrush));
    }
    resizeRangeBox();
}


void TfingerBoard::prepareAnswer() {
    setAttribute(Qt::WA_TransparentForMouseEvents, false); // unlock guitar for mouse
    if (m_rangeBox1)
        m_rangeBox1->show();
    if (m_rangeBox2)
        m_rangeBox2->show();
}


void TfingerBoard::deleteRangeBox() {
    if (m_rangeBox1) {
        delete m_rangeBox1;
        m_rangeBox1 = 0;
    }
    if (m_rangeBox2) {
        delete m_rangeBox2;
        m_rangeBox2 = 0;
    }
}


void TfingerBoard::setHighlitedString(char realStrNr) {
  if (!m_highString) {
      m_highString = new QGraphicsLineItem();
      m_scene->addItem(m_highString);
  }
  m_hilightedStrNr = realStrNr;
  m_highString->setZValue(40);
  m_highString->setPen(QPen(QColor(gl->EanswerColor.name()), m_strWidth[realStrNr - 1] + 2, Qt::SolidLine));
	m_highString->setGraphicsEffect(new QGraphicsBlurEffect());
  m_highString->setLine(m_strings[realStrNr - 1]->line());
}


void TfingerBoard::correctPosition(TfingerPos& pos, const QColor color) {
	m_goodPos = pos;
	if (m_fingerPos.fret() != 39 && m_fingerPos.str() != 7) {
    if (m_fingerPos.fret()) {
				m_strikeOut = new TstrikedOutItem(m_fingers[gl->strOrder(m_fingerPos.str() - 1)]);
    } else {
				m_strikeOut = new TstrikedOutItem(m_strings[gl->strOrder(m_fingerPos.str() - 1)]);
		} 
	} else {
			QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem();
			ellipse->setPen(Qt::NoPen);
			ellipse->setBrush(gl->GselectedColor);
// 			TfingerPos ff(3, 12);
// 			ellipse->setPos(fretToPos(ff) + QPointF(0, m_strGap / 2));
			ellipse->setPos(width() - ellipse->boundingRect().width(), 1.0); // right top corner
			m_scene->addItem(ellipse);
			ellipse->setRect(fingerRect());
			m_movingItem = (QGraphicsItem*)ellipse;
// 			m_strikeOut = new TstrikedOutItem(ellipse);
			m_strikeOut = new TstrikedOutItem(m_rangeBox1);
	}
	QPen pp(QColor(color.name()), m_strWidth[3]);
	m_strikeOut->setPen(pp);
	connect(m_strikeOut, SIGNAL(strikedFInished()), this, SLOT(strikeBlinkingFinished()));
	m_strikeOut->startBlinking();
}


QPointF TfingerBoard::fretToPos(TfingerPos& pos) {
  qreal xPos = fbRect().x();
  if (pos.fret())
			xPos = m_fretsPos[pos.fret() - 1] - qRound(m_fretWidth / 1.5);
	return QPointF(xPos, m_fbRect.y() + m_strGap * (pos.str() - 1) + m_strGap / 5);
}


QRectF TfingerBoard::fingerRect() const {
	return m_workFinger->rect();
}


//################################################################################################
//################################################ PROTECTED #####################################
//################################################################################################

void TfingerBoard::setTune() {
	for (int i = 0; i < gl->Gtune()->stringNr(); i++) {
		if (gl->Gtune()->str(i + 1.5).getChromaticNrOfNote() > 14) { // highest than cis1
			m_strColors[i] = QColor(255, 255, 255, 175); // are nylon
			m_widthFromPitch[i] = 2; // and thiner
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > 10) { // highest than gis
				m_strColors[i] = QColor(255, 255, 255, 175); // are nylon
				m_widthFromPitch[i] = 2.5; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > 4) { // highest than dis
				m_strColors[i] = QColor(255, 255, 255, 125); // are nylon
				m_widthFromPitch[i] = 3; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > 0) { // highest than b-1(contra)
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 3; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > -5) { // highest than g-1(contra) 1-st string of bass
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 3.5; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > -10) { // highest than d-1(contra)
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 4; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > -15) { // highest than gis-2(subcontra)
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 4.5; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > -20) { // highest than dis-1(contra)
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 5; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > -25) { // highest than
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 6; // and more thick
		} else if (gl->Gtune()->str(i + 1).getChromaticNrOfNote() > -30) { // highest than
				m_strColors[i] = QColor("#C29432"); // are gold-plated
				m_widthFromPitch[i] = 7; // and more thick
		}
	}
	m_loNote = gl->loString().getChromaticNrOfNote();
	m_hiNote = gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber;
}


void TfingerBoard::paint() {
    m_fbRect = QRect(10, height() / 18, (6 * width()) / 7, height() - height() / 18);
    m_fretWidth = ((m_fbRect.width() + ((gl->GfretsNumber / 2)*(gl->GfretsNumber / 2 + 1))
                  + gl->GfretsNumber / 4) / (gl->GfretsNumber+1)) + 1;
//     m_strGap = m_fbRect.height() / 6;
		m_strGap = m_fbRect.height() / gl->Gtune()->stringNr();
    m_fretsPos[0] = m_fbRect.x() + m_fretWidth;
    for (int i = 2; i < gl->GfretsNumber + 1; i++)
        m_fretsPos[i - 1] = m_fretsPos[i - 2] + (m_fretWidth-(i / 2));
    lastFret = m_fretsPos[gl->GfretsNumber - 1];
    if (lastFret > (m_fbRect.width() + 10)) {
        m_fbRect.setWidth(lastFret - 8);
        qDebug("fretboard size changed");
    }
  // Let's paint
    QPixmap pixmap(size());
    pixmap.fill(QColor(palette().window().color()));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setWindow(0 , 0, width(), height());
    resetTransform();
    if (!gl->GisRightHanded) {
        translate(width(), 0);
        scale(-1, 1);
    }
  // FINGERBOARD
    painter.setPen(QPen(Qt::black, 0, Qt::NoPen));
		if (gl->instrument == e_classicalGuitar)
				painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
		else {
				QColor fbEdgeColor = QColor("#AFA072");
				fbEdgeColor.setAlpha(220);
				painter.setBrush(QBrush(fbEdgeColor, Qt::SolidPattern));
		}
    QPolygon a;
		int fbThick = ((m_strGap * gl->Gtune()->stringNr()) / 6) / 4;
    a.setPoints(6, m_fbRect.x() - 6, m_fbRect.y(),
								m_fbRect.x() + 2, m_fbRect.y() - fbThick,
                m_fbRect.x() + m_fbRect.width() + fbThick, m_fbRect.y() - fbThick,
                m_fbRect.x() + m_fbRect.width() + fbThick, height() - fbThick,
                m_fbRect.x() + m_fbRect.width(), height(),
                m_fbRect.x(), height());
    painter.drawPolygon(a);
		if (gl->instrument == e_classicalGuitar)
				painter.setBrush(QBrush(QPixmap(gl->path + "picts/fingbg.png")));
		else
				painter.setBrush(QBrush(QPixmap(gl->path + "picts/fingbg-el.png")));
    painter.drawRect(m_fbRect);
  // FRETS
   // zero fret (upper bridge or HUESO)
		if (gl->instrument == e_classicalGuitar) {
				painter.setPen(Qt::NoPen);
				painter.setBrush(QBrush(QColor("#FFFBF0"),Qt::SolidPattern)); //#FFFBF0 cream color for hueso
				painter.drawRect(m_fbRect.x() - 8, m_fbRect.y() + 4, 7, m_fbRect.height());
				a.setPoints(4, m_fbRect.x() - 8,							m_fbRect.y() + 4,
										m_fbRect.x() - 1, 								m_fbRect.y() + 4,
										m_fbRect.x() + m_strGap / 3 - 1,	m_fbRect.y() - m_strGap / 3,
										m_fbRect.x() + m_strGap / 3 - 8,	m_fbRect.y() - m_strGap / 3);
				painter.setBrush(QBrush(QColor("#FFEEBC"),Qt::SolidPattern)); // a bit darker for its rant
				painter.drawPolygon(a);
		} else {
				QLinearGradient fretGrad(fbRect().x() - 8, 10.0, fbRect().x() + 6, 10.0);
        fretGrad.setColorAt(0.0, QColor("#DAE4E4"));
        fretGrad.setColorAt(0.4, QColor("#7F806E"));
        fretGrad.setColorAt(0.7, QColor("#3B382B"));
        fretGrad.setColorAt(0.9, QColor("#000000"));
        painter.setBrush(fretGrad);
        painter.drawRoundedRect(fbRect().x() - 8, m_fbRect.y() + 2, 14, m_fbRect.height() - 4, 2, 2);
		}
 // others frets
    for (int i = 0; i < gl->GfretsNumber; i++) {
        QLinearGradient fretGrad(m_fretsPos[i], 10.0, m_fretsPos[i] + 8, 10.0);
        fretGrad.setColorAt(0.0, QColor("#DAE4E4"));
        fretGrad.setColorAt(0.4, QColor("#7F806E"));
        fretGrad.setColorAt(0.7, QColor("#3B382B"));
        fretGrad.setColorAt(0.9, QColor("#000000"));
        painter.setBrush(fretGrad);
        painter.drawRoundedRect(m_fretsPos[i], m_fbRect.y() + 2, 9, m_fbRect.height() - 4, 2, 2);
        if ( (gl->instrument == e_bassGuitar && i == 2) || i==4 || i==6 || i==8 || i==14 || i==16)	{
                //white color for circles marking 5,7,9... frets
            painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
            painter.drawEllipse(m_fretsPos[i] - 4 - (m_fretsPos[i] - m_fretsPos[i - 1]) / 2,
                                m_fbRect.y() + m_strGap * (int)(gl->Gtune()->stringNr() / 2) - 2, 8, 8);
        } else if (i==11)	{
            painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
            painter.drawEllipse(m_fretsPos[i] - 12 - (m_fretsPos[i] - m_fretsPos[i - 1]) / 2,
                                m_fbRect.y() + m_strGap * (int)(gl->Gtune()->stringNr() / 2) - 2, 8, 8);
						painter.drawEllipse(m_fretsPos[i] + 4 - (m_fretsPos[i] - m_fretsPos[i - 1]) / 2,
                                m_fbRect.y() + m_strGap * (int)(gl->Gtune()->stringNr() / 2) - 2, 8, 8);
        }
    }
  // STRINGS
    QFont strFont = font();
    strFont.setPixelSize((int)qRound(0.75 * m_strGap));//setting font for digits
    painter.setFont(strFont);
    painter.setBrush(QBrush(Qt::NoBrush));
    for (int i = 0; i < 6; i++) {
//  drawing main strings
			qreal lineYpos = m_fbRect.y() + m_strGap / 2 + i * m_strGap;
			QLinearGradient strGrad(1.0, lineYpos - m_strWidth[i] / 2, 1.0, lineYpos + m_strWidth[i] / 2);
			strGrad.setColorAt(0.0, m_strColors[i]);
			strGrad.setColorAt(0.5, m_strColors[i].darker());
			painter.setPen(QPen(QBrush(strGrad), m_strWidth[i], Qt::SolidLine));
			painter.drawLine(1, lineYpos, width() - 1 - m_strGap, lineYpos);
			if (i < gl->Gtune()->stringNr()) {
					m_workStrings[i]->setPen(QPen(gl->GfingerColor, m_strWidth[i] + 2, Qt::SolidLine));
					m_workStrings[i]->setLine(1, lineYpos, width() - 1 - m_strGap, lineYpos);
					m_strings[i]->setPen(QPen(gl->GselectedColor, m_strWidth[i], Qt::SolidLine));
					m_strings[i]->setLine(m_workStrings[i]->line());
					m_strings[i]->show();
// drawing digits of strings in circles
					painter.setPen(QPen(m_strColors[i], 1, Qt::SolidLine));
					painter.setBrush(QBrush(QColor(0, 0, 0, 180)));
					int circleX;
					if (!gl->GisRightHanded) {
							painter.scale (-1, 1);
							painter.translate(-width(), 0);
							circleX = 1;
					} else
							circleX = width() - 1 - m_strGap;
					painter.drawEllipse(circleX, m_fbRect.y() + i * m_strGap, m_strGap - 1, m_strGap - 1);
					painter.setPen(QPen(Qt::green,1,Qt::SolidLine));// in green color
					if (gl->GisRightHanded)
							painter.drawText(width() - ((int)qreal(m_strGap * 0.75)),
															m_fbRect.y() + (int)qreal(m_strGap * (0.75 + i)), QString::number(i + 1));
						else {
								painter.drawText((int)qreal(m_strGap * 0.28),
																m_fbRect.y()+(int)qreal(m_strGap * (0.75 + i)), QString::number(i + 1));
								painter.translate(width(), 0);
								painter.scale (-1, 1);
						}
		// shadow of the strings
						painter.setPen(QPen(QColor(0, 0, 0, 150), m_strWidth[i], Qt::SolidLine)); // on the fingerboard
// 							int yy = m_fbRect.y() + m_strGap / 2 + (i - 1) * m_strGap - 3 - m_strWidth[i];
						int yy = lineYpos + m_strWidth[i] * 1.3;
						painter.drawLine(m_fbRect.x() + 3, yy, m_fbRect.x() + m_fbRect.width() - 1, yy);
						painter.setPen(QPen(Qt::black, 1, Qt::SolidLine)); //on upper bridge
						painter.drawLine(m_fbRect.x() - 8, lineYpos - 2,
														m_fbRect.x(), lineYpos - 2);
						painter.drawLine(m_fbRect.x() - 8, lineYpos + m_strWidth[i] - 1,
														m_fbRect.x() - 1, lineYpos + m_strWidth[i] - 1);

			} else { // hide rest lines (strings)
					m_workStrings[i]->hide();
					m_strings[i]->hide();
			}
		}
    m_workFinger->setRect(0, 0, m_fretWidth / 1.6, qRound(0.7 * m_strGap));
    for (int i = 0; i < 6; i++) {
			if (i < gl->Gtune()->stringNr()) {
        m_fingers[i]->setRect(m_workFinger->rect());
				m_fingers[i]->show();
			}
			else
				m_fingers[i]->hide();
		}

    if (m_questFinger) {
        m_questFinger->setRect(m_workFinger->rect());
        paintFinger(m_questFinger, m_questPos.str() - 1, m_questPos.fret());
    }
    if (m_questString)
        m_questString->setLine(m_strings[m_questPos.str() - 1]->line());
    if (m_questMark) paintQuestMark();
    resizeRangeBox();
    m_scene->setBackgroundBrush(QBrush(pixmap));
    setFinger(m_selNote);
    if (m_highString)
      setHighlitedString(m_hilightedStrNr);
}


Tnote TfingerBoard::posToNote(int str, int fret) {
    return Tnote(gl->Gtune()->str(str + 1).getChromaticNrOfNote() + fret);
}


//################################################################################################
//################################################   EVENTS    ###################################
//################################################################################################

void TfingerBoard::resizeEvent(QResizeEvent *){
    m_scene->setSceneRect(0, 0, width(), height());
    qreal wFactor = height() / 150.0;
    m_strWidth[0] = m_widthFromPitch[0] * wFactor;
    m_strWidth[1] = m_widthFromPitch[1] * wFactor;
    m_strWidth[2] = m_widthFromPitch[2] * wFactor;
    m_strWidth[3] = m_widthFromPitch[3] * wFactor;
    m_strWidth[4] = m_widthFromPitch[4] * wFactor;
    m_strWidth[5] = m_widthFromPitch[5] * wFactor;
    paint();
}


bool TfingerBoard::event(QEvent *event) {
    if (event->type() == QEvent::Leave) {
        if (m_curStr != 7) m_workStrings[m_curStr]->hide();
        m_curStr = 7;
        m_workFinger->hide();
        m_curFret = 99;
        m_isCursorOverGuitar = false;
    }
    if (event->type() == QEvent::Enter) {
        m_isCursorOverGuitar = true;
				deleteBeyondTip();
		}
    return QGraphicsView::event(event);
}


void TfingerBoard::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDisabled)
        return;
    paintFingerAtPoint(event->pos());
}


void TfingerBoard::mousePressEvent(QMouseEvent *event) {
	if (!m_isDisabled && event->button() == Qt::LeftButton) {
		if (m_curFret != 99 && m_curStr != 7) {
			m_selNote = posToNote(m_curStr, m_curFret);
			m_fingerPos = TfingerPos(m_curStr + 1, m_curFret);
			if (gl->GpreferFlats)
				if (m_selNote.note != 3 && m_selNote.note != 7) // eliminate Fb from E and Cb from B
					m_selNote = m_selNote.showWithFlat();
			if (gl->GshowOtherPos)
				setFinger(m_selNote);
			else
				setFinger(TfingerPos(m_curStr + 1, m_curFret));
			emit guitarClicked(m_selNote);
		} else {
			m_selNote = Tnote(0,0,0);
	//        setFinger(m_selNote);
		}
	}
}

//################################################################################################
//################################################   PRIVATE    ##################################
//################################################################################################

void TfingerBoard::paintFinger(QGraphicsEllipseItem *f, char strNr, char fretNr) {
		TfingerPos pp(strNr + 1, fretNr);
		f->setPos(fretToPos(pp));
}


void TfingerBoard::paintQuestMark() {
    if (!m_questMark) {
        QColor qC = gl->EquestionColor;
        qC.setAlpha(200); //it is too much opaque
        m_questMark = new QGraphicsSimpleTextItem();
        m_questMark->setBrush(QBrush(qC));
        m_scene->addItem(m_questMark);
        m_questMark->setZValue(110);
        m_questMark->setText("?");
    }
    QFont f = QFont("nootka");
    f.setPointSizeF(3 * m_strGap);
    QFontMetricsF fMetr(f);
    f.setPointSizeF(f.pointSizeF() * ((qreal)(fbRect().height() / 2.0f) / fMetr.boundingRect("?").height()));
    m_questMark->setFont(f);
    int off = -1, off2 = 0;
    if (!gl->GisRightHanded) {
        m_questMark->scale(-1, 1);
        if (m_questPos.fret() == 1)
            off = 1;
        else
            off2 = m_fretWidth/2;
    }
    QPoint markPoint(0, 0);
    if (m_questPos.fret()) {
        markPoint.setX(m_fretsPos[m_questPos.fret() + off] - off2);
        if (m_questPos.str() == 1)
          markPoint.setY(0);
        else if (m_questPos.str() == gl->Gtune()->stringNr())
          markPoint.setY(m_fbRect.bottom() - m_questMark->boundingRect().height() - 2);
        else
//           markPoint.setY((m_questPos.str() - 2) * m_strGap);
					markPoint.setY(m_questPos.str() * m_strGap - m_questMark->boundingRect().height() / 2);
    } else {
        if (m_questPos.str() *m_strGap < fbRect().height() / 2) // below string
            markPoint = QPoint(lastFret + m_fretWidth, m_fbRect.y() + m_questPos.str() * m_strGap - m_strGap / 2);
        else //above string
            markPoint = QPoint(lastFret + m_fretWidth, 
                               m_fbRect.y() + (m_questPos.str()) * m_strGap 
                               - m_questMark->boundingRect().height() - m_strGap/2);
    }
    if (!markPoint.isNull())
      m_questMark->setPos(markPoint);
}


void TfingerBoard::resizeRangeBox() {
    if (m_rangeBox1) {
//         QColor C = gl->EanswerColor;
        QColor C(0, 182, 182, 200);
        QPen pen = QPen(C, m_strGap / 3);
        pen.setJoinStyle(Qt::RoundJoin);
        int xxB, xxE; // begin and end points of a box
        if (m_loFret == 0 || m_loFret == 1)
            xxB = m_fbRect.x() - 4;
        else
            xxB = m_fretsPos[m_loFret-2] - 4;
        if (m_loFret == 0) {//  surely box for open strings
            if (m_hiFret == 0) { // one box over hole
                xxB = lastFret + m_strGap;
                xxE = width() - m_strGap;
            }
            else if (m_hiFret < gl->GfretsNumber) { // both, one over hole
                m_rangeBox2->setPen(pen);
                m_rangeBox2->setRect(0, 0, width() - lastFret - 2 * m_strGap, m_fbRect.height());
                m_rangeBox2->setPos(lastFret + m_strGap , m_fbRect.y() - 4);
                xxE = m_fretsPos[m_hiFret - 1] + 12;
            } else { // one - over whole guitar
                xxE = width() - m_strGap;
            }
        } else { //one
            xxE = m_fretsPos[m_hiFret - 1] + 12;
        }
        m_rangeBox1->setPen(pen);
        m_rangeBox1->setRect(0, 0, xxE - xxB, m_fbRect.height());
        m_rangeBox1->setPos(xxB, m_fbRect.y() - 4);
    }
}


void TfingerBoard::paintFingerAtPoint(QPoint p) {
    int strNr = 7, fretNr = 99;
    if ( (p.y() >= m_fbRect.y()) && (p.y() <= (height() - m_fbRect.y() - 4)) ) {
        int tx, ty = p.y();
        tx = mapToScene(p.x(), p.y()).x();
        strNr = (ty - m_fbRect.y()) / m_strGap;
        if (tx < m_fbRect.x() || tx > lastFret /*or some mouse button*/ )
            fretNr = 0;
        else {
            for (int i = 0; i < gl->GfretsNumber; i++) {
                if (tx <= m_fretsPos[i]) {
                    fretNr = i + 1;
                    break;
                }
            }
        }
    }
    if (m_curStr != strNr || m_curFret != fretNr) {
        if ( fretNr > 0 && fretNr < 99) { // show finger
            paintFinger(m_workFinger, strNr, fretNr);
            if (!m_workFinger->isVisible())
                m_workFinger->show();
            if (m_curStr != 7) m_workStrings[m_curStr]->hide();
        } else { // show string line
            m_workFinger->hide();
            if (m_curStr != 7) m_workStrings[m_curStr]->hide();
            if (strNr != 7) m_workStrings[strNr]->show();
        }
        m_curStr = strNr;
        m_curFret = fretNr;
				deleteBeyondTip();
    }
}


void TfingerBoard::deleteBeyondTip() {
		delete m_beyondTip;
		m_beyondTip = 0;
}

//################################################################################################
//################### CORRECTING ANSWERS  ANIMATIONS    #########################################
//################################################################################################
void TfingerBoard::strikeBlinkingFinished() {
		m_strikeOut->hide();
		m_strikeOut->deleteLater();
		m_strikeOut = 0;
		markAnswer(Qt::transparent);
		deleteNoteName();
		bool isLine = false, toLine;
		qreal strWidth = 0.0;
		QLineF lineAnim;
		if (m_fingerPos.fret() || m_goodPos.fret()) { // move ellipse
				QGraphicsEllipseItem *ellipse;
				if (m_movingItem)
						ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(m_movingItem);
				else {
						ellipse = new QGraphicsEllipseItem();
						ellipse->setPen(Qt::NoPen);
						ellipse->setBrush(gl->GselectedColor);
						ellipse->setPos(m_fingers[m_fingerPos.str() - 1]->pos());
						m_scene->addItem(ellipse);
						ellipse->setRect(fingerRect());
				}
				if (m_fingerPos.fret() && m_goodPos.fret() == 0) { // ellipse to line
						lineAnim = stringLine(m_goodPos.str());
						toLine = true;
						strWidth = stringWidth(m_goodPos.str());
				} else if (m_goodPos.fret() && m_fingerPos.fret() == 0) { // line to ellipse
						lineAnim.setPoints(fretToPos(m_goodPos), 
															 QPointF(fretToPos(m_goodPos).x() + fingerRect().width(), fretToPos(m_goodPos).y()));
						ellipse->setRect(0, /*stringWidth(m_goodPos.str()) + 2.*/0, stringLine(1).length(),
														 stringWidth(m_goodPos.str()) + 2.0);
						ellipse->setPos(stringLine(m_fingerPos.str()).p1());
						toLine = false;
						strWidth = fingerRect().height();
				}
				m_movingItem = (QGraphicsItem*)ellipse;
				m_fingers[m_fingerPos.str() - 1]->hide();
    } else if (m_fingerPos.str() != 7) { // moving line only when both are open strings
				QGraphicsLineItem *line = new QGraphicsLineItem();
				line->setLine(m_strings[m_fingerPos.str() - 1]->line());
				line->setPen(QPen(gl->GselectedColor, m_strWidth[m_fingerPos.str() - 1]));
				m_scene->addItem(line);
				m_movingItem = (QGraphicsItem*)line;
				m_strings[m_fingerPos.str() - 1]->hide();
				isLine = true;
		} else 
				return;
		m_animation = new TcombinedAnim(m_movingItem, this);
		m_animation->setDuration(300);
		QPointF startPos, endPos;
		if (isLine) {
				startPos = m_strings[m_fingerPos.str() - 1]->line().p1();
				endPos = m_strings[m_goodPos.str() - 1]->line().p1();;
		} else {
					startPos = m_movingItem->pos();
					if (strWidth != 0.0 && toLine)
						endPos = stringLine(m_goodPos.str()).p1();
					else
						endPos = fretToPos(m_goodPos);
		}
// 		connect(m_animation, SIGNAL(finished()), this, SLOT(finishCorrection()));
		m_animation->setMoving(startPos, endPos);
		m_animation->moving()->setEasingCurveType(QEasingCurve::OutExpo);
		if (strWidth != 0.0) {
			m_animation->setMorphing(lineAnim, strWidth, toLine);
			m_animation->morphing()->setEasingCurveType(QEasingCurve::InQuint);
		} else if (!isLine) {
				m_animation->setScaling(1.0, 2.0);
				m_animation->scaling()->setEasingCurveType(QEasingCurve::OutExpo);
		}
		m_animation->startAnimations();
		QTimer::singleShot(m_animation->duration() + 100, this, SLOT(finishCorrection()));
}


void TfingerBoard::finishCorrection() {
		m_animation->deleteLater();
		m_animation = 0;
		if (m_movingItem) {
			delete m_movingItem;
			m_movingItem = 0;
		}
		setFinger(m_goodPos);
		markAnswer(QColor(gl->EanswerColor.lighter().name()));
		if (m_nameInCorrection)
				showName(m_corrStyle, gl->EanswerColor);
}




