/***************************************************************************
 *   Copyright (C) 2015-2017 by Tomasz Bojczuk                             *
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

#ifndef TAUDIOBUFFER_H
#define TAUDIOBUFFER_H


#include <QtCore/qiodevice.h>


/**
 * The @p TaudioBuffer class is proxy between physical mobile sound device
 * and Nootka @p TaudioOUT and @p TaudioIN classes.
 * It catch @p readData() method and emits @p feedAudio signal from there (output)
 * or @p writeData() and emits @p readAudio (input)
 * then audio data is send to/from the physical device.
 *
 * Both re-implemented methods of @p QIODevice are performed in device thread, so
 * when those signals are connected with @p Qt::DirectConnection
 * also the slots will be done in that thread.
 */
class TaudioBuffer : public QIODevice
{

  Q_OBJECT

public:
  TaudioBuffer(QObject* parent = nullptr) : QIODevice(parent), m_bufferSize(2048) {}

      /**
       * In fact, there is no any buffer!
       * That value controls size of data to get by @p feedAudio()
       * instead of @p maxlen value sending in @p readData(data, maxlen)
       * or @p len value from @p writeData(data, len).
       * If this value is set to 0, then values send by device are respected.
       */
  void setBufferSize(qint64 s) { m_bufferSize = s; }
  qint64 bufferSize() const { return m_bufferSize; }

signals:
  void feedAudio(char*, qint64, qint64&);
  void readAudio(const char*, qint64&);

protected:

  virtual qint64 readData(char *data, qint64 maxlen) {
    qint64 wasRead = 0;
    emit feedAudio(data, m_bufferSize ? m_bufferSize : maxlen, wasRead);
    return wasRead;
  }


      /**
       * When @p m_bufferSize is set to 0 @p len parameter is respected
       */
  virtual qint64 writeData(const char *data, qint64 len) {
    qint64 dataLenght = m_bufferSize ? m_bufferSize : len;
    emit readAudio(data, dataLenght);
    return dataLenght;
  }

private:
  qint64          m_bufferSize;

};

#endif // TAUDIOBUFFER_H
