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


#ifndef TIOAUDIODEVICE_H
#define TIOAUDIODEVICE_H

#include <QIODevice>

class QAudioFormat;

class TIOaudioDevice : public QIODevice
{
  Q_OBJECT
public:
  
  explicit TIOaudioDevice(QObject* parent);
  virtual ~TIOaudioDevice();
  
  void start() { open(QIODevice::WriteOnly); }
  void stop()  { close(); }
  
  qint16 maxPeak() { return m_maxPeak; }
  float *floatBuffer() { return m_floatBuffer; }
  
signals:
  void chunkReady();
  
  
protected:
  virtual qint64 writeData(const char* data, qint64 len);
  virtual qint64 readData(char* data, qint64 maxlen);
  
private:
  qint16 m_maxPeak;
  float *m_floatBuffer;
  quint32 m_floatsWriten;
  
};

#endif // TIOAUDIODEVICE_H
