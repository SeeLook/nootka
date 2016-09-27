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

#ifndef TSCORENOTE_H
#define TSCORENOTE_H

#include <nootkacoreglobal.h>
#include "tscoreitem.h"
#include <QtCore/qelapsedtimer.h>


class TscoreLines;
class Tnote;
class Trhythm;
class TnoteControl;
class TcombinedAnim;
class TcrossFadeTextAnim;
class TscoreControl;
class TscoreScene;
class TnoteItem;
class TscoreBeam;
class TscoreTie;


/*!
 * This class represents single note on a score.
 * It is a rectangle area over the staff with handling mouse move event to display working note cursor.
 * It also grabs wheel event to manipulate accidentals
 * It can be set to read-only mode through setReadOnly() then mouse events are ignored.
 * When mouse cursor appears over it, @p TscoreNote starts to be a parent for @p TscoreControl
 * and it displays panes on the note sides with additional switches (accidentals, name menu, add/delete note)
 */
class NOOTKACORE_EXPORT TscoreNote : public TscoreItem
{

  friend class TscoreMeasure;
  friend class TscoreBeam;
  friend class TscoreTie;

  Q_OBJECT

public:
  TscoreNote(TscoreScene *scene, TscoreStaff *staff, int index);
  ~TscoreNote();

      /** Index of this note instance. It is connected with note number in the list */
  int index() { return m_index; }
  void changeIndex(int newIndex) { m_index = newIndex; }

  void setReadOnly(bool ro);
  bool isReadOnly() { return m_readOnly; }

  void adjustSize(); /**< Grabs height from staff and adjust to it. */

      /**
       * Sets color of main note. 
       */
  void setColor(const QColor& color);
  QColor color() const { return m_mainColor; }

      /** It sets background of the note segment. When sets to -1 means transparent - no background. */
  void setBackgroundColor(QColor bg) { m_bgColor = bg; update(); }

      /** Adds blur effect to main note. If blurColor is -1 deletes the effect. */
  void markNote(QColor blurColor);

  void moveNote(int posY);
  void moveWorkNote(const QPointF& newPos);

      /** Returns true if note was selected */
  bool isSelected() const { return m_selected; }
  void selectNote(bool sel);

      /** Points to Tnote instance.
        * TscoreNote is not aware of it. It is just container.
        * This value has to be set through setNote() */
  Tnote* note() { return m_note; }

      /** Sets note-head at given position and given accidental accidental. Also puts Tnote of it. */
  void setNote(int notePos, int accNr, const Tnote& n);

  Trhythm* rhythm() const; /**< Returns Trhythm of note */
  void setRhythm(const Trhythm& r); /**< Sets rhythm of note */

  void setRhythmEnabled(bool rhythmEnabled); /**< Switches rhythm on/off */

      /** Returns pointer to main note @p TnoteItem.  */
  TnoteItem* mainNote() { return m_mainNote; }

      /** A line representing note stem */
  QGraphicsLineItem* stem();

      /** Returns pointer to main accidental QGraphicsSimpleTextItem.  */
  QGraphicsSimpleTextItem *mainAccid() { return m_mainAccid; }

      /** Min and Max values of Y coefficient on the staff */
  void setAmbitus(int lo, int hi);

      /** This return value of -2 is bb,  1 is #,  etc... */
  qint8 accidental() const {return m_accidental;}

      /** NOTE: for this moment it is unused and set to 0 */
  qint8 ottava() const { return m_ottava; }

      /** Y Position of note head */
  qint16 notePos() const { return m_mainPosY; }

      /** Position of note after click but before head was moved */
  qint16 newNotePos() const { return m_newPosY; }

      /** Accidental going to be set. */
  qint8 newAccidental() const { return m_newAccid; }

  bool accidChanged() const { return (bool)m_newAccid != (bool)m_accidental; } /**< @p TRUE only when accidental appears or hides */
  bool pitchChanged() const { return m_newPosY != m_mainPosY; }
  bool rhythmChanged() const;

  static QString getAccid(int accNr); /**< Returns QString with accidental symbol*/

      /**
       * Pointer to the note after this segment or the first one in the next staff
       * or null if no note.
       */
  TscoreNote* nextNote();

      /**
       * Pointer to the note before this segment or the last one in the previous staff
       * or null if no note.
       */
  TscoreNote* prevNote();

      /**
       * Returns space factor after note with given @p r rhythm.
       */
  static qreal space(const Trhythm& r);

      /**
       * Space factor after this note
       */
  qreal space();

      /** It paints string number symbol. Automatically determines above or below staff. */
  void setString(int realNr);
  void removeString(); /**< Removes string number */
  int stringNumber() const { return m_stringNr; } /**< Number of displayed string or 0 if none. */

      /** Starts displaying note name of this note.
        * Name will change appropriate to moved note until removeNoteName() will be invoked.
        * If no color is set the default one defined in TscoreScene will be used. */
  void showNoteName(const QColor& dropShadowColor = -1);
  void removeNoteName();
  QGraphicsTextItem* noteName() { return m_nameText; } /**< Graphics item of note name text */

