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

#ifndef TSCOREMEASURE_H
#define TSCOREMEASURE_H

#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


class TscoreMeter;
class TscoreStaff;
class TscoreNote;
class TscoreBeam;
class QGraphicsLineItem;
class TmeasPatcher;

/**
 * Manages notes on the staff
 */
class NOOTKACORE_EXPORT TscoreMeasure : public QObject
{

  Q_OBJECT

public:
  explicit TscoreMeasure(TmeasPatcher* dispatcher);
  virtual ~TscoreMeasure();

  void updateMeter();

  void insertNote(int id, TscoreNote* sn);
  void removeNote(int noteToRemove);

  QList<TscoreNote*>& notes() { return m_notes; }

  void recalculate(TscoreNote* sn);

  int firstNoteId();
  int lastNoteId();

  int free(); /**< Free 'rhythm space' remained in the measure */

signals:
      /**< Emitted when measure was changed and re-beamed.
       * @class TscoreNote parameter is a note that invoked change (or 0 if new one was added)
       * so all other beam groups that don't contain this note has to be updated */
  void updateBeams(TscoreNote*);

protected:
  void calcDuration();
  void noteChangedSlot(TscoreNote* sn);

private:
  TmeasPatcher            *m_dispatcher;
  int                      m_duration;
  int                      m_free;
  QList<TscoreNote*>       m_notes;
  QList<TscoreBeam*>       m_beams;
  QGraphicsLineItem       *m_barLine;
};


/**
 * Measure dispatcher is a class to handle all changes in notes in the staves
 * (adding, deleting) and apply those changes to appropriate measure
 * (create it or delete or move between staves)
 */
class TmeasPatcher : public QObject
{

  Q_OBJECT

public:
  explicit TmeasPatcher(QObject* parent);
  virtual ~TmeasPatcher();

  void addNote(int id, TscoreNote* sn);
  void removeNote(int staffNr, int noteId);

  QList<TscoreMeasure*>* measures() { return &m_measures; }
  TscoreMeasure* measure(int mId) { return m_measures[mId]; }

  void setScoreMeter(TscoreMeter* sMeter);
  TscoreMeter* scoreMeter() { return m_scoreMeter; }

private:
  QList<TscoreMeasure*>             m_measures;
  TscoreMeter                      *m_scoreMeter;
};

#endif // TSCOREMEASURE_H
