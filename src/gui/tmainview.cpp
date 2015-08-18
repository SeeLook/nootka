/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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
#if defined (Q_OS_ANDROID)
  #include <widgets/tmelodyitem.h>
#endif
#include <widgets/tpitchview.h>
#include <guitar/tguitarview.h>
#include <animations/tcombinedanim.h>
#include <tlayoutparams.h>
#include <touch/ttouchproxy.h>
#include <touch/ttouchmenu.h>
#include <tpath.h>
#include <tmtr.h>
#include <notename/tnotename.h>
#include <QtWidgets>


TmainView::TmainView(TlayoutParams* layParams, TtoolBar* toolW, QWidget* statLabW, TpitchView* pitchW,
                     QGraphicsView* scoreW, QGraphicsView* guitarW, TnoteName* name, QWidget* parent) :
	QGraphicsView(parent),
	m_layParams(layParams),
	m_tool(toolW),
	m_status(statLabW),
	m_pitch(pitchW),
	m_score(scoreW),
	m_guitar(guitarW),
	m_touchedWidget(0),
	m_name(name),
	m_nameLay(0),
	m_mainMenuTap(false), m_scoreMenuTap(false), m_playBarTap(false)
{
	setScene(new QGraphicsScene(this));
	
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
	setObjectName("TmainView");
	setStyleSheet(("QGraphicsView#TmainView { background: transparent }"));
#if !defined (Q_OS_ANDROID) // no status messages under Android
	toolW->installEventFilter(this);
	pitchW->installEventFilter(this);
	guitarW->installEventFilter(this);
#endif
	toolW->setObjectName("toolBar");
	
	m_mainLay = new QBoxLayout(QBoxLayout::TopToBottom);
  #if defined (Q_OS_ANDROID)
    m_mainLay->setContentsMargins(0, 0, 0, 0);
  #else
		m_mainLay->setContentsMargins(2, 2, 2, 2);
  #endif
		m_statAndPitchLay = new QBoxLayout(QBoxLayout::LeftToRight);
#if !defined (Q_OS_ANDROID)
      m_statAndPitchLay->addWidget(m_status);
#endif
		  m_statAndPitchLay->addWidget(m_pitch);
	m_mainLay->addLayout(m_statAndPitchLay);
		m_scoreAndNameLay = new QBoxLayout(QBoxLayout::LeftToRight);
			m_scoreAndNameLay->addWidget(m_score);
		m_mainLay->addLayout(m_scoreAndNameLay);
		m_mainLay->addWidget(m_guitar);
	   m_container = new QWidget;
     m_score->setParent(m_container);
     m_guitar->setParent(m_container);
	   m_container->setObjectName("proxyWidget");
	   m_container->setStyleSheet(("QWidget#proxyWidget { background: transparent }"));
	   m_container->setLayout(m_mainLay);
	m_proxy = scene()->addWidget(m_container);

  m_isAutoHide = !m_layParams->toolBarAutoHide; // revert to activate it first time
	setBarAutoHide(m_layParams->toolBarAutoHide);
	m_name->createNameTip(scene());
	
	connect(Tmenu::menuHandler(), &TmenuHandler::menuShown, this, &TmainView::menuSlot);
#if defined (Q_OS_ANDROID)
  TmelodyItem *melItem = new TmelodyItem(m_tool->playMelody(), m_tool->recordMelody(), m_pitch->pauseAction());
  scene()->addItem(melItem);
  melItem->setPos(0, 0);
  connect(melItem, &TmelodyItem::scoreMenuSignal, this, &TmainView::scoreMenuExec);
  connect(melItem, &TmelodyItem::mainMenuSignal, this, &TmainView::mainMenuExec);
#endif
}


