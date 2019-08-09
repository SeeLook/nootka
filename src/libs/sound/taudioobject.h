/***************************************************************************
 *   Copyright (C) 2014-2019 by Tomasz Bojczuk                             *
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


#ifndef TAUDIOOBJECT_H
#define TAUDIOOBJECT_H


#include <QtCore/qobject.h>
#include "nootkasoundglobal.h"


/**
 * This class is like 'emitter' of signals for TrtAudio class
 * which can not derive directly from @p QObject
 */
class NOOTKASOUND_EXPORT TaudioObject : public QObject
{

  Q_OBJECT

public:
  explicit TaudioObject(QObject* parent = nullptr) : QObject(parent) {}

  void emitStreamOpened() { emit streamOpened(); }

      /**
       * Emitted after @p TrtAudio::updateAudioParams()
       */
  void emitParamsUpdated() { emit paramsUpdated(); }

      /**
       * When a single note or a melody starts playing
       */
  void emitPlayingStarted() { emit playingStarted(); }

      /**
       * Emitted when all note data were send to output buffer through callback
       */
  void emitPlayingFinished() { emit playingFinished(); }

      /**
       * In out callback, when next note of melody is going to be played,
       * to synchronize note selected on the score 
       */
  void emitNextNoteStarted() { emit nextNoteStarted(); }

signals:
  void streamOpened();
  void paramsUpdated();
  void playingStarted();
  void playingFinished();
  void nextNoteStarted();

};

#endif // TAUDIOOBJECT_H
