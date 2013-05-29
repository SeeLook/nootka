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
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
// #include <QGraphicsEffect>

TclefSelector::TclefSelector()
{
// 	createEntry(m_G, Tclef(Tclef::e_treble_G));
// 		setAcceptHoverEvents(true);
		setZValue(100);
		Tclef cl;
		QString entry = "<style>a:link {text-decoration:none; color: #000;} a:visited {text-decoration:none;} a:hover {text-decoration:none;} a:active {text-decoration:none;} </style>";
		entry += "<a href=\"treble\">" + clefToHtml(cl) + "&nbsp;" +  cl.name() + "</a>";
		cl = Tclef(Tclef::e_treble_G_8down);
		entry += "<br><a href=\"treble_8\">" + clefToHtml(cl) + "&nbsp;" + cl.name() + "</a>";
		setHtml(entry);
		setScale(0.1);
// 		QGraphicsBlurEffect *effect = new QGraphicsBlurEffect;
// 		setGraphicsEffect(effect);
		
		setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(this, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
}


void TclefSelector::linkActivatedSlot(QString link) {
	qDebug() << link;
}


void TclefSelector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
		QColor bg = option->palette.window().color();
// 		bg.setAlpha(30);
		painter->setPen(QPen(option->palette.windowText().color(), 5));
    painter->setBrush(bg);
		painter->drawRoundedRect(boundingRect(), 10.0, 10.0);
		QGraphicsTextItem::paint(painter, option, widget);
}


QString TclefSelector::clefToHtml(Tclef clef) {
		return "<span style=\"font-size: 20px; font-family: nootka;\">" + QString(TscoreClef::clefToChar(clef)) + "</span>";
}


// void TclefSelector::createEntry(QGraphicsTextItem* textItem, Tclef clef) {
// 	textItem = new QGraphicsTextItem();
// 	registryItem(textItem);
// 	textItem->setHtml(clef.name());
// }



