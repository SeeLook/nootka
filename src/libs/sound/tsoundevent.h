/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#ifndef TSOUNDEVENT_H
#define TSOUNDEVENT_H

#include <QEvent>

/**
 * The @class TsoundEvent class is QEvent sends by TaudioIN or TaudioOUT
 * to the main qApp instance.
 * It informs about audio devices state changes.
 * States are available through @p 
 */
class TsoundEvent : public QEvent
{

  Q_GADGET
  
public:
  enum EsoundEventType { e_playingEvent = User + 108,
                         e_listeningEvent = User + 109 };
  
  TsoundEvent(EsoundEventType t, bool process) : QEvent((Type)t), m_type(t), m_processing(process) {}
  
  virtual ~TsoundEvent() {}
  
  bool processing() { return m_processing; }
  void setProcessing(bool pl) { m_processing = pl; }
  
  Type type() const { return (Type)m_type; }
  
private:
  bool                     m_processing;
  EsoundEventType          m_type;
  
};

#endif // TSOUNDEVENT_H
