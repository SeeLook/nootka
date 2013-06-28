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
#include <QButtonGroup>
#include <QPainter>
#include <QEvent>



//######### TselectClefPrivate #######################################################################

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
	
	if (isMenu) {
			QVBoxLayout *leftLay = new QVBoxLayout;
			QVBoxLayout *rightLay = new QVBoxLayout;
			leftLay->addWidget(treble);
			leftLay->addWidget(treble_8);
			leftLay->addWidget(bass);
			leftLay->addWidget(bass_8);
			rightLay->addWidget(alto);
			rightLay->addWidget(tenor);
			rightLay->addWidget(piano);
			QHBoxLayout *lay = new QHBoxLayout;
			lay->addLayout(leftLay);
			lay->addLayout(rightLay);
			setLayout(lay);
	} else {
			QHBoxLayout *upLay = new QHBoxLayout;
			QHBoxLayout *lowLay = new QHBoxLayout;
			upLay->addWidget(treble);
			lowLay->addWidget(treble_8);
			upLay->addWidget(bass);
			lowLay->addWidget(bass_8);
			upLay->addWidget(alto);
			lowLay->addWidget(tenor);
			QVBoxLayout *upLowLay = new QVBoxLayout;
			upLowLay->addLayout(upLay);
			upLowLay->addLayout(lowLay);
			QHBoxLayout *lay = new QHBoxLayout;
			lay->addLayout(upLowLay);
			lay->addWidget(piano);
			
			setLayout(lay);
	}
	connect(treble, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
	connect(treble_8, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
	connect(bass, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
	connect(bass_8, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
	connect(alto, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
	connect(tenor, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
	connect(piano, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
}


void TselectClefPrivate::selectClef(Tclef clef) {
	switch(clef.type()) {
		case Tclef::e_treble_G:
			treble->setChecked(true); break;
		case Tclef::e_treble_G_8down:
			treble_8->setChecked(true); break;
		case Tclef::e_bass_F:
			bass->setChecked(true); break;
		case Tclef::e_bass_F_8down:
			bass_8->setChecked(true); break;
		case Tclef::e_alto_C:
			alto->setChecked(true); break;
		case Tclef::e_tenor_C:
			tenor->setChecked(true); break;
		case Tclef::e_pianoStaff:
			piano->setChecked(true); break;
			
	}
}


//########## TradioClef ##############################################################################


TradioClef::TradioClef(Tclef clef, QWidget* parent, bool isMenu) :
	QWidget(parent),
	m_clef(clef),
	m_hasMouseOver(false)
{
		QHBoxLayout *lay = new QHBoxLayout;
		m_radio = new QRadioButton(this);
		lay->addWidget(m_radio);
		
		QLabel *pixLabel = new QLabel(TtipChart::wrapPixToHtml(Tnote(0, 0, 0), m_clef.type(), 0, 3.5), this);
    lay->addWidget(pixLabel);
    if (isMenu) {
        QLabel *textLabel = new QLabel(m_clef.name(), this);
        lay->addWidget(textLabel);
    }
		lay->addStretch();
		setLayout(lay);
		setStatusTip("<b>" + m_clef.name() + "</b>  (" + m_clef.desc() + ")");
		m_radio->setStatusTip(statusTip());
		pixLabel->setStatusTip(statusTip());
}


void TradioClef::setChecked(bool checked) {
		m_radio->setChecked(checked);
}


void TradioClef::clefClickedSlot() {
		if (!m_radio->isChecked()) {
			m_radio->setChecked(true);
			emit selectedClef(m_clef);
		}
}


bool TradioClef::event(QEvent* event) {
    if (event->type() == QEvent::Leave) {
        m_hasMouseOver = false;
				update();
		} else if (event->type() == QEvent::Enter) {
				m_hasMouseOver = true;
				update();
		}	else if (event->type() == QEvent::MouseButtonPress)
				clefClickedSlot();
    return QWidget::event(event);
}


void TradioClef::paintEvent(QPaintEvent* event) {
	if (m_hasMouseOver) {
		QPainter painter(this);
		QLinearGradient lg(0, 0, width(), height());
		QColor bg = palette().highlight().color();
		bg.setAlpha(100);
		lg.setColorAt(0.0, bg);
		lg.setColorAt(0.9, palette().highlight().color());
		painter.setPen(Qt::NoPen);
		painter.setBrush(lg);
		painter.drawRoundedRect(0, 0, width(), height(), 6, 6);
	}
}



//########### TselectClef #################################################################################

TselectClef::TselectClef(QWidget* parent) :
	TselectClefPrivate(false, parent)
{
		QButtonGroup *clefsGroup = new QButtonGroup(this);
		clefsGroup->addButton(treble->radio());
		clefsGroup->addButton(treble_8->radio());
		clefsGroup->addButton(bass->radio());
		clefsGroup->addButton(bass_8->radio());
		clefsGroup->addButton(alto->radio());
		clefsGroup->addButton(tenor->radio());
		clefsGroup->addButton(piano->radio());
}


void TselectClef::clefWasSelected(Tclef clef) {
		emit clefSelected(clef);
}


//########### TclefMenu ###########################################################################
TclefMenu::TclefMenu(QWidget* parent) :
	m_menu(new QMenu(parent)),
	TselectClefPrivate(true, m_menu)
{
		m_menu->setLayout(TselectClefPrivate::layout());
		m_curentClef = Tclef(Tclef::e_none);
}


Tclef TclefMenu::exec(QPoint pos) {
		m_menu->exec(pos);
		return m_curentClef;
}



void TclefMenu::clefWasSelected(Tclef clef) {
		m_curentClef = clef;
		m_menu->close();
}






