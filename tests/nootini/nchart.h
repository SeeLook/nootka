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
#include <tyaxis.h>
#include <music/tclef.h>
#include "nanalyzedata.h"
#include <QThread>
#include <QMutex>

class TgraphicsTextTip;
class NaudioLoader;
class TscoreStaff;
class NoteData;
class AnalysisData;
class TpitchFinder;

/**
 * Nootini chart
 */
class Nchart : public Tchart
{

  Q_OBJECT
public:
  Nchart(QWidget* parent = 0);
  virtual ~Nchart();

  struct SnoteStruct {
    int       index;
    qreal     realPitch;
    int       basePitch; /** Rounded pitch value */
    qreal     duration;
    int       chunkDur; /** How many chunks note takes */
  }; /** Values describing detected note after Nootka processing */

  void setAudioLoader(NaudioLoader* loader); /** Pitch finder processing audio data */
  void setXnumber(int xN); /** Number of X values on the chart */

  void allDataLoaded(); /** May be called when all data was sent to the chart to adjust its size */

  void drawChunk();

  void setNootkaIndexing(bool yes);
  bool isNootkaIndexing() { return m_nootkaIndexing; }

  qreal chunkDuration() { return m_chunkDuration; } /** Time of single chunk in seconds [s] */

  void setMinVolToSplit(qreal minVol) { m_minVolToSplit = minVol; }
  qreal minVolToSplit() { return m_minVolToSplit; }

  bool drawVolume() { return m_drawVolume; }
  void setDrawVolume(bool drawVol) { m_drawVolume = drawVol; }

signals:
  void chunkDone(); /** Emitted when @p chunkSlot() finished. */

protected:
  void copyChunk(AnalysisData* ad, NoteData* nd);
  void clefChanged(Tclef clef);
  void drawNoteSegment(int firstNoteChunk, int lastNoteChunk);
  void drawNoteSegment2(int firstNoteChunk, int lastNoteChunk);
  void emptyRect(int firstChunk, qreal width);

  int xMap(int xx) { return m_xLine->line().x1() + (xx + 1) * xSc; }
  qreal yMap(qreal yy) { return yAxis->y() + yAxis->mapValue(yy); }

  QList<NanalyzeData>  dl;

protected slots:
  void adjustHeight();

private:
  TpitchFinder                *m_pitchF;
  TscoreStaff                 *m_staff;
  TgraphicsTextTip            *m_progresItem;
  int                          m_chunkNr, m_prevNoteIndex;
  int                          m_totalXnr;
  QLinearGradient              m_pitchGrad;
  QGraphicsLineItem           *m_xLine;
  const int                    xSc, hSc; /** @p xSc is scale of x axis ans @p hSc is half of it  */
  int                          m_pass;
  bool                         m_drawVolume;

// Nootka indexing method
  bool                         m_nootkaIndexing;
  qreal                        m_chunkDuration;
  int                          m_minChunkDur; /** Minimal numbers of chunk for note */
  SnoteStruct                  m_newNote, m_prevNote;
  double                       m_hiVol, m_loVol;
  qreal                        m_minVolToSplit;
  int                          m_nootkaIndex;
  NaudioLoader                *m_loader;

  void nootkaMethod(int c); /** Process of indexing chunk of given number */
};

#endif // NCHART_H
