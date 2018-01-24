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

#include "tnoteobject.h"
#include "tscoreobject.h"
#include "tstaffobject.h"
#include "tmeasureobject.h"
#include "tbeamobject.h"
#include "tnotepair.h"
#include "music/tnote.h"

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qtimer.h>
#include <QtCore/qelapsedtimer.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


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
// static const qreal accWidthTable[6] = { 2.78125, 1.671875, 0.0, 1.765625, 2.03125, 2.34375 };

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


QString tieDebug(Trhythm::Etie t) {
  switch (t) {
    case Trhythm::e_tieEnd:
      return "TE";
    case Trhythm::e_tieStart:
      return "TS";
    case Trhythm::e_tieCont:
      return "TC";
    default:
      return QString();
  }
}


TnoteObject::TnoteObject(TstaffObject* staffObj, TnotePair* wrapper) :
  QQuickItem(staffObj->staffItem()),
  m_staff(staffObj),
  m_wrapper(wrapper),
  m_stemHeight(STEM_HEIGHT)
{
  setParent(m_staff->score()); // to avoid deleting with parent staff
  m_note = new Tnote();

  m_staff->score()->component()->setData("import QtQuick 2.9; Rectangle {}", QUrl());
  m_stem = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
  m_stem->setParentItem(this);
  m_stem->setWidth(0.3);
  m_stem->setHeight(m_stemHeight);
  m_stem->setVisible(false);

  for (int i = 0; i < 7; ++i) {
    m_upperLines << createAddLine();
    m_lowerLines << createAddLine();
  }

  m_staff->score()->component()->setData("import QtQuick 2.9; Text { font { family: \"Scorek\"; pixelSize: 7 }}", QUrl());
  m_head = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
  m_head->setParentItem(this);

  m_alter = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
  m_alter->setParentItem(m_head);
  connect(m_alter, &QQuickItem::widthChanged, this, &TnoteObject::alterWidthChanged);

  m_flag = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
  m_flag->setParentItem(m_stem);
  m_flag->setX(0.1);

//   m_staff->score()->component()->setData("import QtQuick 2.9; Text { font { pixelSize: 1 }}", QUrl());
//   m_debug = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
//   m_debug->setParentItem(this);

  setColor(qApp->palette().text().color());
  setHeight(staffObj->staffItem()->height());
  setAcceptHoverEvents(true);
  setZ(10);
  setAcceptedMouseButtons(Qt::LeftButton);

  updateNoteHead();
}


TnoteObject::~TnoteObject() {
//   qDebug() << debug() << "is going deleted";
  delete m_note;
}


int TnoteObject::index() const {
  return m_wrapper->index();
}


void TnoteObject::setStaff(TstaffObject* staffObj) {
  if (staffObj != m_staff) {
      m_staff = staffObj;
      if (m_staff) {
          setParentItem(m_staff->staffItem());
          if (m_wrapper->beam() && m_wrapper->beam()->last()->item() == this)
            m_wrapper->beam()->changeStaff(m_staff);
      } else {
          setParentItem(nullptr);
      }
      if (m_name)
        m_name->setParentItem(parentItem());
  } else
      qDebug() << debug() << "has staff set already";
}


void TnoteObject::setMeasure(TmeasureObject* m) {
  m_measure = m;
}


void TnoteObject::setStemHeight(qreal sh) {
  m_stem->setHeight(sh);
}


