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

class QTimer;
class QLabel;
class TvolumeMeter;

class TpitchView : public QWidget
{
  Q_OBJECT
public:
  explicit TpitchView(TaudioIN *audioIn, QWidget *parent = 0);
  virtual ~TpitchView();
  
  void setAudioInput(TaudioIN *audioIn) { m_audioIN = audioIn; }
  
  void startVolume();
  void stopVolume();
  
protected slots:
  void pitchState(TaudioIN::Estate state);
  void noteSlot(Tnote note);
  void updateLevel();
  
private:
  TvolumeMeter *m_volMeter;
  QLabel *m_stateLabel;
  TaudioIN *m_audioIN;
  QTimer *m_volTimer;

  
  
  
};

#endif // TPITCHVIEW_H
