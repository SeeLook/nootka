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

#include "tnotenamelabel.h"
#include <animations/tstrikedoutitem.h>
#include <animations/tblinkingitem.h>
#include <animations/tmovedanim.h>
#include <tgraphicstexttip.h>
#include <QGraphicsTextItem>
#include <QGraphicsEffect>
#include <QTimer>


QString getBgColorText(const QColor& C) {
	if ( C != -1)
    return QString("background-color: rgba(%1, %2, %3, %4); ").arg(C.red()).arg(C.green()).arg(C.blue()).arg(C.alpha());
  else
    return QString("background-color: transparent; ");
}


/*static*/
QString TnoteNameLabel::borderStyleText() {
	return "border: 1px solid palette(Text); border-radius: 10px;";
}

//################################################################################################
//#################################   PUBLIC     #################################################
//################################################################################################
TnoteNameLabel::TnoteNameLabel(const QString& text, QWidget* parent) :
	QGraphicsView(parent),
	m_strikeOut(0), m_blinking(0),
	m_questMark(0),
	m_stringNumber(0),
	m_p2Time(0)
{
  setAttribute(Qt::WA_TransparentForMouseEvents, true);
	setMouseTracking(false);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
	
	QGraphicsScene *scene = new QGraphicsScene(this);
	setScene(scene);
	m_textItem = new QGraphicsTextItem(0, scene);
	QFont f = font();
	f.setStyleHint(QFont::SansSerif);
	m_textItem->setFont(f);
	scene->setSceneRect(geometry());
	setText(text);
	
	
	m_bgColor = palette().base().color();
	m_bgColor.setAlpha(220);
	setBackgroundColor(m_bgColor);
}


void TnoteNameLabel::setText(const QString& text) {
	if (m_questMark) {
		delete m_questMark;
		m_questMark = 0;
	}
	if (m_stringNumber) {
		delete m_stringNumber;
		m_stringNumber = 0;
	}
	m_textItem->setGraphicsEffect(0);
	m_textItem->setHtml(text);
	center();
}


void TnoteNameLabel::center() {
	m_textItem->setPos(mapToScene(0, 0).x() + (width() - m_textItem->boundingRect().width() * m_textItem->scale()) / 2,
				(scene()->height() - m_textItem->boundingRect().height() * m_textItem->scale()) / 2	+ height() / 16.0);
#if defined(Q_OS_MAC)
    scene()->update();
#endif
}


void TnoteNameLabel::showQuestionMark(const QColor& color) {
	if (m_questMark)
		return;
	m_questMark = new QGraphicsSimpleTextItem("?", 0, scene());
	m_questMark->setFont(QFont("nootka"));
	m_questMark->setBrush(color);
	m_questMark->setScale(height() / m_questMark->boundingRect().height());
	m_questMark->setPos(m_textItem->pos().x() + m_textItem->boundingRect().width() * m_textItem->scale() + 10.0, 
											(height() - m_questMark->boundingRect().height() * m_questMark->scale()) / 2 + height() / 10.0);
}


void TnoteNameLabel::showStringNumber(int strNr, const QColor &color) {
	if (m_stringNumber)
		return;
	m_stringNumber = new QGraphicsSimpleTextItem(QString("%1").arg(strNr), 0, scene());
	m_stringNumber->setFont(QFont("nootka"));
	m_stringNumber->setBrush(color);
	m_stringNumber->setScale(((height()) / m_stringNumber->boundingRect().height()) * 0.9) ;
	qreal xOff = m_textItem->pos().x() + m_textItem->boundingRect().width() * m_textItem->scale();
	if (m_questMark)
		xOff = m_questMark->pos().x() + m_questMark->boundingRect().width() * m_questMark->scale();
	m_stringNumber->setPos(xOff + 10.00, 
												 (mapToScene(0, 0).y() + 
												 (m_stringNumber->boundingRect().height() * m_stringNumber->scale()) / 10.0 + // nice offset
												 (height() - m_stringNumber->boundingRect().height() * m_stringNumber->scale()) / 2));
}


void TnoteNameLabel::markText(const QColor& color) {
	QGraphicsDropShadowEffect *blur = new QGraphicsDropShadowEffect;
	blur->setColor(color);
	blur->setOffset(0.5, 0.5);
	blur->setBlurRadius(7.0);
	m_textItem->setGraphicsEffect(blur);
}



void TnoteNameLabel::setStyleSheet(const QString& style) {
	QGraphicsView::setStyleSheet(borderStyleText() + m_bgColorText + style);
	m_styleText = style;
}


void TnoteNameLabel::setBackgroundColor(const QColor& color) {
	m_bgColor = color;
	m_bgColorText = getBgColorText(color);
	QGraphicsView::setStyleSheet(borderStyleText() + m_bgColorText + m_styleText);
	repaint();
}