void TnoteObject::setColor(const QColor& c) {
  m_head->setProperty("color", c);
  m_alter->setProperty("color", c);
  m_flag->setProperty("color", c);
  m_stem->setProperty("color", c);
  for (auto line : m_upperLines)
    line->setProperty("color", c);
  for (auto line : m_lowerLines)
    line->setProperty("color", c);
  if (m_midLine)
    m_midLine->setProperty("color", c);
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
  bool fixBeam = n.isRest() != m_note->isRest();
  bool updateStem = updateHead || fixBeam || ((n.rtm.beam() != Trhythm::e_noBeam) != (m_note->rtm.beam() != Trhythm::e_noBeam))
        || (n.rtm.stemDown() != m_note->rtm.stemDown() || m_stem->height() != m_stemHeight);
  bool updateTie = n.rtm.tie() != m_note->rtm.tie();

  *m_note = n;

  if (fixBeam) {
    if (m_note->isRest()) {
      if (m_wrapper->beam())
        m_measure->noteGoingRest(m_wrapper);
    } else {
        if (m_note->rhythm() > Trhythm::Quarter)
          m_measure->restGoingNote(m_wrapper);
    }
  }

  if (updateHead)
    updateNoteHead();

  int oldNotePos = static_cast<int>(m_notePosY);
  if (m_note->isRest())
    m_notePosY = staff()->upperLine() + (m_note->rhythm() == Trhythm::Whole ? 2.0 : 4.0);
  else {
    if (m_note->isValid()) {
        m_notePosY = staff()->score()->clefOffset().total() + staff()->upperLine() - (n.octave * 7 + (n.note - 1));
        if (staff()->score()->isPianoStaff() && m_notePosY > staff()->upperLine() + 10.0)
          m_notePosY += 2.0;
    } else {
        if (staff()->score()->singleNote()) {
            m_notePosY = 0.0;
            oldNotePos = -1.0; // cheat the logic to force add lines check
        } else // no clef - rhythm only, note placed at lower staff field
            m_notePosY = staff()->upperLine() + 7.0;
    }
  }
  if (m_notePosY < 2.0 || m_notePosY > height() - 2.0)
    m_notePosY = 0.0;

  if (oldNotePos != static_cast<int>(m_notePosY)) {
    if (m_notePosY) {
        m_head->setVisible(true);
        m_head->setY(m_notePosY - 15.0);
    } else
        m_head->setVisible(false);

    checkAddLinesVisibility();
    updateStem = true;
  }

if (updateStem)
  checkStem();

  updateAlter();
  updateWidth();

  if (updateTie)
    checkTie();

  if (oldNotePos != static_cast<int>(m_notePosY))
    emit notePosYchanged();

  updateNamePos();

//   m_debug->setProperty("text", QString("%1 %2").arg(index()).arg(tieDebug(m_note->rtm.tie())));
//   m_debug->setY(height() - (2 + index() % 2) * m_debug->height());
//   qDebug() << debug() << "set note" << m_note->toText() << m_note->rtm.string() << "note pos" << m_notePosY << "width:" << width();
}


quint32 TnoteObject::technical() const { return m_wrapper ? m_wrapper->technical() : 255; }


void TnoteObject::setTechnical(quint32 tech) {
  if (m_wrapper)
    m_wrapper->setTechnical(tech);
}


void TnoteObject::setX(qreal xx) {
  if (staff()->score()->singleNote())
      QQuickItem::setX(xx);
  else {
      updateTieScale();
      QQuickItem::setX(xx + (m_alter->width()));
      if (m_wrapper->beam() && m_wrapper->beam()->last()->item() == this)
        m_wrapper->beam()->last()->beam()->drawBeam();
      if (m_name)
        m_name->setX(x() - m_alter->width() + (width() - m_name->width()) / 2.0);
      emit rightXChanged();
  }
}


Trhythm TnoteObject::rhythm() const {
  return m_note->rtm;
}


qreal TnoteObject::rightX() const {
  return x() + width() + staff()->gapFactor() * rhythmFactor() - m_alter->width();
}


void TnoteObject::setHeight(qreal hh) {
  if (hh != height()) {
    QQuickItem::setHeight(hh);
    for (int i = 0; i < 7; ++i) {
      m_upperLines[i]->setY(2 * (i + 1) - 0.1);
      m_lowerLines[i]->setY(staff()->upperLine() + (staff()->score()->isPianoStaff() ? 22.0 : 10.0) + 2 * i - 0.1);
    }
    if (staff()->score()->isPianoStaff()) {
        if (m_midLine == nullptr) {
          m_staff->score()->component()->setData("import QtQuick 2.9; Rectangle {}", QUrl());
          m_midLine = createAddLine();
          m_midLine->setParent(this);
          m_midLine->setProperty("color", m_head->property("color"));
        }
        m_midLine->setY(m_staff->upperLine() + 10.0);
    } else {
        if (m_midLine) {
          delete m_midLine;
          m_midLine = nullptr;
        }
    }
    checkAddLinesVisibility();
  }
}


qreal TnoteObject::rhythmFactor() const {
  if (m_note->rhythm() == Trhythm::NoRhythm)
    return 0.75;

  int add = m_note->hasDot() ? 1 : (m_note->isTriplet() ? 2 : 0);
  return rtmGapArray[static_cast<int>(m_note->rhythm()) - 1][add];
}


