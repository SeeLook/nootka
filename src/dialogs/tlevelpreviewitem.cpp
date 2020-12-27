/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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
#include <exam/tlevel.h>
#include <music/tinstrument.h>
#include <qtr.h>
#include <tglobals.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qfontmetrics.h>
#include <QtCore/qdebug.h>


QString qaTypeSymbol(TQAtype::Etype type, Tlevel* l) {
  switch (type) {
    case TQAtype::e_onScore : return QStringLiteral("s");
    case TQAtype::e_asName : return QStringLiteral("c");
    case TQAtype::e_onInstr : return Tinstrument(l->instrument).glyph();
    case TQAtype::e_asSound : return QStringLiteral("n");
    default : return QString();
  }
}

QString getLevelSumary(const QString& levelName) {
  return QGuiApplication::translate("TlevelPreviewItem", "Level summary:") + QLatin1String("<br><b>") + levelName + QLatin1String("</b>");
}


TlevelPreviewItem::TlevelPreviewItem(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_instrGlyph = Tinstrument(Tinstrument::NoInstrument).glyph();
}


TlevelPreviewItem::~TlevelPreviewItem() {}


void TlevelPreviewItem::setLevel(Tlevel* tl) {
  m_validLevel = tl != nullptr;
  if (tl) {
      static const QString space = QStringLiteral(" ");
      m_instrGlyph = Tinstrument(tl->instrument).glyph();
      m_header = getLevelSumary(tl->name);
      m_description = tl->desc;
      m_instrument = Tinstrument(tl ? tl->instrument : Tinstrument::NoInstrument).name();
      m_clef = tl->clef.glyphOnStaff();

      if (tl->randMelody == Tlevel::e_randFromRange) // makes sense for single note and melody from range
        m_noteRange = tl->loNote.styledName() + QLatin1String(" - ") + tl->hiNote.styledName();
      else
        m_noteRange.clear();

      if (GLOB->instrument().isGuitar() && tl->canBeGuitar())
        m_fretRange = QString("<b>%1 - %2</b>").arg(static_cast<int>(tl->loFret)).arg(static_cast<int>(tl->hiFret));
      else
        m_fretRange.clear();

      if (tl->useKeySign) {
          m_keyRange = tl->loKey.getMajorName().remove(QLatin1String("-") + GLOB->majorKeyNameSufix());
          m_keyRange += QLatin1String(" (") + tl->loKey.accidNumber(true) + QLatin1String(")");
          if (!tl->isSingleKey) {
            m_keyRange += QLatin1String(" - ") + tl->hiKey.getMajorName().remove(QLatin1String("-") + GLOB->majorKeyNameSufix());
            m_keyRange += QLatin1String(" (") + tl->hiKey.accidNumber(true) + QLatin1String(")");
          }
      } else
          m_keyRange.clear();

      m_accidentals.clear(); // none by default
      if (tl->withSharps)
        m_accidentals += QLatin1String("#");
      if (tl->withFlats)
        m_accidentals += (m_accidentals.isEmpty() ? QString() : space) + QLatin1String("b");
      if (tl->withDblAcc)
        m_accidentals += (m_questions.isEmpty() ? QString() : space) + QLatin1String("x B");

      m_questions.clear();
      if (tl->questionAs.isOnScore())
        m_questions += qaTypeSymbol(TQAtype::e_onScore, tl);
      if (tl->questionAs.isName())
        m_questions += (m_questions.isEmpty() ? QString() : space) + qaTypeSymbol(TQAtype::e_asName, tl);
      if (tl->questionAs.isOnInstr())
        m_questions += (m_questions.isEmpty() ? QString() : space) + qaTypeSymbol(TQAtype::e_onInstr, tl);
      if (tl->questionAs.isSound()) {
        m_questions += m_questions.isEmpty() ? QString() : space;
        if (tl->canBeMelody())
          m_questions += QLatin1String("m");
        else
          m_questions += qaTypeSymbol(TQAtype::e_asSound, tl);
      }

      m_answers.clear();
      if (tl->answerIsNote())
        m_answers += qaTypeSymbol(TQAtype::e_onScore, tl);
      if (tl->answerIsName())
        m_answers += (m_answers.isEmpty() ? QString() : space) + qaTypeSymbol(TQAtype::e_asName, tl);
      if (tl->answerIsGuitar())
        m_answers += (m_answers.isEmpty() ? QString() : space) + qaTypeSymbol(TQAtype::e_onInstr, tl);
      if (tl->answerIsSound()) {
        m_answers += m_answers.isEmpty() ? QString() : space;
        if (tl->canBeMelody())
          m_answers += QLatin1String("m");
        else
          m_answers += qaTypeSymbol(TQAtype::e_asSound, tl);
      }

      if (tl->canBeName() || tl->canBeScore() || tl->canBeSound())
        m_requireOctave = tl->requireOctave ? tr("proper octave is required") : tr("octave does no matter");
      else
        m_requireOctave.clear();
  } else {
      m_header = getLevelSumary(tr("no level selected"));
      m_instrGlyph = QStringLiteral("?");
  }

  emit levelChanged();
}
