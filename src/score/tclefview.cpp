/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tclefview.h"

TclefView::TclefView(TscoreWidgetSimple *parent) :
    QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setRenderHint(QPainter::TextAntialiasing, true);

    m_scene = new QGraphicsScene();
    setScene(m_scene);

    m_clefText = new QGraphicsSimpleTextItem();
    m_clefText->setBrush(QBrush(palette().text().color()));
    m_scene->addItem(m_clefText);
    m_clefText->setText(QString(QChar(0xe1a7)));

    show();
}

void TclefView::resize(int co) {
    m_scene->setSceneRect(0, 0, width(), height());
    QFont font(QFont("nootka", co*12.5, QFont::Normal));
    m_clefText->setFont(font);
    m_clefText->setPos(1, qRound(11.2*co));
}
