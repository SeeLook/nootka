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

#ifndef NCHART_H
#define NCHART_H

#include <tchart.h>
#include "nanalyzedata.h"
#include <QThread>
#include <QMutex>

class TscoreStaff;
class NoteData;
class AnalysisData;
class TpitchFinder;
class NdrawObject;

/**
 * Nootini chart
 */
class Nchart : public Tchart
{

  friend class NdrawObject;
  Q_OBJECT
public:
  Nchart(QWidget* parent = 0);
  virtual ~Nchart();

  void setPitchFinder(TpitchFinder* pf); /** Pitch finder processing audio data */
  void setXnumber(int xN); /** Number of X values on the chart */

  void allDataLoaded(); /** May be called when all data was sent to the chart to adjust its size */

  void chunkSlot();

signals:
  void chunkDone(); /** Emitted when @p chunkSlot() finished. */

protected:
  void runThread(AnalysisData* ad, NoteData* nd);

  int xMap(int xx) { return m_xLine->line().x1() + (xx + 1) * xSc; }

  QList<NanalyzeData>  dl;
  QGraphicsTextItem   *progresItem;
  int                  totalXnr, currentChunk;

private:
  TpitchFinder                *m_pitchF;
  TscoreStaff                 *m_staff;
  double                       m_volume, m_pitchDiff;
  int                          m_lastDrawnChunk;
  QLinearGradient              m_pitchGrad;
  QGraphicsLineItem           *m_xLine;
  const int                    xSc;
  NdrawObject                 *m_drawObject;
};


/**
 * QObject that draws on Nchart in separate thread
 */
class NdrawObject : public QObject
{
  Q_OBJECT
public:
  explicit NdrawObject(Nchart *chart, QObject* parent = 0);
  virtual ~NdrawObject();

  void collect(AnalysisData* ad, NoteData* nd);
  void chunkProcessed();
  void reset(); /** Resets state of drawing object. */

private:
  Nchart                      *m_chart;
  QThread                      m_thread;
  QMutex                       m_mutex;
  int                          m_chunkNr, m_prevNoteIndex;
};

#endif // NCHART_H
