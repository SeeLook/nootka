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

#include "tscorebeam.h"
#include "tscorenote.h"
#include "tscorestaff.h"
#include "tnoteitem.h"
#include "tscoremeasure.h"
#include <music/trhythm.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtCore/qdebug.h>


/**
 * Simple structure to describe second beam line (sixteenth)
 * which can be chunked when there are rests or eights in the group
 */
class T16beam {
public:
  T16beam(int firstStemNr = 0, QGraphicsPolygonItem* beamPoly = nullptr) : startStem(firstStemNr), b(beamPoly) {}
  ~T16beam() { delete b; }
  int startStem = -1; /**< Undefined by default */
  int endStem = -1; /**< When remains undefined - beam is partial */
  bool isHalf() { return endStem == -1; } /**< @p TRUE when beam is not connected to another stem */
  QGraphicsPolygonItem* b = nullptr; /**< beam item */
};


#define MIN_STEM_HEIGHT (4) // minimal stem height (distance of note head to staff boundary)
#define STEM_HEIGHT (6.1)
#define HALF_STEM (0.125) // half of stem line width - this is also distance that stem line takes above stem points
#define BEAM_THICK (0.8) // thickness of a beam


TscoreBeam::TscoreBeam(TscoreNote* sn, TscoreMeasure* m) :
  QObject(m),
  m_measure(m)
{
  addNote(sn);
}


TscoreBeam::~TscoreBeam()
{
  for (TscoreNote* note : m_notes) // restore beams
    note->newRhythmToFix()->setBeam(Trhythm::e_noBeam);
  qDeleteAll(m_stems);
  qDeleteAll(m_16_beams);
  delete m_8_beam;
  qDebug() << "[BEAM] deleted";
}


void TscoreBeam::addNote(TscoreNote* sn) {
  if (m_notes.isEmpty() && sn->newRhythm()->beam() != Trhythm::e_beamStart) // TODO: delete it when no errors
    qDebug() << "[BEAM] new beam starts but not proper flag is set!";
  else if (!m_notes.isEmpty() && m_notes.last()->newRhythm()->beam() == Trhythm::e_beamEnd)
    qDebug() << "[BEAM] Adding note to beam group that already ended!";

  if (m_notes.isEmpty())
    m_summaryPos = 0;
  m_summaryPos += sn->newNotePos();

  m_notes << sn;
//   if (sn->newRhythm()->beam() == Trhythm::e_beamEnd)
    connect(sn, &TscoreNote::noteWasClicked, this, &TscoreBeam::performBeaming);
  m_stems << new QGraphicsLineItem(sn);
  if (sn->newRhythm()->isRest()) // when there is a rest - stem is used to determine beam position over it
    m_stems.last()->hide();
  else
    m_stems.last()->setPen(QPen(sn->mainNote()->color(), 2 * HALF_STEM));

// 'main' beam of eights - always exists
  if (sn->newRhythm()->beam() == Trhythm::e_beamStart) { // resume grouping, prepare for painting
    m_8_beam = createBeam(sn);
  }

// beam of 16th - it may be chunked when rest or eight occurs
  if (sn->newRhythm()->rhythm() == Trhythm::e_sixteenth) {
    if (sn->newRhythm()->isRest()) {

    } else {
        if (m_16_beams.isEmpty() || m_notes.at(m_notes.count() - 2)->newRhythm()->rhythm() != Trhythm::e_sixteenth
           || (m_notes.at(m_notes.count() - 2)->newRhythm()->rhythm() == Trhythm::e_sixteenth && m_notes.at(m_notes.count() - 2)->newRhythm()->isRest())) {
        // is first in beam  or previous note was not a sixteenth or it was sixteenth but rest
              m_16_beams << new T16beam(m_notes.count() - 1, createBeam(sn)); // then create new beam segment
        } else if (!m_16_beams.isEmpty() && m_notes.at(m_notes.count() - 2)->newRhythm()->rhythm() == Trhythm::e_sixteenth
                   && !m_notes.at(m_notes.count() - 2)->newRhythm()->isRest()) {
        // there is 16 beam and previous note was 16th and not a rest
              m_16_beams.last()->endStem = m_notes.count() - 1; // then set end stem for it
        }
    }
  }

//   if (sn->newRhythm()->beam() == Trhythm::e_beamEnd) { // resume grouping, prepare for painting
//   }
}


