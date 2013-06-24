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
		if (notesCount)
				staff->setNote(0, note);
		staff->setScale(factor);
		
		QPixmap pix(scene->width(), scene->height());    
		pix.fill(Qt::transparent); // white background
		QPainter painter(&pix);
		scene->render(&painter);
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
    txt += QApplication::translate("AnswerText", "It was good!");
  } else
      if (answer->wrongNote() || answer->wrongPos())
          txt += QApplication::translate("AnswerText", "Wrong answer!");
            else {
                txt += QApplication::translate("AnswerText", "Not so bad, but:") + "<br>";
                QString misMes = ""; // Message with mistakes
                if (answer->wrongString())
                    misMes = QApplication::translate("AnswerText", "wrong string");
                if (answer->wrongAccid())
                    misMes = QApplication::translate("AnswerText", "wrong accidental");
                if (answer->wrongKey()) {
                    if (misMes != "")
                        misMes += ", ";
                    misMes += QApplication::translate("AnswerText", "wrong key signature");
                }
                if (answer->wrongOctave()) {
                    if (misMes != "")
                        misMes += ", ";
                    if (misMes.length() > 20)
                        misMes += "<br>";
                    misMes += QApplication::translate("AnswerText", "wrong octave");
                }
                txt += misMes;
            }
    txt += "</span><br>";
    return txt;
}



