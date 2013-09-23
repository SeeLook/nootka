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


#include "tquestiontip.h"
#include "tqaunit.h"
#include "texam.h"
#include <tnotename.h>
#include "tquestionaswdg.h"
#include "tglobals.h"
#include <texamlevel.h>
#include <ttipchart.h>
// #include <QDebug>

extern Tglobals *gl;




/* static */
QString TquestionTip::getTextHowAccid(Tnote::Eacidentals accid) {
    QString S = QString("<br><span style=\"color: %1\">").arg(gl->GfingerColor.name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  "</span>";
    return S;
}


QString TquestionTip::onStringTxt(quint8 strNr) {
    return tr("on <span style=\"font-family: nootka;\">%1</span> string.").arg(strNr);
}

//##########################################################################################
//#################################### CONSTRUCTOR #########################################
//##########################################################################################

TquestionTip::TquestionTip(Texam* exam, double scale) :
  TgraphicsTextTip(getQuestion(exam->question(exam->count()-1), exam->count(), exam->level(), scale))
{
  setBgColor(gl->EquestionColor);
}

TquestionTip::~TquestionTip() {}


//##########################################################################################
//#################################### PROTECTED ###########################################
//##########################################################################################

QString TquestionTip::getNiceNoteName(Tnote note, Tnote::EnameStyle style) {
    return QString("<b><span style=\"%1\">&nbsp;").arg(gl->getBGcolorText(gl->EquestionColor)) +
            note.toRichText(style) + " </span></b>";
}

QString TquestionTip::getQuestion(TQAunit& question, int questNr, TexamLevel* level, double scale) {
  m_scoreFree = true;
  m_nameFree = true;
  m_guitarFree = true;
  QString quest;
  double sc = 4.0;
// 	qDebug() << "scale" << sc << "font size" << qRound(scale * 22.0);
  if (scale) {
    quest = QString("<p style=\"font-size: %1px;\">").arg(qRound(scale * 22.0));
    sc = 4.0 * scale;     
  }
  quest += QString("<b><u>&nbsp;%1.&nbsp;</u></b><br>").arg(questNr);
    QString apendix = "";
    QString noteStr;
    switch (question.questionAs) {
      case TQAtype::e_asNote:
        m_scoreFree = false;
        if (question.answerAs == TQAtype::e_asNote) {
            if (question.qa.note.acidental != question.qa_2.note.acidental)
                quest += tr("Change enharmonically and show on the staff");
            else
                quest += tr("Given note show on the staff");
          if (level->useKeySign && level->manualKey) {
            apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(question.key.getName());
          }
//           if (level->forceAccids)
            quest += getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Show on the guitar");
              if (level->showStrNr)
                apendix = "<br><b> " + onStringTxt(question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing");
              }
        if (level->useKeySign && level->manualKey && question.answerAs == TQAtype::e_asNote) // hide key signature
            quest += "<br>" + TtipChart::wrapPixToHtml(question.qa.note, true, TkeySignature(0), sc);
        else
            quest += "<br>" + TtipChart::wrapPixToHtml(question.qa.note, true, question.key, sc);
        if (apendix != "")
          quest += apendix;
      break;
      
      case TQAtype::e_asName:
        m_nameFree = false;
        noteStr = "<br>" + getNiceNoteName(question.qa.note, question.styleOfQuestion());
        if (question.answerAs == TQAtype::e_asNote) {
          m_nameFree = false;
          quest += tr("Show on the staff") + noteStr;
          if (level->useKeySign && level->manualKey) {
            quest += tr("<br><b>in %1 key.</b>", "in key signature").arg(question.key.getName());
          }
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            noteStr = "<br>" + getNiceNoteName(question.qa.note, question.styleOfQuestion());
            if (question.qa.note.acidental != question.qa_2.note.acidental) {
                quest += tr("Change enharmonically and give name of");
                quest += noteStr + getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
            } else
                quest += tr("Use another style to give name of") + noteStr;
//             quest += noteStr + getTextHowAccid((Tnote::Eacidentals)question.qa_2.note.acidental);
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Show on the guitar") + noteStr;
              if (level->showStrNr)
                quest += "<br><b> " + onStringTxt(question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += "<br>" + tr("Play or sing") + noteStr;
              }
      break;
      
      case TQAtype::e_asFretPos:
        quest += "";
        m_guitarFree = false;
        if (question.answerAs == TQAtype::e_asNote) {
          m_scoreFree = false;
          quest += tr("Show on the staff note played on");
          if (level->useKeySign && level->manualKey) {
            apendix = tr("<b>in %1 key.</b>", "in key signature").arg(question.key.getName());
          }
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of");
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              quest += tr("Show sound from position:", "... and string + fret numbers folowing");
              apendix = "<br><b> " + onStringTxt(question.qa_2.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                  quest += tr("Play or sing");
              }
        quest += QString("<br><span style=\"font-size: 30px; %1\">&nbsp;").arg(gl->getBGcolorText(gl->EquestionColor)) +
                    question.qa.pos.toHtml() + " </span>";
        if (apendix != "")
          quest += "<br>" + apendix;
        if (question.answerAs == TQAtype::e_asNote || question.answerAs == TQAtype::e_asName)
          if (level->forceAccids)
            quest += "<br" + getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
        
      break;
      
      case TQAtype::e_asSound:
        if (question.answerAs == TQAtype::e_asNote) {
          m_scoreFree = false;
          quest += tr("Listened sound show on the staff");
          if (level->useKeySign && level->manualKey) {
            quest += tr("<br><b>in %1 key.</b>", "in key signature").arg(question.key.getName());
          }
          if (level->forceAccids)
            quest += getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
        } else
          if (question.answerAs == TQAtype::e_asName) {
            m_nameFree = false;
            quest += tr("Give name of listened sound");
            if (level->forceAccids)
                quest += getTextHowAccid((Tnote::Eacidentals)question.qa.note.acidental);
          } else
            if (question.answerAs == TQAtype::e_asFretPos) {
              m_guitarFree = false;
              quest += tr("Listened sound show on the guitar");
              if (level->showStrNr)
              quest += "<br><b> " + onStringTxt(question.qa.pos.str()) + "</b>";
            } else
              if (question.answerAs == TQAtype::e_asSound) {
                quest += tr("Play or sing listened sound");          
              }
      break;
    }
    if (level->requireOctave)
			if (question.answerAs == TQAtype::e_asName)
				quest += "<br><nobr>" + tr("Check octave button!") + "</nobr>";
    if (scale)
      quest += "</p>";
    quest = "<table><tr><td valign=\"middle\" style=\"font-family: nootka; color: transparent; font-size: 30px;\">" +
            TquestionAsWdg::qaTypeSymbol(question.questionAs) + "</td><td valign=\"middle\" align=\"center\">" + quest + "</td>" +
            "<td valign=\"middle\" style=\"font-family: nootka; color: transparent; font-size: 30px;\">" +
            TquestionAsWdg::qaTypeSymbol(question.answerAs) +
            "</td></tr></table>";
    return quest;
  
}







