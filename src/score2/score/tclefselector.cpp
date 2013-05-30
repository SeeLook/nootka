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
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsScene>
#include <QGraphicsEffect>

TclefSelector::TclefSelector(TscoreScene *scene) :
	TscoreItem(scene)
{
		setZValue(100);
		setScale(0.4);
		
		createEntry(scene, m_treble, Tclef(Tclef::e_treble_G));
		m_treble->setPos(1, -8);
		createEntry(scene, m_treble_8, Tclef(Tclef::e_treble_G_8down));
		m_treble_8->setPos(1, 8);
		createEntry(scene, m_bass, Tclef(Tclef::e_bass_F));
		m_bass->setPos(1, 24);
		createEntry(scene, m_bass_8, Tclef(Tclef::e_bass_F_8down));
		m_bass_8->setPos(1, 37);
		createEntry(scene, m_alto, Tclef(Tclef::e_alto_C));
		m_alto->setPos(50, -8);
		createEntry(scene, m_tenor, Tclef(Tclef::e_tenor_C));
		m_tenor->setPos(50, 8);
		createEntry(scene, m_piano, Tclef(Tclef::e_pianoStaff));
		m_piano->setPos(50, 24);

		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
		setGraphicsEffect(effect);
		

}

void TclefSelector::clefClicked(Tclef clef) {
		qDebug() << clef.name();
		emit clefSelected(clef);
}



QRectF TclefSelector::boundingRect() const {
// 		return QRectF(0, 0, m_G->boundingRect().width() + 5, m_G->boundingRect().height() + 5);
	return QRectF(0, -7.0, 95.0, 70.0);
}



void TclefSelector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
		QColor bg = option->palette.base().color();
// 		bg.setAlpha(30);
		painter->setPen(QPen(option->palette.windowText().color(), 0.5));
    painter->setBrush(bg);
		painter->drawRoundedRect(boundingRect(), 2.0, 2.0);
// 		QGraphicsTextItem::paint(painter, option, widget);
}


void TclefSelector::createEntry(TscoreScene* scene, TclefPreview*& clefView, Tclef clef) {
		clefView = new TclefPreview(scene, clef);
		clefView->setParentItem(this);
		connect(clefView, SIGNAL(clicked(Tclef)), this, SLOT(clefClicked(Tclef)));
}



