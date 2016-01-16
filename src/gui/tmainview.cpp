/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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
#include "ttoolbar.h"
#include "tmenu.h"
#include <animations/tcombinedanim.h>
#include <tlayoutparams.h>
#include <notename/tnotename.h>
#include <QtWidgets>


TmainView::TmainView(TlayoutParams* layParams, QWidget* toolW, QWidget* statLabW, QWidget* pitchW,
										 QWidget* scoreW, QWidget* guitarW, TnoteName* name, QWidget* parent) :
	QGraphicsView(parent),
	m_layParams(layParams),
	m_tool(toolW),
	m_status(statLabW),
	m_pitch(pitchW),
	m_score(scoreW),
	m_guitar(guitarW),
	m_name(name),
	m_nameLay(0)
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
	toolW->setObjectName("toolBar");
	
	m_mainLay = new QBoxLayout(QBoxLayout::TopToBottom);
		m_mainLay->setContentsMargins(2, 2, 2, 2);
		m_statAndPitchLay = new QBoxLayout(QBoxLayout::LeftToRight);
		  m_statAndPitchLay->addWidget(m_status);
		  m_statAndPitchLay->addWidget(m_pitch);
	m_mainLay->addLayout(m_statAndPitchLay);
		m_scoreAndNameLay = new QBoxLayout(QBoxLayout::LeftToRight);
			m_scoreAndNameLay->addWidget(m_score);
		m_mainLay->addLayout(m_scoreAndNameLay);
		m_mainLay->addWidget(m_guitar);
	   m_container = new QWidget;
	   m_container->setObjectName("proxyWidget");
	   m_container->setStyleSheet(("QWidget#proxyWidget { background: transparent }"));
	   m_container->setLayout(m_mainLay);
	m_proxy = scene()->addWidget(m_container);
	m_isAutoHide = !m_layParams->toolBarAutoHide; // revert to activate it first time
	setBarAutoHide(m_layParams->toolBarAutoHide);
	m_name->createNameTip(scene());
	
	connect(Tmenu::menuHandler(), &TmenuHandler::menuShown, this, &TmainView::menuSlot);
}


void TmainView::addNoteName() {
	if (!m_nameLay) {
    m_mainLay->setContentsMargins(7, 2, 7, 2);
		m_name->installEventFilter(this);
		m_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_name->setParent(0);
		m_name->enableArrows(false);
			m_nameLay = new QBoxLayout(QBoxLayout::TopToBottom);
				m_nameLay->addStretch();
				m_nameLay->addWidget(m_name);
				m_nameLay->addStretch();
			m_scoreAndNameLay->addLayout(m_nameLay);
		m_name->show();
	}
}


void TmainView::takeNoteName() {
	if (m_nameLay) {
		m_nameLay->removeWidget(m_name);
		delete m_nameLay;
		m_nameLay = 0;
		m_name->hide();
		m_name->enableArrows(true);
    m_mainLay->setContentsMargins(2, 2, 2, 2);
	}
}


void TmainView::addExamViews(QWidget* resultsW, QWidget* progressW) {
	m_results = resultsW;
	m_progress = progressW;
	m_results->hide();
	m_progress->hide();
  if (!m_resultLay) {
    m_resultLay = new QBoxLayout(QBoxLayout::LeftToRight);
    m_results->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  }
		m_resultLay->addWidget(m_progress);
		m_resultLay->addWidget(m_results);
		int examResultsPos = 2; // third layout row by default
		if (isAutoHide())
			examResultsPos--;
		m_mainLay->insertLayout(examResultsPos, m_resultLay);
	resultsW->installEventFilter(this);
	progressW->installEventFilter(this);
}


void TmainView::takeExamViews() {
	delete m_results;
	delete m_progress;
  delete m_resultLay;
}

/** We are assuming that vertical layout of exam views can occur only when note name is already there.
 * Also there is no other way back than remove (delete) exam views at all. */
void TmainView::moveExamToName() {
	m_progress->show();
  m_results->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_results->show();
	if (m_nameLay && m_resultLay && m_resultLay->direction() == QBoxLayout::LeftToRight) {
		m_mainLay->removeItem(m_resultLay);
		m_resultLay->setDirection(QBoxLayout::TopToBottom);
		m_nameLay->insertLayout(0, m_resultLay);
	}
}


