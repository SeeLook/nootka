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

#include "tlevelpreview.h"
#include "texamlevel.h"
#include "tquestionaswdg.h"
#include <tnotename.h>
#include <QVBoxLayout>
#include <QLabel>
#include "tglobals.h"
#include <ttipchart.h>


extern Tglobals *gl;


TlevelPreview::TlevelPreview(QWidget* parent) :
  QWidget(parent)
{
		setFixedWidth(400);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel(tr("Level summary:"), this);
    mainLay->addWidget(headLab);
		QHBoxLayout *contLay = new QHBoxLayout;
    m_summLab = new QLabel(tr("\n no level selected"), this);
//     summLab->setFixedWidth(300);
    contLay->addWidget(m_summLab);
		contLay->addSpacing(10);
		m_clefLabel = new QLabel(this);
		m_clefLabel->setAlignment(Qt::AlignCenter);
		contLay->addWidget(m_clefLabel);
		mainLay->addLayout(contLay);
    mainLay->addStretch(1);
    setLayout(mainLay);
}

TlevelPreview::~TlevelPreview() {}

void TlevelPreview::setLevel(TexamLevel& tl) {
  QString S;
    S = "<center><b>" + tl.name + "</b>";
    S += "<table border=\"1\">";
    S += "<tr><td>" + notesRangeTxt() + " </td>";
    S += "<td>" + TnoteName::noteToRichText(tl.loNote) + " - "
         + TnoteName::noteToRichText(tl.hiNote) + "</td></tr>";
    if (tl.questionAs.isFret() || tl.answersAs[0].isFret()
        || tl.answersAs[1].isFret() || tl.answersAs[2].isFret()
        || tl.answersAs[3].isFret()) { // level uses guitar
        S += "<tr><td>" + fretsRangeTxt() + " </td>";
        S += QString("<td>%1 - %2").arg(int(tl.loFret)).arg(int(tl.hiFret)) + "</td></tr>";
    }
    if (tl.useKeySign) {
        S += "<tr><td>" + tr("key signature:") + " </td><td>";
        S += tl.loKey.getMajorName().remove("-"+gl->SmajKeyNameSufix);
        S += " (" + tl.loKey.accidNumber(true) +")";
        if (!tl.isSingleKey) {
            S += " - " + tl.hiKey.getMajorName().remove("-"+gl->SmajKeyNameSufix);
            S += " (" + tl.hiKey.accidNumber(true) + ")";
        }
        S += "</td></tr>";
    }
    S += "<tr><td>" + tr("accidentals:") + " </td><td>";
    if (!tl.withSharps && !tl.withFlats && !tl.withDblAcc)
        S += tr("none");
    else {
        if (tl.withSharps) S += " <i>#</i>";
        if (tl.withFlats) S += " <i>b</i>";
        if (tl.withDblAcc) S += " <i>x bb</i>";
    }
    S += "</td></tr>";
    S += "<tr><td>" + TquestionAsWdg::questionsTxt() + ": </td><td align=\"center\">"; // QUESTIONS
    QString tmp;
    if (tl.questionAs.isNote())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asNote) + " ";
    if (tl.questionAs.isName())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asName) + " ";
    if (tl.questionAs.isFret())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asFretPos) + " ";
    if (tl.questionAs.isSound())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asSound);
    int fontSize = fontMetrics().boundingRect("A").height() * 1.3;
    S += TquestionAsWdg::spanNootka(tmp, fontSize);
    S += "</td></tr>";
    tmp   = "";
    S += "<tr><td>" + TquestionAsWdg::answersTxt() + ": </td><td align=\"center\">"; // ANSWERS
      /** Checking questions would be skiped because Level creator avoids selecting answer without question.
       * Unfortunaletly built-in leves are not so perfect.*/
    if (  (tl.questionAs.isNote() && tl.answersAs[TQAtype::e_asNote].isNote()) ||
          (tl.questionAs.isName() && tl.answersAs[TQAtype::e_asName].isNote()) ||
          (tl.questionAs.isFret() && tl.answersAs[TQAtype::e_asFretPos].isNote()) ||
          (tl.questionAs.isSound() && tl.answersAs[TQAtype::e_asSound].isNote()) )
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asNote) + " ";
    if (  (tl.questionAs.isNote() && tl.answersAs[TQAtype::e_asNote].isName()) ||
          (tl.questionAs.isName() && tl.answersAs[TQAtype::e_asName].isName()) ||
          (tl.questionAs.isFret() && tl.answersAs[TQAtype::e_asFretPos].isName()) ||
          (tl.questionAs.isSound() && tl.answersAs[TQAtype::e_asSound].isName()) )
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asName) + " ";
    if (  (tl.questionAs.isNote() && tl.answersAs[TQAtype::e_asNote].isFret()) ||
          (tl.questionAs.isName() && tl.answersAs[TQAtype::e_asName].isFret()) ||
          (tl.questionAs.isFret() && tl.answersAs[TQAtype::e_asFretPos].isFret()) ||
          (tl.questionAs.isSound() && tl.answersAs[TQAtype::e_asSound].isFret()) )
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asFretPos) + " ";
    if (  (tl.questionAs.isNote() && tl.answersAs[TQAtype::e_asNote].isSound()) ||
          (tl.questionAs.isName() &&  tl.answersAs[TQAtype::e_asName].isSound()) ||
          (tl.questionAs.isFret() && tl.answersAs[TQAtype::e_asFretPos].isSound()) ||
          (tl.questionAs.isSound() && tl.answersAs[TQAtype::e_asSound].isSound()) )
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asSound);
    S += TquestionAsWdg::spanNootka(tmp, fontSize);
    S += "</td></tr>";
    if (tl.canBeName() || tl.canBeScore() || tl.canBeSound()) {
      S += "<tr><td colspan=\"2\" align=\"center\">";
      if (tl.requireOctave)
          S += tr("propper octave is required");
      else
          S += tr("octave has no matter");
      S += "</td></tr>";
    }
    S += "</table></center>";
    m_summLab->setText(S);
		m_clefLabel->setText("<center>" + tr("Clef") + 
				QString(":<br><br><span style=\"font-family: nootka; font-size: 60px;\"> %1</span></center>").
				arg(TtipChart::wrapPixToHtml(Tnote(0, 0, 0), tl.clef.type(), TkeySignature(0), 5.0)));
}


