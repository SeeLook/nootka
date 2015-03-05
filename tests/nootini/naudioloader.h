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

#ifndef NAUDIOLOADER_H
#define NAUDIOLOADER_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QFile>
#include <QtCore/QDataStream>

class TartiniParams;
class TpitchFinder;

/**
 * Manages loading audio data from file in separate thread.
 * Grabbed data are push to @class TpitchFinder (in offline mode)
 * After all, @p processingFinished() signal is emitted.
 */
class NaudioLoader : public QObject
{
  Q_OBJECT
public:
  explicit NaudioLoader(QObject* parent = 0);
  virtual ~NaudioLoader();

      /** Sets audio file to be loaded with @p startLoading().
       * Only when file is set a valid @p TpitchFinder is returned by @p finder() */
  bool setAudioFile(const QString& fileName);
  void startLoading(); /** Starts loading audio data and push it into TpitchFinder.  */

  TpitchFinder* finder() { return m_pf; } /** Pointer to @class TpitchFinder processing data of current file or 0 if no file. */

  int totalChunks() { return m_totalChunks; }

  void fillTartiniParams(TartiniParams* tp); /** Applies Tartini parameters to existing @class TpitchFinder instance */

signals:
  void processingFinished(); /** Emitted when all data were processed */

protected:
  void performThread();

private:
  QThread          m_thread;
  TpitchFinder    *m_pf;
  int              m_totalChunks, m_samplesCount;
  quint16          m_channelsNr;
  QDataStream      m_in;
  QFile            m_audioFile;
};

#endif // NAUDIOLOADER_H
