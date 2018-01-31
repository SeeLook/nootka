/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#include "tstatisticstip.h"
#include "tgroupedqaunit.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <tcolor.h>
#include <tinitcorelib.h>
#include <QApplication>



QString trStyle(QColor c) {
    c.setAlpha(30);
    return QString("style=\"%1\"").arg(Tcolor::bgTag(c));
}

/*static*/
TstatisticsTip::Ekind TstatisticsTip::m_kind = e_full;

QString TstatisticsTip::getTipText(TgroupedQAunit* qaGroup)  {
    QString tipText = QApplication::translate("TstatisticTip", "Statistics") + "<br>";
    tipText += qaGroup->fullDescription() + "<hr>";
    tipText += "<table>";
    if (m_kind != e_mistakes)
        tipText += "<tr><td>" + 
            TexTrans::effectTxt() + ": </td><td> <b>" + QString("%1 %").arg(qaGroup->effectiveness(), 2, 'f', 0, '0') + 
            "</b></td></tr>";
    tipText += "<tr><td>" + TexTrans::averAnsverTimeTxt() + ": </td><td> <b>" + Texam::formatReactTime(qaGroup->averTime(), true) + "</b></td></tr>";
    if (m_kind != e_mistakes)
        tipText += "<tr><td>" + 
        QApplication::translate("TanalysDialog", "Questions number") + QString(": </td><td> <b>%1</b></td></tr>").arg(qaGroup->size());
    if (qaGroup->size() > (qaGroup->mistakes() + qaGroup->notBad())) {
      tipText += "<tr " + trStyle(Tcore::gl()->EanswerColor) + "><td>";
      tipText += TexTrans::corrAnswersNrTxt() + QString(": </td><td> <b>%1</b></td></tr>")
        .arg(qaGroup->size() - qaGroup->mistakes() - qaGroup->notBad());
    }
    if (qaGroup->mistakes())
      tipText += "<tr " + trStyle(Tcore::gl()->EquestionColor)  + "><td>" + TexTrans::mistakesNrTxt() +
              QString(": </td><td> <b>%1</b></td></tr>").arg(qaGroup->mistakes());
    if (qaGroup->notBad())
      tipText += "<tr " + trStyle(Tcore::gl()->EnotBadColor) + "><td>" + TexTrans::halfMistakenTxt() +
              QString(": </td><td> <b>%1</b></td></tr>").arg(qaGroup->notBad());
    tipText += "</table>";
    return tipText;
}

QString TstatisticsTip::getAverTimeStat(TgroupedQAunit* qaGroup, QString ofSomething) {
    if (qaGroup)
      return "<p>" + ofSomething + "<br>" + TexTrans::averAnsverTimeTxt() + "<br>" + "<big><b>" +
        Texam::formatReactTime(qRound(qaGroup->averTime()), true) +"</b></big></p>";
    else 
      return ofSomething;
}




TstatisticsTip::TstatisticsTip(TgroupedQAunit* qaGroup, Ekind kind, QString desc) :
    TgraphicsTextTip(),
    m_qaGroup(qaGroup)
{
    m_kind = kind;
    setBgColor(QColor(0, 192, 192)); // light blue
    if (kind == e_simple)
      setHtml(getAverTimeStat(qaGroup, desc));
    else
      setHtml(getTipText(qaGroup));
}

TstatisticsTip::~TstatisticsTip() {}




