/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#include "tnotepixmap.h"
#include <QPainter>

#define NOTES_PER_HEIGHT (25)

TnotePixmap::TnotePixmap(Tnote note, bool clef, int height, TkeySignature key) :
  QPixmap((height/NOTES_PER_HEIGHT)*2*(NOTES_PER_HEIGHT/3), height)
{
  // sizes from TscoreWidgetSimple
  // clef 5.5 * coeff
  // key 5 * coeff
  // note 6 * coeff
  
    double coeff = height / NOTES_PER_HEIGHT;
    fill(); // white background

    int noteOffset = 10 - (note.octave*7 + note.note);
    int hiLinePos = 4;
    if (noteOffset < -4)
        hiLinePos = 9;
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setWindow(0, 0, width(), height);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    for (int i=hiLinePos; i < (hiLinePos + 10); i += 2)
        painter.drawLine(0 ,i*coeff, width(), i*coeff);
    if (clef) {
        painter.setFont(QFont("nootka", coeff*12.5, QFont::Normal));
        painter.drawText(QRectF(1, (hiLinePos - 3.8)*coeff, coeff*6, coeff*18), Qt::AlignLeft, QString(QChar(0xe1a7)));
    }
    painter.drawEllipse( 9*coeff, (hiLinePos + noteOffset)*coeff, coeff*3, coeff*2);
    
}

TnotePixmap::~TnotePixmap()
{}

