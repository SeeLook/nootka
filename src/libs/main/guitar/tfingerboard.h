/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#ifndef TFINGERBOARD_H
#define TFINGERBOARD_H

#include <music/tnote.h>
#include <tfingerpos.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsitem.h>


class TcombinedAnim;
class TstrikedOutItem;
class TgraphicsTextTip;

#define   GUITAR      TfingerBoard::instance()

class TfingerBoard : public QGraphicsView
{

  friend class TguitarView;

  Q_OBJECT
public:
  explicit TfingerBoard(QWidget *parent = 0);
  virtual ~TfingerBoard();

  static TfingerBoard* instance() { return m_instance; }

  void acceptSettings();

      /** It paints fingerprint on the fretboard in place where note is.
      * If globals GshowOtherPos is true all possibilities are shown.
      * If @param realStr is set is shown only on pointed string (if any).
      * It returns true if something is shown.*/
  void setFinger(const Tnote& note);
  void setFinger(TfingerPos pos);

  TfingerPos getfingerPos() { return m_fingerPos; } /**< Returns position of currently selected finger */

  void askQuestion(TfingerPos pos);
  void clearFingerBoard();

      /** Creates rectangle (or two) indicates range of frets in exam.*/
  void createRangeBox(char loFret, char hiFret);
  void prepareAnswer();
  void deleteRangeBox();
  void setGuitarDisabled(bool disabled);

  void setHighlitedString(char realStrNr); /**< Highlights given string */
  void clearHighLight();

  int posX12fret(); /**< Returns x coordinate of 12th fret where guitar body starts. */

  QRect fbRect() { return m_fbRect; } /**< Guitar fingerboard rectangle */
  QRect* pickRect() { return m_pickRect; } /**< Rectangle of guitar pickup or empty if none */

      /** Sets Rectangle describing pickup image
        * x() has to point its x coordinate in TfingerBoard measures. */
  void setPickUpRect(const QRect& rect) {
    m_pickRect->setRect(rect.x(), rect.y(), rect.width(), rect.height()); }


      /** Returns width of a string */
  qreal stringWidth(int realStr) { return m_strWidth[qBound(1, realStr, 6)]; }

      /** Returns QLineF represents given string [1 - 6] */
  QLineF stringLine(int realStr) { return m_strings[qBound(0, realStr - 1, 5)]->line(); }

      /** Returns @param true when cursor is over the widget. */
  bool isCursorOverGuitar() { return m_isCursorOverGuitar; }

      /** Marks selected string or fret. Marking is cleaned when clearFingerBoard() is invoked. */
  void markAnswer(QColor blurColor);

      /** Highlights m_questString or m_questFinger after answering. */
  void markQuestion(QColor blurColor);
  void correctPosition(TfingerPos &pos, const QColor color);

      /** Displays name of note at current position
      * When note is empty the method tries to grab note (position) from that what was selected. */
  void showName(Tnote::EnameStyle st, const Tnote& note, const QColor& textColor);
  void showName(Tnote::EnameStyle st, const QColor& textColor) { showName(st, Tnote(), textColor); }
  void deleteNoteName();

  QPointF fretToPos(TfingerPos &pos); /**< Returns scene coordinates of given guitar position (between bars) */
  int fretPositionX(int fretNr); /**< X absolute coordinate of given fret or end of fingerboard. */
  short averFretWidth() { return m_fretWidth; }
  QRectF fingerRect() const; /**< Average width of a fret */

  TfingerPos pointToFinger(const QPoint& point); /**< Returns fret/string position form given position (view coordinates) */

  bool guitarEnabled() { return !m_isDisabled; } /**< @p TRUE when guitar accepts mouse/touch */

  bool isRightHanded();
  QColor& fingerColor();
  QColor& selectedColor();
  int guitarTypeId(); /**< id of current guitar type */

signals:
  void guitarClicked(const Tnote&);
  void correctingFinished(); /**< Emitted when correction animation finish */
  void settingsUpdated(); /**< Emmited when settings where changed */
  void enabilityChanged(bool); /**< When guitar goes through disable/enable states this is sent. */


protected:
  void resizeEvent(QResizeEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mousePressEvent(QMouseEvent *event);
  bool event(QEvent *event);

  void paint();
  Tnote posToNote(int str, int fret);

      /** Determines string width by its note pitch. Sets loNote & hiNote */
  void setTune();
  
  void fakePress(const QPoint& viewPos); /**< Imitates mouse press, available only for friendly classes  */

  void deleteBeyondTip();

private:
  QRect 			m_fbRect; /**< Represents top left positions and size of a fingerboard */
  short 			m_strGap; /**< Distance between strings */
  short 			m_fretWidth; /**< Average width of fret */
  short 			lastFret; /**< Position of the last fret (in whole widget coordinates) */
  short 			m_curStr, m_curFret; /**< Actual position of cursor over the guitar in strings/frets coordinates */
  short 			m_loNote, m_hiNote; /**< Chromatic numbers of lowest note in tune and highest. */
  TfingerPos 	m_fingerPos; /**< It keeps position of selected fingerprint.*/
  short 			m_fretsPos[24]; /**< @param fretsPos stores X positions of frets in global widget coordinates */
  qreal 			m_strWidth[6]; /**< Array of width each string. Width depends on fretboard height. */
  qreal 			m_widthFromPitch[6]; /**< Base values from which m_strWidth is calculated determined from tune. */
  QColor 			m_strColors[6];

  QGraphicsScene 						*m_scene;
  QGraphicsPixmapItem       *m_bgPix;
  QGraphicsEllipseItem 			*m_workFinger, *m_fingers[6], *m_questFinger;
  int 											 m_strNr, m_fretNr;
  QGraphicsLineItem 				*m_workStrings[6], *m_strings[6], *m_questString, *m_highString;
  QGraphicsSimpleTextItem 	*m_questMark;
  TgraphicsTextTip 					*m_beyondTip; /**< Tip about a note is impossible to show with current tune. */
  TgraphicsTextTip 					*m_noteName; /**< Note name text. */
  Tnote::EnameStyle					 m_corrStyle; /**< Name style of corrected note */
  bool											 m_nameInCorrection;
  Tnote 										 m_selNote; /**< Keeps selected note */

      /** Position from a question - is needed to calculate size of questioned finger
      * or string if naughty user changes window size. */
  TfingerPos 								 m_questPos;
  char 											 m_loFret, m_hiFret; /**< Frets range in an exam*/
  QGraphicsRectItem 				*m_rangeBox1, *m_rangeBox2;
  bool 											 m_isDisabled;
  int 											 m_hilightedStrNr;
  bool 											 m_isCursorOverGuitar;
  TfingerPos 								 m_goodPos;
  TstrikedOutItem 					*m_strikeOut;
  TcombinedAnim							*m_animation;
  QGraphicsItem 						*m_movingItem; /**< string line during animation */
  QRect											*m_pickRect; /**< Rectangle of guitar pickup or empty if none */
  static TfingerBoard       *m_instance;

private:
  void paintFinger(QGraphicsEllipseItem *f, char strNr, char fretNr);
  void paintQuestMark();
  void resizeRangeBox();
  void paintFingerAtPoint(QPoint p);

private slots:
  void strikeBlinkingFinished();
  void finishCorrection();


};

#endif // TFINGERBOARD_H
