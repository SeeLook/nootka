/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "trhythmpane.h"
#include "tpaneitem.h"
#include "tscorestaff.h"
#include <music/trhythm.h>
#include <tnoofont.h>
#include <graphics/tdropshadoweffect.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>



TrhythmPane::TrhythmPane(TscoreStaff* staff, TscoreScene* scene) :
  TscoreItem(scene)
{
  setParentItem(staff);
  setZValue(60);
  hide();

  m_hideTimer = new QTimer(this);
  m_hideTimer->setSingleShot(true);
  connect(m_hideTimer, &QTimer::timeout, [=]{ hide(); });

  m_whole = createPaneItem(67, QPointF(0.0, 0.0));
  m_half = createPaneItem(68, QPointF(0.0, m_whole->bottomY()));
  m_quarter = createPaneItem(69, QPointF(0.0, m_half->bottomY()));
  m_eight = createPaneItem(70, QPointF(0.0, m_quarter->bottomY()));
  m_sixteen = createPaneItem(71, QPointF(0.0, m_eight->bottomY()));
  m_dot = createPaneItem(46, QPointF(0.0, m_sixteen->bottomY()));
  m_wholeRest = createPaneItem(0xe107, QPointF(WIDTH, 0.0));
  m_halfRest = createPaneItem(0xe108, QPointF(WIDTH, m_wholeRest->bottomY()));
  m_quarterRest = createPaneItem(0xe109, QPointF(WIDTH, m_halfRest->bottomY()));
  m_eightRest = createPaneItem(0xe10a, QPointF(WIDTH, m_quarterRest->bottomY()));
  m_sixteenRest = createPaneItem(0xe10b,  QPointF(WIDTH, m_eightRest->bottomY()));
  m_triplet = createPaneItem(387, QPointF(WIDTH, m_sixteenRest->bottomY()));

  m_tie = createPaneItem(0xe18c, QPointF(0.0, m_dot->bottomY()));
  QTransform t;
  t.scale(2.0, 1.0);
  m_tie->setTransform(t);

  m_quarter->setSelected(true); // some rhythm value has to be selected
  m_selectedRhythmItem = m_quarter;

  m_rhythm = new Trhythm(Trhythm::e_quarter);

  m_notes << m_whole << m_half << m_quarter << m_eight << m_sixteen;
  m_rests << m_wholeRest << m_halfRest << m_quarterRest << m_eightRest << m_sixteenRest;

  setGraphicsEffect(new TdropShadowEffect);
}


TrhythmPane::~TrhythmPane() {
  delete m_rhythm;
}


void TrhythmPane::setRhythm(Trhythm* r) {
  m_selectedRhythmItem->setSelected(false); // un-select previous items
  if (r->rhythm() == Trhythm::e_none)
    return;

  if (r->isRest())
    m_selectedRhythmItem = m_rests[std::log2<int>((int)r->rhythm())];
  else
    m_selectedRhythmItem = m_notes[std::log2<int>((int)r->rhythm())];
  m_selectedRhythmItem->setSelected(true);
  m_dot->setSelected(r->hasDot());
  m_triplet->setSelected(r->isTriplet());
  m_rhythm->setRhythm(*r);
  showHideDotTriplet();
}


QRectF TrhythmPane::boundingRect() const {
  return QRectF(0.0, 0.0, WIDTH * 2, 24.0);
}


void TrhythmPane::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

  painter->setPen(Qt::NoPen);
    QColor bc = qApp->palette().base().color();
  bc.setAlpha(240);
  painter->setBrush(QBrush(bc));
  painter->drawRoundedRect(boundingRect(), 0.25, 0.25);
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TrhythmPane::itemClicked() {
  if (sender() == m_tie) {
      m_tie->setSelected(!m_tie->isSelected());
      return;
  }
  if (sender() == m_dot) {
      m_triplet->setSelected(false); // no tuplets with dot
      m_rhythm->setTriplet(false);
      m_dot->setSelected(!m_dot->isSelected());
  } else if (sender() == m_triplet) {
      m_dot->setSelected(false); // un-select dot
      if (m_selectedRhythmItem != m_whole && m_selectedRhythmItem != m_wholeRest) // tuplets of whole notes not supported
        m_triplet->setSelected(!m_triplet->isSelected());
  } else if (sender() != m_selectedRhythmItem) {
      m_selectedRhythmItem->setSelected(false);
      m_selectedRhythmItem = qobject_cast<TpaneItem*>(sender());
      m_selectedRhythmItem->setSelected(true);
      m_dot->setSelected(false);
      m_triplet->setSelected(false);
  }
  int valueIndex = m_notes.indexOf(m_selectedRhythmItem);
  if (valueIndex == -1) { // check is it a note or a rest
      valueIndex = m_rests.indexOf(m_selectedRhythmItem);
      m_rhythm->setRest(m_rests.contains(m_selectedRhythmItem));
  } else
      m_rhythm->setRest(false);
  if (valueIndex != -1)
    m_rhythm->setRhythmValue(Trhythm::Erhythm(qPow(2, valueIndex)));
  m_rhythm->setDot(m_dot->isSelected());
  m_rhythm->setTriplet(m_triplet->isSelected());
  showHideDotTriplet();

  emit rhythmChanged();
}


void TrhythmPane::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  m_hideTimer->stop();
//   TscoreItem::hoverEnterEvent(event);
}


void TrhythmPane::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  m_hideTimer->start(1000);
// TscoreItem::hoverLeaveEvent(event);
}



//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################
TpaneItem* TrhythmPane::createPaneItem(int charNr, const QPointF& p) {
  auto it = new TpaneItem(charNr, this);
  it->setPos(p);
  it->setZValue(255);
  connect(it, &TpaneItem::clicked, this, &TrhythmPane::itemClicked);
//   connect(it, &TpaneItem::entered, [=]{ m_hideTimer->stop(); });
  return it;
}


/** Triplets of whole notes are not supported and dots of sixteenths also.
 * This common method hides or show appropriate items (dot/triplet) depends on m_rhythm state */
void TrhythmPane::showHideDotTriplet() {
  if (m_rhythm->rhythm() == Trhythm::e_whole)
    m_triplet->hide();
  else
    m_triplet->show();
  if (m_rhythm->rhythm() == Trhythm::e_sixteenth)
    m_dot->hide();
  else
    m_dot->show();
}


