/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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
#include "taudioin.h"
#include "tnote.h"

class QPushButton;
class QTimer;
class TvolumeMeter;

  /** This class represents volume meter of audio signal
   * and displays note symbol when TaudioIN detected it  */
class TpitchView : public QWidget
{
  Q_OBJECT
public:
  explicit TpitchView(TaudioIN *audioIn, QWidget *parent = 0);
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
  void resize(int fontSize);
  
protected slots:
  void noteSlot(Tnote note);
  void updateLevel();
  void voiceClicked();
  void pauseClicked();
  
private:
  TvolumeMeter *m_volMeter;
  TaudioIN *m_audioIN;
  QTimer *m_volTimer;
  QColor m_pitchColor;
  bool m_isPaused;
  bool m_isVoice;  
};

#endif // TPITCHVIEW_H