void TmainView::setBarAutoHide(bool autoHide) {
	if (autoHide != m_isAutoHide) {
		m_isAutoHide = autoHide;
		if (m_isAutoHide) {
			m_mainLay->removeWidget(m_tool);
			if (!m_proxyBar) {
				m_proxyBar = scene()->addWidget(m_tool);
				m_barLine = new QGraphicsLineItem;
				scene()->addItem(m_barLine);
				m_barLine->setGraphicsEffect(new QGraphicsBlurEffect());
				m_animBar = new TcombinedAnim(m_proxyBar, this);
				m_timer = new QTimer(this);
				connect(m_timer, &QTimer::timeout, this, &TmainView::showToolBar);
			} else {
				m_proxyBar->setWidget(m_tool);
			}
			QGraphicsDropShadowEffect *barBlur = new QGraphicsDropShadowEffect();
			barBlur->setColor(palette().highlight().color());
			barBlur->setOffset(0, 0);
			barBlur->setBlurRadius(15);
			m_proxyBar->setZValue(200);
			m_proxyBar->setGraphicsEffect(barBlur);
			m_proxyBar->setPos(20, 0); // move it from left side
			m_barLine->hide();
			updateBarLine();
			static_cast<TtoolBar*>(m_tool)->setProxy(m_proxyBar);
      startHideAnim();
		} else {
			if (m_proxyBar) {
				m_proxyBar->setWidget(0);
				m_proxyBar->setGraphicsEffect(0);
			}
			m_mainLay->insertWidget(0, m_tool);
			static_cast<TtoolBar*>(m_tool)->setProxy(0);
		}
	}
}


//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

void TmainView::resizeEvent(QResizeEvent* event) {
	Q_UNUSED (event)
  QTimer::singleShot(0, this, SLOT(updateLayout()));
}


bool TmainView::eventFilter(QObject* ob, QEvent* event) {
	if (event->type() == QEvent::StatusTip) {
			QStatusTipEvent *tip = static_cast<QStatusTipEvent *>(event);
			emit statusTip(tip->tip());
	}
	if (isAutoHide() && ob->objectName() == "toolBar" && event->type() == QEvent::Leave)
			startHideAnim();
	return QAbstractScrollArea::eventFilter(ob, event);
}


void TmainView::mouseMoveEvent(QMouseEvent* event) {
	if (isAutoHide()) {
		if (event->y() > 0 && event->y() < height() * 0.02) {
			if (!m_barLine->isVisible()) {
				m_barLine->show();
				m_timer->start(400);
			}
		} else {
				if (m_barLine->isVisible()) {
					m_barLine->hide();
				}
				m_timer->stop();
				if (m_proxyBar->pos().y() == 0.0 && event->y() > m_proxyBar->boundingRect().height())
					startHideAnim();
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}


void TmainView::startHideAnim() {
	m_animBar->setMoving(m_proxyBar->pos(), QPointF(m_proxyBar->x(), -m_proxyBar->boundingRect().height() - 15));
	m_animBar->startAnimations();
}


void TmainView::updateBarLine() {
	if (m_proxyBar) {
			m_barLine->setLine(10.0, height() * 0.005, width() - 20.0, height() * 0.005);
			m_barLine->setPen(QPen(palette().highlight().color(), height() * 0.005));
	}
}


void TmainView::updateLayout() {
//   m_proxy->setGeometry(0, 0, width(), height());
  scene()->setSceneRect(0, 0, width(), height());
  updateBarLine();
  m_container->setFixedSize(size());
  emit sizeChanged(size());
}


void TmainView::showToolBar() {
	m_timer->stop();
	m_proxyBar->show();
	qreal xx = qBound<qreal>(2, mapFromGlobal(cursor().pos()).x() - m_proxyBar->boundingRect().width() / 2,
													 width() - m_proxyBar->boundingRect().width() - 2);
	m_animBar->setMoving(QPointF(xx, -m_proxyBar->boundingRect().height()), QPointF(xx, 0.0));
	m_animBar->startAnimations();
}


void TmainView::menuSlot(Tmenu* m) {
	int xOff = 0;
	if (isAutoHide()) {
		startHideAnim();
		xOff = m_proxyBar->pos().x();
	}
	QPoint scoreGlobalPos = mapToGlobal(m_score->pos());
  m->move(QCursor::pos().x() - 5, scoreGlobalPos.y());
}





