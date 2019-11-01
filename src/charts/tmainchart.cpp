/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tmainchart.h"
#include "sorting.h"
#include <exam/tlevel.h>
#include <exam/texam.h>
#include <music/tnamestylefilter.h>
#include <tnoofont.h>


TmainChart::TmainChart(QObject* parent):
  Tchart(parent),
  p_hasListUnrelated(false),
  p_goodSize(0)
{  
}


void TmainChart::setExam(Texam* e) {
  if (e != p_currExam) {
    p_currExam = e;
  }
}


void TmainChart::setChartSettings(const Tchart::Tsettings& s) {
  p_chartSett = s;
}


QString TmainChart::ticText(TQAunit* unit, int questNr) {
  if (!unit)
    return QString();

  QString txt;
  if (questNr)
    txt = QString("%1.<br>").arg(questNr);
  QString altStyleText;
  auto l = p_currExam->level();
  if (l->requireStyle || (l->questionAs.isName() && l->answersAs[TQAtype::e_asName].isName())) {
    /** Display alternate to user pref names but only for levels where different styles can occur */
    Tnote::EnameStyle altStyle;
    if (Tnote::defaultStyle == Tnote::e_italiano_Si || Tnote::defaultStyle == Tnote::e_russian_Ci)
      altStyle = TnameStyleFilter::get(Tnote::e_english_Bb);
    else
      altStyle = TnameStyleFilter::get(Tnote::e_italiano_Si);
    altStyleText = QString(" <small><i>(%1)</small></i>").arg(unit->qa.note.toRichText(altStyle, false));
  }
  if (unit->melody()) {
      txt += "<small>" + QGuiApplication::translate("TXaxis", "%n attempt(s)", "", unit->attemptsCount()) + "</small>";
      txt.replace(QLatin1String("<br>"), QString());
  } else {
      txt += QString("<b>%1</b>").arg(unit->qa.note.toRichText()) + altStyleText;
      if (unit->questionAs == TQAtype::e_onInstr || unit->answerAs == TQAtype::e_onInstr || unit->answerAs == TQAtype::e_asSound)
        txt += QLatin1String("<br>") + TnooFont::span(QString::number(static_cast<int>(unit->qa.pos().str())), 15)
        + QString("<span style=\"font-size: 15px;\">%1</span>").arg(TfingerPos::romanFret(unit->qa.pos().fret()));
  }
  if (l->useKeySign && (unit->questionAs == TQAtype::e_onScore || unit->answerAs == TQAtype::e_onScore))
    txt += QLatin1String("<br><i>") + unit->key.getName() + QLatin1String("</i>");
  return txt;
}


TQAunit* TmainChart::getSorted(int unitNr) {
  int cnt = 0;
  for (int i = 0; i < p_sortedLists.count(); i++) {
    if (unitNr < cnt + p_sortedLists[i].size())
      return p_sortedLists[i][unitNr - cnt].qaPtr;
    cnt += p_sortedLists[i].size();
  }
  qDebug() << "[TmainChart] BOOOOOM, looking for sorted question failed";
  return nullptr;
}


TqaPtr* TmainChart::getSortedPtr(int ptrNr) {
  int cnt = 0;
  for (int i = 0; i < p_sortedLists.count(); i++) {
    if (ptrNr < cnt + p_sortedLists[i].size())
      return &p_sortedLists[i][ptrNr - cnt];
    cnt += p_sortedLists[i].size();
  }
  qDebug() << "[TmainChart] BOOOOOM, looking for sorted pointer failed";
  return nullptr;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmainChart::sort() {
  TgroupedQAunit::setSkipWrong(!p_chartSett.inclWrongAnsw);
  if (p_chartSett.separateWrong) {
      divideGoodAndBad(p_currExam->answList(), p_goodAnsw, p_badAnsw);
      if (p_chartSett.order == e_byNote)
        p_sortedLists = sortByNote(p_goodAnsw, p_currExam->level(), p_hasListUnrelated);
      else if (p_chartSett.order == e_byFret)
        p_sortedLists = sortByFret(p_goodAnsw, p_currExam->level(), p_hasListUnrelated);
      else if (p_chartSett.order == e_byKey)
        p_sortedLists = sortByKeySignature(p_goodAnsw, p_currExam->level(), p_hasListUnrelated);
      else if (p_chartSett.order == e_byAccid)
        p_sortedLists = sortByAccidental(p_goodAnsw, p_currExam->level(), p_hasListUnrelated, p_kindOfAccids);
      // e_byQuestAndAnsw & e_byMistake never separate wrong answers
      p_goodSize = p_sortedLists.size(); // number without wrong answers
      if (p_chartSett.order == e_byNote)
        p_sortedLists.append(sortByNote(p_badAnsw, p_currExam->level(), p_hasListUnrelated));
      else if (p_chartSett.order == e_byFret)
        p_sortedLists.append(sortByFret(p_badAnsw, p_currExam->level(), p_hasListUnrelated));
      else if (p_chartSett.order == e_byKey)
        p_sortedLists.append(sortByKeySignature(p_badAnsw, p_currExam->level(), p_hasListUnrelated));
      else if (p_chartSett.order == e_byAccid)
        p_sortedLists.append(sortByAccidental(p_badAnsw, p_currExam->level(), p_hasListUnrelated, p_kindOfAccids));
  } else {
      TgroupedQAunit convList;
      convertToGroupedQAunit(p_currExam->answList(), convList);
      switch (p_chartSett.order) {
        case e_byNote:
          p_sortedLists = sortByNote(convList, p_currExam->level(), p_hasListUnrelated);
          break;
        case e_byFret:
          p_sortedLists = sortByFret(convList, p_currExam->level(), p_hasListUnrelated);
          break;
        case e_byKey:
          p_sortedLists = sortByKeySignature(convList, p_currExam->level(), p_hasListUnrelated);
          break;
        case e_byAccid:
          p_sortedLists = sortByAccidental(convList, p_currExam->level(), p_hasListUnrelated, p_kindOfAccids);
          break;
        case e_byQuestAndAnsw: // in this case wrong answers aren't separate
          p_sortedLists = sortByQAtype(convList, p_currExam->level(), p_hasListUnrelated);
          break;
        case e_byMistake:
          p_sortedLists = sortByMisakes(convList, p_currExam->level(), p_hasListUnrelated);
          break;
        default: break;
      }
      p_goodSize = p_sortedLists.size();
  }
  for (int i = 0; i < p_sortedLists.count(); i++) {
    for (int j = 0; j < p_sortedLists[i].size(); j++) {
      p_sortedLists[i][j].grNr = i;
    }
  }
}


void TmainChart::prepareChart(int maxX) {
  p_xCount = maxX;
}

