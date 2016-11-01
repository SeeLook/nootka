/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include "tbgpixmap.h"
#if defined (Q_OS_ANDROID)
  #include "tmaterialmenu.h"
  #include <widgets/tmelodyitem.h>
  #include <ttouchmenu.h>
  #include <touch/ttouchparams.h>
  #include <ttouchmessage.h>
  #include <widgets/tintestwidget.h>
#endif
#include <widgets/tpitchview.h>
#include <guitar/tguitarview.h>
#include <animations/tcombinedanim.h>
#include <tlayoutparams.h>
#include <touch/ttouchproxy.h>
#include <tpath.h>
#include <tmtr.h>
#include <qtr.h>
#include <graphics/tdropshadoweffect.h>
#include <notename/tnotename.h>
#include <QtWidgets/QtWidgets>


class TsceneHandler : public QGraphicsScene {

public:
  TsceneHandler(QObject* parent = 0) : QGraphicsScene(parent) {}

protected:
  virtual bool event(QEvent* event) {
    if (event->type() == QEvent::GraphicsSceneMousePress ||
        event->type() == QEvent::GraphicsSceneMouseRelease ||
        event->type() == QEvent::GraphicsSceneMouseMove) {
          qDebug() << "scene got mouse event" << event->type();
    }
    return QGraphicsScene::event(event);
  }
};


class TnameBgWidget : public QWidget
{
public:
  explicit TnameBgWidget(QWidget* parent = 0) : QWidget(parent) {}

protected:
  virtual void paintEvent(QPaintEvent* event) {
    if (!BG_PIX->isNull() && BG_PIX->rightHandedGuitar() && event->rect().bottomRight().x() >= BG_PIX->globalPos().x() - x()
                          && event->rect().bottomRight().y() >= BG_PIX->globalPos().y() - y()) {
      QPainter painter(this);
      painter.drawPixmap(BG_PIX->globalPos().x() - x() + 2, BG_PIX->globalPos().y() - y(), *BG_PIX);
    }
  }
};


TmainView* TmainView::m_instance = nullptr;


static TnameBgWidget *m_nameBgWidget = nullptr;
static QSpacerItem *m_nameSpacer = nullptr;


TmainView::TmainView(TlayoutParams* layParams, TtoolBar* toolW, QWidget* statLabW, TpitchView* pitchW,
                     QGraphicsView* scoreW, QGraphicsView* guitarW, TnoteName* name, QMainWindow* parent) :
  QGraphicsView(parent),
  m_mainWindow(parent),
  m_status(statLabW),
  m_score(scoreW),
  m_guitar(guitarW),
  m_results(nullptr),
  m_progress(nullptr),
  m_touchedWidget(nullptr),
  m_pitch(pitchW),
  m_name(name),
  m_tool(toolW),
  m_layParams(layParams),
  m_mainMenuTap(false), m_scoreMenuTap(false), m_playBarTap(false)
#if defined (Q_OS_ANDROID)
  ,m_menuItem(nullptr)
#endif
{
  if (m_instance) {
    qDebug() << "TmainView instance already exists";
    return;
  }

  m_instance = this;
// 	setScene(new TsceneHandler(this)); // for debugging
  setScene(new QGraphicsScene(this));

	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
	setObjectName("TmainView");
#if !defined (Q_OS_ANDROID) // no status messages under Android
	toolW->installEventFilter(this);
	pitchW->installEventFilter(this);
	guitarW->installEventFilter(this);
#endif
	toolW->setObjectName("toolBar");
	
	m_mainLay = new QBoxLayout(QBoxLayout::TopToBottom);
  m_mainLay->setSpacing(0);
  #if defined (Q_OS_ANDROID)
    m_mainLay->setContentsMargins(0, 0, 0, 0);
  #else
		m_mainLay->setContentsMargins(2, 0, 2, 0);
  #endif
		m_statAndPitchLay = new QBoxLayout(QBoxLayout::LeftToRight);
#if !defined (Q_OS_ANDROID)
      m_statAndPitchLay->addWidget(m_status);
#endif
		  m_statAndPitchLay->addWidget(m_pitch);
	m_mainLay->addLayout(m_statAndPitchLay);
		m_scoreAndNameLay = new QBoxLayout(QBoxLayout::LeftToRight);
			m_scoreAndNameLay->addWidget(m_score);
    m_mainLay->addSpacing(2);
		m_mainLay->addLayout(m_scoreAndNameLay);
		m_mainLay->addWidget(m_guitar);
	   m_container = new QWidget;
     m_score->setParent(m_container);
     m_guitar->setParent(m_container);
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

  m_inVolAct = new QAction(style()->standardIcon(QStyle::SP_MediaVolume), qTR("TsettingsDialog", "Sound"), this);
  connect(m_inVolAct, &QAction::triggered, this, &TmainView::showInVolume);
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
  m_instance = nullptr;
}


#if defined (Q_OS_ANDROID)
QList<QAction*>* TmainView::flyActions() {
  return m_menuItem->actions();
}
#endif


void TmainView::addNoteName() {
  if (!m_nameLay) {
#if !defined (Q_OS_ANDROID)
    m_name->installEventFilter(this);
#endif
    m_name->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_name->setParent(0);
    m_name->enableArrows(false);
    m_nameBgWidget = new TnameBgWidget(m_container);
    m_nameLay = new QBoxLayout(QBoxLayout::TopToBottom);
    m_nameLay->setContentsMargins(0, 0, 0, 0);
    m_nameLay->addStretch();
#if defined (Q_OS_ANDROID)
    m_name->setMinimumWidth(qRound(width() * 0.4));
    m_nameLay->addWidget(m_name, 0, Qt::AlignCenter);
#else
    m_nameLay->addWidget(m_name);
#endif
    m_nameLay->addStretch();
    m_nameBgWidget->setLayout(m_nameLay);
    m_nameBgWidget->setContentsMargins(0, 0, 0, 0);
    m_nameSpacer = new QSpacerItem(5, 5);
    m_scoreAndNameLay->addSpacerItem(m_nameSpacer);
    m_scoreAndNameLay->addWidget(m_nameBgWidget);
    m_name->show();
  }
}