char TnoteObject::debug() {
  QTextStream o(stdout);
  o << "   \033[01;29m[NOTE " << index() << "]\033[01;00m";
  return 32; // fake
}


void TnoteObject::shiftHead(qreal shift) {
  if (shift != m_head->x()) {
    m_head->setX(shift);
    for (int i = 0; i < 7; ++i) {
      m_upperLines[i]->setX(-0.5 + shift);
      m_lowerLines[i]->setX(-0.5 + shift);
    }
    if (m_midLine)
      m_midLine->setX(-0.5 + shift);
  }
}


void TnoteObject::checkTie() {
  if (m_tie && (m_note->rtm.tie() == Trhythm::e_noTie || m_note->rtm.tie() == Trhythm::e_tieEnd)) {
      delete m_tie;
      m_tie = nullptr;
  } else if (m_tie == nullptr && (m_note->rtm.tie() == Trhythm::e_tieStart || m_note->rtm.tie() == Trhythm::e_tieCont)) {
      QQmlComponent comp(m_staff->score()->qmlEngine(), QUrl(QStringLiteral("qrc:/Tie.qml")));
      m_tie = qobject_cast<QQuickItem*>(comp.create());
      m_tie->setParentItem(m_head);
      m_tie->setProperty("color", qApp->palette().text().color());
      updateTieScale();
      m_tie->setX(m_head->width());
  }
//   m_debug->setProperty("text", QString("%1 %2").arg(index()).arg(tieDebug(m_note->rtm.tie())));
//   m_debug->setY(height() - (2 + index() % 2) * m_debug->height());
}


qreal TnoteObject::tieWidth() {
  return staff()->gapFactor() * rhythmFactor() + (this == m_measure->last()->item() ? 1.5 : 0.0) + (m_note->rtm.stemDown() ? 0.0 : m_flag->width() - 0.2);
}


QPointF TnoteObject::stemTop() {
  return mapToItem(parentItem(), QPointF(m_stem->x(), m_stem->y() + (m_note->rtm.stemDown() ? m_stem->height() : 0.0)));
}


/**
 * When set to @p TRUE - creates @p m_name object, which belongs to parent staff (due to issues with blocking hover events over this name text item)
 * @p updateNamePos() method takes care about @p y() coordinate,
 * but @p x() has to be set when this @p TnoteObject changes its X coordinate
 */
void TnoteObject::setNoteNameVisible(bool nameVisible) {
  if (nameVisible) {
      if (!m_name) {
        m_staff->score()->component()->setData(
              "import QtQuick 2.9; Text { font { pixelSize: 3; family: \"Scorek\" } textFormat: Text.PlainText; style: Text.Outline }", QUrl());
        m_name = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
        m_name->setParentItem(parentItem());
        m_name->setProperty("color", qApp->palette().text().color());
        m_name->setProperty("styleColor", m_measure->score()->nameColor());
        updateNamePos();
      }
  } else {
      if (m_name) {
        delete m_name;
        m_name = nullptr;
      }
  }
}


QQuickItem* TnoteObject::staffItem() {
  return m_staff->staffItem();
}


/**
 * Used glyphs are:
 * - note heads: @p 0xf4be @p 0xf4bd (half and black over-sized) and @p 0xf486 (whole smaller)
 * - rests: starts form 0xe4e3 (whole)
 */
QString TnoteObject::getHeadText(const Trhythm& r) {
  if (r.rhythm() == Trhythm::NoRhythm)
      return QStringLiteral("\uf4be"); // just black note-head
      if (r.isRest())
        return QString(QChar(0xe4e2 + static_cast<int>(r.rhythm())));
  else {
      if (r.rhythm() == Trhythm::Whole)
        return QStringLiteral("\uf468");
      else if (r.rhythm() == Trhythm::Half)
        return QStringLiteral("\uf4bd");
      else
        return QStringLiteral("\uf4be");
  }
}


void TnoteObject::setStringNumber(int strNr) {
  if (!m_stringNumber && strNr > 0 && strNr < 7) {
    m_staff->score()->component()->setData("import QtQuick 2.9; Text { z: -1; font { pixelSize: 4; family: \"Nootka\" } }", QUrl());
    m_stringNumber = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
    m_stringNumber->setParentItem(this);
  }
  if (strNr > 0 && strNr < 7) {
      m_stringNumber->setProperty("text", QString::number(strNr));
      m_stringNumber->setX((width() - m_stringNumber->width()) / 2.0);
      // TODO set Y position
      m_stringNumber->setVisible(true);
  } else {
      if (m_stringNumber)
        m_stringNumber->setVisible(false);
  }
}


