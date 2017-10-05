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

#include "tnameitem.h"
#include <tglobals.h>


static const char* const shortOctaveNames[8] = { QT_TR_NOOP("Sub"), 	QT_TR_NOOP("Contra"), QT_TR_NOOP("Great"), QT_TR_NOOP("Small"),
  QT_TR_NOOP("1-line"), QT_TR_NOOP("2-line"), QT_TR_NOOP("3-line"), QT_TR_NOOP("4-line") };


TnameItem::TnameItem(QQuickItem* parent) :
  QQuickItem(parent),
  m_nameStyle(Tnote::e_english_Bb)
{
  m_note.octave = -4;
}


TnameItem::~TnameItem() {}


void TnameItem::setNote(const Tnote& n) {
  if (!m_note.compareNotes(n)) {
    bool stepCh = n.note != m_note.note;
    bool octaveCh = n.octave != m_note.octave;
    bool alterCh = n.alter != m_note.alter;
    m_note = n;
    if (stepCh)
      emit stepChanged();
    if (octaveCh)
      emit octaveChanged();
    if (alterCh)
      emit alterChanged();
    nameTextChanged();
  }
}


void TnameItem::setStep(int st) {
  char stepChar = static_cast<char>(st);
  if (stepChar != m_note.note) {
    if (m_note.octave == -4) // initial octave value is fake, revert it
      setOctave(0);
    m_note.note = stepChar;
    emit stepChanged();
    emit nameTextChanged();
    emit noteChanged();
  }
}


void TnameItem::setAlter(int alt) {
  char alterChar = static_cast<char>(alt);
  if (alterChar != m_note.alter) {
    m_note.alter = alterChar;
    emit alterChanged();
    emit nameTextChanged();
    emit noteChanged();
  }
}


void TnameItem::setOctave(int oct) {
  char octaveChar = static_cast<char>(oct);
  if (octaveChar != m_note.octave) {
    m_note.octave = octaveChar;
    emit octaveChanged();
    emit nameTextChanged();
    emit noteChanged();
  }
}


void TnameItem::setNameStyle(int nStyle) {
  if (static_cast<Tnote::EnameStyle>(nStyle) != m_nameStyle) {
    m_nameStyle = static_cast<Tnote::EnameStyle>(nStyle);
    emit nameStyleChanged();
    emit nameTextChanged();
  }
}


QString TnameItem::nameText() const {
  QString enharmText;
  if (GLOB->showEnharmNotes() && m_note.isValid()) {
    TnotesList enharmList = m_note.getTheSameNotes(GLOB->enableDoubleAccids());
    TnotesList::iterator it = enharmList.begin();
    ++it;
    if (it != enharmList.end()) {
        auto n1 = *(it);
        enharmText += QString(" <font color=\"%1\" size=\"1\">(").arg(GLOB->getEnharmNoteColor().name()) + n1.styledName();
    }
    ++it;
    if (it != enharmList.end()) {
      auto n2 = *(it);
      enharmText += QLatin1String("  ") + n2.styledName();
    }
    if (!enharmText.isEmpty())
      enharmText += QLatin1String(")</font>");
  }
  return m_note.isValid() ? m_note.styledName() + enharmText : QString();
}


QString TnameItem::octaveName(int oNr) const {
  return oNr > -4 && oNr < 5 ? shortOctaveNames[oNr + 3] : QString();
}


QString TnameItem::octavesLink() const {
  // TODO: here is a status tip, make it available when tips will back
  QString l = tr("Click to see what <i>octaves</i> are at \"http://en.wikipedia.org/wiki/Octave\"",
                                "You can change this link to article in your language. Leave quotation marks around the address!");
  return QLatin1String("<a href=") + l.mid(l.indexOf("\"")) + QLatin1String(">") + tr("Octaves") + QLatin1String(":</a>");
}


QString TnameItem::noteButtonText(int noteNr, int nStyle) {
  Q_UNUSED(nStyle)
  return Tnote(noteNr, 0, 0).toText(m_nameStyle, false);
}

