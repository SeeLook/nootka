/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include "tnote.h"
#include "trtaudioin.h"

class QBoxLayout;
class TvolumeView;
class TintonationView;
class QPushButton;
class QTimer;

  /** This class represents volume meter of audio signal
   * and displays note symbol when TaudioIN detected it.
   * It can has two buttons, but by setting @param withButtons to false
   * they are not created. 
	 * It also has TintonationView widget that shows current sound intonation.*/
class TpitchView : public QWidget
{
  Q_OBJECT
public:
  explicit TpitchView(TaudioIN *audioIn,  QWidget *parent = 0, bool withButtons = true);
  virtual ~TpitchView();
  
  void setAudioInput(TaudioIN *audioIn) { m_audioIN = audioIn; }
      /** Starts grabbing of peak level*/
  void startVolume();
  void stopVolume();
  void setPitchColor(QColor col);
      /** Button to toggle between voice/instrument mode in TaudioIN */
  QPushButton *voiceButt;
      /** Button to pause or activate pitch detection */
  QPushButton *pauseButt;
  bool isPaused() { return m_isPaused; }
  bool isVoice() { return m_isVoice; }
  void setIsVoice(bool isVoice);
  void resize(int fontSize);
  void setBgColor(QColor col) { m_bgColor = col; }
  void setMinimalVolume(float vol);
  void setDisabled(bool isDisabled);
			/** Sets an accuracy of intonation. 
			 * When 0 - 'do not check' m_intoView becames disabled. */
	void setIntonationAccuracy(int accuracy);
			/** Places view horizontally  (true) or vertically (false) */
	void setHorizontal(bool isHorizontal = true);
	bool isHorizontal() { return m_isHorizontal; }
  
protected slots:
  void noteSlot(Tnote note);
  void updateLevel();
  void voiceClicked();
  void pauseClicked();
  void stopTimerDelayed(); // to call stop() on m_volTimer after note detected animation
	void minimalVolumeChanged(float minVol);
  
protected:
  virtual void paintEvent(QPaintEvent* );
  
private:
  TvolumeView 			*m_volMeter;
  TintonationView 	*m_intoView;
  TaudioIN 					*m_audioIN;
  QTimer 						*m_volTimer;
  QColor 						m_pitchColor, m_bgColor;
  bool 							m_isPaused;
  bool 							m_isVoice;
  bool 							m_withButtons;
	bool 							m_isHorizontal;
	QBoxLayout 				*m_lay;
  int 							m_hideCnt; // counter of m_volTimer loops.
};

#endif // TPITCHVIEW_H
