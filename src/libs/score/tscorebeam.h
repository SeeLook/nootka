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

#ifndef TSCOREBEAM_H
#define TSCOREBEAM_H


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


class TscoreNote;
class QGraphicsLineItem;
class QGraphicsPolygonItem;
class TscoreMeasure;
class TscoreStaff;
class T16beam;


/**
 * This class manages displaying beams of note rhythmic groups
 * also it paints stems of them
 */
class NOOTKACORE_EXPORT TscoreBeam : public QObject
{

  friend class TscoreMeasure;
  friend class TscoreStaff;
  friend class TscoreNote;

  Q_OBJECT

public:
  explicit TscoreBeam(TscoreNote* sn, TscoreMeasure* m);
  virtual ~TscoreBeam();

      /** Adds @p TscoreNote to beam group.
       * note has to have properly set beam state in TscoreNote::newRhythm()
       * It changes stem direction of note(s) when necessary.
       * It does not perform painting yet
       */
  void addNote(TscoreNote* sn);

      /** Returns note @p id in beam group */
  TscoreNote* note(int id) { return m_notes[id]; }

      /** First note in the beam group */
  TscoreNote* first() { return m_notes.first(); }

      /** Last note in the beam group */
  TscoreNote* last() { return m_notes.last(); }

      /** Number of notes in the beam group */
  int count() { return m_notes.count(); }
  
      /** Sets beam flag of the last note to 'beam end'  */
  void closeBeam();

protected:
      /** calls @p performBeaming when note belongs to beam group */
  void beamsUpdateSlot(TscoreNote* sn);
  void performBeaming();

      /**
       * Because beams are parented with staff it is important
       * to change their staff when measure is shifted between staves
       */
  void changeStaff(TscoreStaff* st);

private:
      /**
       * Given polygon has to have two points (upper beam boundary line).
       * This method adds bottom boundary, closes polygon
       * and applying it into @c QGraphicsPolygonItem. 
       */
  void applyBeam(QPolygonF& poly, const QPointF& offset, QGraphicsPolygonItem* polyItem);

      /** Initial routines of beam (polygon) - color of note, staff as the parent  */
  QGraphicsPolygonItem* createBeam(TscoreNote* sn);

private:
  TscoreMeasure                    *m_measure;
  QList<TscoreNote*>                m_notes;
  QList<QGraphicsLineItem*>         m_stems;
  QGraphicsPolygonItem             *m_8_beam; /**< line of main beam of eight */
  QList<T16beam*>                   m_16_beams; /**< list of lines of sixteenths */
  int                               m_summaryPos;
};

#endif // TSCOREBEAM_H
