/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <exam/tresulttext.h>
#include <widgets/tquestionaswdg.h>
#include <graphics/tnotepixmap.h>
#include <tnoofont.h>
#include <QApplication>


/* static */
Tclef TtipChart::defaultClef = Tclef(Tclef::e_treble_G_8down);

// ###################### CONSTRUCTOR ############################

TtipChart::TtipChart(TquestionPoint *point) :
  TgraphicsTextTip(),
  m_point(point)
{ 
  if (point->question()->melody())
    return;
	TnotePixmap::setDefaultClef(defaultClef);
  QString txt;
  if (point->nr())
    txt = QString("<big><b>%1.</b></big>").arg(point->nr());
  txt += "<br>" + TquestionAsWdg::questionTxt() + ": <b>" + TquestionAsWdg::qaTypeText(point->question()->questionAs) + "</b><br>" +
          TquestionAsWdg::answerTxt() + ": <b>" + TquestionAsWdg::qaTypeText(point->question()->answerAs) + "</b><br>";
  QString qS = "", aS = "";
  switch (point->question()->questionAs) {
    case TQAtype::e_onScore : {
      int qStrNr = 0;
      if (point->question()->answerAs == TQAtype::e_onInstr) {
        qStrNr = point->question()->qa.pos.str();
        if (qStrNr < 1 || qStrNr > 6)
          qStrNr = 0; // to avoid any stupidity
      }
      qS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key, 4.0, qStrNr);
      if (point->question()->answerAs == TQAtype::e_onScore) {
          qS = wrapPixToHtml(point->question()->qa.note, true, TkeySignature(0)); // no key in question
          aS = wrapPixToHtml(point->question()->qa_2.note, true, point->question()->key);
      }
      break;
    }
    case TQAtype::e_asName:
      qS = "<span style=\"font-size: xx-large;\">" + point->question()->qa.note.toRichText(point->question()->styleOfQuestion())
            + "</span>";
      if (point->question()->answerAs == TQAtype::e_asName)
          aS = "<span style=\"font-size: xx-large;\">" + point->question()->qa_2.note.toRichText(point->question()->styleOfAnswer())
                + "</span>";
      break;
    case TQAtype::e_onInstr:
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
        case TQAtype::e_onScore :
          aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
          break;
        case TQAtype::e_asName:
          aS = "<span style=\"font-size: xx-large;\">" + point->question()->qa.note.toRichText(point->question()->styleOfAnswer())
              + "</span>";
          break;
        case TQAtype::e_onInstr:
          if (point->question()->questionAs == TQAtype::e_onInstr)
              aS = point->question()->qa_2.pos.toHtml();
          else
              aS = point->question()->qa.pos.toHtml();
          break;
        case TQAtype::e_asSound:
          if (point->question()->questionAs == TQAtype::e_onScore)
              aS = TnooFont::span("n",45);
          else
              aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
          break;

      }
  }
  txt += "<table valign=\"middle\" align=\"center\"><tr><td> " + qS + " </td><td>" + TnooFont::span("?", 45, "color: red") +
          " </td><td> " + aS + " </td></tr></table>";
  txt += wasAnswerOKtext(point->question(), point->color());
	if (point->question()->melody() && !point->question()->isWrong())
		txt += TexTrans::effectTxt() + QString(": <b>%1%</b><br>").arg(point->question()->effectiveness(), 0, 'f', 1, '0');
  txt += TexTrans::reactTimeTxt() +
        QString("<big><b>  %1</b></big>").arg(Texam::formatReactTime(point->question()->time, true));
  
  setBgColor(point->color());
  setHtml(txt);  
}







