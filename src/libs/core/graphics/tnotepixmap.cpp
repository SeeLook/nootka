/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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
#include <music/tmelody.h>
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
void addSpaceToNotEmpty(QString& txt) {
	if (txt != "")
			txt += ", ";
}


/** Checks the length of string @p txt and adds new line tag if necessary */
void newLineText(QString& txt, const QString& newText) {
	if (txt.length() > 20 && !txt.contains("<br>"))
			txt += "<br>";
	txt += newText;
}

Tclef TnotePixmap::m_clef = Tclef(Tclef::e_treble_G_8down);

QPixmap getNotePixmap(const Tnote& note, Tclef::Etype clef, TkeySignature key, qreal factor, int strNr) {
	TscoreScene *scene = new TscoreScene();
	int notesCount = 1;
	if (note.note == 0) // no note in preview
				notesCount = 0;
	TscoreStaff *staff =	new TscoreStaff(scene, notesCount);
	if (notesCount == 0) { // creating note item initializes accidentals scaling
		TscoreNote *n = new TscoreNote(scene, staff, 0);
		delete n;
	}
	staff->onClefChanged(Tclef(clef));
	if (key.value()) {
			staff->setEnableKeySign(true);
			staff->scoreKey()->setKeySignature(key.value());
			staff->scoreKey()->showKeyName(false);
      staff->setTidyKey(true);
	}
	staff->setNoteDisabled(0, true);
// determine image height by note position to avoid empty spaces above or below a staff
/** It never shows whole piano staff - just single staff where is some note,
	* but when no note, both staves are displayed. */
	int topPix = staff->hiNotePos(), bottomPix = staff->loNotePos(), leftPix = 0;
	if (notesCount) {
			staff->setNote(0, note);
			topPix = staff->hiNotePos();
			bottomPix = staff->loNotePos() + 2;
			if (clef == Tclef::e_pianoStaff) {
				Tnote tmpN = note;
				if (tmpN.chromatic() < 13)
					topPix = staff->lowerLinePos() - 4;
				else
					bottomPix = staff->lowerLinePos() - 2;
			}
			if (strNr) {
					QGraphicsSimpleTextItem *strItem = new QGraphicsSimpleTextItem(QString("%1").arg(strNr));
					strItem->setFont(QFont("nootka", 5, QFont::Normal));
					strItem->setParentItem(staff->noteSegment(0));
					strItem->setPos(6.5, staff->noteSegment(0)->notePos() - 1.0);
			}
	}
	if (clef == Tclef::e_pianoStaff)
		leftPix = -1;
	staff->setScale(factor);
	qreal pixWidth = scene->width();
	if (notesCount == 0)
		pixWidth = 9 * factor;
	
	QPixmap pix(pixWidth, qRound((bottomPix - topPix) * factor));
	pix.fill(Qt::transparent);
	QPainter painter(&pix);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	QRectF rect(0, 0, scene->width(), (bottomPix - topPix) * factor);
	scene->render(&painter, rect, QRectF(QPointF(leftPix * factor, topPix * factor), pix.size()));
	delete scene;
	return pix;
}


QPixmap getMelodyPixmap(Tmelody* mel, bool showStrings, qreal factor) {
	if (mel == 0 || mel->length() == 0)
		return QPixmap();
	TscoreScene *scene = new TscoreScene();
	
	TscoreStaff *staff =	new TscoreStaff(scene, qMin(mel->length(), 15)); // TODO long melodies preview
	staff->onClefChanged(Tclef(mel->clef()));
	if (mel->key().value()) {
		staff->setEnableKeySign(true);
		staff->scoreKey()->setKeySignature(mel->key().value());
		staff->scoreKey()->showKeyName(false);
    staff->setTidyKey(true);
	}
	staff->setDisabled(true);
	for (int i = 0; i < staff->count(); ++i)
		staff->setNote(i, mel->note(i)->p());
	int topPix = staff->hiNotePos(), bottomPix = staff->loNotePos(), leftPix = 0;
	if (showStrings) { // TODO if necessary
// 					QGraphicsSimpleTextItem *strItem = new QGraphicsSimpleTextItem(QString("%1").arg(strNr));
// 					strItem->setFont(QFont("nootka", 5, QFont::Normal));
// 					strItem->setParentItem(staff->noteSegment(0));
// 					if (staff->noteSegment(0)->notePos() > staff->upperLinePos() + 7.0)
// 						strItem->setPos(6.5, staff->noteSegment(0)->notePos() - 3.0);
// 					else
// 						strItem->setPos(6.5, staff->noteSegment(0)->notePos() - 1.0);
	}
	if (mel->clef() == Tclef::e_pianoStaff)
		leftPix = -1;
	staff->setScale(factor);
	qreal pixWidth = scene->width();
	
	QPixmap pix(pixWidth, qRound((bottomPix - topPix) * factor));
	pix.fill(Qt::transparent);
	QPainter painter(&pix);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	QRectF rect(0, 0, scene->width(), (bottomPix - topPix) * factor);
	scene->render(&painter, rect, QRectF(QPointF(leftPix * factor, topPix * factor), pix.size()));
	delete scene;
	return pix;
}


