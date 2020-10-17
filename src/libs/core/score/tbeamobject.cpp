/***************************************************************************
 *   Copyright (C) 2016-2018 by Tomasz Bojczuk                             *
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

#include "tbeamobject.h"
#include "tscoreobject.h"
#include "tstaffitem.h"
#include "tmeasureobject.h"
#include "tnoteitem.h"
#include "tnotepair.h"
#include <music/tnote.h>

#include <qmath.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtGui/qpainter.h>

#include <QtCore/qdebug.h>


/**
 * Simple structure to describe second beam line (sixteenth)
 * which can be chunked when there are rests or eights in the group
 */
class T16beam {
public:
  T16beam(int firstStemNr = 0) : startStem(firstStemNr) {}
  int startStem = -1; /**< Undefined by default (-1) */
  int endStem = -1; /**< When remains undefined (-1) - beam is partial */

      /** @p TRUE when beam is not connected to another stem */
  bool isHalf() { return endStem == -1; }
};


#define MIN_STEM_HEIGHT (4.0) // minimal stem height (distance of note head to staff boundary)
#define BEAM_THICK (0.8) // thickness of a beam
#define HALF_THICK (0.4) // half of beam thickness



TbeamObject::TbeamObject(TnotePair* sn, TmeasureObject* m) :
  QQuickPaintedItem(m->staff()),
  m_measure(m)
{
  setAcceptHoverEvents(true);
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);
  addNote(sn);
  setParent(m_measure->score());
  connect(qApp, &QGuiApplication::paletteChanged, [=]{ update(); });
  connect(this, &QQuickPaintedItem::visibleChanged, [=]{
    if (isVisible() && count() > 1)
      drawBeam();
  });
}


TbeamObject::~TbeamObject()
{
//   qDebug() << "     [BEAM] deleted of id" << (m_notes.isEmpty() ? -1 : first()->index());
  for (TnotePair* np : qAsConst(m_notes)) {
    np->addChange(TnotePair::e_beamChanged);
    np->setBeam(nullptr);
//     resetBeam(np);
  }
}


void TbeamObject::addNote(TnotePair* np) {
  if (np == nullptr)
    return;

  if (np->beam() == nullptr)
    np->setBeam(this);
  else
    qDebug() << "     [BEAM] note" << np->index() << "has already a beam";

  if (m_notes.count() > 1)
    m_notes.last()->note()->rtm.setBeam(Trhythm::e_beamCont); // no need to be updated
  if (m_notes.isEmpty())
    np->note()->rtm.setBeam(Trhythm::e_beamStart);
  else
    np->note()->rtm.setBeam(Trhythm::e_beamEnd);
  if (np->item()) // mark it changed only when object exists, otherwise its note will be updated during creation
    np->addChange(TnotePair::e_beamChanged);
  m_notes << np;

  if (np->note()->rhythm() == Trhythm::Sixteenth) {
    Tnote* beforeLastNote = m_notes.count() > 1 ? m_notes[m_notes.count() - 2]->note() : nullptr;
    if (m_16beams.isEmpty() || (beforeLastNote && beforeLastNote->rhythm() != Trhythm::Sixteenth)) {
        // is first in beam or previous note was not a sixteenth
        m_16beams << T16beam(m_notes.count() - 1); // then create new beam segment
    } else if (!m_16beams.isEmpty() && (beforeLastNote && beforeLastNote->rhythm() == Trhythm::Sixteenth)) {
        // there is 16 beam and previous note and this notes are 16th
        m_16beams.last().endStem = m_notes.count() - 1; // then set end stem for it
    }
  }
}


/**
 * With @p stemDirStrength (strength of stem direction) we trying to determine preferred common direction of stems in the group.
 * More far from middle of the staff (18) note is placed - stronger its direction has to be preserved
 * to keep beaming more natural and looking good
 *
 * @p stemsUpPossible keep true when there is enough space between note head and staff border for whole stem
 * if not stems go down.
 * @p stemTop it top stem position of all notes. It is set to middle of the staff if necessary (to look well)
 */