void TmainView::addNoteName() {
	if (!m_nameLay) {
    m_mainLay->setContentsMargins(7, 2, 7, 2);
#if !defined (Q_OS_ANDROID)
		m_name->installEventFilter(this);
#endif
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
    #if defined (Q_OS_ANDROID)
      m_mainLay->setContentsMargins(0, 0, 0, 0);
    #else
      m_mainLay->setContentsMargins(2, 2, 2, 2);
    #endif
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
		int examResultsPos = 2; // third layout row by default
		if (isAutoHide())
			examResultsPos--;
		m_mainLay->insertLayout(examResultsPos, m_resultLay);
#if !defined (Q_OS_ANDROID)
	resultsW->installEventFilter(this);
	progressW->installEventFilter(this);
#endif
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
				m_timerBar = new QTimer(this);
        connect(m_timerBar, &QTimer::timeout, this, &TmainView::showToolBar);
			} else {
				m_proxyBar->setWidget(m_tool);
			}
			QGraphicsDropShadowEffect *barBlur = new QGraphicsDropShadowEffect();
			barBlur->setColor(palette().highlight().color());
			barBlur->setOffset(0, 0);
			barBlur->setBlurRadius(15);
			m_proxyBar->setZValue(200);
			m_proxyBar->setGraphicsEffect(barBlur);
      if (TtouchProxy::touchEnabled())
        m_proxyBar->hide();
      else {
        m_proxyBar->setPos(20, 0); // move it from left side
        m_barLine->hide();
        updateBarLine();
        m_tool->setProxy(m_proxyBar);
        startHideAnim();
      }
		} else {
			if (m_proxyBar) {
				m_proxyBar->setWidget(0);
				m_proxyBar->setGraphicsEffect(0);
			}
			m_mainLay->insertWidget(0, m_tool);
			m_tool->setProxy(0);
		}
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

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
  m_proxy->setGeometry(0, 0, width(), height());
  scene()->setSceneRect(0, 0, width(), height());
  if (!TtouchProxy::touchEnabled())
    updateBarLine();
  m_container->resize(size());
  emit sizeChanged(size());
}


void TmainView::showToolBar() {
  if (!TtouchProxy::touchEnabled()) {
    m_timerBar->stop();
    m_proxyBar->show();
    qreal xx = qBound<qreal>(2, mapFromGlobal(cursor().pos()).x() - m_proxyBar->boundingRect().width() / 2,
                            width() - m_proxyBar->boundingRect().width() - 2);
    m_animBar->setMoving(QPointF(xx, -m_proxyBar->boundingRect().height()), QPointF(xx, 0.0));
    m_animBar->startAnimations();
  }
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

//##########################################################################################
//#######################     EVENTS       #################################################
//##########################################################################################

void TmainView::resizeEvent(QResizeEvent* event) {
  Q_UNUSED (event)
  QTimer::singleShot(0, this, SLOT(updateLayout()));
}


#if !defined (Q_OS_ANDROID)
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
  if (!TtouchProxy::touchEnabled() && isAutoHide()) {
    if (event->y() > 0 && event->y() < height() * 0.02) {
      if (!m_barLine->isVisible()) {
        m_barLine->show();
        m_timerBar->start(400);
      }
    } else {
        if (m_barLine->isVisible()) {
          m_barLine->hide();
        }
        m_timerBar->stop();
        if (m_proxyBar->pos().y() == 0.0 && event->y() > m_proxyBar->boundingRect().height())
          startHideAnim();
    }
  }
  QGraphicsView::mouseMoveEvent(event);
}
#endif


void TmainView::mainMenuExec() {
  m_mainMenuTap = false;
  TtouchMenu menu(this);
  menu.addAction(m_tool->settingsAct);
  menu.addAction(m_tool->levelCreatorAct);
  menu.addAction(m_tool->startExamAct);
  menu.addAction(m_tool->aboutSimpleAct);
  menu.exec(QPoint(2, 2), QPoint(-menu.sizeHint().width(), 2));
}


void TmainView::scoreMenuExec() {
  m_scoreMenuTap = false;
  TtouchMenu menu(this);
  menu.addAction(m_tool->generateMelody());
  menu.addAction(m_tool->scoreShowNames());
  menu.addAction(m_tool->scoreExtraAccids());
  menu.addAction(m_tool->scoreZoomIn());
  menu.addAction(m_tool->scoreZoomOut());
  menu.addAction(m_tool->scoreDeleteAll());
  menu.exec(QPoint(width() - menu.sizeHint().width() - 2, 2), QPoint(width(), 2));
}


