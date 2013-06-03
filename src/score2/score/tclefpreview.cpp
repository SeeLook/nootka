/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tclefpreview.h"
#include "tscorescene.h"
#include "tscoreclef.h"
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsView>

#define ALPHA (30)

TclefPreview::TclefPreview(TscoreScene* scene, Tclef clef) :
	TscoreStaff(scene, 0, e_upper),
	m_marked(false),
	m_lower(0)
{
	scoreClef()->setIsClickable(false);
	scoreClef()->setAcceptHoverEvents(false);
	m_bgColor = scene->views()[0]->palette().highlight().color();
	m_bgColor.setAlpha(0); // transparent - not marked
	m_desc = new QGraphicsSimpleTextItem();
	registryItem(m_desc);
	m_desc->setParentItem(this);
	m_desc->setText(clef.name().replace(" ", "\n"));
// 	m_desc->font().setPointSize(3);
	m_desc->setBrush(scene->views()[0]->palette().windowText().color());
	m_desc->setScale(0.4);
	setStatusTip(clef.name() + " (" + clef.desc() + ")");
	if (clef.type() != Tclef::e_pianoStaff) {
			scoreClef()->setClef(clef);
			m_desc->setPos(8.0, upperLinePos());			
	} else {
			m_lower = new TscoreStaff(scene, 0, e_lower);
			m_lower->setParentItem(this);
			m_lower->setPos(0, 18);
			m_desc->setPos(11.0, upperLinePos());
			m_lower->setStatusTip(clef.name() + " (" + clef.desc() + ")");
			m_lower->scoreClef()->setIsClickable(false);
			m_lower->scoreClef()->setAcceptHoverEvents(false);
			QGraphicsSimpleTextItem *brace = new QGraphicsSimpleTextItem();
			registryItem(brace);
			brace->setParentItem(this);
			QFont ff = QFont("nootka");
			qreal fontFactor = 22.8;
			ff.setPointSizeF(fontFactor);
			QFontMetrics fMetr(ff);
			qreal fact = ff.pointSizeF() / fMetr.boundingRect(QChar(0xe16c)).height();
			ff.setPointSizeF(ff.pointSizeF() * fact);
// 			ff.setPointSizeF(6.9);
			brace->setFont(ff);
			brace->setText(QString(QChar(0xe16c)));
			brace->setPos(0.8, 7);
	}
}


TclefPreview::~TclefPreview() {}


void TclefPreview::mark() {
	m_bgColor.setAlpha(ALPHA);
	m_marked = true;
	update();
}


void TclefPreview::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setPen(Qt::NoPen);
	painter->setBrush(m_bgColor);
	painter->drawRoundedRect(boundingRect(), 2, 2);	
}


QRectF TclefPreview::boundingRect() const {
	if (m_lower)
    return QRectF(0, 4.0, 43.0, 30.0);
	else
		return QRectF(0, 4.0, 43.0, 16.0);
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TclefPreview::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (m_lower)
		emit clicked(Tclef(Tclef::e_pianoStaff));
	else
		emit clicked(scoreClef()->clef());
}

void TclefPreview::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    TscoreItem::hoverEnterEvent(event);
		m_bgColor.setAlpha(ALPHA * 2);
		update();
}


void TclefPreview::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    TscoreItem::hoverLeaveEvent(event);
		if (m_marked)
			m_bgColor.setAlpha(ALPHA);
		else
			m_bgColor.setAlpha(0);
		update();
}