void TscoreBeam::closeBeam() {
  m_notes.last()->newRhythmToFix()->setBeam(Trhythm::e_beamEnd);
  connect(m_measure, &TscoreMeasure::updateBeams, [=](TscoreNote* sn){
      if (!sn || (sn->index() >= m_notes.first()->index() && sn->index()))
          performBeaming();
  });
  if (m_notes.first()->newRhythm()->beam() == Trhythm::e_beamStart && m_notes.first()->newRhythm()->beam() == Trhythm::e_beamStart)
    qDebug() << "[BEAM] closed correctly with" << count() << "notes";
  else
    qDebug() << "[BEAM] is corrupted!!!!";
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

    /**
    * This method (slot) is invoked after TscoreNote is updated and placed apparently rhythm in measure (staff).
    * TscoreNote::noteWasClicked() signal invokes it actually
    *
    * With @p stemDirStrength (strength of stem direction) we trying to determine preferred common direction of stems in the group.
    * More far from middle of the staff (18) note is placed - stronger its direction has to be preserved
    * to keep beaming more natural and looking good
    * @p stemsDownPossible and @p stemsUpPossible keep true when there is enough space between note head and staff border
    * if there is not - common stem direction has to be adjusted for this fact
    */
void TscoreBeam::performBeaming() {
// find common stem direction for beaming
  int stemDirStrength = 0;
  bool stemsUpPossible = true, stemsDownPossible = true;
  for (TscoreNote* sn : m_notes) {
      stemDirStrength += sn->notePos() - 18;
      if (sn->notePos() < MIN_STEM_HEIGHT)
          stemsUpPossible = false;
      else if (sn->notePos() > sn->staff()->height() - MIN_STEM_HEIGHT)
          stemsDownPossible = false;
  }
  bool allStemsDown = false;
  if (stemDirStrength < 0)
    allStemsDown = true;

//   qreal averStemAdd = (qreal)m_summaryPos / count();
// setting stems directions and stem lines
  for (int i = 0; i < m_notes.size(); ++i) {
    if (m_notes[i]->rhythm()->stemDown() != allStemsDown) {
        Trhythm r(*m_notes[i]->rhythm());
        r.setStemDown(allStemsDown);
        m_notes[i]->setRhythm(r);
    }
  // set only first and last stems - the inner ones later - adjusted to leading beam line
//     if ( i == 0 || i == m_notes.count() - 1) {
      if (m_notes[i]->rhythm()->stemDown())
          m_stems[i]->setLine(m_notes[i]->mainNote()->x() + 0.13, m_notes[i]->mainNote()->y() + 1.14,
                              m_notes[i]->mainNote()->x() + 0.13, m_notes[i]->mainNote()->y() + 1.14 + STEM_HEIGHT);
      else
          m_stems[i]->setLine(m_notes[i]->mainNote()->x() + 2.23, m_notes[i]->mainNote()->y() + 0.85,
                              m_notes[i]->mainNote()->x() + 2.23, m_notes[i]->mainNote()->y() -(STEM_HEIGHT - 0.85));
//     }
  }

  QPointF stemOff(0.0, m_notes.last()->rhythm()->stemDown() ? HALF_STEM : -HALF_STEM); // offset to cover stem line thickness
  QLineF beamLine(m_notes.first()->pos() + m_stems.first()->line().p2() + stemOff, m_notes.last()->pos() + m_stems.last()->line().p2() + stemOff);
// adjust stem lines length of inner notes in the beam group to leading beam line
  for (int i = 1; i < m_stems.count() - 1; ++i) {
      QPointF stemEnd;
      beamLine.intersect(QLineF(m_notes[i]->pos() + m_stems[i]->line().p1(), m_notes[i]->pos() + m_stems[i]->line().p2()), &stemEnd);
      m_stems[i]->setLine(QLineF(m_stems[i]->line().p1(), stemEnd - m_notes[i]->pos() - 2 * stemOff));
  }
  QPolygonF poly;
  poly << beamLine.p1();
  poly << beamLine.p2();
      /** @p beamOff the lower point on a stem for bottom beam outline (it depends on beam line angel) */
  QPointF beamOff(0.0, (m_notes.last()->rhythm()->stemDown() ? -BEAM_THICK : BEAM_THICK) / qCos(qDegreesToRadians(beamLine.angle())));
  applyBeam(poly, beamOff, m_8_beam);

  if (!m_16_beams.isEmpty()) {
    QPointF beam16LineOff = beamOff * 1.5;
    beamLine.translate(beam16LineOff);
    for (T16beam* b16 : m_16_beams) {
        poly.clear();
        poly << m_notes[b16->startStem]->pos() + m_stems[b16->startStem]->line().p2() + stemOff + beam16LineOff;
        if (b16->isHalf()) { // 16th beam of fist stem is right-sided others are left-sided
            QPointF halfPoint;
            qreal halfX = poly.last().x() + BEAM_THICK * (b16->startStem == 0 ? 2 : -2);
            // intersection point with fake stem line for chunked beam
            beamLine.intersect(QLineF(halfX, poly.last().y(), halfX, poly.last().y() - 6.0), &halfPoint);
            poly << halfPoint;
        } else
            poly << m_notes[b16->endStem]->pos() + m_stems[b16->endStem]->line().p2()  + stemOff + beam16LineOff;
        applyBeam(poly, beamOff, b16->b);
    }
  }
  qDebug() << "performBeaming" << stemDirStrength << m_16_beams.count();
}

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TscoreBeam::applyBeam(QPolygonF& poly, const QPointF& offset, QGraphicsPolygonItem* polyItem) {
  poly << poly.last() + offset;
  poly << poly.first() + offset;
  poly << poly.first();
  polyItem->setPolygon(poly);
}


QGraphicsPolygonItem* TscoreBeam::createBeam(TscoreNote* sn) {
  QGraphicsPolygonItem* b = new QGraphicsPolygonItem(sn->staff());
  b->setPen(Qt::NoPen);
  b->setBrush(sn->mainNote()->color());
  b->setZValue(55);
  return b;
}



