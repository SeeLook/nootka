/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

#include "tnotestruct.h"


void TnoteStruct::init(int _index, int chunkNr, qreal floatPitch) {
  startChunk = chunkNr;
  endChunk = chunkNr;
  pitchF = floatPitch;
  index = _index;
  maxVol = 0.0f;
  minVol = 1.0f;
  maxPCMvol = 0.0f;
  m_totalAver = 0.0;
  m_shortAver = 0.0;
  m_pList.clear();
  m_pList << floatPitch;
  bestPitch = 0.0;
  idChangedAt.clear();
  idChangedAt << 0;
}


void TnoteStruct::sumarize(qreal chunkTime) {
  freq = pitchToFreq(bestPitch);
  duration = numChunks() * chunkTime;
  if (!m_pList.isEmpty())
    pitchF = m_pList.size() > 3 ? m_pList[3] : m_pList.last();
}


void TnoteStruct::update(int chunkNr, qreal floatPitch, float vol) {
  if (floatPitch > 1.0) {
    m_pList << floatPitch;
    pitchF = m_pList.size() > 3 ? m_pList[3] : m_pList.last();
    if (m_pList.size() == 2 || m_pList.size() == 3)
      basePitch = qRound(pitchF); // it is better to take base pitch not from first chunks
  }
  endChunk = chunkNr;
  maxVol = qMax<float>(maxVol, vol);
  if (numChunks() > 3) // skip first 3 chunks - Tartini may detected a note with low volume
    minVol = qMin<float>(minVol, vol);
  if (qAbs(floatPitch - static_cast<qreal>(basePitch)) < qAbs(bestPitch - static_cast<qreal>(basePitch)))
    bestPitch = floatPitch;
}


qreal TnoteStruct::getAverage(unsigned int start, unsigned int stop) {
  qreal sum = 0.0;
  int cnt = 0;
  for (int i = qMin<int>(start - 1, m_pList.size() - 1); i < qMin<int>(m_pList.size(), stop); ++i) {
    sum += m_pList[i];
    cnt++;
  }
  return sum / cnt;
}


QString TnoteStruct::debug() {
  QString pp = QStringLiteral(" [ ");
  for (qreal p : m_pList) pp += QString("%1, ").arg(p);
  pp += QStringLiteral(" ]");
  return QString::number(pitchF) + QLatin1String(", chunks: ") + QString::number(m_pList.size()) + pp
        + QString(" %1").arg(bestPitch);
}
