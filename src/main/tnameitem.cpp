/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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
#include <tcolor.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>

#include <QtCore/qdebug.h>


static const char* const shortOctaveNames[8] = { QT_TR_NOOP("Sub"), 	QT_TR_NOOP("Contra"), QT_TR_NOOP("Great"), QT_TR_NOOP("Small"),
  QT_TR_NOOP("1-line"), QT_TR_NOOP("2-line"), QT_TR_NOOP("3-line"), QT_TR_NOOP("4-line") };


TnameItem* TnameItem::m_instance = nullptr;


TnameItem::TnameItem(QQuickItem* parent) :
  QQuickItem(parent),
  m_nameStyle(Tnote::e_english_Bb),
  m_buttonNameStyle(Tnote::e_english_Bb)
{
  if (m_instance) {
    qDebug() << "TnameItem instance already exists!";
    return;
  }

  m_instance = this;
  m_note.octave = -4;
  m_bgColor = qApp->palette().base().color();
  connect(qApp, &QGuiApplication::paletteChanged, [=]{
    if (m_appendix.isEmpty()) { // update color only when question is not asked
        changeNameBgColor(qApp->palette().base().color());
        emit bgColorChanged();
      }
  });
  connect(GLOB, &Tglobals::showEnharmNotesChanged, [=]{
    if (m_note.isValid())
      emit nameTextChanged();
  });
}


TnameItem::~TnameItem()
{
  m_instance = nullptr;
}


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
    emit nameTextChanged();
    if (!n.isValid()) {
      if (!m_appendix.isEmpty()) {
        m_appendix.clear();
        emit appendixChanged();
        m_bgColor = qApp->palette().base().color();
        emit bgColorChanged();
      }
    }
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


/**
 * Set alter button but do not change name text if note is invalid (not set)
 */
void TnameItem::setAlter(int alt) {
  char alterChar = static_cast<char>(alt);
  if (alterChar != m_note.alter) {
    m_note.alter = alterChar;
    emit alterChanged();
    if (m_note.isValid()) {
      emit nameTextChanged();
      emit noteChanged();
    }
  }
}


/**
 * Set octave button but do not change name text if note is invalid (not set)
 */
void TnameItem::setOctave(int oct) {
  char octaveChar = static_cast<char>(oct);
  if (octaveChar != m_note.octave) {
    m_note.octave = octaveChar;
    emit octaveChanged();
    if (m_note.isValid()) {
      emit nameTextChanged();
      emit noteChanged();
    }
  }
}


void TnameItem::setNameStyle(Tnote::EnameStyle style) {
  if (m_nameStyle != style) {
    m_nameStyle = style;
    Tnote::defaultStyle = style;
    emit nameStyleChanged();
    emit nameTextChanged();
  }
}

void TnameItem::setButtonNameStyle(Tnote::EnameStyle style) {
  if (style != m_buttonNameStyle) {
    m_buttonNameStyle = style;
    emit buttonNameStyleChanged();
  }
}


void TnameItem::setDisabled(bool dis) {
  if (dis != m_disabled) {
    m_disabled = dis;
    emit disabledChanged();
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
  if (m_note.isValid())
    return QString("<font color=\"%1\">").arg(qApp->palette().text().color().name()) + m_note.styledName() + QLatin1String("</font>") + enharmText;
  else
    return QString();
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


/**
 * Text on name buttons depends on style
 */
QString TnameItem::noteButtonText(int noteNr, int nStyle) {
  return Tnote(static_cast<char>(noteNr), 0, 0).toText(static_cast<Tnote::EnameStyle>(nStyle), false);
}


void TnameItem::askQuestion(const Tnote& note, Tnote::EnameStyle questStyle, quint8 strNr) {
  changeNameBgColor(Tcolor::merge(Tcolor::alpha(GLOB->EquestionColor, 40), qApp->palette().base().color()));
  setNameStyle(questStyle);
  setNote(note);
  m_appendix.clear();
  if (strNr > 0)
    m_appendix = QString::number(strNr);
  m_appendix += QLatin1String("?");
  emit appendixChanged();
}


/**
 * To keep note name text unchanged we silently (without emitting any signal) are changing name style.
 * To unset note button we tricky set note to 0 and emitting only @p stepChanged() signal.
 */
void TnameItem::prepareAnswer(Tnote::EnameStyle answStyle) {
  changeNameBgColor(Tcolor::merge(Tcolor::alpha(GLOB->EanswerColor, 40), qApp->palette().base().color()));
  m_nameStyle = answStyle;
  Tnote::defaultStyle = answStyle;
  setDisabled(false);
  m_note.note = 0;
  emit stepChanged();
}


void TnameItem::forceAccidental(char accid) {
  m_note.alter = accid;
  emit alterChanged();
}