TguitarView* guitarView = 0;
bool TmainView::viewportEvent(QEvent *event) {
  if (TtouchProxy::touchEnabled()) {
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
      QTouchEvent *te = static_cast<QTouchEvent*>(event);
// 1.  Main widget of view was touched
      if (itemAt(mapFromScene(te->touchPoints().first().pos())) == m_proxy) {
//         if (te->touchPoints().size() == 1) {
// 1.1 with one finger
          if (m_mainMenuTap || te->touchPoints().first().pos().x() < Tmtr::fingerPixels() / 3) {
// 1.1.1 on the left screen edge - main menu
              if (event->type() == QEvent::TouchBegin) {
                event->accept();
                m_mainMenuTap = true;
              } else if (event->type() == QEvent::TouchEnd) {
                  if (m_mainMenuTap && te->touchPoints().first().pos().x() > width() * 0.15)
                    QTimer::singleShot(10, this, SLOT(mainMenuExec()));
                  else
                    m_mainMenuTap = false;
              }
              return true;
// 1.1.2 on the right screen edge - score menu
          } else if (m_scoreMenuTap || te->touchPoints().first().pos().x() > width() - Tmtr::fingerPixels() / 3) {
              if (event->type() == QEvent::TouchBegin) {
                event->accept();
                m_scoreMenuTap = true;
              } else if (event->type() == QEvent::TouchEnd) {
                if (m_scoreMenuTap && te->touchPoints().first().pos().x() < width() * 0.85)
                    QTimer::singleShot(10, this, SLOT(scoreMenuExec()));
                  else
                    m_scoreMenuTap = false;
              }
              return true;
          } else if (m_touchedWidget == m_score->viewport() ||
                      m_container->childAt(mapFromScene(te->touchPoints().first().pos())) == m_score->viewport()) {
// 1.1.4 score was touched
              if (guitarView) {
                delete guitarView;
                guitarView = 0;
              }
// mapping all touches to score
              QList<QTouchEvent::TouchPoint> pointList;
              for (int i = 0; i < te->touchPoints().size(); ++i) {
                QTouchEvent::TouchPoint touchPoint(te->touchPoints()[i]);
                touchPoint.setPos(m_score->mapFromParent(touchPoint.pos().toPoint())); // map to score
                touchPoint.setLastPos(m_score->mapFromParent(touchPoint.lastPos().toPoint()));
//                 touchPoint.setStartPos(m_score->mapFromParent(touchPoint.startPos().toPoint())); // So far unused
                pointList << touchPoint;
              }
              QTouchEvent touchToSend(event->type(), te->device(), te->modifiers(), te->touchPointStates(), pointList);
              if (qApp->notify(m_score->viewport(), &touchToSend)) {
                event->accept();
                m_touchedWidget = m_score->viewport();
              }
              if (event->type() == QEvent::TouchEnd)
                m_touchedWidget = 0;
              return true;
// 1.1.5 guitar was touched
          } else if (m_touchedWidget == m_guitar->viewport() ||
                      m_container->childAt(mapFromScene(te->touchPoints().first().pos())) == m_guitar->viewport()) {
                event->accept();
                m_touchedWidget = m_guitar->viewport();
              if (event->type() == QEvent::TouchEnd)
                m_touchedWidget = 0;
              return true;
          }
/*        } else if (te->touchPoints().size() == 2) {
// 1.2 two fingers touch
            if (event->type() == QEvent::TouchUpdate) { // TouchBegin occurs when first finger touches size() == 1
              if (m_touchedWidget == m_guitar->viewport()) {
// 1.2.2 guitar double touched - bigger preview of fingerboard
                  if (guitarView) {
                    guitarView->horizontalScrollBar()->setValue(
                        guitarView->horizontalScrollBar()->value() + (te->touchPoints()[0].lastPos().x() - te->touchPoints()[0].pos().x()));
                  } else if (qAbs(te->touchPoints()[0].pos().y() - te->touchPoints()[0].startPos().y()) > height() / 4) {
                    if (!guitarView) {
                      guitarView = new TguitarView(m_guitar, this);
                      guitarView->horizontalScrollBar()->setValue(te->touchPoints().first().pos().x() /
                        ((guitarView->horizontalScrollBar()->maximum() - guitarView->horizontalScrollBar()->minimum()) / width()));
                    }
                  }
                }
            }
        }*/
// 2. Other temporary item was touched
      } else if (guitarView && itemAt(te->touchPoints().first().pos().toPoint()) == guitarView->proxy()) {
          return guitarView->mapTouchEvent(te);
      }
    }
  }
  return QAbstractScrollArea::viewportEvent(event);
}


#if defined (Q_OS_ANDROID)
void TmainView::keyPressEvent(QKeyEvent* event) {
//   qDebug() << "pressed" << (Qt::Key)event->key();
  QGraphicsView::keyPressEvent(event);
}


void TmainView::keyReleaseEvent(QKeyEvent* event) {
  if ((Qt::Key)event->key() == Qt::Key_Menu)
    QTimer::singleShot(10, this, SLOT(mainMenuExec()));
  QGraphicsView::keyReleaseEvent(event);
}
#endif