QString wrapPixToHtml(const Tnote& note, bool defClef, TkeySignature key, qreal factor, int strNr) {
  return wrapPixToHtml(note, TnotePixmap::clef().type(), key, factor, strNr);
}


QString wrapPixToHtml(const Tnote& note, Tclef::Etype clef, TkeySignature key, qreal factor, int strNr) {
	return pixToHtml(getNotePixmap(note, clef, key, factor, strNr));
}


QString pixToHtml(const QPixmap& pix) {
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	pix.save(&buffer, "PNG");
	return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
}


QPixmap pixFromString(const QString& glif, const QFont& font, const QColor& color) {
  QFontMetricsF metrics(font);
  QPixmap pix(metrics.width(glif), metrics.height());
  pix.fill(Qt::transparent);
  QPainter painter(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setWindow(0, 0, pix.width(), pix.height());
  if (color == -1)
    painter.setPen(qApp->palette().text().color());
  else
    painter.setPen(color);
  painter.setBrush(QBrush(Qt::NoBrush));
  painter.setFont(font);
//  painter.drawRect(0, 0, pix.width(), pix.height());
  painter.drawText(0, 0, pix.width(), pix.height(), Qt::AlignCenter, glif);
  return pix;
}


QString pixToHtml(const QString& imageFile, int width) {
  if (!width)
    return QString("<img src=\"%1\">").arg(imageFile);
  else {
    QPixmap orgPix;
    if (!orgPix.load(imageFile))
      return "";
    return pixToHtml(orgPix.scaled(width, width, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
}


QString wasAnswerOKtext(TQAunit* answer, const QColor& textColor, int fontSize) {
    QString txt;
    if (fontSize != -1)
        txt = QString("<span style=\"color: %1; font-size: %2px;\">").arg(textColor.name()).arg(fontSize);
    else
        txt = QString("<span style=\"color: %1;\">").arg(textColor.name());
    if (answer->isCorrect()) {
    txt += QApplication::translate("AnswerText", "Good answer!", "or 'Good!' or 'Correct!' would be somewhat more specific than merely 'It was good!' (previous version) 'It' in this case certainly does refer to a specific thing, which is in this case the answer, but it might be momentarily confused with some other specific thing, such as a shoe or a crocodile, or the wind on one's back. I know that's probably confusing, but the implied subject of 'Correct! is in a certain sense much more specific than a mere 'It' and is more certain to refer to the answer.");
  } else
      if (answer->wrongNote() || answer->wrongPos() || answer->veryPoor())
          txt += QApplication::translate("AnswerText", "Wrong answer!");
			else {
					txt += QApplication::translate("AnswerText", "Not bad, but:", "'Not so bad, but:' is perfectly clear, but a little less common in US English. To be a bit shorter, it might just as well be, 'Not bad, but:'") + "<br>";
					QString misMes = ""; // Message with mistakes
					if (answer->wrongString())
							misMes = QApplication::translate("AnswerText", "wrong string");
					if (answer->melody() && answer->littleNotes())
							misMes = QApplication::translate("AnswerText", "little valid notes", "the amount of correct notes in an answer is little");
					if (answer->poorEffect()) {
							addSpaceToNotEmpty(misMes);
							if (!misMes.isEmpty())
								misMes += "<br>";
							misMes += QApplication::translate("AnswerText", "poor effectiveness");
					}
					if (answer->wrongAccid())
							misMes = QApplication::translate("AnswerText", "wrong accidental");
					if (answer->wrongKey()) {
							addSpaceToNotEmpty(misMes);
							newLineText(misMes, QApplication::translate("AnswerText", "wrong key signature"));
					}
					if (answer->wrongOctave()) {
							addSpaceToNotEmpty(misMes);
							newLineText(misMes, QApplication::translate("AnswerText", "wrong octave"));
					}
					if (answer->wrongIntonation()) {
							addSpaceToNotEmpty(misMes);
							newLineText(misMes, QApplication::translate("AnswerText", "out of tune"));
					}
					txt += misMes;
			}
    txt += "</span><br>";
    return txt;
}












