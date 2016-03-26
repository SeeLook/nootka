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
#include <tnoofont.h>
#include <music/trhythm.h>
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


/** Subclass of @class TpaneItem to paint note symbol with dot or triplet */
class TrhythmItem : public TpaneItem {
public:
  explicit TrhythmItem(int charNr, QGraphicsObject* parent) :
    TpaneItem(charNr, parent),
    m_hasDot(false), m_isTriplet(false)
  {}

  void setRhythmGlyph(Trhythm* r) {
    setSymbol(TnooFont::getCharFromRhythm(r->rhythm(), true, r->isRest()));
    m_hasDot = r->hasDot();
    m_isTriplet = r->isTriplet();
  }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {
    TpaneItem::paint(painter, option, widget);
    if (m_hasDot || m_isTriplet) {
        painter->setPen(QPen(color(), 1));
        if (m_hasDot) {
            painter->drawText(QPointF(2.1, 2.5), QStringLiteral("."));
        } else if (m_isTriplet) {
            painter->setFont(TnooFont(2));
            painter->drawText(QPointF(0.2, 1.0), TnooFont::digit(3));
        }
    }
  }
private:
  bool        m_hasDot, m_isTriplet;
};



QGraphicsDropShadowEffect* ItemHighLight() {
  QGraphicsDropShadowEffect *hiBlur = new QGraphicsDropShadowEffect();
  hiBlur->setColor(qApp->palette().highlight().color());
  hiBlur->setOffset(1.0, 1.0);
  hiBlur->setBlurRadius(7.0);
  return hiBlur;
}


TnoteControl::TnoteControl(bool isLeft, TscoreStaff* staff, TscoreScene* scene) :
  TscoreItem(scene),
  m_isLeft(isLeft),
  m_scoreNote(0),
  m_isEnabled(true), m_entered(false),
  m_underItem(0),
  m_moveNote(false),
  m_accidental(0), m_prevAccidIt(0),
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

    m_accidGap = createPaneItem(32, 10.0); // gap item between sharp and flat - just space glyph
    m_sharp = createPaneItem(35, 7.5, tr("<b>sharp</b> - raises a note by a half tone (semitone).<br>On the guitar it is one fret up."));
    m_flat =  createPaneItem(98, 11.0, tr("<b>flat</b> - lowers a note by a half tone (semitone).<br>On the guitar it is one fret down."));
} else {
    m_gradient.setStart(0.0, 1.0);
    m_gradient.setFinalStop(WIDTH, 1.0);
    m_gradient.setColorAt(0.5, startC);
    m_gradient.setColorAt(1.0, endC);
  // 'name' glyph for editing note through its name
    m_name = createPaneItem(99, 5.0, tr("Click %1 to edit note name")
            .arg(TnooFont::span(QStringLiteral("c"), qApp->fontMetrics().boundingRect(QStringLiteral("A")).height() * 1.5,
                                QStringLiteral("color: ") + scoreScene()->nameColor().name())));
      m_name->setColor(scoreScene()->nameColor());
  // 'crossed note' symbol for deleting notes
    m_deleteNote = createPaneItem(111, boundingRect().height() - 3.5, tr("Click %1 to remove a note")
            .arg(TnooFont::span(QStringLiteral("o"), qApp->fontMetrics().boundingRect(QStringLiteral("A")).height() * 2.0, QStringLiteral("color: #ff0000"))));
      m_deleteNote->setColor(Qt::red);

    m_rhythmItem = new TrhythmItem(0x45, this); // initialize with quarter TODO it has to depend on lower meter digit
      m_rhythmItem->setPos(0.0, 11.0);
      m_rhythmItem->setStatusTip(tr("select rhythm value"));
      m_rhythmItem->setColor(Qt::darkYellow);
      connect(m_rhythmItem, &TpaneItem::clicked, this, &TnoteControl::rightItemClicked);
      connect(m_rhythmItem, &TpaneItem::entered, this, &TnoteControl::itemHoverEntered);
      connect(m_rhythmItem, &TpaneItem::leaved, this, &TnoteControl::itemHoverLeaved);
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


void TnoteControl::setRhythm(Trhythm* r) {
  if (!isLeftPane()) {
    if (r->rhythm() == Trhythm::e_none)
      m_rhythmItem->hide();
    else {
      m_rhythmItem->show();
      m_rhythmItem->setRhythmGlyph(r);
    }
  }
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
        m_dblSharp = createPaneItem(120, 4.0,
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
    return QRectF(0.0, 0.0, WIDTH, 21.0);
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


void TnoteControl::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  TscoreItem::hoverEnterEvent(0);
  m_entered = true;
//   m_delayTimer->start(150);
}


void TnoteControl::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
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
  if (touchEnabled() && notesAddingEnabled()) {
      m_scoreNote->hideWorkNote();
      if (!isLeftPane()) { // right control - append a note
          if (m_scoreNote->index() == staff()->count() - 1)
            QTimer::singleShot(50, [=]{ hide(); }); // hide right pane when note is added at staff end to edit new one easily (otherwise pane covers it)
          staff()->insertNote(m_scoreNote->index() + 1);
      } else { // left control - preppend a note
          staff()->insertNote(m_scoreNote->index());
      }
  } else {
    if (m_adding) {
      if (!isLeftPane()) { // right control - append a note
          if (m_scoreNote->index() == staff()->count() - 1)
            QTimer::singleShot(50, [=]{ hide(); }); // hide right pane when note is added at staff end to edit new one easily (otherwise pane covers it)
          staff()->insertNote(m_scoreNote->index() + 1);
      } else { // left control - preppend a note
          staff()->insertNote(m_scoreNote->index());
      }
    }
  }
}


void TnoteControl::touched(const QPointF& scenePos) {
  TscoreItem::hoverEnterEvent(0);
//   QGraphicsSceneHoverEvent he(QEvent::GraphicsSceneHoverMove);
//   he.setPos(mapFromScene(scenePos));
  hoverMoveEvent(0);
  m_entered = true;
}


void TnoteControl::untouched(const QPointF& scenePos) {
  mousePressEvent(0);
  hoverLeaveEvent(0);
}

//##########################################################################################################
//####################################         PRIVATE     #################################################
//##########################################################################################################
TpaneItem* TnoteControl::createPaneItem(int charNr, qreal yPos, const QString& status) {
  TpaneItem* it = new TpaneItem(charNr, this);
  it->setPos(0.0, yPos);
  it->setStatusTip(status);
  if (isLeftPane()) {
    if (charNr != 32) // do not connect m_accidGap with accidental slot
      connect(it, &TpaneItem::clicked, this, &TnoteControl::accidClicked);
  } else {
      connect(it, &TpaneItem::clicked, this, &TnoteControl::rightItemClicked);
  }
  connect(it, &TpaneItem::entered, this, &TnoteControl::itemHoverEntered);
  connect(it, &TpaneItem::leaved, this, &TnoteControl::itemHoverLeaved);
  return it;
}








