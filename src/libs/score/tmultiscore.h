/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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


#ifndef TMULTISCORE_H
#define TMULTISCORE_H

#include "nootkacoreglobal.h"
#include "tsimplescore.h"
#include "tscorenote.h"
#include "tscorestaff.h"


class Tscore5lines;
class QMainWindow;
class TscoreStaff;
class Tmeter;


/**
 * This is subclass of @p TsimpleScore which adds support for multiple staves/systems
 */
class NOOTKACORE_EXPORT TmultiScore : public TsimpleScore
{
  Q_OBJECT

public:

  TmultiScore(QMainWindow* mw, QWidget *parent = 0);
  ~TmultiScore();

  QMainWindow* mainWindow() { return m_mainWindow; } /**< Nootka main window */

  enum EinMode {
    e_single, // single note mode
    e_multi, // adding notes and multiple staves
    e_record // calling setNote() from outside switches current index to the next note or adds one
  }; /**< Describes insert mode of a score */

  EinMode insertMode() { return m_inMode; }
  void setInsertMode(EinMode mode);

  int currentIndex() { return m_currentIndex; }

  TscoreStaff* staves(int id) { return m_staves[id]; } /**< Returns given staff */
  TscoreStaff* lastStaff() { return m_staves.last(); }
  TscoreStaff* firstStaff() { return m_staves.first(); }
  int staffCount() { return m_staves.size(); }

      /** Returns pointer to TscoreNote of given @p id on the score.
       * It doesn't check existence! */
  TscoreNote* noteFromId(int id) { return staves(id / staff()->maxNoteCount())->noteSegment(id % staff()->maxNoteCount()); }

  virtual void setNote(const Tnote& note); /**< Sets current note */
  virtual void setNote(int index, const Tnote& note); /**< Sets note of @p index to @p note */
     Tnote getNote(int index);

  virtual void setEnableKeySign(bool isEnabled);

  void setScoreDisabled(bool disabled);
  bool isScoreDisabled() { return m_isDisabled; }

  void setMeter(const Tmeter& m);
  Tmeter* meter(); /**< Returns @class Tmeter object or 0 if no rhythms. */

  qreal scoreScale() { return m_scale; } /**< Scale factor of user zoom. */
  void setScoreScale(qreal sc);

  bool ainmationsEnabled() { return m_useAinim; }
  void setAnimationsEnabled(bool aEnabled) { m_useAinim = aEnabled; }

  int notesCount(); /**< Total number of notes on the score */

  void selectNote(int id); /**< Marks given note or cancel selection if @p -1 */

      /** When @p TRUE notes on locked score can be selected by click
      * and @p lockedNoteClicked(Tnote) signal is emitted */
  void setReadOnlyReacting(bool doIt) { m_selectReadOnly = doIt; }
  bool readOnlyReacting() { return m_selectReadOnly; }

  char debug();

signals:
  void lockedNoteClicked(int noteNumber); /**< Emitted number is in range [0 to notesCount()] */
  void lockedNoteSelected(int noteNumber); /**< Emitted number is in range [0 to notesCount()] */

public slots:
  void noteWasClicked(int index);
  void noteWasSelected(int index);
  void onClefChanged(Tclef cl);
  void removeCurrentNote(); /**< Removes currently selected note, if any. */
  void deleteNotes(); /**< Deletes all notes from the score - single one remains */
  void ensureNoteIsVisible(); /**< Calls QGraphicsView::ensureVisible on current selected note */

protected:
  virtual void resizeEvent(QResizeEvent* event);

  int clickedOffset() { return m_clickedOff; }
  void updateSceneRect(); /**< Adjusts score scene to space required by staff(staves) */
  void changeCurrentIndex(int newIndex);
  TscoreStaff* currentStaff();
  void resetClickedOff() { m_clickedOff = 0; }
  void checkAndAddNote(TscoreStaff* sendStaff, int noteIndex);
  void connectForReadOnly(TscoreNote* sn); /**< Connects given TscoreNote to appropriate slots. */

  void deleteFakeLines(int lastNr); /**< Deletes last @p lastNr lines from the fake list. */

      /**
       * Calculates score view scale factor with given user scale @p sc
       * and minimal score height @p minH 
       */
  qreal getScaleFactor(int minH, qreal sc);

protected slots:
  void keyChangedSlot();

  void staffHasNoSpace(int staffNr); /**< Create new (next) staff */
  void staffHasFreeSpace(int staffNr, int notesFree); /**< Move notes to this staff from the next one */
  void noteGetsFree(int staffNr, TscoreNote* freeNote);
  void noteRemovingSlot(int staffNr, int noteToDel);
  void noteAddingSlot(int staffNr, int noteToDel);
  void staffLoNoteChanged(int staffNr, qreal loNoteYoff);
  void staffHiNoteChanged(int staffNr, qreal hiNoteYoff);
  void adjustStaffWidth(TscoreStaff *st); /**< Calls TscoreStaff::setViewWidth with score width  */

      /** Staff @p st wants to take out its last measure number @p measureNr.
       * This method (slot) moves it to the next staff (creates a new staff if necessary). */
  void moveMeasureSlot(TscoreStaff* st, int measureNr);

      /**
       * Checks is next staff to @p st available
       * and puts pointer of it to @p st or null if not
       */
  void giveNextStaffSlot(TscoreStaff*& st);

      /**
       * Returns pointer to previous staff.
       * Better check is the first staff calling it
       */
  void givePrevStaffSlot(TscoreStaff*& st);

      /** Adds given staff at the end of m_staves list or creates a new one.
      * Sets staff number corresponding to its index in the m_staves list,
      * connects the staff with TmainScore slots */
  virtual void addStaff(TscoreStaff* st = 0);
  void deleteLastStaff();
  void emptyStaffSlot();

  void roClickedSlot(TscoreNote* sn, const QPointF& clickPos);
  void roSelectedSlot(TscoreNote* sn, const QPointF& clickPos);

private:
  void addNewStaff();

private:
  QMainWindow                  *m_mainWindow;
  EinMode                       m_inMode;
  qreal                         m_scale;

      /** m_clickedOff indicates whether setNote() is set to m_currentIndex and whether to the next after current */
  int                           m_clickedOff, m_currentIndex;
  bool                          m_useAinim, m_addNoteAnim;
  bool                          m_selectReadOnly, m_isDisabled;

  QList<TscoreStaff*>           m_staves; // list of staves in page view
  QList<Tscore5lines*>          m_fakeLines; // list of blurred staves displayed when score scene has much space
};

#endif // TMULTISCORE_H
