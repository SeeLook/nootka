/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#ifndef TMIDIIN_H
#define TMIDIIN_H

#include "tcommonlistener.h"


class RtMidiIn;
class TaudioParams;


/**
 * @class TmidiIn
 */
class TmidiIn : public TcommonListener
{

  Q_OBJECT

public:
  TmidiIn(TaudioParams* params, QObject* parent = nullptr);
  ~TmidiIn();

  static QStringList getMidiInPorts();

  void setMidiParams();

  void deleteMidi();

  RtMidiIn* midiIn() { return m_midiIn; }

protected:
  void openMidiPort();


private:
  RtMidiIn              *m_midiIn;
  unsigned int           m_portNr;
};

#endif // TMIDIIN_H
