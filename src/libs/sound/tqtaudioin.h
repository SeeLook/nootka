/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#ifndef TAUDIOIN_H
#define TAUDIOIN_H


#include "tcommonlistener.h"

class QAudioInput;
class QIODevice;
class TaudioParams;

/**
 * Audio input class for Nootka based on Qt audio (Qt multimedia)
 */
class NOOTKASOUND_EXPORT TaudioIN : public TcommonListener
{
  Q_OBJECT

public:
  explicit TaudioIN(TaudioParams* params, QObject *parent = 0);
  ~TaudioIN();
  
public slots:
  virtual void startListening();
  virtual void stopListening();


protected slots:
  void dataReady();

private:
  QAudioInput           *m_audioIN;
  QIODevice             *m_inDevice;
  qint16                *m_buffer;
  QThread               *m_thread;
};

#endif // TAUDIOIN_H
