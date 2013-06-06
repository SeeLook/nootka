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

#include "tclefselector.h"
#include "tscoreclef.h"
#include "tclefpreview.h"
#include "tscorestaff.h"
#include "tscorescene.h"
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsScene>
#include <QGraphicsEffect>

TclefSelector::TclefSelector(TscoreScene* scene, Tclef clefToMark) :
	TscoreItem(scene)
{
		setZValue(100);
		setScale(0.3);
		// This is two columns table with clef symbols.
		createEntry(scene, m_treble, Tclef(Tclef::e_treble_G));
		m_treble->setPos(1, -10);
		if (clefToMark.type() == Tclef::e_treble_G)
			m_treble->mark();
		createEntry(scene, m_treble_8, Tclef(Tclef::e_treble_G_8down));
		m_treble_8->setPos(1, 6);
		if (clefToMark.type() == Tclef::e_treble_G_8down)
			m_treble_8->mark();
		createEntry(scene, m_bass, Tclef(Tclef::e_bass_F));
		m_bass->setPos(1, 22);
		if (clefToMark.type() == Tclef::e_bass_F)
			m_bass->mark();
		createEntry(scene, m_bass_8, Tclef(Tclef::e_bass_F_8down));
		m_bass_8->setPos(1, 35);
		if (clefToMark.type() == Tclef::e_bass_F_8down)
			m_bass_8->mark();
		createEntry(scene, m_alto, Tclef(Tclef::e_alto_C));
		m_alto->setPos(50, -10);
		if (clefToMark.type() == Tclef::e_alto_C)
			m_alto->mark();
		createEntry(scene, m_tenor, Tclef(Tclef::e_tenor_C));
		m_tenor->setPos(50, 6);
		if (clefToMark.type() == Tclef::e_tenor_C)
			m_tenor->mark();
		createEntry(scene, m_piano, Tclef(Tclef::e_pianoStaff));
		m_piano->setPos(50, 22);
		if (clefToMark.type() == Tclef::e_pianoStaff)
			m_piano->mark();

		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
		effect->setOffset(2, 2);
		setGraphicsEffect(effect);
}


QRectF TclefSelector::boundingRect() const {
	return QRectF(0, -7.0, 95.0, 70.0);
	/** Hardcoded values describing selector rectiangle. */
}


void TclefSelector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QColor startColor = option->palette.highlight().color();
    startColor.setAlpha(25);
    QColor endColor = startColor;
    endColor.setAlpha(100);
    painter->setPen(QPen(option->palette.highlight().color(), 0.2));
    painter->setBrush(option->palette.base().color());
    painter->drawRoundedRect(boundingRect(), 2.0, 2.0);
    QLinearGradient grad(boundingRect().topLeft(), boundingRect().bottomRight());
    grad.setColorAt(0.1, startColor);
    grad.setColorAt(1.0, endColor);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(boundingRect(), 2.0, 2.0);
}

//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################


void TclefSelector::createEntry(TscoreScene* scene, TclefPreview*& clefView, Tclef clef) {
		clefView = new TclefPreview(scene, clef);
		clefView->setParentItem(this);
		connect(clefView, SIGNAL(clicked(Tclef)), this, SLOT(clefClicked(Tclef)));
}


void TclefSelector::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  emit clefSelected(Tclef(Tclef::e_none));
}


void TclefSelector::clefClicked(Tclef clef) {
		emit clefSelected(clef);
}


