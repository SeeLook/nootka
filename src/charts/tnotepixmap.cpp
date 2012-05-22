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
#include "tkeysignature.h"
#include "tkeysignatureview.h"
#include "tquestionpoint.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>


QPixmap getNotePixmap(Tnote note, bool clef, TkeySignature key, double factor) {
    int noteNr = note.octave*7 + note.note;
    
    QString accidString = TnoteView::getAccid(note.acidental);
    if (note.acidental) {
        if (qAbs(note.acidental) == 1) { // double accids already assigned
            if (note.acidental == TkeySignature::scalesDefArr[key.value()+7][note.note-1])
                accidString = ""; // accid in key signature
        }
    } else // no accids
        if (TkeySignature::scalesDefArr[key.value()+7][note.note-1] != 0)
                        accidString = TnoteView::getAccid(3); // so paint natural

    int h = factor * 18; // height
    int w = factor * 13; // width
    int xPosOfNote = 8;
    if (key.value()) {
        w += factor * (2 * qAbs(key.value()));
        xPosOfNote += 2 * qAbs(key.value());
    }
    if (accidString != "") {
        w += factor * 3;
        xPosOfNote += 3;
    }
    if (noteNr > 12)
        h = factor * (18 + (noteNr - 12));
    if (noteNr < -1)
        h = factor * (18 + (-1 - noteNr));
    
    QPixmap pix(w, h);
    
    pix.fill(Qt::transparent); // white background

    int noteOffset = 10 - noteNr;
    int hiLinePos = 4;
    if (noteNr > 12)
        hiLinePos = 4 + noteNr - 12;
    
    QPainter painter(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setWindow(0, 0, w, h);
    
    painter.setPen(QApplication::palette().foreground().color());
    painter.setBrush(QApplication::palette().foreground().color());
    // main staff lines
    for (int i = hiLinePos; i < (hiLinePos + 10); i += 2)
        painter.drawLine(0 ,i * factor, w, i * factor);
    // upper lines if needed
    if (noteNr > 12)
        for (int i = hiLinePos - 2; i > (2); i -= 2)
            painter.drawLine((xPosOfNote - 1) * factor, i * factor, (xPosOfNote + 4) * factor, i * factor);
    // lower lines if needed
    if (noteNr < 2) {
        for (int i = (hiLinePos + 10); i <= (hiLinePos + 10 + (1+ (-1 * noteNr))); i += 2)
            painter.drawLine((xPosOfNote - 1) * factor, i * factor, (xPosOfNote + 4) * factor, i * factor);
    }
    
    double fontF = 1;
    if (clef) {
//    #if defined(Q_OS_MAC)
//        painter.setFont(QFont("nootka", factor * 18.5, QFont::Normal));
//        painter.drawText(QRectF(1, (hiLinePos - 4.4) * factor, factor * 6, factor * 18),
//                         Qt::AlignLeft, QString(QChar(0xe1a7)));
//        QFont cFont = QFont("nootka", factor * 18.5, QFont::Normal);
//        QFontMetricsF cMetr(cFont);
//        double clefWidth = cMetr.width(QString(QChar(0xe1a7)));
//        qDebug() << clefWidth;
//    #else
        QFont cFont = QFont("nootka", factor * 15.5, QFont::Normal);
        QFontMetricsF cMetr(cFont);
        double clefWidth = cMetr.width(QString(QChar(0xe1a7)));
        fontF = 80.0 / clefWidth; // 70 - Linux
        if ((fontF < 1) || (fontF > 1.25)) {
            cFont.setPointSizeF(cFont.pointSizeF() * fontF);
            cMetr = QFontMetricsF(cFont);
            clefWidth = cMetr.width(QString(QChar(0xe1a7)));
        }
        painter.setFont(cFont);
//    #endif
//    #if defined(Q_OS_LINUX)
        painter.drawText(QRectF(0, (hiLinePos + 4) * factor - cMetr.height() / 2, clefWidth, cMetr.height()),
                         Qt::AlignCenter, QString(QChar(0xe1a7))); // 4.4
//    #else
//        painter.drawText(QRectF(1, (hiLinePos - 3.2)*coeff, coeff*6, coeff*18), Qt::AlignLeft, QString(QChar(0xe1a7)));
//    #endif
    }
    
  // ALL ACCIDENTALS  
    QFont accFont = QFont("nootka");
    #if defined (Q_OS_MAC)
    accFont.setPointSizeF(6.5 * factor);
    #else    
    accFont.setPointSizeF(6 * factor * fontF);
    #endif
    painter.setFont(accFont);
    QFontMetricsF metrics(accFont);
//    QRectF rect = metrics.boundingRect(TnoteView::getAccid(1));
    // key signature
    if (key.value()) {
        QString keyAccidString;
        char ff;
        if (key.value() < 0) {
            keyAccidString = TnoteView::getAccid(-1); // flat
            ff = -1;
        }
        else {
            keyAccidString = TnoteView::getAccid(1); // sharp
            ff = 1;
        }
        double accWidth = metrics.width(keyAccidString);
        for (int i = 1; i <= (qAbs(key.value())); i++) {
//        #if defined (Q_OS_MAC)
//            painter.drawText(QRectF( (4 + i*1.7) * factor,
//                                     (TkeySignatureView::getPosOfAccid((7 + ((i)*ff))%8) - 19 + hiLinePos) * factor,
//                                     rect.width() * 3, rect.height()),
//                            Qt::AlignCenter, keyAccidString);
//        #else
            painter.drawText(QRectF( (4 + i*1.7) * factor,
                                     (TkeySignatureView::getPosOfAccid((7 + ((i)*ff))%8) - 19 + hiLinePos) * factor,
                                     accWidth, metrics.height()),
                            Qt::AlignCenter, keyAccidString);
            
//        #endif
        }
    }    
    // note
    painter.drawEllipse( xPosOfNote * factor, (hiLinePos + noteOffset) * factor, factor * 3, factor * 2);
    // accidental
    if (note.acidental) {
      double accWidth = metrics.width(accidString);
      
//        #if defined (Q_OS_MAC)
//        painter.drawText(QRectF((xPosOfNote - 1.5) * factor - (rect.width()),
//                                (hiLinePos + noteOffset) * factor - (factor * 2) + 1,
//                                rect.width() *3, rect.height() ),
//                         Qt::AlignCenter, accidString );
//        #else
        painter.drawText(QRectF((xPosOfNote) * factor - accWidth / 2,
                                (hiLinePos + noteOffset - 2.2) * factor,
                                accWidth, metrics.height() ),
                         Qt::AlignCenter, accidString );
//        #endif
    }
    
    return pix;
}
