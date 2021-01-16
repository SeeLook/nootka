/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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

#include "tsaxbg.h"
#include "tglobals.h"

#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>


#define LOWEST_NOTE (11)  // lowest note for saxophone notation is a#/bb in small octave
#define HIGHEST_NOTE (49) // highest note for saxophone notation is f3 in three-lines octave
#define SUPPORTED_NOTES (39) // number of notes supported by fingering chart


TsaxBg::TsaxBg(QQuickItem* parent) :
  TcommonInstrument(parent)
{
  m_notesArray = new quint32[SUPPORTED_NOTES];
  m_notesArray[0]   = 4227322;    // a#/bb
  m_notesArray[1]   = 4202746;    // b
  m_notesArray[2]   = 4194554;    // c1
  m_notesArray[3]   = 4210938;    // c#1/db1
  m_notesArray[4]   = 250;        // d1
  m_notesArray[5]   = 2097402;    // d#1/eb1
  m_notesArray[6]   = 122;        // e1
  m_notesArray[7]   = 58;         // f1
  m_notesArray[8]   = 90;         // f#1/gb1      or 524346
  m_notesArray[9]   = 26;         // g1
  m_notesArray[10]  = 4122;       // g#1/ab1
  m_notesArray[11]  = 10;         // a1           or 4106
  m_notesArray[12]  = 262154;     // a#1/bb1  or 6            or 34
  m_notesArray[13]  = 2;          // b1            or 262146
  m_notesArray[14]  = 8;          // c2            or 131074
  m_notesArray[15]  = 0;          // c#2           or 2064
  m_notesArray[16]  = 2298;       // d2            or 10490
  m_notesArray[17]  = 2099450;    // d#2/eb2
  m_notesArray[18]  = 2170;       // e2
  m_notesArray[19]  = 2106;       // f2
  m_notesArray[20]  = 2138;       // f#2/gb2      or 526394
  m_notesArray[21]  = 2074;       // g2
  m_notesArray[22]  = 6170;       // g#2/ab2
  m_notesArray[23]  = 2058;       // a2
  m_notesArray[24]  = 264202;       // a#2/bb2    or 2054    or 2082
  m_notesArray[25]  = 2050;       // b2
  m_notesArray[26]  = 2056;       // c3           or 133122
  m_notesArray[27]  = 2048;       // c#3/db3      or 2144
  m_notesArray[28]  = 3072;       // d3
  m_notesArray[29]  = 3584;       // d#3/eb3
  m_notesArray[30]  = 69120;      // e3           or 2073
  m_notesArray[31]  = 69376;      // f3           or 2057
  m_notesArray[32]  = 264225;     // f#3/g#3      or 593664
  m_notesArray[33]  = 264193;     // g3           or 2049
  m_notesArray[34]  = 133170;     // g#3/ab3      or 788488
  m_notesArray[35]  = 2296;       // a3           or 133144
  m_notesArray[36]  = 2160;       // a#3/bb3      or 2552
  m_notesArray[37]  = 2320;       // b3           or 133656
  m_notesArray[38]  = 2832;       // c3           or 2099378
}


TsaxBg::~TsaxBg()
{
  delete[] m_notesArray;
}


void TsaxBg::setNote(const Tnote& n, quint32 noteDataValue) {
  Q_UNUSED(noteDataValue)
  bool out = false;
  if (n.isValid()) {
      int ch = n.chromatic();
//       qDebug() << "setNote" << n.toText() << ch;
      if (!p_note.isValid() || ch != p_note.chromatic()) {
        p_note.setChromatic(ch);
        if (ch >= LOWEST_NOTE && ch <= HIGHEST_NOTE) {
            ch -= LOWEST_NOTE;
            m_fingeringId = m_notesArray[ch];
        } else {
            m_fingeringId = 0;
            out = true;
        }
        emit fingeringIdChanged();
      }
  } else {
      if (m_fingeringId) {
        m_fingeringId = 0;
        p_note.setNote(0);
        emit fingeringIdChanged();
      }
  }
  setOutOfScale(out);
}


void TsaxBg::askQuestion(const Tnote& n, quint32 noteDataValue) {
  Q_UNUSED(noteDataValue)
  setNote(n);
}


void TsaxBg::setFlapNumber(int fNr) {
  quint32 flatPower = qFloor(qPow(2.0, fNr));
  bool flapOn = m_fingeringId & flatPower;
  if (flapOn)
    m_fingeringId &= ~flatPower;
  else
    m_fingeringId |= flatPower;
//   qDebug() << "setFlapNumber" << fNr << m_fingeringId;
  emit fingeringIdChanged();
  for (int f = 0; f < SUPPORTED_NOTES; ++f) {
    if (m_notesArray[f] == m_fingeringId) {
      p_note.setChromatic(LOWEST_NOTE + f + GLOB->instrument().transposition());
      emit noteChanged();
      break;
    }
  }
}


void TsaxBg::markSelected(const QColor& markColor) {
}


void TsaxBg::showNoteName() {
}


void TsaxBg::correct(const Tnote& n, quint32 noteData) {
  markSelected(GLOB->correctColor());
  setNote(n, noteData);
  QTimer::singleShot(1500, [=]{ emit correctionFinished(); }); // Fake so far
}
