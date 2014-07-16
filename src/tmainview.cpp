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

QHBoxLayout *scoreAndNameLay = 0;
QVBoxLayout *lay;

TmainView::TmainView(QWidget* toolW, QWidget* statLabW, QWidget* pitchW, QWidget* scoreW, QWidget* guitarW, QWidget* parent) :
	QGraphicsView(parent),
	m_tool(toolW),
	m_status(statLabW),
	m_pitch(pitchW),
	m_score(scoreW),
	m_guitar(guitarW),
	m_name(0)
{
	setScene(new QGraphicsScene(this));
	
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
	setObjectName("TmainView");
	setStyleSheet(("QGraphicsView#TmainView { background: transparent }"));
	
	toolW->installEventFilter(this);
	pitchW->installEventFilter(this);
	guitarW->installEventFilter(this);
	
	
	lay = new QVBoxLayout;
		lay->setContentsMargins(1, 2, 1, 2);
		lay->addWidget(m_tool);
		QHBoxLayout *statAndPitchLay = new QHBoxLayout;
			statAndPitchLay->addWidget(m_status);
			statAndPitchLay->addWidget(m_pitch);
		lay->addLayout(statAndPitchLay);
		scoreAndNameLay = new QHBoxLayout;
		scoreAndNameLay->addWidget(m_score);
		lay->addLayout(scoreAndNameLay);
		lay->addWidget(m_guitar);
	QWidget *w = new QWidget;
	w->setObjectName("proxyWidget");
	w->setStyleSheet(("QWidget#proxyWidget { background: transparent }"));
	w->setLayout(lay);
	m_form = scene()->addWidget(w);

// 	m_tool->hide();
// 	m_guitar->hide();
	
}


void TmainView::addNoteName(QWidget* name) {
	if (!m_name) {
		m_name = name;
		m_name->installEventFilter(this);
		m_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		scoreAndNameLay->insertStretch(0);
// 		scoreAndNameLay->addStretch(0);
		scoreAndNameLay->addWidget(m_name, 0, Qt::AlignCenter);
		scoreAndNameLay->addStretch(0);
		m_score->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
// 		scoreAndNameLay->parentWidget()->updateGeometry();
	}
}


void TmainView::takeNoteName() {
	if (m_name) {
// 		m_name->setParent(0);
		scoreAndNameLay->removeWidget(m_name);
		m_name = 0;
		delete scoreAndNameLay->takeAt(0);
		delete scoreAndNameLay->takeAt(1);
	}
}


void TmainView::addExamViews(QWidget* resultsW, QWidget* progressW) {
	m_results = resultsW;
	m_progress = progressW;
	QHBoxLayout *resultLay = new QHBoxLayout;
		resultLay->addWidget(m_progress);
		resultLay->addWidget(m_results);
	lay->insertLayout(2, resultLay);
	resultsW->installEventFilter(this);
	progressW->installEventFilter(this);
}


void TmainView::takeExamViews() {
// 	delete m_results;
// 	delete m_progress;
}


void TmainView::setNamePos() {
	if (m_name) {
// 		m_name->setPos(width() / 3 + ((width() * 0.66 - m_name->widget()->width()) / 2), 
// 									m_score->pos().y() + (m_score->widget()->height() - m_name->widget()->height()) / 2);
	}
}


//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

void TmainView::resizeEvent(QResizeEvent* event) {
	Q_UNUSED (event)
	m_form->setGeometry(0, 0, width(), height());
  scene()->setSceneRect(0, 0, width(), height());
// 	m_score->resize(m_score->size());
// 	if (m_name) {
// 		m_name->resize(m_name->size());
// 		m_name->updateGeometry();
// 	}
// 	setNamePos();
}


bool TmainView::eventFilter(QObject* ob, QEvent* event) {
	if (event->type() == QEvent::StatusTip) {
			QStatusTipEvent *tip = static_cast<QStatusTipEvent *>(event);
			emit statusTip(tip->tip());
	}
	return QAbstractScrollArea::eventFilter(ob, event);
}






