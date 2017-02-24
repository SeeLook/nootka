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

#include "tnoteobject.h"
#include "tscoreobject.h"
#include "tstaffobject.h"
#include "tmeasureobject.h"
#include "tnotepair.h"
#include "music/tnote.h"

#include <QtQml/qqmlengine.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>

#include <QtCore/qdebug.h>


/** Unicode numbers of accidentals in Scorek font. */
static const QString accCharTable[6] = {
          QStringLiteral("\ue264"), // [0] = bb - double flat
          QStringLiteral("\ue260"), // [1] = b - flat
          QString(),                // [2] = none
          QStringLiteral("\ue262"), // [3] = # - sharp
          QStringLiteral("\ue263"), // [4] = x - double sharp
          QStringLiteral("\ue261")  // [5] = neutral
};

/**
 * Width of every accidental for Scorek font of pixel size set to 7.0
 * It was measured by QML and corresponds to QFont size @p QFont::setPointSizeF(5.5) (except of neutral)
 */
static const qreal accWidthTable[6] = { 2.78125, 1.671875, 0.0, 1.765625, 2.03125, 2.34375 };

/**
 * Static array with space definitions for each rhythm value
 */
const qreal rtmGapArray[5][3] = {
//  | bare | dot | triplet |
    { 5.0,   6.0,   4.5}, // whole note
    { 4.0,   5.0,   3.3}, // half note
    { 2.0,   2.5,   1.3}, // quarter note
    { 1.0,   1.5,   0.3}, // eighth note
    { 0.15,  0.5,   0.0}  // sixteenth note
};



TnoteObject::TnoteObject(TstaffObject* staffObj) :
  QQuickItem(staffObj->staffItem()),
  m_staff(staffObj),
  m_index(-1),
  m_stemHeight(6.0)
{
  m_note = new Tnote();
  QQmlEngine engine;
  QQmlComponent comp(&engine, this);

//   comp.setData("import QtQuick 2.7; Rectangle { radius: 1 }", QUrl());
//   m_bg = qobject_cast<QQuickItem*>(comp.create());
//   m_bg->setParentItem(this);
//   QColor bgColor = qApp->palette().highlight().color();
//   bgColor.setAlpha(50);
//   m_bg->setProperty("color", bgColor);

  comp.setData("import QtQuick 2.7; Rectangle {}", QUrl());
  m_stem = qobject_cast<QQuickItem*>(comp.create());
  m_stem->setParentItem(this);
  m_stem->setWidth(0.3);
  m_stem->setHeight(m_stemHeight);
  m_stem->setVisible(false);

  for (int i = 0; i < 7; ++i) {
    m_upperLines << createAddLine(comp);
    m_upperLines.last()->setY(2 * (i + 1) - 0.1);
    m_lowerLines << createAddLine(comp);
    m_lowerLines.last()->setY(staff()->upperLine() + 10.0 + 2 * i - 0.1);
  }

  comp.setData("import QtQuick 2.7; Text { font { family: \"Scorek\"; pixelSize: 7 }}", QUrl());
  m_head = qobject_cast<QQuickItem*>(comp.create());
  m_head->setParentItem(this);
  m_head->setProperty("id", QStringLiteral("head"));

  m_alter = qobject_cast<QQuickItem*>(comp.create());
  m_alter->setParentItem(m_head);
  m_head->setProperty("id", QStringLiteral("alter"));

  m_flag = qobject_cast<QQuickItem*>(comp.create());
  m_flag->setParentItem(m_stem);
  m_flag->setX(0.1);


  setColor(qApp->palette().text().color());
  setHeight(staffObj->staffItem()->height());
}


TnoteObject::~TnoteObject() {
  delete m_note;
}


void TnoteObject::setStaff(TstaffObject* staffObj) {
  if (staffObj != m_staff) {
    m_staff = staffObj;
    setParentItem(m_staff->staffItem());
  }
}


void TnoteObject::setMeasure(TmeasureObject* m) {
  m_measure = m;
}


void TnoteObject::setColor(const QColor& c) {
  m_head->setProperty("color", c);
  m_alter->setProperty("color", c);
  m_flag->setProperty("color", c);
  m_stem->setProperty("color", c);
}


/**
 *                @p pitch/octave
 * - note pos Y
 * - when stem is visible - stem length
 *                @p accidental
 * - entire width
 *                @p rhythm-value (whole, half, etc.)
 * - head text
 * - stem state and flag
 * - width
 *                @p rest
 * - all
 *                @p dot (???)
 * - width maybe
 *                @p stem up/down
 * - stem length
 *                @p beam
 * - stem visibility
 * - width
 */