void TbeamObject::prepareBeam() {
  int stemDirStrength = 0;
  bool stemsUpPossible = true;
  qreal hiNote = 99.0, loNote = 0.0;
  for (TnotePair* np : qAsConst(m_notes)) {
    stemDirStrength += np->item()->notePosY() - (m_measure->staff()->upperLine()
              + (m_measure->score()->isPianoStaff() && np->item()->notePosY() > m_measure->staff()->upperLine() + 13.0 ? 26.0 : 4.0));
    if (np->item()->notePosY() < MIN_STEM_HEIGHT)
      stemsUpPossible = false;
    hiNote = qMin(hiNote, np->item()->notePosY());
    loNote = qMax(loNote, np->item()->notePosY());
  }
  bool allStemsDown = !stemsUpPossible;
  qreal minStemHeight = MIN_STEM_HEIGHT + (m_16beams.empty() ? 0.0 : 1.0);
  if (stemDirStrength < 0)
    allStemsDown = true; // stems down are always possible
  qreal stemTop = allStemsDown ? loNote + minStemHeight : hiNote - minStemHeight;
  if (m_measure->score()->isPianoStaff() && !first()->note()->onUpperStaff()) { // when note lays on the lower staff
      qreal lowerMidLine = m_measure->staff()->upperLine() + 26.0;
      if ((allStemsDown && stemTop < lowerMidLine) || (!allStemsDown && stemTop > lowerMidLine))
        stemTop = lowerMidLine; // keep beam on the lower staff middle line
  } else {
      qreal upperMidLine = m_measure->staff()->upperLine() + 4.0;
      if ((allStemsDown && stemTop < upperMidLine) || (!allStemsDown && stemTop > upperMidLine))
        stemTop = upperMidLine; // keep beam on staff middle line
  }
  for (TnotePair* np : qAsConst(m_notes)) {
    np->note()->rtm.setStemDown(allStemsDown);
    np->addChange(TnotePair::e_stemDirChanged);
    np->item()->setStemHeight(qAbs(np->item()->notePosY() - stemTop));
    np->approve();
  }
}


/**
 * Polygons are painted, single for 8ths and possible a few for 16ths.
 * Painter canvas orientation depends on stems direction (up or down),
 * for stems-up, top beam is 8ths and bottom are 16ths in contrary to stems-down
 */
