/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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
#include <tscorenote.h>
#include <tscorescene.h>
#include <tscoreclef.h>
#include <tscorekeysignature.h>
#include <tscorestaff.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtCore/qbuffer.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>



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