void TnoteObject::setBowing(EbowDirection bowDir) {
  if (!m_bowing && bowDir != BowUndefined) {
    m_staff->score()->component()->setData("import QtQuick 2.9; Text { z: -1; font { pixelSize: 5; family: \"Scorek\" } }", QUrl());
    m_bowing = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
    m_bowing->setParentItem(this);
  }
  if (bowDir != BowUndefined) {
      m_bowing->setProperty("text", bowDir == BowDown ? QStringLiteral("\uE610") : QStringLiteral("\uE612"));
      m_bowing->setX((width() - m_bowing->width()) / 2.0);
//       m_bowing->setY(m_staff->upperLine());
      m_bowing->setVisible(true);
  } else {
      if (m_bowing)
        m_bowing->setVisible(false);
  }
}


void TnoteObject::setFingerNumber(int fiNr)
{
}


void TnoteObject::markNoteHead(const QColor& outlineColor) {
  if (outlineColor.alpha() == 0) {
      m_head->setProperty("style", 0); // In Qt private qquicktext_p.h it is first value of enum TextStyle
  } else {
      m_head->setProperty("style", 1); // In Qt private qquicktext_p.h it is second value of enum TextStyle
      m_head->setProperty("styleColor", outlineColor);
  }
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

QString TnoteObject::getAccidText() {
  if (!m_note->isValid() || (m_note->rtm.tie() && m_note->rtm.tie() != Trhythm::e_tieStart)) // accidental only for starting tie
    return QString();

  QString a = accCharTable[m_note->alter + 2];
  qint8 accidInKey = m_staff->score()->accidInKey(m_note->note - 1);
  if (accidInKey) { // key signature has an accidental on this note
    if (m_note->alter == 0) // show neutral if note has not any accidental
        a = accCharTable[5];
    else {
      if (accidInKey == m_note->alter) { // accidental in key, do not show
        if (m_staff->score()->showExtraAccids() && accidInKey) { // or user wants it at any cost
            a.prepend(QStringLiteral("\ue26a"));
            a.append(QStringLiteral("\ue26b"));
        } else
            a.clear();
      }
    }
  }
  int id = index() - 1; // check the previous notes for accidentals
  while (id >= 0 && m_staff->score()->noteSegment(id)->item()->measure() == measure()) {
    auto checkNote = m_staff->score()->noteSegment(id)->note();
    if (checkNote->note == m_note->note) {
      if (checkNote->rtm.tie() && checkNote->rtm.tie() != Trhythm::e_tieStart) {
        // Ignore notes prolonged with ties - they could be continued from the previous measure
        // and then, the accidental has to be displayed again in current measure
        id--;
        continue;
      }
      char prevAlter = checkNote->alter;
      if (prevAlter != 0 && m_note->alter == 0) {
          if (a.isEmpty())
            a = accCharTable[5]; // and add neutral when some of previous notes with the same step had an accidental
      } else if (prevAlter == m_note->alter) // do not display it twice
          a.clear();
      else if (accidInKey == m_note->alter && prevAlter != m_note->alter)
          a = accCharTable[m_note->alter + 2]; // There is already accidental in key signature but some of the previous notes had another one, show it again
      break;
    }
    id--;
  }
//   if (m_staff->score()->remindAccids() && m_measure->number() > 0) { TODO
//     auto prevMeas = m_staff->score()->measure(m_measure->number() - 1);
//     auto accidState = prevMeas->accidState(m_note->note - 1);
//     if (accidState != 100 && accidState != 0 && a.isEmpty() && m_alter == 0) {
//       a = a = accCharTable[5];
//       m_alter->setProperty("color", QColor(255, 0, 0));
//     } else if (accidState == 0 && accidInKey != 0) {
//       a = accCharTable[m_note->alter + 2];
//       m_alter->setProperty("color", QColor(255, 0, 0));
//     }
//   }
  return a;
}


QString TnoteObject::getHeadText() const {
  return getHeadText(m_note->rtm);
}


QString TnoteObject::getFlagText() {
  if (m_note->rhythm() < Trhythm::Eighth || m_note->isRest() || m_note->rtm.beam() != Trhythm::e_noBeam)
    return QString();
  // In Scorek font, flag glyphs are placed: flag for stem-up, then flag for stem-down, starting from 0xe240
  return QString(QChar(0xe240 + (static_cast<int>(m_note->rhythm()) - 4) * 2 + (m_note->rtm.stemDown() ? 1 : 0)));
}


void TnoteObject::keySignatureChanged() {
  updateAlter();
  updateWidth();
}


void TnoteObject::hoverEnterEvent(QHoverEvent*) {
  if (!m_staff->score()->readOnly()) {
    m_measure->score()->setHoveredNote(this);
    m_measure->score()->changeActiveNote(this);
  }
}


void TnoteObject::hoverLeaveEvent(QHoverEvent*) {
  if (!m_staff->score()->readOnly()) {
    m_measure->score()->setHoveredNote(nullptr);
    m_measure->score()->changeActiveNote(nullptr);
  }
}


void TnoteObject::hoverMoveEvent(QHoverEvent* event) {
  if (!m_staff->score()->readOnly()) {
    if (m_staff->score()->isPianoStaff() && event->pos().y() >= m_staff->upperLine() + 10.6 && event->pos().y() <= m_staff->upperLine() + 11.6)
      return;

    if (m_staff->score()->clefType() == Tclef::NoClef)
      return;

    if (!m_measure->score()->pressedNote() && m_measure->score()->hoveredNote()
        && static_cast<int>(m_measure->score()->activeYpos()) != static_cast<int>(event->pos().y()))
            m_measure->score()->setActiveNotePos(qFloor(event->pos().y()));
  }
}


static QElapsedTimer m_touchDuration;
/**
 * Mouse events are used to handle touch when supported (they are ignored when enter event occurred before invoked by mouse)
 * Press event displays note cursor and locks grabbing the mouse - so moving a finger doesn't scroll entire score
 */
void TnoteObject::mousePressEvent(QMouseEvent* event) {
  if (!m_staff->score()->readOnly()) {
    setKeepMouseGrab(true);
    m_measure->score()->setPressedNote(this);
    m_measure->score()->touchHideTimer()->stop();
    if (m_measure->score()->activeNote() != this) {
      if (!(m_staff->score()->isPianoStaff() && event->pos().y() >= m_staff->upperLine() + 10.6 && event->pos().y() <= m_staff->upperLine() + 11.6)) {
        m_measure->score()->changeActiveNote(this);
        m_measure->score()->setActiveNotePos(qFloor(event->pos().y()));
      }
    }
    m_touchDuration.restart();
    if (!m_measure->score()->hoveredNote())
      m_measure->score()->setTouched(true);
  }
}


/**
 * @p touchHideTimer() of score becomes active after first release of a finger,
 * note cursor remains visible for 2.5 seconds.
 * Second touch will set the note and hide cursor.
 *
 * Release event is also used to set (confirm) a note by mouse press
 */
void TnoteObject::mouseReleaseEvent(QMouseEvent*) {
  if (!m_staff->score()->readOnly()) {
      if (keepMouseGrab())
        setKeepMouseGrab(false);
      if (m_measure->score()->hoveredNote()) { // mouse
          if (m_measure->score()->hoveredNote() == this)
            m_measure->score()->noteClicked(m_measure->score()->activeYpos());
          m_measure->score()->setPressedNote(nullptr);
      } else { // touch
          if (m_touchDuration.elapsed() < 190) { // confirm note
              m_measure->score()->touchHideTimer()->stop();
              if (m_measure->score()->activeNote() == this) // set note only when it was touched second time
                m_measure->score()->noteClicked(m_measure->score()->activeYpos());
              m_measure->score()->setPressedNote(nullptr);
              m_measure->score()->changeActiveNote(nullptr);
          } else { // keep cursor visible
              m_measure->score()->touchHideTimer()->start(2500);
          }
          m_measure->score()->setTouched(false);
      }
    } else {
        if (m_measure->score()->selectInReadOnly())
          emit m_measure->score()->readOnlyNoteClicked(index());
  }
}


void TnoteObject::mouseMoveEvent(QMouseEvent* event) {
  if (!m_staff->score()->readOnly()) {
    if (m_measure->score()->pressedNote() && !m_measure->score()->touchHideTimer()->isActive() && m_touchDuration.elapsed() > 200
        && static_cast<int>(m_measure->score()->activeYpos()) != static_cast<int>(event->pos().y()))
          m_measure->score()->setActiveNotePos(qFloor(event->pos().y()));
  }
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

QQuickItem* TnoteObject::createAddLine() {
  auto line = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
  line->setParentItem(this);
  line->setWidth(3.5);
  line->setHeight(0.2);
  line->setX(m_staff->score()->singleNote() ? 1.0 : -0.5);
  line->setVisible(false);
  line->setProperty("color", qApp->palette().text().color());
  return line;
}


void TnoteObject::updateAlter() {
  if ((m_note->rtm.tie() == Trhythm::e_tieCont || m_note->rtm.tie() == Trhythm::e_tieEnd))
    m_accidText.clear();
  else
    m_accidText = getAccidText();
  m_alter->setProperty("text", m_accidText);
  if (!m_accidText.isEmpty())
    m_alter->setX(-m_alter->width() - 0.1);
}


void TnoteObject::updateWidth() {
  if (m_measure->score()->singleNote())
      setWidth(5.0);
  else {
      qreal w = m_alter->width() + m_head->width();
      if (!m_note->isRest() && !m_note->rtm.stemDown() && m_stem->isVisible() && m_flag->width() > 0.0)
        w += m_flag->width() - 0.5;
      setWidth(w);
      updateTieScale();
  }
}


void TnoteObject::updateNoteHead() {
  QString headText = getHeadText();
  if (m_note->hasDot())
    headText.append(QStringLiteral("\ue1e7"));
  m_head->setProperty("text", headText);
}


void TnoteObject::updateTieScale() {
  if (m_tie) {
    m_tie->setProperty("xScale", tieWidth() / 2.90625);
    m_tie->setProperty("stemDown", m_note->rtm.stemDown());
  }
}


void TnoteObject::checkStem() {
  if (m_notePosY && !m_note->isRest() && m_note->rhythm() > Trhythm::Whole ) {
      if (m_note->rtm.beam() == Trhythm::e_noBeam) {
          m_note->rtm.setStemDown(m_notePosY < staff()->upperLine() + 4.0);
          m_stem->setHeight(qMax(STEM_HEIGHT, qAbs(m_notePosY - (staff()->upperLine() + 4.0))));
          QString flagText = getFlagText();
          m_flag->setProperty("text", flagText);
          if (!flagText.isEmpty())
            m_flag->setY((m_note->rtm.stemDown() ? m_stem->height() : 0.0) - 15.0);
      } else {
          if (m_flag->width() > 0.0)
            m_flag->setProperty("text", QString());
      }
      m_stem->setX(m_head->x() + (m_note->rtm.stemDown() ? 0.0 : 2.0));
      m_stem->setY(m_notePosY + (m_note->rtm.stemDown() ? 0.0 : - m_stem->height()));
      m_stem->setVisible(true);
  } else
      m_stem->setVisible(false);
  bool stemHeightChanged = m_stemHeight != m_stem->height();
  m_stemHeight = m_stem->height();
  if (stemHeightChanged)
    updateNamePos();
}


// TODO: name may collide with next/previous note name (16ths), octave mark out-shots too much
void TnoteObject::updateNamePos() {
  if (m_name) {
    if (m_note->isValid()) {
        m_name->setVisible(true);
        qreal yOff;
        if (m_note->rtm.stemDown())
          yOff = m_notePosY > 6.0 ? -9.5 : m_stemHeight - 4.0;
        else
          yOff = m_notePosY > height() - 6.0 && height() - m_stemHeight > 8.0 ? -m_stemHeight - 8.0 : -2.5;
        m_name->setY(m_notePosY + yOff);
        m_name->setProperty("text", m_note->styledName());
        m_name->setX(x() - m_alter->width() + (width() - m_name->width()) / 2.0);
    } else {
        m_name->setVisible(false);
    }
  }
}


void TnoteObject::checkAddLinesVisibility() {
  for (int i = 0; i < 7; ++i) {
    m_upperLines[i]->setVisible(m_head->isVisible() && !m_note->isRest() && m_notePosY > 0.0 && i >= qFloor((m_notePosY - 1.0) / 2.0));
    m_lowerLines[i]->setVisible(m_head->isVisible() && !m_note->isRest() && staff()->upperLine() + (m_midLine ? 22 : 10.0) + i * 2 <= m_notePosY);
  }
  if (m_midLine)
    m_midLine->setVisible(m_head->isVisible() && !m_note->isRest() && m_notePosY == staff()->upperLine() + 10.0);
}
