/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tmainview.h"
#include <QtWidgets>


TmainView::TmainView(QWidget* toolW, QWidget* statLabW, QWidget* pitchW, QWidget* scoreW, QWidget* guitarW, QWidget* parent) :
	QGraphicsView(parent)
{
	setScene(new QGraphicsScene(this));
	
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
	setStyleSheet(("background: transparent"));
	
	m_tool = scene()->addWidget(toolW);
	m_status = scene()->addWidget(statLabW);
	m_pitch = scene()->addWidget(pitchW);
	m_score = scene()->addWidget(scoreW);
	m_guitar = scene()->addWidget(guitarW);
	
	toolW->installEventFilter(this);
	pitchW->installEventFilter(this);
	scoreW->installEventFilter(this);
	guitarW->installEventFilter(this);
	
	m_score->setZValue(20);
	
	QGraphicsLinearLayout *lay = new QGraphicsLinearLayout(Qt::Vertical);
		lay->setContentsMargins(1, 2, 1, 2);
		lay->addItem(m_tool);
		QGraphicsLinearLayout *statAndPitchLay = new QGraphicsLinearLayout(Qt::Horizontal);
			statAndPitchLay->addItem(m_status);
			statAndPitchLay->addItem(m_pitch);
		lay->addItem(statAndPitchLay);
		lay->addItem(m_score);
		lay->addItem(m_guitar);
	m_form = new QGraphicsWidget();
	m_form->setLayout(lay);
	scene()->addItem(m_form);

}

//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

void TmainView::resizeEvent(QResizeEvent* event) {
	m_form->setGeometry(0, 0, width(), height());
  scene()->setSceneRect(0, 0, width(), height());
}


bool TmainView::eventFilter(QObject* ob, QEvent* event) {
	if (event->type() == QEvent::StatusTip) {
			QStatusTipEvent *tip = static_cast<QStatusTipEvent *>(event);
			emit statusTip(tip->tip());
	}
	return QAbstractScrollArea::eventFilter(ob, event);
}





