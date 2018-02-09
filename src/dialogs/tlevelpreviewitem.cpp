/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "tlevelpreviewitem.h"
#include "exam/tlevel.h"
#include "music/tinstrument.h"
#include <qtr.h>
#include <tnoofont.h>
#include <tglobals.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qfontmetrics.h>
#include <QtCore/qdebug.h>


QString tdAlign() {
  return QStringLiteral("<td valign=\"middle\" align=\"center\">");
}


QString qaTypeSymbol(TQAtype::Etype type) {
  QString symbol;
  switch (type) {
    case TQAtype::e_onScore : symbol = "s"; break;
    case TQAtype::e_asName : symbol = "c"; break;
    case TQAtype::e_onInstr : symbol = "g"; break;
    case TQAtype::e_asSound : symbol = "n"; break;
  }
  return symbol;
}


TlevelPreviewItem::TlevelPreviewItem(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_instrGlyph = Tinstrument(Tinstrument::NoInstrument).glyph();
}


TlevelPreviewItem::~TlevelPreviewItem() {}


void TlevelPreviewItem::setLevel(Tlevel* tl) {
  Tinstrument instr(tl ? tl->instrument : Tinstrument::NoInstrument);
  if (tl) {
      const QString spTDEnd = QStringLiteral(" </td>");
      const QString nbsp = QStringLiteral("&nbsp;");
      const QString TRTD = QStringLiteral("<tr><td>");
      const QString TDTREnd = QStringLiteral("</td></tr>");
      QFontMetrics fm(qApp->font());
      int fontSize = fm.height() * 1.3;
      m_levelText = QLatin1String("<center>") + tr("Level summary:") + QLatin1String("<br><b>") + tl->name + QLatin1String("</b><br>");
      m_levelText += QLatin1String("<table border=\"1\" cellpadding=\"3\">");
      m_levelText += QLatin1String("<tr><td colspan=\"2\" align=\"center\" valign=\"middle\">") + instr.name() + spTDEnd;
      QString clefString(tr("Clef") + QLatin1String(":")); // "Clef:  "
      if (tl->loNote.isValid() && tl->hiNote.isValid())
        clefString += QString("<br><span style=\"font-family: Scorek; font-size: %1px;\">%2</span></td></tr>")
              .arg(fontSize * 2).arg(tl->clef.type() == Tclef::PianoStaffClefs ? QStringLiteral("\ue060") : tl->clef.glyph());
      m_levelText += QLatin1String("<td rowspan=\"_ROW_SPAN_\" align=\"center\" valign=\"middle\"><br>") + clefString;
      m_levelText += TRTD + qTR("RangePage", "note range:") + spTDEnd;
      if (tl->loNote.note() && tl->hiNote.note())
        m_levelText += tdAlign() + tl->loNote.toRichText() + QLatin1String(" - ") + tl->hiNote.toRichText() + TDTREnd;
      if (tl->canBeGuitar()) { // level uses guitar
          m_levelText += TRTD + qTR("RangePage", "fret range:") + spTDEnd;
          m_levelText += tdAlign() + QString("%1 - %2").arg(int(tl->loFret)).arg(int(tl->hiFret)) + TDTREnd;
      }
      if (tl->useKeySign) {
          m_levelText += TRTD + tr("key signature:") + spTDEnd + tdAlign();
          m_levelText += tl->loKey.getMajorName().remove(QLatin1String("-") + GLOB->majorKeyNameSufix());
          m_levelText += QLatin1String(" (") + tl->loKey.accidNumber(true) + QLatin1String(")");
          if (!tl->isSingleKey) {
              m_levelText += QLatin1String(" - ") + tl->hiKey.getMajorName().remove(QLatin1String("-") + GLOB->majorKeyNameSufix());
              m_levelText += QLatin1String(" (") + tl->hiKey.accidNumber(true) + QLatin1String(")");
          }
          m_levelText += TDTREnd;
      }
      m_levelText += TRTD + tr("accidentals:") + spTDEnd + tdAlign();
      if (!tl->withSharps && !tl->withFlats && !tl->withDblAcc)
          m_levelText += tr("none");
      else {
          if (tl->withSharps) m_levelText += TnooFont::span(QLatin1String(" #"));
          if (tl->withFlats) m_levelText += TnooFont::span(QLatin1String(" b"));
          if (tl->withDblAcc) m_levelText += TnooFont::span(QLatin1String(" x B"));
      }
      m_levelText += TDTREnd;
      m_levelText += TRTD + qTR("LevelCreator", "Questions") + QLatin1String(": </td>") + tdAlign(); // QUESTIONS
      QString tmp;
      const QString space = QStringLiteral(" ");
      if (tl->questionAs.isOnScore())
        tmp += qaTypeSymbol(TQAtype::e_onScore) + space;
      if (tl->questionAs.isName())
        tmp += qaTypeSymbol(TQAtype::e_asName) + space;
      if (tl->questionAs.isOnInstr())
        tmp += qaTypeSymbol(TQAtype::e_onInstr) + space;
      if (tl->questionAs.isSound()) {
        if (tl->canBeMelody())
          tmp += QLatin1String("m");
        else
          tmp += qaTypeSymbol(TQAtype::e_asSound) + space;
      }

      m_levelText += TnooFont::span(tmp, fontSize);
      m_levelText += TDTREnd;
      tmp.clear();
      m_levelText += TRTD + QGuiApplication::translate("LevelCreator", "Answers") + QLatin1String(": </td><td align=\"center\">"); // ANSWERS
      if (tl->answerIsNote())
              tmp += qaTypeSymbol(TQAtype::e_onScore) + space;
      if (tl->answerIsName())
              tmp += qaTypeSymbol(TQAtype::e_asName) + space;
      if (tl->answerIsGuitar())
              tmp += qaTypeSymbol(TQAtype::e_onInstr) + space;
      if (tl->answerIsSound()) {
        if (tl->canBeMelody())
          tmp += QLatin1String("m");
        else
          tmp += qaTypeSymbol(TQAtype::e_asSound) + space;
      }
      m_levelText += TnooFont::span(tmp, fontSize);
      m_levelText += TDTREnd;
      if (tl->canBeName() || tl->canBeScore() || tl->canBeSound()) {
        m_levelText += QLatin1String("<tr><td colspan=\"2\" align=\"center\">");
        if (tl->requireOctave)
            m_levelText += tr("proper octave is required");
        else
            m_levelText += tr("octave does no matter");
        m_levelText += TDTREnd;
      }
      m_levelText += QLatin1String("</table></center>");
      m_levelText.replace(QLatin1String("_ROW_SPAN_"), QString("%1").arg(m_levelText.count(QLatin1String("<tr>"))));
      m_levelText += QLatin1String("<br><br>") + tl->desc;
  } else {
    
  }
  if (instr.glyph() != m_instrGlyph) {
    m_instrGlyph = instr.glyph();
    emit instrumentGlyphChanged();
  }
  emit levelHtmlChanged();
}