void TbeamObject::paint(QPainter* painter) {
  if (count() > 1) {
    qreal s = first()->note()->rtm.stemDown() ? -1.0 : 1.0; // scale
    qreal t = first()->note()->rtm.stemDown() ? height() : 0.0; // translation
    painter->setPen(Qt::NoPen);
    painter->setBrush(qApp->palette().text().color());
    QPolygonF topBeam;
    qreal endX = last()->item()->stemTop().x() - x() + 0.3;
    topBeam << QPointF(0.0, t) << QPointF(0.0, t + s * BEAM_THICK) << QPointF(endX, t + s * BEAM_THICK) << QPointF(endX, t) << QPointF(0.0, t);
    painter->drawPolygon(topBeam);
    for (int b = 0; b < m_16beams.count(); ++b) {
      T16beam& b16 = m_16beams[b];
      qreal startX = m_notes[b16.startStem]->item()->stemTop().x() - x();
      // 16th beam of fist stem is right-sided (2.0) others are left-sided (-2.0)
      endX = (b16.isHalf() ? startX + BEAM_THICK * (b16.startStem == 0 ? 2.0 : -2.0) : m_notes[b16.endStem]->item()->stemTop().x() - x()) + 0.3;
      QPolygonF polyB;
      polyB << QPointF(startX, t + s * 1.5 * BEAM_THICK) << QPointF(startX, t + s * 2.5 * BEAM_THICK) << QPointF(endX, t + s * 2.5 * BEAM_THICK)
            << QPointF(endX, t + s * 1.5 * BEAM_THICK) << QPointF(startX, t + s * 1.5 * BEAM_THICK);
      painter->drawPolygon(polyB);
    }
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

/**
 * According to first and last notes in the beam, this method sets item geometry.
 * @p setTextureSize() is called to make texture big enough and avoid pixelization
 */
void TbeamObject::drawBeam() {
  auto p1 = first()->item()->stemTop();
  auto p2 = last()->item()->stemTop();
  setWidth(qAbs(p2.x() - p1.x()) + 1.0);
  setHeight(qAbs(p1.y() - p2.y()) + BEAM_THICK * 2.5);
  setX(p1.x());
  setY(qMin(p1.y(), p2.y()) - (first()->note()->rtm.stemDown() ? 2.0 * BEAM_THICK : HALF_THICK));
  setTextureSize(QSize(qCeil(width() * m_measure->staff()->scale()), qCeil(height() * m_measure->staff()->scale())));
  /**
   * @p update() should be called here to invoke @p paint()
   * but it is invoked by Qt itself whenever width, height, x or y change
   */
}


void TbeamObject::changeStaff(TstaffItem* st) {
  setParentItem(st);
}


bool TbeamObject::removeNote(TnotePair* np) {
  bool deleteBeam = false;
  int noteId = m_notes.indexOf(np);
  if (noteId == -1) { // TODO remove this if does not occur
    qDebug() << "     [BEAM] of note id" << first()->index() << "has no note to remove";
    return false;
  }
  if (noteId > 1) { // there are at least two notes at the beam beginning
      if (count() - noteId > 2) { // split beam
          resetBeam(m_notes.takeAt(noteId));
          int tempCount = count();
          TbeamObject *otherBeam = nullptr;
          for (int n = noteId; n < tempCount; ++n) {
            auto noteForOtherBeam = m_notes.takeAt(noteId);
            resetBeam(noteForOtherBeam);
            if (otherBeam)
              otherBeam->addNote(noteForOtherBeam);
            else
              otherBeam = m_measure->score()->getBeam(noteForOtherBeam, m_measure); //new TbeamObject(noteForOtherBeam, m_measure);
          }
          otherBeam->prepareBeam();
          otherBeam->drawBeam();
      } else { // remove rest of notes
          int tempCount = count();
          for (int n = noteId; n < tempCount; ++n)
            resetBeam(m_notes.takeLast());
      }
  } else {
      if (count() - noteId > 2) {
          for (int n = 0; n <= noteId; ++n)
            resetBeam(m_notes.takeFirst());
      } else
          deleteBeam = true;
  }
  if (!deleteBeam && !m_16beams.isEmpty()) { // renew 16th beam(s) state
    m_16beams.clear();
    for (int n = 0; n < count(); ++n) {
      auto nt = m_notes[n];
      if (nt->note()->rhythm() == Trhythm::Sixteenth) {
        if (m_16beams.isEmpty())
            m_16beams << T16beam(n);
        else {
            T16beam& last16Beam = m_16beams.last();
            if (last16Beam.isHalf()) {
                if (last16Beam.startStem == n - 1)
                  last16Beam.endStem = n;
                else
                  m_16beams << T16beam(n);
            } else {
                if (last16Beam.endStem == n - 1)
                  last16Beam.endStem = n;
                else
                  m_16beams << T16beam(n);
            }
        }
      }
    }
  }
  return deleteBeam;
}


void TbeamObject::setMeasure(TmeasureObject* m) {
  if (m != m_measure) {
    m_measure = m;
    if (m_measure)
      changeStaff(m_measure->staff());
  }
}


void TbeamObject::deleteBeam() {
  m_measure->score()->storeBeam(this);
  for (TnotePair* np : qAsConst(m_notes)) {
    resetBeam(np);
  }
  m_16beams.clear();
  m_notes.clear();
  changeStaff(nullptr);
  m_measure = nullptr;
}


void TbeamObject::resetBeam(TnotePair* noteToRemove) {
  noteToRemove->note()->rtm.setBeam(Trhythm::e_noBeam); // restore beams
  noteToRemove->addChange(TnotePair::e_beamChanged); // and inform TnotePair about changes (it has to invoke approve to fix them)
  noteToRemove->setBeam(nullptr);
}
