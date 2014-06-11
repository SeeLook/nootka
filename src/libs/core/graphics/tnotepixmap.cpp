/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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
#include <music/tkeysignature.h>
#include <exam/tqaunit.h>
#include <score/tscorenote.h>
#include <score/tscorescene.h>
#include <score/tscoreclef.h>
#include <score/tscorekeysignature.h>
#include <score/tscorestaff.h>
#include <QPainter>
#include <QPalette>
#include <QBuffer>
#include <QApplication>
#include <QDebug>


/** Adds comma and space ', ' to not empty string or returns the same. */
QString addSpaceToNotEmpty(QString &txt) {
	if (txt != "")
			txt += ", ";
	return txt;
}


Tclef TnotePixmap::m_clef = Tclef(Tclef::e_treble_G_8down);

QPixmap getNotePixmap(Tnote note, Tclef::Etype clef, TkeySignature key, qreal factor, int strNr) {
		TscoreScene *scene = new TscoreScene();
		int notesCount = 1;
		if (note.note == 0) // no note in preview
					notesCount = 0;
		TscoreStaff *staff =	new TscoreStaff(scene, notesCount);
		staff->onClefChanged(Tclef(clef));
		if (key.value()) {
				staff->setEnableKeySign(true);
				staff->scoreKey()->setKeySignature(key.value());
				staff->scoreKey()->showKeyName(false);
		}
		staff->setNoteDisabled(0, true);
	// determine image height by note position to avoid empty spaces above or below a staff
	/** It never shows whole piano staff - just single staff where is some note.
	 * When no note, both staves are displayed. */
		int topPix = 0, bottomPix = staff->boundingRect().height(), leftPix = 0;
		if (notesCount) {
				TscoreStaff *workStaff;
				staff->setNote(0, note);
				if (staff->noteSegment(0)->notePos()) {
					if (staff->noteSegment(0)->notePos() < staff->upperLinePos() - 3) { // note above a staff
						topPix = staff->noteSegment(0)->notePos() - 2;
						bottomPix = staff->upperLinePos() + 14;
					} else if (staff->noteSegment(0)->notePos() < staff->upperLinePos() + 10) { // note on a staff
						topPix = staff->upperLinePos() - 4;
						bottomPix = staff->upperLinePos() + 14;
					} else { // note placed below staff
						topPix = staff->upperLinePos() - 4;
						bottomPix = staff->noteSegment(0)->notePos() + 3;
					}
					workStaff = staff;
				} 
// 					else { // piano staff
// 					if (clef == Tclef::e_pianoStaff && staff->lower()) {
// 						topPix = staff->lower()->pos().y();
// 						if (staff->lower()->noteSegment(0)->notePos() < staff->lower()->upperLinePos() + 13) {
// 								bottomPix = staff->lower()->pos().y() + staff->lower()->upperLinePos() + 14; // note in lower staff
// 						} else // note below lower staff
// 								bottomPix = staff->lower()->pos().y() + staff->lower()->noteSegment(0)->notePos() + 3;
// 					}
// 					workStaff = staff->lower();
// 				}
				if (strNr) {
						QGraphicsSimpleTextItem *strItem = new QGraphicsSimpleTextItem(QString("%1").arg(strNr));
						strItem->setFont(QFont("nootka", 5, QFont::Normal));
						strItem->setParentItem(workStaff->noteSegment(0));
						if (workStaff->noteSegment(0)->notePos() > workStaff->upperLinePos() + 7.0)
							strItem->setPos(6.5, workStaff->noteSegment(0)->notePos() - 3.0);
						else
							strItem->setPos(6.5, workStaff->noteSegment(0)->notePos() - 1.0);
				}
		} else { // no notes - clef only
				if (clef == Tclef::e_pianoStaff) {
						topPix = staff->upperLinePos() - 3;
						bottomPix = staff->lowerLinePos() + 12;
				} else {
						topPix = staff->upperLinePos() - 3;
						bottomPix = staff->upperLinePos() + 13;
				}
		}
		if (clef == Tclef::e_pianoStaff)
			leftPix = -1;
		staff->setScale(factor);
		qreal pixWidth = scene->width();
		if (notesCount == 0)
			pixWidth = 13 * factor;
		
		QPixmap pix(pixWidth, qRound((bottomPix - topPix) * factor));
		pix.fill(Qt::transparent);
		QPainter painter(&pix);
		painter.setRenderHints(QPainter::Antialiasing);
		QRectF rect(0, 0, scene->width(), (bottomPix - topPix) * factor);
		scene->render(&painter, rect, QRectF(QPointF(leftPix * factor, topPix * factor), pix.size()));
		delete scene;
		return pix;
}


