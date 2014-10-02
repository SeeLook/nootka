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
	
	
	m_mainLay = new QBoxLayout(QBoxLayout::TopToBottom);
		m_mainLay->setContentsMargins(1, 2, 1, 2);
		m_mainLay->addWidget(m_tool);
		m_statAndPitchLay = new QBoxLayout(QBoxLayout::LeftToRight);
		  m_statAndPitchLay->addWidget(m_status);
		  m_statAndPitchLay->addWidget(m_pitch);
	m_mainLay->addLayout(m_statAndPitchLay);
		m_scoreAndNameLay = new QBoxLayout(QBoxLayout::LeftToRight);
			m_scoreAndNameLay->addWidget(m_score);
		m_mainLay->addLayout(m_scoreAndNameLay);
		m_mainLay->addWidget(m_guitar);
	QWidget *w = new QWidget;
	w->setObjectName("proxyWidget");
	w->setStyleSheet(("QWidget#proxyWidget { background: transparent }"));
	w->setLayout(m_mainLay);
	m_form = scene()->addWidget(w);

// 	m_tool->hide();
// 	m_guitar->hide();
	
}


void TmainView::addNoteName(QWidget* name) {
	if (!m_name) {
		m_name = name;
		m_name->installEventFilter(this);
		m_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			m_scoreAndNameLay->insertStretch(0);
			m_scoreAndNameLay->addStretch(0);
			m_nameLay = new QBoxLayout(QBoxLayout::TopToBottom);
				m_nameLay->addStretch();
				m_nameLay->addWidget(m_name);
				m_nameLay->addStretch();
			m_scoreAndNameLay->addLayout(m_nameLay);
			m_scoreAndNameLay->addStretch(0);
// 		m_score->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	}
}


void TmainView::takeNoteName() {
	if (m_name) {
		m_nameLay->removeWidget(m_name);
		delete m_nameLay;
		m_name = 0;
		delete m_scoreAndNameLay->takeAt(0);
		delete m_scoreAndNameLay->takeAt(1);
		delete m_scoreAndNameLay->takeAt(1);
	}
}


void TmainView::addExamViews(QWidget* resultsW, QWidget* progressW) {
	m_results = resultsW;
	m_progress = progressW;
	m_results->hide();
	m_progress->hide();
	m_resultLay = new QHBoxLayout;
		m_resultLay->addWidget(m_progress);
		m_resultLay->addWidget(m_results);
		m_mainLay->insertLayout(2, m_resultLay);
	resultsW->installEventFilter(this);
	progressW->installEventFilter(this);
}


void TmainView::takeExamViews() {
	delete m_results;
	delete m_progress;
}

/** We are assuming that vertical layout of exam views can occur only when note name is already there.
 * Also there is no other way back than remove (delete) exam views at all. */
void TmainView::moveExamToName() {
	m_progress->show();
	m_results->show();
	if (m_nameLay && m_resultLay && m_resultLay->direction() == QBoxLayout::LeftToRight) {
		m_mainLay->removeItem(m_resultLay);
		m_resultLay->setDirection(QBoxLayout::TopToBottom);
		m_nameLay->insertLayout(0, m_resultLay);
	}
}

//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

void TmainView::resizeEvent(QResizeEvent* event) {
	Q_UNUSED (event)
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






