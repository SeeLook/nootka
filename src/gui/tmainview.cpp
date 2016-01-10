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
#include "mainwindow.h"
#if defined (Q_OS_ANDROID)
  #include "tmaterialmenu.h"
  #include <widgets/tmelodyitem.h>
  #include <touch/ttouchmenu.h>
#endif
#include <widgets/tpitchview.h>
#include <guitar/tguitarview.h>
#include <animations/tcombinedanim.h>
#include <tlayoutparams.h>
#include <touch/ttouchproxy.h>
// #include <touch/ttouchparams.h>
#include <tpath.h>
#include <tmtr.h>
#include <graphics/tdropshadoweffect.h>
#include <notename/tnotename.h>
#include <QtWidgets/QtWidgets>


TmainView::TmainView(TlayoutParams* layParams, TtoolBar* toolW, QWidget* statLabW, TpitchView* pitchW,
                     QGraphicsView* scoreW, QGraphicsView* guitarW, TnoteName* name, MainWindow* parent) :
	QGraphicsView(parent),
	m_mainWindow(parent),
	m_layParams(layParams),
	m_tool(toolW),
	m_status(statLabW),
	m_pitch(pitchW),
	m_score(scoreW),
	m_guitar(guitarW),
	m_touchedWidget(0),
	m_name(name),
	m_progress(0),
	m_results(0),
#if defined (Q_OS_ANDROID)
  m_menuItem(0),
#endif
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
  m_menuItem = new TmelodyItem();
  scene()->addItem(m_menuItem);
  m_menuItem->setPos(0, 0);
  connect(m_menuItem, &TmelodyItem::menuSignal, this, &TmainView::mainMenuExec);
#endif
  if (TtouchProxy::touchEnabled()) {
    m_fretView = new TguitarView(m_guitar, this);
#if defined (Q_OS_ANDROID)
    QTimer::singleShot(1000, this, [this]{ m_fretView->checkIsPreview(); }); // check it after all sizes were set
#endif
  }
}


TmainView::~TmainView()
{
  if (TtouchProxy::touchEnabled())
    delete m_fretView;
}


#if defined (Q_OS_ANDROID)
QList<QAction*>* TmainView::flyActions() {
  return m_menuItem->actions();
}
#endif