void TnoteObject::setNote(const Tnote& n) {
  bool updateHead = n.rhythm() != m_note->rhythm() || n.isRest() != m_note->isRest() || n.hasDot() != m_note->hasDot();
  bool updateStem = updateHead || (n.rtm.beam() != Trhythm::e_noBeam) != (m_note->rtm.beam() != Trhythm::e_noBeam);

  *m_note = n;

  if (updateHead) {
    QString headText = getHeadText();
    if (m_note->hasDot())
      headText.append(QStringLiteral("\ue1e7"));
    m_head->setProperty("text", headText);
  }

  if (m_note->isRest())
    m_notePosY = staff()->upperLine() + (m_note->rhythm() == Trhythm::Whole ? 2.0 : 4.0);
  else
    setNotePosY(staff()->score()->clefOffset().total() + staff()->upperLine() - (n.octave * 7 + (n.note - 1)));
  if (m_notePosY < 2.0 || m_notePosY > 38.0)
    m_notePosY = 0.0;
  if (static_cast<int>(m_notePosY) != static_cast<int>(m_head->y())) {
    if (m_notePosY) {
        m_head->setVisible(true);
        m_head->setY(m_notePosY - 15.0);
    } else
        m_head->setVisible(false);
    for (int i = 0; i < 7; ++i) {
      m_upperLines[i]->setVisible(m_head->isVisible() && !m_note->isRest() && m_notePosY > 0.0 && i >= qFloor((m_notePosY - 1.0) / 2.0));
      m_lowerLines[i]->setVisible(m_head->isVisible() && !m_note->isRest() && staff()->upperLine() + 10.0 + i * 2 <= m_notePosY);
    }
    updateStem = true;
  }

  if (updateStem) {
    if (m_notePosY && !m_note->isRest() && m_note->rhythm() > Trhythm::Whole ) {
        if (m_note->rtm.beam() == Trhythm::e_noBeam) {
            m_note->rtm.setStemDown(m_notePosY < staff()->upperLine() + 4.0);
            m_stem->setHeight(qMax(6.0, qAbs(m_notePosY - (staff()->upperLine() + 4.0))));
            QString flag = getFlagText();
            m_flag->setProperty("text", flag);
            if (!flag.isEmpty())
              m_flag->setY((m_note->rtm.stemDown() ? m_stem->height() : 0.0) - 15.0);
        }
        m_stem->setX(m_head->x() + (m_note->rtm.stemDown() ? 0.0 : 2.0));
        m_stem->setY(m_notePosY + (m_note->rtm.stemDown() ? 0.0 : - m_stem->height()));
        m_stem->setVisible(true);
    } else
        m_stem->setVisible(false);
  }

  m_alter->setProperty("text", getAccidText());
  if (m_note->alter)
    m_alter->setX(-m_alter->width() - 0.1);

  setWidth(m_alter->width() + m_head->width() + (m_note->rtm.stemDown() ? 0.0 : m_flag->width() - 0.5));

//   m_bg->setX(m_note->alter && m_alter->isVisible() ? -m_alter->width() - 0.4 : -0.4);
//   m_bg->setHeight(m_stem->height() + 4.0);
//   m_bg->setY(m_stem->y() - (m_note->rtm.stemDown() ? 3.25 : 1.25));
//   m_bg->setWidth(width());

  qDebug() << debug() << "set note" << m_note->toText() << m_note->rtm.string() << "note pos" << m_notePosY << "width:" << width();
}


void TnoteObject::setX(qreal xx) {
  QQuickItem::setX(xx + (m_note->alter ? m_alter->width() : 0.0));
}


qreal TnoteObject::rightX() {
  return x() + width() + staff()->gapFactor() * rhythmFactor() - (m_note->alter ? m_alter->width() : 0.0);
}


qreal TnoteObject::rhythmFactor() {
  if (m_note->rhythm() == Trhythm::NoRhythm)
    return 0.0;

  int add = m_note->hasDot() ? 1 : (m_note->isTriplet() ? 2 : 0);
  return rtmGapArray[static_cast<int>(m_note->rhythm()) - 1][add];
}


char TnoteObject::debug() {
  QTextStream o(stdout);
  o << "   \033[01;29m[NOTE" << index() << "]\033[01;00m";
  return 32; // fake
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

QString TnoteObject::getAccidText() {
  return accCharTable[m_note->alter + 2];
}


/**
 * Used glyphs are:
 * - note heads: @p 0xf4be @p 0xf4bd (half and black over-sized) and @p 0xf486 (whole smaller)
 * - rests: starts form 0xe4e3 (whole)
 */
QString TnoteObject::getHeadText() {
  if (m_note->rhythm() == Trhythm::NoRhythm)
    return QStringLiteral("\uf4be"); // just black note-head
  if (m_note->isRest())
    return QString(QChar(0xe4e2 + static_cast<int>(m_note->rhythm())));
  else {
    if (m_note->rhythm() == Trhythm::Whole)
      return QStringLiteral("\uf468");
    else if (m_note->rhythm() == Trhythm::Half)
      return QStringLiteral("\uf4bd");
    else
      return QStringLiteral("\uf4be");
  }
}


QString TnoteObject::getFlagText() {
  if (m_note->rhythm() < Trhythm::Eighth || m_note->isRest() || m_note->rtm.beam() != Trhythm::e_noBeam)
    return QString();
  // In Scorek font, flag glyphs are placed: flag for stem-up, then flag for stem-down, starting from 0xe240
  return QString(QChar(0xe240 + (static_cast<int>(m_note->rhythm()) - 4) * 2 + (m_note->rtm.stemDown() ? 1 : 0)));
}


QQuickItem* TnoteObject::createAddLine(QQmlComponent& comp) {
  auto line = qobject_cast<QQuickItem*>(comp.create());
  line->setParentItem(this);
  line->setWidth(3.5);
  line->setHeight(0.2);
  line->setX(-0.5);
  line->setVisible(false);
  line->setProperty("color", qApp->palette().text().color());
  return line;
}
