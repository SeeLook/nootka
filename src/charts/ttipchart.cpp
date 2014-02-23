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
#include "tqaunit.h"
#include "texamview.h"
#include "tquestionaswdg.h"
#include "tquestionpoint.h"
#include "tnotepixmap.h"
#include <QApplication>
#include <QBuffer>


/* static */
Tclef TtipChart::defaultClef = Tclef(Tclef::e_treble_G_8down);

QString TtipChart::insertQMark() {
    return QString("<span style=\"color: red; font-family: nootka; font-size: 45px;\">?</span>");
}

QString TtipChart::wrapPixToHtml(const Tnote& note, bool defClef, TkeySignature key, qreal factor, int strNr) {
  return wrapPixToHtml(note, defaultClef.type(), key, factor, strNr);
}


QString TtipChart::wrapPixToHtml(const Tnote& note, Tclef::Etype clef, TkeySignature key, qreal factor, int strNr) {
    QPixmap pixmap = getNotePixmap(note, clef, key, factor, strNr);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
}


// ###################### CONSTRUCTOR ############################

TtipChart::TtipChart(TquestionPoint *point) :
    TgraphicsTextTip(),
    m_point(point)
{
//   if ((point->question()->questionAs == TQAtype::e_asName || point->question()->answerAs == TQAtype::e_asName) 
//     && (int)point->question()->styleOfQuestion() < 0) {
//         point->question()->setStyle(point->question()->styleOfAnswer(), point->question()->styleOfAnswer());
//         qDebug("!!! It still has got wrong style !!!");
//   }
 
  QString txt;
  if (point->nr())
    txt = QString("<span style=\"font-size: 20px\"><b>%1.</b></span><br>").arg(point->nr());
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
        qS = QString("<span style=\"font-family: nootka; font-size: 45px;\">n</span>");
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
          aS = "<span style=\"font-size: 25px;\">" + point->question()->qa.note.toRichText(point->question()->styleOfQuestion())
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
              aS = QString("<span style=\"font-family: nootka; font-size: 45px;\">n</span>");
          else
              aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
          break;

      }
  }
  txt += "<table valign=\"middle\" align=\"center\"><tr><td> " + qS + " </td><td>" + insertQMark() + " </td><td> " + aS + " </td></tr></table>";
  txt += wasAnswerOKtext(point->question(), point->color());
  txt += TexamView::reactTimeTxt() +
        QString("<span style=\"font-size: 20px\">  %1</span>").arg(TexamView::formatReactTime(point->question()->time, true));
  
  setBgColor(point->color());
  setHtml(txt);
  
  setZValue(75);  
  setFlag(QGraphicsItem::ItemIgnoresTransformations);
  
}

TtipChart::~TtipChart() {
}

