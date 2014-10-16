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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "ttipchart.h"
#include "tquestionpoint.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <widgets/tquestionaswdg.h>
#include <graphics/tnotepixmap.h>
#include <tnoofont.h>
#include <QApplication>
#include <QBuffer>


/* static */
Tclef TtipChart::defaultClef = Tclef(Tclef::e_treble_G_8down);

QString TtipChart::insertQMark() {
    return TnooFont::span("?", 45, "color: red");
}


// ###################### CONSTRUCTOR ############################

TtipChart::TtipChart(TquestionPoint *point) :
    TgraphicsTextTip(),
    m_point(point)
{ 
	TnotePixmap::setDefaultClef(defaultClef);
  QString txt;
  if (point->nr())
    txt = QString("<span style=\"font-size: 20px\"><b>%1.</b></span><br>").arg(point->nr());
	if (point->question()->melody()) {
		if (point->question()->questionAsNote() && point->question()->answerAsSound())
			txt += ("<b>" + QApplication::translate("TtipChart", "play a melody") + "</b><br>");
		else if (point->question()->questionAsSound() && point->question()->answerAsNote())
			txt += ("<b>" + QApplication::translate("TtipChart", "write a melody") + "</b><br>");
		txt += pixToHtml(getMelodyPixmap(point->question()->melody())) + "<br>";
		txt += QApplication::translate("TtipChart", "%n attempt(s)", "", point->question()->attemptsCount()) + "<br>";
	} else {
		txt += TquestionAsWdg::questionTxt() + ": <b>" + TquestionAsWdg::qaTypeText(point->question()->questionAs) + "</b><br>" +
						TquestionAsWdg::answerTxt() + ": <b>" + TquestionAsWdg::qaTypeText(point->question()->answerAs) + "</b><br>";
		QString qS = "", aS = "";
		switch (point->question()->questionAs) {
			case TQAtype::e_asNote : {
				int qStrNr = 0;
				if (point->question()->answerAs == TQAtype::e_asFretPos) {
					qStrNr = point->question()->qa.pos.str();
					if (qStrNr < 1 || qStrNr > 6)
						qStrNr = 0; // to avoid any stupidity
				}
				qS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key, 4.0, qStrNr);
				if (point->question()->answerAs == TQAtype::e_asNote) {
						qS = wrapPixToHtml(point->question()->qa.note, true, TkeySignature(0)); // no key in question
						aS = wrapPixToHtml(point->question()->qa_2.note, true, point->question()->key);
				}
				break;
			}
			case TQAtype::e_asName:
				qS = "<span style=\"font-size: 25px;\">" + point->question()->qa.note.toRichText(point->question()->styleOfQuestion())
							+ "</span>";
				if (point->question()->answerAs == TQAtype::e_asName)
						aS = "<span style=\"font-size: 25px;\">" + point->question()->qa_2.note.toRichText(point->question()->styleOfAnswer())
									+ "</span>";
				break;
			case TQAtype::e_asFretPos:
					qS = point->question()->qa.pos.toHtml();
				break;
			case TQAtype::e_asSound:
					qS = TnooFont::span("n", 45);
					if (point->question()->answerAs == TQAtype::e_asSound)
							aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
				break;
		}
		if (aS == "") {
				switch (point->question()->answerAs) {
					case TQAtype::e_asNote :
						aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
						break;
					case TQAtype::e_asName:
						aS = "<span style=\"font-size: 25px;\">" + point->question()->qa.note.toRichText(point->question()->styleOfAnswer())
								+ "</span>";
						break;
					case TQAtype::e_asFretPos:
						if (point->question()->questionAs == TQAtype::e_asFretPos)
								aS = point->question()->qa_2.pos.toHtml();
						else
								aS = point->question()->qa.pos.toHtml();
						break;
					case TQAtype::e_asSound:
						if (point->question()->questionAs == TQAtype::e_asNote)
								aS = TnooFont::span("n",45);
						else
								aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
						break;

				}
		}
		txt += "<table valign=\"middle\" align=\"center\"><tr><td> " + qS + " </td><td>" + insertQMark() + " </td><td> " + aS + " </td></tr></table>";
	}
  txt += wasAnswerOKtext(point->question(), point->color());
	if (point->question()->melody() && !point->question()->isWrong())
		txt += TexTrans::effectTxt() + QString(": <b>%1%</b><br>").arg(point->question()->effectiveness(), 0, 'f', 1, '0');
  txt += TexTrans::reactTimeTxt() +
        QString("<span style=\"font-size: 20px\">  %1</span>").arg(Texam::formatReactTime(point->question()->time, true));
  
  setBgColor(point->color());
  setHtml(txt);
  
  setZValue(75);  
  setFlag(QGraphicsItem::ItemIgnoresTransformations);
  
}

TtipChart::~TtipChart() {
}

