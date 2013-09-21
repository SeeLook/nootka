/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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
#include "tkeysignature.h"
#include "tquestionpoint.h"
#include "tqaunit.h"
#include "tscorenote.h"
#include "tscorescene.h"
#include "tscoreclef.h"
#include "tscorekeysignature.h"
#include "tscorepianostaff.h"
#include <QPainter>
#include <QPalette>

// #include <QDebug>

/** Adds comma and space ', ' to not empty string or returns the same. */
QString addSpaceToNotEmpty(QString &txt) {
	if (txt != "")
			txt += ", ";
	return txt;
}


QPixmap getNotePixmap(Tnote note, Tclef::Etype clef, TkeySignature key, qreal factor) {
		TscoreScene *scene = new TscoreScene();
		int notesCount = 1;
		if (note.note == 0) // no note in preview
					notesCount = 0;
		TscoreStaff *staff; 
		if (clef == Tclef::e_pianoStaff)
				staff = new TscorePianoStaff(scene, notesCount);
		else {
				staff =	new TscoreStaff(scene, notesCount);
				staff->scoreClef()->setClef(Tclef(clef));
        staff->onClefChanged();
		}
		if (key.value()) {
				staff->setEnableKeySign(true);
				staff->scoreKey()->setKeySignature(key.value());
		}
	// determine pixmap height by note position to avoid empty spaces above or below a staff
		int topPix = 0, bottomPix = staff->boundingRect().height(), leftPix = 0;
		if (notesCount) {
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
				} else { // piano staff
					if (clef == Tclef::e_pianoStaff && staff->lower()) {
						topPix = staff->upperLinePos() - 4;
						if (staff->lower()->noteSegment(0)->notePos() < staff->lower()->upperLinePos() + 13)
								bottomPix = staff->lower()->pos().y() + staff->lower()->upperLinePos() + 14; // note in lower staff
						else // note below lower staff
								bottomPix = staff->lower()->pos().y() + staff->lower()->noteSegment(0)->notePos() + 3;
					}
				}
		} else { // no notes - clef only
				if (clef == Tclef::e_pianoStaff && staff->lower()) {
						topPix = staff->upperLinePos() - 3;
						bottomPix = staff->lower()->pos().y() + staff->lower()->upperLinePos() + 12;
				} else {
						topPix = staff->upperLinePos() - 3;
						bottomPix = staff->upperLinePos() + 12;
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



