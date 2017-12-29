/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "tpitchview.h"
#include "tvolumeview.h"
#include "tintonationview.h"
// #include "tpcmview.h"
#include <tcolor.h>
#include <graphics/tnotepixmap.h>
#if defined (Q_OS_ANDROID)
  #include "widgets/tmelodyitem.h"
#endif
#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qstyle.h>
#include <QtCore/qdebug.h>


TpitchView::TpitchView(TaudioIN* audioIn, QWidget* parent, bool pauseActive) :
  QWidget(parent),
  m_audioIN(audioIn),
  m_pitchColor(Qt::red),
  m_bgColor(Qt::transparent),
  m_hideCnt(8),
  m_prevState((int)TaudioIN::e_paused),
  m_pauseActive(pauseActive)
{
	QHBoxLayout *outLay = new QHBoxLayout;
  m_lay = new QBoxLayout(QBoxLayout::TopToBottom);
    
  m_intoView = new TintonationView(TintonationView::e_perfect, this);
		m_intoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
  m_volumeView = new TvolumeView(this);
		m_volumeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_volumeView->setPauseActive(pauseActive);

//   m_pcmView = new TpcmView(this);
  
	m_lay->addWidget(m_intoView, 0, Qt::AlignBottom);
	m_lay->addWidget(m_volumeView);
//   m_lay->addWidget(m_pcmView);
	outLay->addLayout(m_lay);
  setLayout(outLay);
  
  m_watchTimer = new QTimer(this);
  connect(m_watchTimer, &QTimer::timeout, this, &TpitchView::updateLevel);
  if (pauseActive)
    connect(m_volumeView, SIGNAL(paused()), this, SLOT(pauseClicked()));
  connect(m_volumeView, SIGNAL(minimalVolume(float)), this, SLOT(minimalVolumeChanged(float)));
  connect(m_intoView, SIGNAL(accuracyChanged()), this, SLOT(accuracyChangedSlot()));
  connect(m_intoView, &TintonationView::animationFinished, this, &TpitchView::intoAnimFinished);
#if defined (Q_OS_ANDROID)
  m_pauseAct = new QAction(tr("Pitch recognition", "Android menu entry, could be 'Note recognition' or 'detection' as well"), this);
  m_pauseAct->setCheckable(true);
  connect(m_pauseAct, &QAction::triggered, this, &TpitchView::pauseActionSlot);
  m_pauseAct->setChecked(false);
  pauseActionSlot();
#endif
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TpitchView::setAudioInput(TaudioIN* audioIn) {
  m_audioIN = audioIn;
  if (m_audioIN) {
      connect(m_audioIN, &TaudioIN::stateChanged, this, &TpitchView::inputStateChanged);
      connect(m_audioIN, &TaudioIN::destroyed, this, &TpitchView::inputDeviceDeleted);
      // Redirect signals, send appropriate messages
      setDisabled(false);
  } else
      inputDeviceDeleted();
}


void TpitchView::watchInput() {
  if (isEnabled() && isVisible() && m_audioIN && m_audioIN->detectingState() == TaudioIN::e_detecting && !m_watchTimer->isActive()) {
    m_prevPitch = -1.0;
    m_watchTimer->start(75);
    connect(m_audioIN, &TaudioIN::noteStarted, this, &TpitchView::noteSlot);
    if (m_intoView->accuracy() == TintonationView::e_noCheck && !m_intoView->accuracyChangeEnabled())
      m_intoView->setDisabled(true);
    else
      m_intoView->setDisabled(false);
  }
}


void TpitchView::stopWatching() {
  if (m_watchTimer->isActive()) {
    m_watchTimer->stop();
    disconnect(m_audioIN, &TaudioIN::noteStarted, this, &TpitchView::noteSlot);
    m_volumeView->setVolume(0.0);
    m_volumeView->setVolume(0.0); // it has to be called twice to reset
    if (m_intoView->isEnabled())
      m_intoView->pitchSlot(0.0);
  }
  m_intoView->setDisabled(true);
}


void TpitchView::setPitchColor(QColor col) {
  m_pitchColor = col;
  m_volumeView->setPitchColor(col);
}


void TpitchView::setMinimalVolume(float vol) {
  m_volumeView->setMinimalVolume(vol);
}


float TpitchView::minimalVolume() {
  return m_audioIN ? m_audioIN->minimalVolume() : 0.0;
}


void TpitchView::setDisabled(bool isDisabled) {
  QWidget::setDisabled(isDisabled);
  if (isDisabled)
    stopWatching();
  else
    watchInput();
  m_volumeView->setDisabled(isDisabled);
  if (isDisabled) {// it can be only disabled here, watchInput() enables it if it is possible
		m_intoView->setDisabled(isDisabled);
    m_hideCnt = 8;
  }
  enableAccuracyChange(m_intoView->accuracyChangeEnabled()); // refresh status tip state
}


void TpitchView::setIntonationAccuracy(int accuracy) {
  m_intoView->setAccuracy(accuracy);
}


int TpitchView::intonationAccuracy() {
  return static_cast<int>(m_intoView->accuracy());
}


void TpitchView::markAnswer(const QColor& col) {
  if (col == Qt::transparent)
    setBgColor(col);
  else
    setBgColor(Tcolor::merge(col, palette().window().color()));
	update();
}


void TpitchView::outOfTuneAnim(float outTune, int duration) {
	setBgColor(palette().window().color());
	m_intoView->outOfTuneAnim(outTune, duration);
}


bool TpitchView::isPaused() {
  if (m_audioIN && m_volumeView->isPauseActive())
    return m_volumeView->isPaused();
  return false;
}


void TpitchView::setDirection(QBoxLayout::Direction dir) {
	if (dir != m_lay->direction()) {
		m_lay->setDirection(dir);
	}
}


bool TpitchView::isAccuracyChangeEnabled() {
  return m_intoView->accuracyChangeEnabled();
}


void TpitchView::enableAccuracyChange(bool enAcc) {
  m_intoView->setAccuracyChangeEnabled(enAcc);
  m_intoView->setStatusTip(tr("Intonation - clarity of the sound. Is it in tune."));
  if (enAcc && m_intoView->isEnabled())
    m_intoView->setStatusTip(m_intoView->statusTip() + QLatin1String("<br>") + tr("Click note symbol to change it."));
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TpitchView::noteSlot() {
  m_hideCnt = 0;
}


void TpitchView::updateLevel() {
  int a = 0;
  if (m_hideCnt < 8)
    switch (m_hideCnt) {
      case 0 : a = 255; break;
      case 1 : a = 225; break;
      case 2 : a = 200; break;
      case 3 : a = 175; break;
      case 4 : a = 150; break;
      case 5 : a = 110; break;
      case 6 : a = 75;  break;
      case 7 : a = 40;  break;
    }
  m_hideCnt++;
	m_volumeView->setVolume(m_audioIN->volume(), a);
	if (m_intoView->accuracy() != TintonationView::e_noCheck && m_prevPitch != m_audioIN->lastChunkPitch())
			m_intoView->pitchSlot(m_audioIN->lastChunkPitch());
	m_prevPitch = m_audioIN->lastChunkPitch();
//   m_pcmView->setPCMvolume(m_audioIN->pcmVolume());
}


void TpitchView::pauseClicked() {
	if (m_audioIN) {
		if (m_volumeView->isPaused()) {
			m_audioIN->stopListening();
			m_audioIN->setStoppedByUser(true);
		} else {
			m_audioIN->setStoppedByUser(false);
			m_audioIN->startListening();
		}
		enableAccuracyChange(m_intoView->accuracyChangeEnabled()); // refresh status tip state
	}	
}

#if defined (Q_OS_ANDROID)
void TpitchView::pauseActionSlot() {
  m_volumeView->setPaused(!m_pauseAct->isChecked());
  updatePauseActIcon();
  pauseClicked();
}


void TpitchView::updatePauseActIcon() {
  QFont nf(QStringLiteral("nootka"), style()->pixelMetric(QStyle::PM_SmallIconSize));
  m_pauseAct->setIcon(QIcon(pixFromString(
          m_volumeView->isPaused() ? QStringLiteral("o") : QStringLiteral("n"), nf, qApp->palette().highlight().color())));
}

#endif


void TpitchView::minimalVolumeChanged(float minVol) {
  m_audioIN->setMinimalVolume(minVol);
}


void TpitchView::inputStateChanged(int inSt) {
  if (isEnabled() && inSt != m_prevState) {
    TaudioIN::Estate inState = (TaudioIN::Estate)inSt;
    if (m_volumeView->isPauseActive()) {
      if (inState == TaudioIN::e_stopped) {
				m_volumeView->setPaused(true);
        stopWatching();
      } else if (inState == TaudioIN::e_detecting) {
        m_volumeView->setPaused(false);
        watchInput();
      }
    }
    m_prevState = inSt;
  }
#if defined (Q_OS_ANDROID)
  if (TmelodyItem::instance())
    TmelodyItem::instance()->setListening((TaudioIN::Estate)inSt == TaudioIN::e_detecting);
  updatePauseActIcon();
#endif
}

void TpitchView::inputDeviceDeleted() {
  setDisabled(true);
}


void TpitchView::accuracyChangedSlot() {
  if (m_audioIN)
    m_audioIN->setIntonationAccuracy((quint8)m_intoView->accuracy());
}


void TpitchView::intoAnimFinished() {
  emit correctingFinished();
}


//#################################################################################################
//###################              EVENTS              ############################################
//#################################################################################################

void TpitchView::paintEvent(QPaintEvent* ) {
  if (m_bgColor != Qt::transparent) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(m_bgColor));
    painter.drawRect(painter.viewport());
  }
}


  void TpitchView::resizeEvent(QResizeEvent*) {
  if (m_lay->direction() == QBoxLayout::TopToBottom || m_lay->direction() == QBoxLayout::BottomToTop) {
      m_volumeView->setFixedHeight(height() * 0.35);
      m_intoView->setFixedHeight(height() * 0.35);
  } else {
      m_volumeView->setFixedHeight(contentsRect().height() * 0.9);
      m_intoView->setFixedHeight(contentsRect().height() * 0.9);
  }
}


void TpitchView::showEvent(QShowEvent* e) {
	if (!isPaused() && m_audioIN)
		watchInput();
	QWidget::showEvent(e);
}


void TpitchView::hideEvent(QHideEvent* e) {
	if (!isPaused() && m_audioIN)
		stopWatching();
	QWidget::hideEvent(e);
}