//################################################################################################
//#################################   ANIMATIONS    ##############################################
//################################################################################################

void TnoteNameLabel::blinkCross(const QColor& color) {
	m_strikeOut = new TstrikedOutItem(m_textItem);
	m_strikeOut->setPen(QPen(color, height() / 30));
	connect(m_strikeOut, SIGNAL(strikedFInished()), this, SLOT(strikeBlinkingSlot()));
	m_strikeOut->startBlinking();
}


void TnoteNameLabel::blinkingText(int count, int period) {
	if (m_blinking)
		return;
	m_blinking = new TblinkingItem(m_textItem, this);
	connect(m_blinking, SIGNAL(finished()), this, SLOT(blinkingSlot()));
	m_blinking->startBlinking(count);
}

/** When p1time is 0 it starts throwingSlot() immediately - brings new text
* when p2Time is 0 it only throws text item away */
void TnoteNameLabel::thrownText(const QString& newText, int p1time, int p2Time) {
	if (p1time == 0 && p2Time == 0)
		return; // it could make something bad
	if (m_questMark) {
		delete m_questMark;
		m_questMark = 0;
	}
	m_newText = newText;
	m_p2Time = p2Time;
	m_throwAnim = 0;
	if (p1time > 0) {
		m_throwAnim = new TmovedAnim(m_textItem, this);
		m_throwAnim->setDuration(p1time);
		m_throwAnim->setEasingCurveType(QEasingCurve::OutExpo);
		connect(m_throwAnim, SIGNAL(finished()), this, SLOT(throwingSlot()));
		m_throwAnim->startMoving(m_textItem->pos(), QPointF(width() + 5, m_textItem->pos().y()));
	} else
		throwingSlot();
	scene()->update();
}


QPoint TnoteNameLabel::textPos() {
	return mapFromScene(m_textItem->pos());
}


//################################################################################################
//################################# PROTECTED    #################################################
//################################################################################################

void TnoteNameLabel::resizeEvent(QResizeEvent* event) {
	scene()->setSceneRect(geometry());
// 	m_textItem->setScale((height() * 0.95) / (m_textItem->boundingRect().height() * m_textItem->scale()));
	QFontMetricsF fm(m_textItem->font());
	m_textItem->setScale((height() * 0.9) / (fm.boundingRect("A").height()));
	if (m_questMark) {
		m_questMark->setScale(height() / m_questMark->boundingRect().height());
		m_questMark->setPos(m_textItem->pos().x() + m_textItem->boundingRect().width() * m_textItem->scale() + 10.0, 
											(height() - m_questMark->boundingRect().height() * m_questMark->scale()) / 2 + height() / 10.0);
	}
	if (m_stringNumber) {
		m_stringNumber->setScale(((height()) / m_stringNumber->boundingRect().height()) * 0.9) ;
		qreal off = 10.0;
		if (m_questMark)
			off = m_questMark->boundingRect().width() * m_questMark->scale() + 10.0;
			m_stringNumber->setPos((m_textItem->pos().x() + m_textItem->boundingRect().width() + off) * m_textItem->scale() + 10.0, 
												(height() - m_stringNumber->boundingRect().height() * m_stringNumber->scale()) / 2 + height() / 10.0);
	}
	center();
}

//################################################################################################
//################################# PROTECTED SLOTS   ############################################
//################################################################################################

void TnoteNameLabel::strikeBlinkingSlot() {
	if (m_strikeOut) {
		m_strikeOut->deleteLater();
		m_strikeOut = 0;
	}
	emit blinkingFinished();
}


void TnoteNameLabel::blinkingSlot() {
	if (m_blinking) {
		m_blinking->deleteLater();
		m_blinking = 0;
	}
	emit blinkingFinished();
}


void TnoteNameLabel::throwingSlot() {
	if (m_throwAnim)
			m_throwAnim->deleteLater();
	if (m_p2Time) { // after first phase (thrown away)
		m_textItem->setHtml(m_newText);
		m_throwAnim = new TmovedAnim(m_textItem, this);
		m_throwAnim->setDuration(m_p2Time);
		m_p2Time = 0;
		m_throwAnim->setEasingCurveType(QEasingCurve::OutExpo);
		connect(m_throwAnim, SIGNAL(finished()), this, SLOT(throwingSlot()));
		m_throwAnim->startMoving(QPointF(m_textItem->boundingRect().width() * (-m_textItem->scale()), m_textItem->pos().y()),
				QPointF((width() - m_textItem->boundingRect().width() * m_textItem->scale()) / 2, m_textItem->pos().y()));
	} else { // after second phase (brought back)
		m_throwAnim = 0;
		center();
		emit throwingFinished();
	}
}





