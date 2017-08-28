/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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


#ifndef TABSTRACTPLAYER_H
#define TABSTRACTPLAYER_H


#include <nootkasoundglobal.h>
#include <QtCore/qobject.h>


class QTimer;


/**
 * Base abstract class for sound output (playing scale).
 */
class NOOTKASOUND_EXPORT TabstractPlayer : public QObject
{

  Q_OBJECT

public:
  TabstractPlayer(QObject *parent = 0);

  bool isPlayable() { return playable; }

      /**
        * Starts playing given note and then returns true, otherwise gets false.
        */
  virtual bool play(int noteNr);

      /**
       * Immediately stops playing. Emits nothing
       */

  virtual void stop();
      /**
       * Does nothing in audio player subclass.
       */
  virtual void deleteMidi();
  virtual void setMidiParams();

  enum EplayerType { e_audio, e_midi };

  EplayerType type() { return playerType; }

signals:
      /**
       * This signal is emitted when playing of a note is finished.
       */
  void noteFinished();


protected:
  void setType(EplayerType type) { playerType = type; }

  bool          playable;

      /**
       * Determines whether noteFinished() signal is emited in offTimer timeOut() slot.
       * Slot is also called by stop() method and then signal can't be emited.
       */
  bool             doEmit;
  QTimer          *offTimer;
  EplayerType      playerType;

};

#endif // TABSTRACTPLAYER_H
