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

#ifndef TAUDIOBUFFER_H
#define TAUDIOBUFFER_H


#include <QIODevice>


/**
 * The TaudioBuffer class is proxy between physical mobile sound device
 * and Nootka @p TaudioOUT class.
 * It catch readData() method and emits @p feedAudio signal from there,
 * then audio data is send to the physical device.
 */
class TaudioBuffer : public QIODevice
{

  Q_OBJECT
  
public:
  TaudioBuffer(QObject* parent = 0) : QIODevice(parent), m_bufferSize(2048) {}

      /** In fact, there is no any buffer!
       * That value controls size of data to get by @p feedAudio()
       * instead of @maxlen value sending in @p readData(data, maxlen) */
  void setBufferSize(qint64 s) { m_bufferSize = s; }
  qint64 bufferSize() const { return m_bufferSize; }
  
signals:
  void feedAudio(char*, qint64, qint64&);
  
protected:

  virtual qint64 readData(char *data, qint64 maxlen) {
    Q_UNUSED(maxlen)
    qint64 wasRead = 0;
    emit feedAudio(data, m_bufferSize, wasRead);
    return wasRead;
  }


  virtual qint64 writeData(const char *data, qint64 len) { return 0; }  /** Dummy - does nothing */

private:
  qint64          m_bufferSize;
  
};

#endif // TAUDIOBUFFER_H
