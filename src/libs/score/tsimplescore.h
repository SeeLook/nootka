/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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
 

#ifndef TSIMPLESCORE_H
#define TSIMPLESCORE_H

#include <nootkacoreglobal.h>
#include <music/tclef.h>
#include <music/tnote.h>
#include <music/tkeysignature.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qpointer.h>


class QTimer;
class TscoreItem;
class QGraphicsSimpleTextItem;
class QGraphicsView;
class TscoreStaff;
class TscoreScene;


/**
 * This class implements score.
 */
class NOOTKACORE_EXPORT TsimpleScore : public QGraphicsView
{
  Q_OBJECT
  
public:

  TsimpleScore(int notesNumber, QWidget *parent = 0);
  ~TsimpleScore();

  virtual void setNote(int index, const Tnote& note);
  Tnote getNote(int index); /** It returns @p note with hope that index points existed Tnote element. */
  void clearNote(int index); /** It hides pointed note and sets note to Tnote(0,0,0)*/
  void setStringNumber(int index, int realNr); /** Adds string number @p realNr to note @p index.  */
  void clearStringNumber(int index); /** Removes string number from note @p index. */
  void setNoteDisabled(int index, bool isDisabled); /** Disables/enables a note. */
  bool isNoteDisabled(int index);
  void setNoteNameEnabled(bool nameEnabled); /** Allows to call name edit from note controller */
  void setControllersEnabled(bool leftCtrl, bool rightCtrl); /** shows/hides a note controllers. */

  void setClef(Tclef clef);
  Tclef clef();
  void setClefDisabled(bool isDisabled);

  TscoreScene* scoreScene() { return m_scene; }

      /** This method returns the key signature if score has got TscoreKeySign item,
        * key can be:
        * @li 0 - C-maj (a-min)
        * @li 1 - G-maj and so on up to 7 (for Cis-maj)
        * @li -1 - F-maj (d-min) and so on down to -7 (for Ges-maj)
        * When TscoreKeySign doesn't exist 0: - a-min/C-maj is returned. */
  TkeySignature keySignature();
  void setKeySignature(TkeySignature keySign);
  virtual void setEnableKeySign(bool isEnabled);
  bool isKeySignEnabled();

  void setEnabledDblAccid(bool isEnabled);

  bool isPianoStaff();

      /** Sets background color. Alpha value will be overridden. */
  void setBGcolor(QColor bgColor);

      /** It disables accids buttons and locks editing,
        * but doesn't make score gray like standard setDisabled() method. */
  void setScoreDisabled(bool disabled);

      /** Sets the lowest and the highest note in all note segments on the score .*/
  void setAmbitus(Tnote lo, Tnote hi);

      /** Sets the lowest and the highest note for @p index of note segment .*/
  void setAmbitus(int index, Tnote lo, Tnote hi);

  Tnote lowestNote(); /** Returns lowest possible note on the staff in current clef */
  Tnote highestNote(); /** Returns highest possible note on the staff in current clef */
  void addBGglyph(int instr); /** Adds background with glyph identified  kind of instrument. */

  virtual QSize sizeHint() const;

  void setMaxKeySign(int maxK);
  void setMinKeySign(int minK);

signals:
      /** As long as QGraphicsScene items haven't got status tips TscoreItems has its own mechanism of tips.
        * This signal is emitted when any TscoreScene element gets hoverEnterEvent
        * with status tip to display. */
  void statusTip(QString);
  void noteWasChanged(int index, Tnote note);

      /** TsimpleScore takes care about changing staves but also emits this signal when changes are done.*/
  void clefChanged(Tclef);

      /** Emitted when score detects pinch touch gesture.
       * Emitted value is zoom factor:
       * - positive (> 0): zoom out
       * - negative (< 0): zoom in
       * Signal is emitted once per touch (m_pinchZoomEmitted is responsible for it) */
  void pinchZoom(int);


public slots:
  virtual void noteWasClicked(int index);

protected:
  bool viewportEvent(QEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

      /** To make usage of touch pads wheel emulation which changes key/accidentals to fast by default,
        * a @p m_wheelFree locks a wheel event for e while to avoid switching keys to fast. */
  void wheelLockSlot();

  TscoreStaff* staff() { return m_staff; } /** Pointer to TscoreStaff. Better keep it protected */

      /** Returns previously set clef. It is used to figure is it scaling of score necessary.  */
  Tclef::Etype clefType() { return m_clefType; }
  void setSizeHint(const QSize& s) { if (s != m_sizeHint) { m_sizeHint = s; updateGeometry(); } }
  virtual QSize minimumSizeHint() const;

      /** Checks is item @it of type @p TscoreItem::ScoreItemType.
      * If not, checks it parent item and parent of parent.
      * Returns pointer to @p TscoreItem or 0 if not found. */
  TscoreItem* castItem(QGraphicsItem* it);

protected slots:
      /** Except response for scaling TscoreView widget to according to new height,
        * this method takes care about new width of the score.
        * It is necessary to call it after staff width changed f.e by:
        * setPianoStaff(), setEnableKeySign() and setScordature()				 */
  virtual void resizeEvent(QResizeEvent* event);

      /** It grabs TscoreItems statusTips and generates QStatusTipEvent for parent widget. */
  void statusTipChanged(QString status);
  void onClefChanged(Tclef clef);

private:
  TscoreScene     						*m_scene;
  TscoreStaff     						*m_staff;
  QGraphicsSimpleTextItem 		*m_bgGlyph;
  int 												 m_notesNr;
  int 												 m_prevBGglyph;
  Tclef::Etype								 m_clefType;
  QSize												 m_sizeHint;
  QPointer<TscoreItem>				 m_currentIt;
  QPointF											 m_initPos; /**< In scene coordinates. */
  bool                         m_wheelFree;
  QTimer                      *m_wheelLockTimer;
  bool                         m_pinchZoomEmitted;
};

#endif // TSIMPLESCORE_H
