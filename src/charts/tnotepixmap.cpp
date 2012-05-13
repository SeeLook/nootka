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
#include "tnoteview.h"
#include <QPainter>
#include <QDebug>

#define NOTES_PER_HEIGHT (25)
#define COEFF (4)

/* static */

TnotePixmap TnotePixmap::pix(Tnote note, bool clef, TkeySignature key) {
    int noteNr = note.octave*7 + note.note;
    int h = COEFF * 18;
    int w = COEFF * 15;
    if (note.acidental)
        w += COEFF * 3;
    if (noteNr > 14)
        h = COEFF * (18 + (noteNr - 13));
    if (noteNr < -1)
        h = COEFF * (18 + (-1 - noteNr));

    return TnotePixmap(note, clef, h, w, key);
}


TnotePixmap::TnotePixmap(Tnote note, bool clef, int height, int width, TkeySignature key) :
  QPixmap(width, height)
{
  // sizes from TscoreWidgetSimple
  // clef 5.5 * coeff
  // key 5 * coeff
  // note 6 * coeff

//    double coeff = height / NOTES_PER_HEIGHT;
    int xPosOfNote = 9;
    if (note.acidental)
        xPosOfNote = 12;
    double coeff = COEFF;
    int noteNr = note.octave*7 + note.note;
    fill(); // white background

    int noteOffset = 10 - noteNr;
    int hiLinePos = 4;
    if (noteNr > 14)
        hiLinePos = 4 + noteNr - 12;

//     qDebug() << noteNr << hiLinePos << noteOffset;
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setWindow(0, 0, width, height);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    // main staff lines
    for (int i = hiLinePos; i < (hiLinePos + 10); i += 2)
        painter.drawLine(0 ,i*coeff, width, i*coeff);
    // upper lines if needed
    if (noteNr > 12)
        for (int i = hiLinePos - 2; i > (1); i -= 2)
            painter.drawLine((xPosOfNote - 1) * coeff, i*coeff, (xPosOfNote + 4) * coeff, i*coeff);
    // lower lines if needed
    if (noteNr < 1) {
//         qDebug() << noteNr << hiLinePos << hiLinePos + 12 + qAbs(noteNr);
        for (int i = (hiLinePos + 10); i < (hiLinePos + 12 + qAbs(noteNr)); i += 2)
            painter.drawLine((xPosOfNote - 1) * coeff, i*coeff, (xPosOfNote + 4) * coeff, i*coeff);
    }
    if (clef) {
#if defined(Q_OS_MAC)
        painter.setFont(QFont("nootka", coeff*18.5, QFont::Normal));
        painter.drawText(QRectF(1, (hiLinePos - 4.4)*coeff, coeff*6, coeff*18), Qt::AlignLeft, QString(QChar(0xe1a7)));
#else
        painter.setFont(QFont("nootka", coeff*14, QFont::Normal));
#endif
#if defined(Q_OS_LINUX)
        painter.drawText(QRectF(1, (hiLinePos - 5)*coeff, coeff*6, coeff*19), Qt::AlignLeft, QString(QChar(0xe1a7)));
#else
//        painter.drawText(QRectF(1, (hiLinePos - 3.2)*coeff, coeff*6, coeff*18), Qt::AlignLeft, QString(QChar(0xe1a7)));
#endif
    }
    // note
    painter.drawEllipse( xPosOfNote * coeff, (hiLinePos + noteOffset) * coeff, coeff * 3, coeff * 2);
    // accidental
    if (note.acidental) {
        QFont f = QFont("nootka", coeff*6, QFont::Normal);
        painter.setFont(f);
        QString accid = TnoteView::getAccid(note.acidental);
        QFontMetricsF metrics = f;
        QRectF rect = metrics.boundingRect(accid);
//         qDebug() << (int)note.acidental;
        painter.drawText(QRectF((xPosOfNote - 1.5) * coeff - (rect.width()), 
                                (hiLinePos + noteOffset) * coeff - (coeff * 3) , 
                                rect.width() * 3, rect.height() ), 
                         Qt::AlignCenter, accid );
    }
    
}

TnotePixmap::~TnotePixmap()
{}

