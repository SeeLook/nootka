/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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


#include "tnotecontrol.h"
#include "tscorestaff.h"
#include "tscorenote.h"
#include "tscorescene.h"
#include "tpaneitem.h"
#include <tnoofont.h>
#include <music/tnote.h>
#include <graphics/tdropshadoweffect.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qgraphicsview.h>


#if defined (Q_OS_ANDROID)
  #define HEIGHT (27.0)
#else
  #define HEIGHT (21.0)
#endif


QGraphicsDropShadowEffect* ItemHighLight() {
  QGraphicsDropShadowEffect *hiBlur = new QGraphicsDropShadowEffect();
  hiBlur->setColor(qApp->palette().highlight().color());
  hiBlur->setOffset(1.0, 1.0);
  hiBlur->setBlurRadius(7.0);
  return hiBlur;
}

/**
 * Left pane items (accidentals) positioning:
 * 3.5 - double sharp
 * 6.5 - sharp
 * 10.0 - gap (space)
 * 11.0 - flat
 * 14.5 - double flat
 */

TnoteControl::TnoteControl(bool isLeft, TscoreStaff* staff, TscoreScene* scene) :
  TscoreItem(scene),
  m_scoreNote(0),
  m_isLeft(isLeft),
  m_isEnabled(true),
  m_entered(false),
  m_prevAccidIt(0),
  m_underItem(0),
  m_moveNote(false),
  m_accidental(0),
  m_notesAdding(true),
  m_adding(false),
  m_delayTimer(new QTimer(this))
{
  setStaff(staff);
  setParentItem(staff);
  setZValue(60);
  hide();

  QColor startC = qApp->palette().text().color(), endC = startC;
  startC.setAlpha(150);
  endC.setAlpha(200);
if (isLeft) {
    m_gradient.setStart(WIDTH, 1.0);
    m_gradient.setFinalStop(0.0, 1.0);
    QColor startC = qApp->palette().text().color(), endC = startC;
    startC.setAlpha(150);
    endC.setAlpha(200);
    m_gradient.setColorAt(0.5, startC);
    m_gradient.setColorAt(1.0, endC);

    m_accidGap = createPaneItem(32, 9.5); // gap item between sharp and flat - just space glyph
    m_sharp = createPaneItem(35, 6.5, tr("<b>sharp</b> - raises a note by a half tone (semitone).<br>On the guitar it is one fret up."));
    m_flat =  createPaneItem(98, 11.0, tr("<b>flat</b> - lowers a note by a half tone (semitone).<br>On the guitar it is one fret down."));
} else {
    m_gradient.setStart(0.0, 1.0);
    m_gradient.setFinalStop(WIDTH, 1.0);
    m_gradient.setColorAt(0.5, startC);
    m_gradient.setColorAt(1.0, endC);
  // 'name' glyph for editing note through its name
    m_name = createPaneItem(99, 5.0, tr("Click %1 to edit note name")
            .arg(TnooFont::span(QStringLiteral("c"), qApp->fontMetrics().boundingRect(QStringLiteral("A")).height() * 1.5,
                                QLatin1String("color: ") + scoreScene()->nameColor().name())));
    m_name->setColor(scoreScene()->nameColor());
  // 'crossed note' symbol for deleting notes
    m_deleteNote = createPaneItem(111, boundingRect().height() - 3.5, tr("Click %1 to remove a note")
            .arg(TnooFont::span(QStringLiteral("o"), qApp->fontMetrics().boundingRect(QStringLiteral("A")).height() * 2.0, QStringLiteral("color: #ff0000"))));
    m_deleteNote->setColor(Qt::red);

}

  connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
//   connect(m_delayTimer, SIGNAL(timeout()), this, SLOT(showDelayed()));
  setGraphicsEffect(new TdropShadowEffect);
}


TnoteControl::~TnoteControl()
{
}


void TnoteControl::adjustSize() {
  if (!isLeftPane())
      m_name->setColor(scoreScene()->nameColor());
}


void TnoteControl::hideWithDelay(int delay) {
  if (delay) {
    if (delay == -1)
      delay = touchEnabled() ? 1000: 300;
    QTimer::singleShot(delay, this, SLOT(hideDelayed()));
  } else
    hideDelayed();
}


