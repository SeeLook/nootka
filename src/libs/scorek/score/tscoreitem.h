/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#ifndef TSCOREITEM_H
#define TSCOREITEM_H

#include <nootkacoreglobal.h>
#include <QGraphicsObject>
#include <QEvent>


#define KEY_WIDTH  (9.0) // key signature width (and scordature)
#define CLEF_WIDTH (6.0) // how many space in width takes key on the score

class QTouchEvent;
class QGraphicsSceneHoverEvent;
class TscoreStaff;
class TscoreScene;

/**
* TscoreItem is base class for all items on the score:
* staff, clef, key signature, notes, scordature etc..
* It automatically adds created item to the TscoreScene
* given as constructor parameter.
* Also this class manages status tips.
* If the status is set, it emits statusTip(QString) signal.
*/
class NOOTKACORE_EXPORT TscoreItem : public QGraphicsObject
{
  Q_OBJECT

public:

      /** Static switch, common for all score items, determining touch events availability. */
  static bool touchEnabled();

  TscoreItem(TscoreScene *scene);

  enum { ScoreItemType = UserType + 1 };

  QString statusTip() { return m_statusTip; }
  void setStatusTip(QString status);
  TscoreScene* scoreScene() { return m_scene; }

    /** Returns pointer to TscoreStaff.
      * Most of the TscoreItem-s are tied with it,
      * but by default is set to 0. */
  TscoreStaff* staff() { return m_staff; }
  void setStaff(TscoreStaff *staff) { m_staff = staff; }

      /** Keeps true when mouse pointer is over item or finger is touching it. */
  bool hasCursor() { return m_hasCursor; }

      /** Finger touched this score item
        * Be aware, during overriding this methods,
        * if you want to have working @p hasCursor() functionality
        * always call @p TscoreItem::touched() */
  virtual void touched(const QPointF &scenePos);

      /** Finger leaved item or it was taken out
        * Be aware, during overriding this methods,
        * if you want to have working @p hasCursor() functionality
        * always call @p TscoreItem::untouched() */
  virtual void untouched(const QPointF &scenePos);
  virtual void touchMove(const QPointF &scenePos) {} /** Finger Moved */

  virtual int type() const { return ScoreItemType; }

signals:
  void statusTip(QString);

protected:
    /** If status tip is set it sends signal.
      * Notice!!
      * Any subclass has to call @p TscoreItem::hoverEnterEvent(event) when hoverEnterEvent is overridden
      * and statusTip functionality is expected.
      * Be aware, during overriding this methods,
      * if you want to have working @p hasCursor() functionality
      * always call @p TscoreItem::hoverEnterEvent() and @p TscoreItem::hoverLeaveEvent() */
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    /** Adds QGraphicsItem to current scene and
      * sets the ownership of this object on it.
      * It is for QGraphicsItem-s only !!!! */
  void registryItem(QGraphicsItem *item);

      /** Paints background rectangle using boundingRect() with given color
        * adding semi-transparency and gradient. */
  void paintBackground(QPainter *painter, QColor bgColor);

private:
  QString                    m_statusTip;
  TscoreScene               *m_scene;
  TscoreStaff               *m_staff;
  bool                       m_hasCursor;

};

#endif // TSCOREITEM_H
