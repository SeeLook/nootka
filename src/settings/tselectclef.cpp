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


#include "tselectclef.h"
#include <tnotepixmap.h>
#include <ttipchart.h>
#include <QAction>
#include <QMenu>
#include <qtoolbar.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QEvent>



//##################################################################################################

TselectClefPrivate::TselectClefPrivate(bool isMenu, QWidget* parent) :
	QWidget(parent)
{
	treble = new TradioClef(Tclef(Tclef::e_treble_G), parent, isMenu);
	treble_8 = new TradioClef(Tclef(Tclef::e_treble_G_8down), parent, isMenu);
	bass = new TradioClef(Tclef(Tclef::e_bass_F), parent, isMenu);
	bass_8 = new TradioClef(Tclef(Tclef::e_bass_F_8down), parent, isMenu);
	alto = new TradioClef(Tclef(Tclef::e_alto_C), parent, isMenu);
	tenor = new TradioClef(Tclef(Tclef::e_tenor_C), parent, isMenu);
	piano = new TradioClef(Tclef(Tclef::e_pianoStaff), parent, isMenu);
	
	QBoxLayout *lay = new QBoxLayout(QBoxLayout::LeftToRight);
	if (isMenu)
		lay->setDirection(QBoxLayout::TopToBottom);
// 	else
// 		lay->setDirection(Qt::Horizontal);
	lay->addWidget(treble);
	lay->addWidget(treble_8);
	lay->addWidget(bass);
	lay->addWidget(bass_8);
	lay->addWidget(alto);
	lay->addWidget(tenor);
	lay->addWidget(piano);
	
	setLayout(lay);
}


//####################################################################################################


TradioClef::TradioClef(Tclef clef, QWidget* parent, bool isMenu) :
	QWidget(parent),
	m_clef(clef)
{
		QHBoxLayout *lay = new QHBoxLayout;
		m_radio = new QRadioButton(this);
		lay->addWidget(m_radio);
		
		QLabel *pixLabel = new QLabel(TtipChart::wrapPixToHtml(Tnote(0, 0, 0), m_clef.type(), 0, 3.0), this);
    lay->addWidget(pixLabel);
    if (isMenu) {
        QLabel *textLabel = new QLabel(m_clef.name(), this);
        lay->addWidget(textLabel);
    }
		lay->addStretch();
		setLayout(lay);
}

bool TradioClef::event(QEvent* event) {
    if (event->type() == QEvent::Leave)
        setStyleSheet("background-color: transparent;");
    else if (event->type() == QEvent::Enter)
        setStyleSheet("background-color: palette(highlight);");
    return QWidget::event(event);
}


//####################################################################################################

TselectClef::TselectClef(QWidget* parent) :
	TselectClefPrivate(false, parent)
{

}


TselectClef::~TselectClef()
{}


void TselectClef::selectClef(Tclef clef)
{

}


//####################################################################################################
TclefMenu::TclefMenu(QWidget* parent) :
	TselectClefPrivate(true, parent),
	QMenu(parent)
{
	QMenu::setLayout(TselectClefPrivate::layout());
}






