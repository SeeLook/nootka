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

#define ALPHA (40)

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
	m_desc->setBrush(scene->views()[0]->palette().text().color());
	m_desc->setText(clef.name().replace(" ", "\n"));
	QFontMetricsF fm = QFontMetricsF(m_desc->font());
	m_desc->setScale((7 / fm.boundingRect("A").height())); // fixed font size for any user fonts quirks
	setStatusTip(clef.name() + " (" + clef.desc() + ")");
	if (clef.type() != Tclef::e_pianoStaff) {
			scoreClef()->setClef(clef);
			m_desc->setPos(10.0, upperLinePos() + 4 - (m_desc->boundingRect().height() * m_desc->scale()) / 2);			
	} else {
			m_lower = new TscoreStaff(scene, 0, e_lower);
			m_lower->setParentItem(this);
			m_lower->setPos(0, 24);
			m_desc->setPos(10.0, upperLinePos() + 10 - (m_desc->boundingRect().height() * m_desc->scale()) / 2);
			m_lower->setStatusTip(clef.name() + " (" + clef.desc() + ")");
			m_lower->scoreClef()->setIsClickable(false);
			m_lower->scoreClef()->setAcceptHoverEvents(false);
			QGraphicsSimpleTextItem *brace = new QGraphicsSimpleTextItem();
			registryItem(brace);
			brace->setParentItem(this);
			QFont ff = QFont("nootka");
			ff.setPointSizeF(22.8);
			QFontMetrics fMetr(ff);
			ff.setPointSizeF(ff.pointSizeF() * (ff.pointSizeF() / fMetr.boundingRect(QChar(0xe16c)).height()));
			brace->setFont(ff);
			brace->setBrush(scene->views()[0]->palette().text().color());
			brace->setText(QString(QChar(0xe16c)));
			brace->setPos(-3.5, upperLinePos());
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
    return QRectF(-1, 10.0, 43.0, 30.0);
	else
		return QRectF(0, upperLinePos() - 4, 43.0, 16.0);
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


