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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "ttipchart.h"
#include "tqaunit.h"
#include "texamview.h"
#include "tquestionaswdg.h"
#include "tquestionpoint.h"
#include "tnotename.h"
#include "tnotepixmap.h"
#include <QApplication>
#include <QBuffer>
// #include <QDebug>


/* static */
QString TtipChart::insertQMark() {
    return QString("<span style=\"color: red; font-family: nootka; font-size: 45px;\">?</span>");
}

QString TtipChart::wrapPixToHtml(Tnote note, bool clef, TkeySignature key, double factor) {
    QPixmap pixmap = getNotePixmap(note, clef, key, factor);
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
  QString txt = TquestionAsWdg::questionTxt() + " " + TquestionAsWdg::qaTypeText(point->question()->questionAs) + "<br>" +
          TquestionAsWdg::answerTxt() + " " + TquestionAsWdg::qaTypeText(point->question()->answerAs) + "<br>";
  QString qS = "", aS = "";
  switch (point->question()->questionAs) {
    case TQAtype::e_asNote :
      qS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
      if (point->question()->answerAs == TQAtype::e_asNote) {
          qS = wrapPixToHtml(point->question()->qa.note, true, TkeySignature(0)); // no key in question
          aS = wrapPixToHtml(point->question()->qa_2.note, true, point->question()->key);
      }
      break;
    case TQAtype::e_asName:
      qS = "<span style=\"font-size: 25px;\">" + TnoteName::noteToRichText(point->question()->qa.note) + "</span>";
      if (point->question()->answerAs == TQAtype::e_asName)
          aS = "<span style=\"font-size: 25px;\">" + TnoteName::noteToRichText(point->question()->qa_2.note) + "</span>";
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
          aS = "<span style=\"font-size: 25px;\">" + TnoteName::noteToRichText(point->question()->qa.note) + "</span>";
          break;
        case TQAtype::e_asFretPos:
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
        QString("<span style=\"font-size: 20px\">  %1s</span>").arg(point->question()->timeText());
  
  setBgColor(point->color());
  setHtml(txt);
  
  setZValue(75);  
  setFlag(QGraphicsItem::ItemIgnoresTransformations);
  
}

TtipChart::~TtipChart() {
}

