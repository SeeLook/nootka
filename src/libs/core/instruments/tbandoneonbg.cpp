/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "tbandoneonbg.h"
#include <QtCore/qdebug.h>


struct BanButt {
  qreal x;
  qreal y;
  short open;
  short close;
};


static BanButt leftArray[33] = {
  { 21.2244, 17.8522, -7, -9 }, // E-1 D-1
  { 8.77049, 63.5745, -9, -7 }, // D-1 E-1
  { 19.7463, 45.3289, 5, -2 }, // E0 A-1
  { 28.0139, 58.4429, 0, 5 }, // H-1 E0
  { 52.674, 51.8859, 20, 19 }, // G1 F#1
  { 43.9788, 38.9145, 9, 5 }, // G#0 E0
  { 35.5687, 25.3728, 3, -4 }, // D0 G-1
  { 45.5468, 13.2566, -2, 3 }, // A-1 D0
  { 57.2354, 2.56579, -3, -3 }, // G#-1 G#-1
  { 60.799, 20.5263, 10, 8 }, // A0 G0
  { 69.4941, 35.3509, 12, 10 }, // H0 A0
  { 77.9042, 49.1776, 22, 21 }, // A1 G#1
  { 73.0577, 8.69517, 8, 11 }, // G0 A#0
  { 86.4568, 19.5285, 13, 12 }, // C1 H0
  { 86.4568, 0, -1, -1 }, // A#-1 A#-1
  { 95.7222, 33.7829, 15, 14 }, // D1 C#1
  { 103.99, 49.1776, 16, 24 }, // D#1 H1
  { 99.5709, 9.26534, 4, 13 }, // D#0 C1
  { 113.968, 1.14036, 2, 4 }, // C#0 D#0
  { 111.83, 20.6688, 17, 15 }, // E1 D1
  { 119.67, 36.4912, 19, 17 }, // F#1 E1
  { 126.084, 11.6886, 18, 2 }, // F1 C#0
  { 128.65, 51.0307, 7, 6 }, // F#0 F0
  { 136.917, 24.0899, 1, 18 }, // C0 F1
  { 142.477, 5.13158, 6, 16 }, // F0 D#1
  { 144.33, 38.9145, 14, 9 }, // C#1 G#0
  { 153.452, 55.0219, -8, -10 }, // D#-1 C#-1
  { 152.74, 17.5329, 11, 1 }, // A#0 C0
  { 161.862, 31.2171, -4, 7 }, // G-1 F#0
  { 170.558, 46.7543, -5, 0 }, // F#-1 H-1
  { 179.11, 61.7215, -11, -6 }, // C-1 F-1
  { 178.825, 25.5153, -6, -5 }, // F-1 F#-1
  { 169.275, 11.9737, 21, 20 } // G#1 G1
};


static BanButt rightArray[38] = {
  { 26.0417, 33.4609, 12, 12 }, // H0 H0
  { 18.9575, 70.2402, 11, 11 }, // A#0 A#0
  { 12.5596, 53.4871, 10, 10 }, // A0 A0
  { 39.8729, 63.0944, 16, 16 }, // D#1 D#1
  { 31.8882, 48.2275, 18, 18 }, // F1 F1
  { 34.7614, 19.897, 13, 15 }, // C1 D1
  { 46.5546, 29.8194, 17, 19 }, // E1 F#1
  { 44.2828, 8.27076, 14, 13 }, // C#1 C1
  { 55.1407, -3.22183, 48, 46 }, // H3 A3
  { 55.9759, 16.8902, 15, 14 }, // D1 C#1
  { 53.9714, 45.1873, 23, 17 }, // A#1 E1
  { 59.7177, 59.6533, 30, 30 }, // F2 F2
  { 66.8003, 27.4474, 26, 31 }, // C#2 F#2
  { 67.6355, 5.96555, 46, 44 }, // A3 G3
  { 78.4934, 15.7543, 20, 21 }, // G1 G#1
  { 79.5958, -4.65841, 45, 45 }, // G#3 G#3
  { 73.5155, 43.2831, 21, 22 }, // G#1 A1
  { 80.6649, 57.4818, 28, 29 }, // D#2 E2
  { 88.3489, 27.748, 19, 19 }, // F#1 F#1
  { 94.4627, 43.116, 24, 26 }, // H1 C#2
  { 101.311, 58.0497, 31, 33 }, // F#2 G#2
  { 109.029, 29.1178, 22, 24 }, // A1 H1
  { 100.61, 16.8902, 35, 23 }, // A#2 A#1
  { 90.2198, 4.89646, 43, 35 }, // F#3 A#2
  { 103.75, -3.9234, 44, 43 }, // G3 F#3
  { 113.272, 6.60032, 41, 37 }, // E3 C3
  { 121.557, 20.1642, 37, 25 }, // C3 C2
  { 115.41, 44.2519, 27, 29 }, // D2 E2
  { 121.958, 59.9874, 34, 36 }, // A2 H2
  { 136.391, 48.1273, 33, 34 }, // G#2 A2
  { 128.84, 33.8284, 25, 27 }, // C2 D2
  { 142.237, 24.2735, 39, 39 }, // D3 D3
  { 134.82, 11.7787, 40, 40 }, // D#3 D#3
  { 126.702, -0.51574, 42, 42 }, // F3 F3
  { 149.487, 38.9733, 29, 32 }, // E2 G2
  { 156.169, 53.5395, 36, 38 }, // H2 C#3
  { 143.206, 62.5933, 38, 41 }, // C#3 E3
  { 161.681, 70.578, -4, 28 } // G-1 D#2
};


TbandoneonBg::TbandoneonBg(QObject* parent) :
  QObject(parent)
{
}


qreal TbandoneonBg::leftXat(int b) { return leftArray[b].x; }

qreal TbandoneonBg::leftYat(int b) { return leftArray[b].y; }

int TbandoneonBg::leftOpenAt(int b) { return leftArray[b].open; }

int TbandoneonBg::leftCloseAt(int b) { return leftArray[b].close; }


qreal TbandoneonBg::rightXat(int b) { return rightArray[b].x; }

qreal TbandoneonBg::rightYat(int b) { return rightArray[b].y; }

int TbandoneonBg::rightOpenAt(int b) { return rightArray[b].open; }

int TbandoneonBg::rightCloseAt(int b) { return rightArray[b].close; }


void TbandoneonBg::setCurrentIndex(int i) {
  if (i != m_currentIndex) {
    m_currentIndex = i;
    emit currentIndexChanged();
    getNote();
  }
}


void TbandoneonBg::setClosing(bool c) {
  if (c != m_isClosing) {
    m_isClosing = c;
    emit closingChanged();
//     getNote();
  }
}


void TbandoneonBg::setNote(const Tnote& n) {
  m_note = n;
//   emit noteChanged();
}


void TbandoneonBg::getNote() {
  if (m_currentIndex < 0)
    return;
  if (m_currentIndex < 33)
    m_note.setChromatic(m_isClosing ? leftArray[m_currentIndex].close : leftArray[m_currentIndex].open);
  else if (m_currentIndex < 71)
    m_note.setChromatic(m_isClosing ? rightArray[m_currentIndex - 33].close : rightArray[m_currentIndex - 33].open);
  emit noteChanged();
}