void TnoteControl::addAccidentals() {
  if (isLeftPane()) {
    if (scoreScene()->doubleAccidsFuse() == 2) { // double accidentals
      if (!m_dblSharp) {
        m_dblSharp = createPaneItem(120, 3.5,
                      tr("<b>double sharp</b> - raises a note by two semitones (whole tone).<br>On the guitar it is two frets up."));
        m_dblFlat = createPaneItem(66, 14.5,
                      tr("<b>double flat</b> - lowers a note by two semitones (whole tone).<br>On the guitar it is two frets down."));
      }
    } else {
        if (m_dblSharp) {
          delete m_dblSharp;
          delete m_dblFlat;
        }
    }
  }
}


QRectF TnoteControl::boundingRect() const {
    return QRectF(0.0, 0.0, WIDTH, HEIGHT);
}


void TnoteControl::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  if (m_entered && m_adding)
    painter->setBrush(QBrush(m_gradient));
  else {
    QColor bc = qApp->palette().base().color();
    bc.setAlpha(240);
    painter->setBrush(QBrush(bc));
  }
  painter->setPen(Qt::NoPen);
  painter->drawRoundedRect(boundingRect(), 0.25, 0.25);
  if ((touchEnabled() && notesAddingEnabled()) || (m_entered && m_adding)) { // 'plus' symbol
    if (touchEnabled())
      painter->setPen(QPen(qApp->palette().text().color(), 0.4, Qt::SolidLine, Qt::RoundCap));
    else
      painter->setPen(QPen(qApp->palette().base().color(), 0.4, Qt::SolidLine, Qt::RoundCap));
    qreal plusW = (WIDTH - 2.0) / 2.0;
    painter->drawLine(QLineF(plusW, 2.0, WIDTH - plusW, 2.0)); // horizontal 'plus' line
    painter->drawLine(QLineF(WIDTH / 2.0, 1.0, WIDTH / 2.0, 3.0)); // vertical 'plus' line
  }
}


void TnoteControl::setScoreNote(TscoreNote* sn) {
  m_scoreNote = sn;
  if (sn) {
      if (sn->parentItem() && parentItem() != sn->parentItem()) {
        if (parentItem())
            parentItem()->setZValue(10);
        setParentItem(sn->parentItem());
        setStaff(sn->staff());
        parentItem()->setZValue(11);
      }
      if (!isLeftPane()) {
        if (notesAddingEnabled()) {
            if (staff()->number() == 0 && staff()->count() < 2)
                m_deleteNote->hide(); // prevent deleting only one note
            else
                m_deleteNote->show();
        }
      }
  } else {
      hide();
  }
}


void TnoteControl::setAccidental(int acc) {
  m_accidental = acc;
  if (m_prevAccidIt) {
    m_prevAccidIt->setSelected(false);
    m_prevAccidIt = 0;
  }
  TpaneItem* it = 0;
  if (acc == -2)
    it = m_dblFlat;
  else if (acc == -1)
    it = m_flat;
  else if (acc == 1)
    it = m_sharp;
  else if (acc == 2)
    it = m_dblSharp;
  if (it) {
    it->setSelected(true);
    m_prevAccidIt = it;
  }
}


void TnoteControl::enableToAddNotes(bool addEnabled) {
  m_notesAdding = addEnabled;
  if (!isLeftPane()) {
    if (notesAddingEnabled()) {
      if (staff()->number() == 0 && staff()->count() < 2)
          m_deleteNote->hide(); // prevent deleting only one note
      else
          m_deleteNote->show();
    } else {
      m_deleteNote->hide();
    }
  }
}


void TnoteControl::hide() {
  m_adding = false;
  QGraphicsItem::hide();
}


void TnoteControl::enableNoteName(bool enableName) {
  enableName ? m_name->show() : m_name->hide();
}


bool TnoteControl::noteNameEnabled() {
  return m_name->isVisible(); 
}


//##########################################################################################################
//####################################         PROTECTED    ################################################
//##########################################################################################################

void TnoteControl::itemHoverEntered(TpaneItem* it) {
  if (m_adding) {
    m_adding = false;
    update();
  }
  scoreScene()->controlMoved();
  if (!it->statusText().isEmpty())
    emit statusTip(it->statusText());
}


void TnoteControl::itemHoverLeaved(TpaneItem* it) {
  if (!it->statusText().isEmpty())
    emit statusTip(QString());
}


