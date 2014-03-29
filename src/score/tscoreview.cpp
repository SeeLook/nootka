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

#include "tscoreview.h"
#include <score/tscorescene.h>
#include <score/tscorenote.h>
#include <score/tscorestaff.h>
// #include <QMouseEvent>
// #include <QStyle>
// #include <QScrollBar>
// #include <QDebug>

TscoreView::TscoreView(QWidget* parent) :
	QGraphicsView(parent)
{
	m_scoreScene = new TscoreScene(this);
	setScene(m_scoreScene);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMouseTracking(true);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
	
}

/*
void TscoreView::wheelEvent(QWheelEvent* event) {
// 	qDebug() << (int)event->buttons();
	  int prevAcc = m_scoreScene->currentAccid();
		int acc = m_scoreScene->currentAccid();
    if (event->delta() < 0) {
        acc--;
        if (acc < (-scoreScene()->doubleAccidsFuse()))
          acc = -(scoreScene()->doubleAccidsFuse());
    } else {
        acc++;
        if (acc > scoreScene()->doubleAccidsFuse())
          acc = scoreScene()->doubleAccidsFuse();
    }
    if (prevAcc != acc) {
				TscoreNote::setWorkAccid(acc);
        scoreScene()->setCurrentAccid(acc);
//         emit accidWasChanged(m_curentAccid);
    }
}


void TscoreView::resizeEvent(QResizeEvent* event) {
	int hh = height(), ww = width();
	if (event) {
		hh = event->size().height();
		ww = event->size().width();
	}
	int scrollV;
	if (horizontalScrollBar()->isVisible()) {
		hh -= horizontalScrollBar()->height();
		scrollV = horizontalScrollBar()->value();
	}
	qreal styleOff = 1.0; // some styles quirks - it steals some space
  if (style()->objectName() == "oxygen" || style()->objectName() == "oxygen transparent" || style()->objectName() == "qtcurve")
			styleOff = 0.0;
  qreal factor = (((qreal)hh / 40.0) / transform().m11()); // m_pianoFactor;
  scale(factor, factor);
// 	if (m_staff->boundingRect().width() * factor < ww - 10.0 * factor)
// 		m_staff->setExternalWidth((ww / transform().m11()) - 2.0);
// 	m_score->setSceneRect(0, 0, (m_staff->boundingRect().width() + styleOff) * factor, 
// 												m_staff->boundingRect().height() * factor	);
// 	m_scene->setSceneRect(0, 0, (m_staff->boundingRect().width() + styleOff) * factor, 
// 												m_staff->boundingRect().height() * factor	);
// 	qDebug() << m_scene->sceneRect() << m_scene->itemsBoundingRect();
// 	m_score->setMaximumSize(m_scene->sceneRect().width(), m_scene->sceneRect().height() / m_pianoFactor);
//   m_score->setMinimumSize(m_scene->sceneRect().width(), m_scene->sceneRect().height());
	if (horizontalScrollBar()->isVisible()) {
		horizontalScrollBar()->setValue(scrollV);
	}
  qreal staffOff = 1.0;
//   if (isPianoStaff())
//     staffOff = 2.0;
// 	m_staff->setPos(m_score->mapToScene(staffOff, 0));
	m_staff->setPos(staffOff, 0.0);

}
*/