QString wasAnswerOKtext(TQAunit* answer, QColor textColor, int fontSize) {
    QString txt;
    if (fontSize != -1)
        txt = QString("<span style=\"color: %1; font-size: %2px;\">").arg(textColor.name()).arg(fontSize);
    else
        txt = QString("<span style=\"color: %1;\">").arg(textColor.name());
    if (answer->isCorrect()) {
    txt += QApplication::translate("AnswerText", "Good answer!", "or 'Good!' or 'Correct!' would be somewhat more specific than merely 'It was good!' (previous version) 'It' in this case certainly does refer to a specific thing, which is in this case the answer, but it might be momentarily confused with some other specific thing, such as a shoe or a crocodile, or the wind on one's back. I know that's probably confusing, but the implied subject of 'Correct! is in a certain sense much more specific than a mere 'It' and is more certain to refer to the answer.");
  } else
      if (answer->wrongNote() || answer->wrongPos())
          txt += QApplication::translate("AnswerText", "Wrong answer!");
            else {
                txt += QApplication::translate("AnswerText", "Not bad, but:", "'Not so bad, but:' is perfectly clear, but a little less common in US English. To be a bit shorter, it might just as well be, 'Not bad, but:'") + "<br>";
                QString misMes = ""; // Message with mistakes
                if (answer->wrongString())
                    misMes = QApplication::translate("AnswerText", "wrong string");
                if (answer->wrongAccid())
                    misMes = QApplication::translate("AnswerText", "wrong accidental");
                if (answer->wrongKey()) {
                    addSpaceToNotEmpty(misMes);
                    misMes += QApplication::translate("AnswerText", "wrong key signature");
                }
                if (answer->wrongOctave()) {
                    addSpaceToNotEmpty(misMes);
                    if (misMes.length() > 20)
                        misMes += "<br>";
                    misMes += QApplication::translate("AnswerText", "wrong octave");
                }
                if (answer->wrongIntonation()) {
										addSpaceToNotEmpty(misMes);
										if (misMes.length() > 20 && !misMes.contains("<br>"))
                        misMes += "<br>";
                    misMes += QApplication::translate("AnswerText", "out of tune");
                }
                txt += misMes;
            }
    txt += "</span><br>";
    return txt;
}

QString wrapPixToHtml(const Tnote& note, bool defClef, TkeySignature key, qreal factor, int strNr) {
  return wrapPixToHtml(note, TnotePixmap::clef().type(), key, factor, strNr);
}


QString wrapPixToHtml(const Tnote& note, Tclef::Etype clef, TkeySignature key, qreal factor, int strNr) {
    QPixmap pixmap = getNotePixmap(note, clef, key, factor, strNr);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
}


QPixmap pixFromString(QString glif, QFont font) {
  QFontMetricsF metrics(font);
  QPixmap pix(metrics.width(glif), metrics.height());
  pix.fill(Qt::transparent);
  QPainter painter(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setWindow(0, 0, pix.width(), pix.height());
  painter.setPen(qApp->palette().text().color());
  painter.setBrush(QBrush(Qt::NoBrush));
  painter.setFont(font);
//  painter.drawRect(0, 0, pix.width(), pix.height());
  painter.drawText(0, 0, pix.width(), pix.height(), Qt::AlignCenter, glif);
  return pix;
}


QString pixToHtml(QString imageFile, int width) {
  if (!width)
    return QString("<img src=\"%1\">").arg(imageFile);
  else {
    QPixmap orgPix;
    if (!orgPix.load(imageFile))
      return "";
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    QPixmap scaledPix = orgPix.scaled(width, width);
    scaledPix.save(&buffer, "PNG");
    return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
  }
}