void TmainView::takeNoteName() {
	if (m_nameLay) {
		m_nameLay->removeWidget(m_name);
    m_name->setParent(0);
    delete m_nameBgWidget;
    m_scoreAndNameLay->removeItem(m_nameSpacer);
    delete m_nameSpacer;
    m_nameLay = nullptr;
		m_name->hide();
		m_name->enableArrows(true);
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
    spacerW->setFixedSize(qRound(Tmtr::fingerPixels() * 0.7), 5);
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
  m_results = nullptr;
  delete m_progress;
  m_progress = nullptr;
  delete m_resultLay;
  m_resultLay = nullptr;
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
  scene()->setSceneRect(0, 0, width(), height());
  if (!TtouchProxy::touchEnabled())
    updateBarLine();
  m_container->setFixedSize(size());
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
  if (isAutoHide())
    startHideAnim();
  QPoint scoreGlobalPos = mapToGlobal(m_score->pos());
  m->move(QCursor::pos().x() - 5, scoreGlobalPos.y());
}


#if defined (Q_OS_ANDROID)
void TmainView::showInVolume() {
  if (m_pitch->isEnabled())
    m_pitch->setDisabled(true);

  m_inVolWidget = new TinTestWidget(this);
  connect(m_inVolWidget, &TinTestWidget::exit, this, &TmainView::inVolExit);
  m_inVolWidget->exec();
}


void TmainView::inVolExit(int exMessage) {
  if (static_cast<TinTestWidget::EexitMessage>(exMessage) == TinTestWidget::e_accepted) {
    m_pitch->setMinimalVolume(m_inVolWidget->pitchView()->minimalVolume());
    m_pitch->setIntonationAccuracy(m_inVolWidget->pitchView()->intonationAccuracy());
  }
  if (m_layParams->soundViewEnabled)
    m_pitch->setDisabled(false);
  m_inVolWidget->deleteLater();
  m_inVolWidget = nullptr;
}

#endif

//##########################################################################################
//#######################     EVENTS       #################################################
//##########################################################################################

void TmainView::resizeEvent(QResizeEvent* event) {
  Q_UNUSED (event)
  updateLayout();
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
  if (!m_nameLay) { // multi notes mode
    connect(&scoreMenuAct, &QAction::triggered, this, &TmainView::scoreMenuExec);
    menu.addAction(&scoreMenuAct);
  }
  if (!m_layParams->soundViewEnabled)
    menu.addAction(m_inVolAct);
  menu.addAction(m_tool->settingsAct);
  QAction closeAct(QIcon(QLatin1String(":/mobile/exit.png")), qTR("QShortcut", "Close"), this);
  connect(&closeAct, &QAction::triggered, parentWidget(), &QWidget::close);
  menu.addAction(&closeAct);
  auto a = menu.exec();
  if (a)
    a->trigger();
  else {
    if (!TOUCHPARAMS->initialAnimAccepted && !(m_name && m_name->isVisible()) && !m_results) {
      // when menu touched first time - display help,
      // but only in full score mode and not during exercise and if user doesn't select any action from menu
      m_score->setDisabled(true); // disable clef and key signature
      connect(m_menuItem, &TmelodyItem::previewFinished, [=]{ m_score->setDisabled(false); });
      m_menuItem->initialAnim();
      TOUCHPARAMS->initialAnimAccepted = true;
    }
  }
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
  menu.exec(QPoint(0, 2), QPoint(- menu.sizeHint().width(), 2));
#endif
}


#if defined (Q_OS_ANDROID)
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
#if defined (Q_OS_ANDROID)
              if (TtouchParams::i()->scoreWasTouched) {
#endif
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
#if defined (Q_OS_ANDROID)
              } else if (!tMessage->isVisible()) { // display hint about score but only when no other message is visible
                  tMessage->setMessage(TtouchProxy::touchScoreHelp(), 0);
                  TtouchParams::i()->scoreWasTouched = true;
              }
#endif
// 1.1.5 guitar was touched
          } else if (m_fretView->guitarEnabled() &&
                      (m_fretView->isVisible() || m_touchedWidget == m_guitar->viewport() ||
                      m_container->childAt(mapFromScene(te->touchPoints().first().startPos())) == m_guitar->viewport())) {
              if (event->type() == QEvent::TouchEnd)
                  m_touchedWidget = 0;
              else
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
#endif

#if defined (Q_OS_ANDROID)
void TmainView::keyPressEvent(QKeyEvent* event) {
//   qDebug() << "pressed" << static_cast<Qt::Key>(event->key());
  QGraphicsView::keyPressEvent(event);
}


void TmainView::keyReleaseEvent(QKeyEvent* event) {
  auto k = static_cast<Qt::Key>(event->key());
  if (k == Qt::Key_Menu)
    QTimer::singleShot(10, this, SLOT(mainMenuExec()));
  else if (k == Qt::Key_VolumeDown || k == Qt::Key_VolumeUp) {
    if (!m_inVolWidget)
      QTimer::singleShot(10, [=]{ showInVolume(); });
  }
  QGraphicsView::keyReleaseEvent(event);
}
#endif


