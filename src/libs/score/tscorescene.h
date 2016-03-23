/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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

#ifndef TSCORESCENE_H
#define TSCORESCENE_H

#include <nootkacoreglobal.h>
#include "tscorenote.h"
#include "tnoteitem.h"
#include "tscorelines.h"
#include <QtWidgets/qgraphicsscene.h>


class QTimer;
class TnoteControl;
class TrhythmPane;
class QGraphicsItem;
class TscoreMeter;
class Tmeter;


/**
 * This is subclass of QGraphicsScene that handles additional operations of Nootka score.
 *
 * It manages a 'note cursor' - note shape under mouse and 'note controllers' - panes on the sides of a note.
 * Three methods are responsible for smart displaying cursor/panes:
 * - @p noteMoved() - invokes timer to display cursor when mouse stays over a note segment 
 * and timer to hide it when cursor stops over the segment
 * - @p noteLeaved() - invokes timer to hide cursor when mouse leaves the segment
 * - @p controlEntered() - to stop timers and keep cursor and pane visible
 * - @p controlLeaved() - to hide (the same as @p noteLeaved())
 */
class NOOTKACORE_EXPORT TscoreScene : public QGraphicsScene
{

  friend class TscoreNote;
  friend class TscoreStaff;

  Q_OBJECT

public:
  TscoreScene(QObject* parent = 0);
  virtual ~TscoreScene();


  void setDoubleAccidsEnabled(bool enable);
      /** Returns value 2 when double accidentals are enabled and 1 if not. */
  qint8 doubleAccidsFuse() { return m_dblAccFuse; }

  void setCurrentAccid(char accid); /**< Working accidental in  also changed by buttons. */
  char currentAccid() { return m_currentAccid; }

  Trhythm* workRhythm() { return m_workRhythm; }

      /** Adds blur graphics effect. In the contrary to QGraphicsItem::setGraphicsEffect()
        * a radius value in global scale.  */
  void addBlur(QGraphicsItem *item, qreal radius);

      /** Adjust note cursor and TnoteControl to new staff size.
        * For performance reason it has to be called once for all adjustSize() of TscoreNote
        * because there is only one instance of note cursor and TnoteControl */
  void adjustCursor(TscoreNote* sn);
  bool isCursorVisible() { return m_workNote->isVisible(); }

      /** Note controllers, appear with cursor.
        * There are automatically created with first note instance
        * when score scene has a view. */
  TnoteControl* right() { return m_rightBox; }
  TnoteControl* left() { return m_leftBox; }
  void setNameColor(const QColor& nameC) { m_nameColor = nameC; }
  QColor nameColor() { return m_nameColor; }

      /** Sets color of pointing (work) note. */
  void setPointedColor(const QColor& color);

  qreal accidYoffset() { return m_accidYoffset; } /**< Y offset of accidental item */
  qreal accidScale() { return m_accidScale; } /**< Scale of accidental text item */

  bool isAccidAnimated() const { return m_accidAnimated; } /**< Whether note accidental to key signature animations are allowed */
  void enableAccidsAnimation(bool anim) { m_accidAnimated = anim; } /**< Sets state of note accidental to key signature animations */

  bool isRhythmEnabled() { return (bool)m_scoreMeter; } /**< @p TRUE when score has rhythm enabled. */
  TscoreMeter* scoreMeter() { return m_scoreMeter; } /**< Score meter - if it is @p nullptr - there is no rhythms */

      /** Additional note controls are displayed when note gets cursor.
        This is default behavior and without those controls accidentals can not be managed with wheel. */
  void setControlledNotes(bool controlled) { m_controlledNotes = controlled; }
  bool controlledNotes() { return m_controlledNotes; }

  void noteEntered(TscoreNote* sn); /**< Prepares note cursor. From @p TscoreNote::hoverEnterEvent() */
  void noteMoved(TscoreNote* sn, int yPos); /**< Starts show timer if hidden or restarts hide timer. From @p TscoreNote::hoverMoveEvent() */
  void noteLeaved(TscoreNote* sn); /**< Starts hide timer. From @p TscoreNote::hoverLeaveEvent() */
  void noteDeleted(TscoreNote* sn); /**< From @p TscoreNote::~TscoreNote() */
  void controlMoved(); /**< Restarts hide timer. From @p TnoteControl::hoverMoveEvent()*/
  void controlLeaved(TscoreNote* sn) { noteLeaved(sn); } /**< From @p TnoteControl::hoverLeaveEvent */
  TscoreNote* currentNote() { return m_scoreNote; }

  void mouseEntersOnKey(bool onKey) { m_mouseOverKey = onKey; } /**< score key informs that has or has not a mouse cursor */
  bool keyHasMouse() { return m_mouseOverKey; }

  void prepareToChangeRect(); /**< It has to be invoked whenever score rectangle is going to change. I.e. by resize event of a view. */
  void restoreAfterRectChange(); /**< Scene will try to adjust itself to new size.  */

signals:
  void statusTip(QString);

protected:
// note cursor
  QColor  workColor;
  int workPosY() { return m_workPosY; }
  void setWorkPosY(int wpY) { m_workPosY = wpY; }
  TnoteItem* workNote() { return m_workNote; }
  TscoreLines* workLines() { return m_workLines; }
  void initNoteCursor(TscoreNote* parentIt);
  QGraphicsSimpleTextItem* workAccid() { return m_workAccid; }
  void setAccidYoffset(qreal aYo) { m_accidYoffset = aYo; }
  void setAccidScale(qreal as) { m_accidScale = as; }
  void setCursorParent(TscoreNote* sn); /**< Sets parent of note cursor to this instance */

      /** Sets meter, enables rhythms if meter is valid or disables rhythm if it is @p nullptr.
       * This is global meter for all score items and it is managed through @class TscoreStaff */
  void setScoreMeter(TscoreMeter* m);


protected slots:
  void statusTipChanged(QString status) { emit statusTip(status); }
  void showTimeOut();
  void hideTimeOut();
  void showPanes();
  void hidePanes();
  void showRhythmPane();

private:
      /** It is @p 2 if double accidentals are enabled and @p 1 if not*/
  qint8 														m_dblAccFuse;
  char  														m_currentAccid;
// note cursor
  int                            		m_workPosY;
  TnoteItem                     	 *m_workNote;
  QGraphicsSimpleTextItem       	 *m_workAccid;
  TscoreLines											 *m_workLines;
  Trhythm                          *m_workRhythm;
  QColor														m_nameColor;
  TnoteControl				  				 	 *m_rightBox, *m_leftBox;
  TrhythmPane                      *m_rhythmBox;
  qreal 									 					m_accidYoffset; /**< difference between y note position. */
  qreal									 						m_accidScale;
  QTimer													 *m_showTimer, *m_hideTimer;
  TscoreNote											 *m_scoreNote; /**< current note segment or NULL. */
  bool															m_controlledNotes;
  bool															m_mouseOverKey, m_rectIsChanging;
  bool                              m_accidAnimated;
  TscoreMeter                      *m_scoreMeter;
};

#endif // TSCORESCENE_H