void TnoteControl::accidClicked() {
  auto accidIt = qobject_cast<TpaneItem*>(sender());
  if (m_prevAccidIt && m_prevAccidIt != accidIt)
    m_prevAccidIt->setSelected(false);
  accidIt->setSelected(!accidIt->isSelected());
  if (accidIt->isSelected()) {
      if (accidIt == m_dblSharp)
          m_accidental = 2;
      else if (accidIt == m_sharp)
          m_accidental = 1;
      else if (accidIt == m_flat)
          m_accidental = -1;
      else if (accidIt == m_dblFlat)
          m_accidental = -2;
      m_prevAccidIt = accidIt;
  } else {
      m_prevAccidIt = 0;
      m_accidental = 0;
  }
  scoreScene()->setCurrentAccid(m_accidental);
}


void TnoteControl::rightItemClicked() {
  hoverLeaveEvent(0);
  if (sender() == m_name)
      emit nameMenu(m_scoreNote);
  else if (sender() == m_deleteNote)
      staff()->removeNote(m_scoreNote->index());
  else if (m_rhythmItem)
      emit rhythmItemClicked();
}


void TnoteControl::hideDelayed() {
  if (scoreScene()->isCursorVisible())
    return;
  if (hasCursor())
    hideWithDelay();
  else
    hide();
}


void TnoteControl::showDelayed() {
  m_delayTimer->stop();
  if (hasCursor()) {
    m_entered = true;
    if (m_adding)
      update();
  }
}


void TnoteControl::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
  TscoreItem::hoverEnterEvent(nullptr);
  m_entered = true;
}


void TnoteControl::hoverMoveEvent(QGraphicsSceneHoverEvent*) {
  scoreScene()->controlMoved();
  if (m_notesAdding) {
      if (!m_adding) {
        emit statusTip(tr("Click to add a new note"));
        m_adding = true;
        update();
      }
  }
}


void TnoteControl::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  if (m_adding) {
    m_adding = false;
    update();
  }
  hideWithDelay();
  TscoreItem::hoverLeaveEvent(event);
  m_entered = false;
  scoreScene()->controlLeaved(scoreNote());
}


void TnoteControl::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  Q_UNUSED(event)
  Tnote insNote(0, 0, 0);
  if (touchEnabled() && notesAddingEnabled()) {
      m_scoreNote->hideWorkNote();
      if (!isLeftPane()) { // right control - append a note
          if (m_scoreNote->index() == staff()->count() - 1)
            QTimer::singleShot(50, [=]{ hide(); }); // hide right pane when note is added at staff end to edit new one easily (otherwise pane covers it)
          staff()->insertNote(m_scoreNote->index() + 1, insNote);
      } else { // left control - preppend a note
          staff()->insertNote(m_scoreNote->index(), insNote);
      }
  } else {
    if (m_adding) {
      if (!isLeftPane()) { // right control - append a note
          if (m_scoreNote->index() == staff()->count() - 1)
            QTimer::singleShot(50, [=]{ hide(); }); // hide right pane when note is added at staff end to edit new one easily (otherwise pane covers it)
          staff()->insertNote(m_scoreNote->index() + 1, insNote);
      } else { // left control - preppend a note
          staff()->insertNote(m_scoreNote->index(), insNote);
      }
    }
  }
}


void TnoteControl::touched(const QPointF& scenePos) {
  Q_UNUSED(scenePos)
  TscoreItem::hoverEnterEvent(nullptr);
  hoverMoveEvent(nullptr);
  m_entered = true;
}


void TnoteControl::untouched(const QPointF& scenePos) {
  Q_UNUSED(scenePos)
  mousePressEvent(nullptr);
  hoverLeaveEvent(nullptr);
}

//##########################################################################################################
//####################################         PRIVATE     #################################################
//##########################################################################################################
TpaneItem* TnoteControl::createPaneItem(int charNr, qreal yPos, const QString& status) {
  auto it = new TpaneItem(charNr, scoreScene());
  it->setParentItem(this);
#if defined (Q_OS_ANDROID)
  if (charNr == 111)
    it->setPos(0.0, boundingRect().height() - 5.0);
  else
    it->setPos(0.0, yPos * 1.4);
#else
  it->setPos(0.0, yPos);
  it->setStatusTip(status);
#endif
  if (isLeftPane()) {
    if (charNr != 32) // do not connect m_accidGap with accidental slot
      connect(it, &TpaneItem::clicked, this, &TnoteControl::accidClicked);
  } else {
      connect(it, &TpaneItem::clicked, this, &TnoteControl::rightItemClicked);
  }
#if !defined (Q_OS_ANDROID)
  connect(it, &TpaneItem::entered, this, &TnoteControl::itemHoverEntered);
  connect(it, &TpaneItem::leaved, this, &TnoteControl::itemHoverLeaved);
#endif
  return it;
}








