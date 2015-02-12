/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#ifndef TPITCHVIEW_H
#define TPITCHVIEW_H

#include <QWidget>
#include <QBoxLayout>
#include <music/tnote.h>
#include "trtaudioin.h"
#include "nootkasoundglobal.h"


class QCheckBox;
class QBoxLayout;
class TvolumeView;
class TintonationView;
class QTimer;

/** 
 * This class manages of displaying volume meter and intonation indicator.
 * When audio input is set through @p setAudioInput() 
 * it grabs volume and intonation with timer event loop.
 * @class TpitchView reacts on device state changes 
 * so it can stop or start a loop when necessary.
 */
class NOOTKASOUND_EXPORT TpitchView : public QWidget
{
  Q_OBJECT
  
public:
  explicit TpitchView(TaudioIN *audioIn,  QWidget *parent = 0, bool withButtons = true);
  
  void setAudioInput(TaudioIN *audioIn);
  void watchInput(); /** Starts displaying volume level and intonation (when enabled). */
	void stopWatching(); /** Stops displaying volume (and intonation) */
  void setPitchColor(QColor col);
	
  bool isPaused();
  void setBgColor(const QColor &col) { m_bgColor = col; }
  void setMinimalVolume(float vol);
  void setDisabled(bool isDisabled);
	
	void markAnswer(const QColor &col);
	
			/** Sets an accuracy of intonation. 
			 * When 0 - 'do not check' m_intoView becomes disabled. */
	void setIntonationAccuracy(int accuracy);
	
			/** Pitch view can be placed in one row (horizontal) or one over another (vertical) - default */
	void setDirection(QBoxLayout::Direction dir) { if (dir != m_lay->direction())	m_lay->setDirection(dir); }
	
			/** Starts animation displaying correction of unclear sound.  */
	void outOfTuneAnim(float outTune, int duration = 300);
	
  
protected slots:
  void noteSlot();
  void updateLevel();
  void pauseClicked();
	void minimalVolumeChanged(float minVol);
  void inputStateChanged(int inSt);
  void inputDeviceDeleted();
  
protected:
  virtual void paintEvent(QPaintEvent*);
	virtual void showEvent(QShowEvent* e);
	virtual void hideEvent(QHideEvent* e);
	virtual void resizeEvent(QResizeEvent*);
  
private:
	QCheckBox		 			*m_pauseButton; /** Button to pause or activate pitch detection */
  TvolumeView 			*m_volumeView;
  TintonationView 	*m_intoView;
  TaudioIN 					*m_audioIN;
  QTimer 						*m_watchTimer;
  QColor 						 m_pitchColor, m_bgColor;
  bool 							 m_withButtons;
	QBoxLayout 				*m_lay;
  int 							 m_hideCnt; // counter of m_volTimer loops.
  float 						 m_prevVolume, m_prevPitch;
  int                m_prevState;
};

#endif // TPITCHVIEW_H
