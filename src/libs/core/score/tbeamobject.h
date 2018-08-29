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

#ifndef TBEAMOBJECT_H
#define TBEAMOBJECT_H


#include <nootkacoreglobal.h>
#include <QtQuick/qquickpainteditem.h>


class TstaffItem;
class TmeasureObject;
class TnoteItem;
class TnotePair;
class T16beam;


/**
 * This class manages displaying beams of rhythmic group.
 * It paints either eight beam line and 16th beam line(s)
 * By @p addNote() a beam flag of @p TnotePair is set.
 * At that moment @p TnotePair::objeject() can be empty.
 * After note objects initialization @p prepareBeam() is called
 * where common stem direction of rhythm group is set and stems length.
 * When @p TnoteObject::setX() is called (note and its stem changes position)
 * @p drawBeam() is invoked to refresh beam size and call @p update() to repaint the beam if necessary.
 */
class NOOTKACORE_EXPORT TbeamObject : public QQuickPaintedItem
{

  Q_OBJECT

  friend class TscoreObject;
  friend class TstaffItem;
  friend class TmeasureObject;
  friend class TnoteItem;
  friend class TnotePair;

public:
  explicit TbeamObject(TnotePair* sn, TmeasureObject* m);
  ~TbeamObject() override;

      /**
       * Adds @p TnotePair to beam group
       * and according to adding order sets appropriate beam flag.
       * It changes stem direction of note(s) when necessary.
       * It does not perform painting yet
       */
  void addNote(TnotePair* np);

      /**
       * Returns note @p id in beam group
       */
  TnotePair* note(int id) { return m_notes[id]; }

  TnotePair* first() { return m_notes.first(); }
  TnotePair* last() { return m_notes.last(); }

      /**
       * Number of notes in the beam group
       */
  int count() { return m_notes.count(); }


  void paint(QPainter* painter) override;

protected:
  void prepareBeam();
  void drawBeam();

      /**
       * Because beams are parented with staff it is important
       * to change their staff when measure is shifted between staves
       */
  void changeStaff(TstaffItem* st);

      /**
       * Removes given note from the beam,
       * if necessary it creates a new beam in case this beam was split by rest.
       * Returns @p TRUE if this beam becomes broken (not enough notes) and has to be deleted
       */
  bool removeNote(TnotePair* np);

  void setMeasure(TmeasureObject* m);

  void deleteBeam();

private:
  void resetBeam(TnotePair* noteToRemove);

private:
  TmeasureObject                   *m_measure;
  QList<TnotePair*>                 m_notes;
  QList<T16beam>                    m_16beams; /**< list of lines of sixteenths */
};

#endif // TBEAMOBJECT_H
