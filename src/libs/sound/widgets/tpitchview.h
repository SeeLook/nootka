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


#ifndef TPITCHVIEW_H
#define TPITCHVIEW_H

#include "nootkasoundglobal.h"
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <music/tnote.h>
#if defined (Q_OS_ANDROID)
  #include "tqtaudioin.h"
#else
  #include "trtaudioin.h"
#endif


class QCheckBox;
class QBoxLayout;
class TvolumeView;
class TintonationView;
// class TpcmView;
class QTimer;
class QAction;


/** 
 * This class manages of displaying volume meter and intonation indicator.
 * When audio input is set through @p setAudioInput() 
 * it grabs volume and intonation with timer event loop,
 * but only when it is enabled and visible, so
 * @class TpitchView reacts on device state changes 
 * so it can stop or start a loop when necessary.
 * For mobile platforms there is @p pauseAction() that works like switch.
 */
class NOOTKASOUND_EXPORT TpitchView : public QWidget
{

  Q_OBJECT

  friend class TquickAudioDialog;

public:
  explicit TpitchView(TaudioIN *audioIn,  QWidget *parent = 0, bool pauseActive = true);
  
  void setAudioInput(TaudioIN *audioIn);
  void watchInput(); /**< Starts displaying volume level and intonation (when enabled). */
  void stopWatching(); /**< Stops displaying volume (and intonation) */
  void setPitchColor(QColor col);

  bool isPaused();
  void setBgColor(const QColor &col) { m_bgColor = col; }
  void setMinimalVolume(float vol);
  float minimalVolume();
  void setDisabled(bool isDisabled);
#if defined (Q_OS_ANDROID)
  QAction* pauseAction() { return m_pauseAct; }
#endif

  void markAnswer(const QColor &col);

      /** Sets an accuracy of intonation.
        * When 0 - 'do not check' m_intoView becomes disabled. */
  void setIntonationAccuracy(int accuracy);
  int intonationAccuracy();
  void enableAccuracyChange(bool enAcc); /**< Redirects suitable method from @p TintonationView */
  bool isAccuracyChangeEnabled(); /**< Redirects suitable method from @p TintonationView */


      /** Pitch view can be placed in one row (horizontal) or one over another (vertical) - default */
  void setDirection(QBoxLayout::Direction dir);

      /** Starts animation displaying correction of unclear sound.  */
  void outOfTuneAnim(float outTune, int duration = 300);

signals:
      /** Emitted when correction animation finish */
  void correctingFinished();


protected slots:
  void noteSlot();
  void updateLevel();
  void pauseClicked();
	void minimalVolumeChanged(float minVol);
  void inputStateChanged(int inSt);
  void inputDeviceDeleted();
  void accuracyChangedSlot();
  void intoAnimFinished();
#if defined (Q_OS_ANDROID)
  void pauseActionSlot();
#endif
  
protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void showEvent(QShowEvent* e);
  virtual void hideEvent(QHideEvent* e);
  virtual void resizeEvent(QResizeEvent*);

      /** Gives protected access (for friend class) to @p TvolumeView widget of @p TpitchView  */
  TvolumeView* volumeView() { return m_volumeView; }

private:
  TvolumeView 			*m_volumeView;
  TintonationView 	*m_intoView;
//   TpcmView          *m_pcmView;
  TaudioIN 					*m_audioIN;
  QTimer 						*m_watchTimer;
  QColor 						 m_pitchColor, m_bgColor;
	QBoxLayout 				*m_lay;
  int 							 m_hideCnt; // counter of m_volTimer loops.
  float 						 m_prevVolume, m_prevPitch;
  int                m_prevState;
	bool							 m_pauseActive; /**< It stores constructor state, so when audio in is created/deleted it reacts on it is was enabled. */
#if defined (Q_OS_ANDROID)
	QAction           *m_pauseAct;
  void updatePauseActIcon(); /**< Depends on action state (checked or not) sets the icon */
#endif
};

#endif // TPITCHVIEW_H