      /** Enables moving note animation during its position (pitch) change.
        * In fact, when accidental is visible it is animated as well. */
  void enableNoteAnim(bool enable, int duration = 150);
  bool isNoteAnimEnabled() const { return (bool)m_noteAnim; }

  void popUpAnim(int durTime); /**< Performs pop-up animation */

      /** Defines when lines above and below staff are visible when note is empty. */
  void setEmptyLinesVisible(bool visible) { m_emptyLinesVisible = visible; }
  bool emptyLinesVisible() const { return m_emptyLinesVisible; }
  bool wasTouched() const { return m_wasTouched; } /**< @p TRUE during touch only */

      /** Overrides standard @p QGraphicsItem::update() method,
       * checks are rhythms (@p note()->rtm and @p workNote->rhythm() the same) */
  void update();

      /** Number of rhythmical group in the measure, -1 (undefined) by default */
  qint8 rhythmGroup() { return m_group; }
  void setRhythmGroup(qint8 g) { m_group = g; }

      /**
       * Pointer to @p TscoreBeam or null if note has no beam 
       */
  TscoreBeam* beam() { return m_beam; }
  void setBeam(TscoreBeam* b) { m_beam = b; }

      /**
       * Pointer to @p TscoreTie (ligature) or null if note has no tie 
       */
  TscoreTie* tie() { return m_tie; }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
  virtual QRectF boundingRect() const { return QRectF(0.0, 0.0, m_width, m_height); }
  qreal width() const { return m_width; }
  qreal height() const { return m_height; }
  qreal rightX(); /**< shortcut to X coordinate of right note corner plus gap related to rhythm and staff gap factor */
  qreal estimateWidth(const Tnote& n); /**< Estimate width of the given note @p n */

  void setX(qreal x);
  void setPos(const QPointF &pos);
  void setPos(qreal ax, qreal ay) { setPos(QPointF(ax, ay)); }

signals:
  void noteWasClicked(int);
  void noteWasSelected(int); /**< When right button was clicked. */

  void roNoteClicked(TscoreNote*, const QPointF&); /**< Emitted after mouse left click in read only state with clicked position. */
  void roNoteSelected(TscoreNote*, const QPointF&); /**< Emitted after mouse right click or double click in read only state */

  void noteGoingToChange(TscoreNote*); /**< Emitted when note was pressed or @p setNote() was invoked */

public slots:
  void keyAnimFinished();
  void hideNote(); /**< Hides main note */
  void hideWorkNote(); /**< Hides pointing (work) note */

protected:
  virtual void touched(const QPointF& scenePos);
  virtual void untouched(const QPointF& scenePos);
  virtual void touchMove(const QPointF& scenePos);

  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

      /** Sets @p TscoreTie */
  void setTie(TscoreTie* t);

private:
  TnoteItem                               *m_mainNote;
  QGraphicsSimpleTextItem                 *m_mainAccid;
  QColor                                   m_mainColor;
  TcrossFadeTextAnim                      *m_accidAnim;
  Tnote                                   *m_note;

  qint16                                   m_mainPosY, m_newPosY;
  qint8                                    m_accidental, m_newAccid;
  int                                      m_index; /**< note index in external list */
//     int                           m_noteNr; // note number depends on octave
  quint16                                  m_ambitMin, m_ambitMax; /**< Represents range (ambitus) of notes on score */
  quint8                                   m_stringNr;
  QGraphicsSimpleTextItem                 *m_stringText;
  qreal                                    m_width, m_height;
  bool                                     m_readOnly, m_emptyLinesVisible;
  QGraphicsTextItem                       *m_nameText;
  qint8                                    m_ottava; /**< values from -2 (two octaves down), to 2 (two octaves up) */
  QColor                                   m_bgColor;
  TcombinedAnim                           *m_noteAnim, *m_popUpAnim;
  QGraphicsSimpleTextItem                 *m_emptyText;
  bool                                     m_selected;
  TscoreLines                             *m_lines;

  bool                                     m_touchedToMove; /**< Determines whether cursor follows moving finger */
  bool                                     m_wasTouched;
  static QString                           m_staticTip, m_selectedTip;
  QElapsedTimer                            m_touchTime;

  qint8                                    m_group = -1;
  TscoreBeam                              *m_beam = nullptr;
  TscoreTie                               *m_tie = nullptr;

  static const qreal                       m_rtmGapArray[5][3]; /**< static array with space definitions for each rhythm value */

private:
  void setStringPos(); /**< Determines and set string number position (above or below the staff) depends on note position */
  void initNoteCursor(); /**< Creates note cursor when first TscoreNote instance is created and there is a view */
  void checkEmptyText(); /**< Decides whether show or hide text about empty note. */
  void changeWidth(); /**< Changes bounding rectangle width appropriate to current accidental and rhythm */
  void emitNoteWasSelected();
  void removeTie(); /**< Checks and removes tie of this note or the previous one if exist */

private slots:
  void popUpAnimFinished();
};

#endif // TSCORENOTE_H
