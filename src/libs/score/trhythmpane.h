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

#ifndef TRHYTHMPANE_H
#define TRHYTHMPANE_H


#include <nootkacoreglobal.h>
#include "tscoreitem.h"


class TpaneItem;
class Trhythm;
class TscoreNote;


/**
 * Side pane to manage rhythm values of note
 */
class NOOTKACORE_EXPORT TrhythmPane : public TscoreItem
{

  Q_OBJECT

public:
  TrhythmPane(TscoreStaff* staff, TscoreScene* scene);
  virtual ~TrhythmPane();

      /**
       * Returns currently selected rhythm 
       */
  Trhythm* rhythm() const { return m_rhythm; }
  void setRhythm(Trhythm* r);

  TscoreNote* currentNote() { return m_currentNote; }
  void setCurrentNote(TscoreNote* sn) { m_currentNote = sn; }

  virtual QRectF boundingRect() const;
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

signals:
  void rhythmChanged();

protected:
  void itemClicked(); /**< Slot connected with all items */

  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

private:
  TpaneItem* createPaneItem(int charNr, const QPointF& p); /**< Creates an item and places it on given @p p position. */
  void showHideDotTriplet();

private:
  Trhythm             *m_rhythm;
  TpaneItem           *m_selectedRhythmItem; /**< Keeps previously selected rhythm item (not dot nor tupplet) */
  TpaneItem           *m_whole, *m_half, *m_quarter, *m_eight, *m_sixteen;
  TpaneItem           *m_wholeRest, *m_halfRest, *m_quarterRest, *m_eightRest, *m_sixteenRest;
  TpaneItem           *m_dot, *m_triplet, *m_tie;
  QList<TpaneItem*>    m_notes, m_rests; // lists with pointers
  QTimer              *m_hideTimer;
  TscoreNote          *m_currentNote = nullptr;
};

#endif // TRHYTHMPANE_H