void TmainView::addNoteName() {
	if (!m_nameLay) {
#if defined (Q_OS_ANDROID)
    m_mainLay->setContentsMargins(0, 0, 0, 0);
#else
    m_mainLay->setContentsMargins(7, 2, 7, 2);
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
  if (!m_resultLay) {
    m_resultLay = new QBoxLayout(QBoxLayout::LeftToRight);
    m_results->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
#if defined (Q_OS_ANDROID)
    m_resultLay->setContentsMargins(0, 5, 0, 0);
    auto spacerW = new QWidget(resultsW);
    spacerW->setFixedSize(Tmtr::fingerPixels() * 0.7, 5);
    m_resultLay->addWidget(spacerW); // space for menu dots, also makes nice gap above exam views when placed above note name
#endif
  }
  m_resultLay->addWidget(m_progress);
  m_resultLay->addWidget(m_results);
  m_mainLay->insertLayout(isAutoHide() ? 1 : 2, m_resultLay);
#if !defined (Q_OS_ANDROID)
	resultsW->installEventFilter(this);
	progressW->installEventFilter(this);
#endif
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
  if (isAutoHide() && ob->objectName() == QLatin1String("toolBar") && event->type() == QEvent::Leave)
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
#if defined (Q_OS_ANDROID)
  m_mainMenuTap = false;
  TmaterialMenu menu(this);
  menu.setAboutAction(m_tool->aboutSimpleAct);
  if (m_progress && m_results) { // exam/exercise is pending
    if (m_tool->checkAct && m_tool->checkAct->isVisible())
      menu.addAction(m_tool->checkAct);
    if (m_tool->nextQuestAct && m_tool->nextQuestAct->isVisible())
      menu.addAction(m_tool->nextQuestAct);
    if (m_tool->prevQuestAct && m_tool->prevQuestAct->isVisible())
      menu.addAction(m_tool->prevQuestAct);
    if (m_tool->attemptAct && m_tool->attemptAct->isVisible())
      menu.addAction(m_tool->attemptAct);
    if (m_tool->correctAct && m_tool->correctAct->isVisible())
      menu.addAction(m_tool->correctAct);
    if (m_tool->repeatSndAct && m_tool->repeatSndAct->isVisible())
      menu.addAction(m_tool->repeatSndAct);
    if (m_tool->tuneForkAct && m_tool->tuneForkAct->isVisible())
      menu.addAction(m_tool->tuneForkAct);
  }
  if (m_menuItem->audioOutEnabled() && m_tool->playMelody()->isVisible())
    menu.addAction(m_tool->playMelody());
  if (m_tool->recordMelody()->isVisible())
    menu.addAction(m_tool->recordMelody());
  if (m_menuItem->audioInEnabled() && m_pitch->pauseAction()->isVisible())
    menu.addAction(m_pitch->pauseAction());
  menu.addAction(m_tool->startExamAct);
  menu.addAction(m_tool->levelCreatorAct);
  QAction scoreMenuAct(QIcon(Tpath::img("score")), tr("score menu"), this);
  connect(&scoreMenuAct, &QAction::triggered, this, &TmainView::scoreMenuExec);
  if (!m_nameLay) // multi notes mode
    menu.addAction(&scoreMenuAct);
  menu.addAction(m_tool->settingsAct);
  QAction closeAct(QIcon(QLatin1String(":/mobile/exit.png")), QApplication::translate("QShortcut", "Close"), this);
  connect(&closeAct, &QAction::triggered, parentWidget(), &QWidget::close);
  menu.addAction(&closeAct);
  auto a = menu.exec();
  if (a)
    a->trigger();
#endif
}


void TmainView::scoreMenuExec() {
#if defined (Q_OS_ANDROID)
  m_scoreMenuTap = false;
  TtouchMenu menu(this);
  menu.setGraphicsEffect(new TdropShadowEffect());
  menu.addAction(m_tool->generateMelody());
  menu.addAction(m_tool->scoreShowNames());
  menu.addAction(m_tool->scoreExtraAccids());
  menu.addAction(m_tool->scoreZoomIn());
  menu.addAction(m_tool->scoreZoomOut());
  menu.addAction(m_tool->scoreDeleteAll());
  menu.exec(QPoint(width() - menu.sizeHint().width() - 2, 2), QPoint(width(), 2));
#endif
}


bool TmainView::viewportEvent(QEvent *event) {
  if (TtouchProxy::touchEnabled()) {
#if defined (Q_OS_ANDROID)
    if (m_menuItem && !m_menuItem->isTouched()) { // ignore touch propagation when melody item was touched
#endif
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
      QTouchEvent *te = static_cast<QTouchEvent*>(event);
// 1.  Main widget of view was touched
      if (itemAt(mapFromScene(te->touchPoints().first().startPos())) == m_proxy) {
// 1.1 with one finger
          if (m_mainMenuTap || te->touchPoints().first().pos().x() < Tmtr::fingerPixels() / 3) {
// 1.1.1 on the left screen edge - main menu
              if (event->type() == QEvent::TouchBegin) {
                m_mainMenuTap = true;
              } else if (event->type() == QEvent::TouchEnd) {
                  if (m_mainMenuTap && te->touchPoints().first().pos().x() > width() * 0.15)
                    QTimer::singleShot(10, this, SLOT(mainMenuExec()));
                  else
                    m_mainMenuTap = false;
              }
              return true;
// 1.1.2 on the right screen edge - score menu
          } else if (!m_nameLay && // // multi notes mode
                     (m_scoreMenuTap || te->touchPoints().first().pos().x() > width() - Tmtr::fingerPixels() / 3)) {
              if (event->type() == QEvent::TouchBegin) {
                m_scoreMenuTap = true;
              } else if (event->type() == QEvent::TouchEnd) {
                if (m_scoreMenuTap && te->touchPoints().first().pos().x() < width() * 0.85)
                    QTimer::singleShot(10, this, SLOT(scoreMenuExec()));
                  else
                    m_scoreMenuTap = false;
              }
              return true;
          } else if (m_touchedWidget == m_score->viewport() ||
                      m_container->childAt(mapFromScene(te->touchPoints().first().startPos())) == m_score->viewport()) {
// 1.1.4 score was touched
              if (m_fretView->isVisible()) { // but check first is fret view visible - handle it then
                if (m_fretView->touchStartedFromView())
                  m_fretView->mapTouchEvent(te);
                else
                  m_fretView->hide();
              }
// mapping all touches to score
//               if (TtouchParams::i()->scoreWasTouched) {
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
                    m_touchedWidget = m_score->viewport();
                  }
                  if (event->type() == QEvent::TouchEnd)
                    m_touchedWidget = 0;
                  return true;
//               } else {
//                   m_mainWindow->setStatusMessage(TtouchProxy::touchScoreHelp(), 0);
//                   TtouchParams::i()->scoreWasTouched = true;
//               }
// 1.1.5 guitar was touched
          } else if (m_fretView->guitarEnabled() &&
                      (m_fretView->isVisible() || m_touchedWidget == m_guitar->viewport() ||
                      m_container->childAt(mapFromScene(te->touchPoints().first().startPos())) == m_guitar->viewport())) {
//               if (m_fretView->isPreview() && !TtouchParams::i()->guitarWasTouched) {
//                 m_mainWindow->setStatusMessage(TtouchProxy::touchGuitarHelp(), 0);
//                 TtouchParams::i()->guitarWasTouched = true;
//                 return false;
//               }
              if (event->type() == QEvent::TouchEnd) {
                m_touchedWidget = 0;
              } else
                  m_touchedWidget = m_guitar->viewport();
              return m_fretView->mapTouchEvent(te);
          }
// 2. Other temporary item was touched
      } else if (m_fretView && m_fretView->guitarEnabled() && itemAt(te->touchPoints().first().startPos().toPoint()) == m_fretView->proxy()) {
          m_fretView->setTouched();
          return m_fretView->mapTouchEvent(te);
      }
    }
#if defined (Q_OS_ANDROID)
  } // CLOSES: if (!m_menuItem->isTouched())
#endif
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


