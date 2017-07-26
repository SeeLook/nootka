/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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
  void emitParamsUpdated() { emit paramsUpdated(); }
  void emitPlayingFinished() { emit playingFinished(); }

signals:
  void streamOpened();

      /**
       * Emitted after @p TrtAudio::updateAudioParams()
       */
  void paramsUpdated();

      /**
       * Emitted when all note data were send
       */
  void playingFinished();

};

#endif // TAUDIOOBJECT_H
