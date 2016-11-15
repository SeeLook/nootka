/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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

#ifndef TGUITARVIEW_H
#define TGUITARVIEW_H


#include <nootkacoreglobal.h>
#include <QtWidgets/qgraphicsview.h>


class QGraphicsProxyWidget;
class TfingerBoard;
class QTouchEvent;

/**
 * Another view of guitar fingerboard.
 * It displays chunk of fingerboard with just touched fret.
 * Automatically determines its scale (depends on real screen size).
 * Scale is determined in constructor but first position 
 * has to be initialized by @p displayAt() with touch coordinates as a parameter.
 * It support scrolling when it was touched and finger is moving (even out of view area).
 * It is difficult to implement, so parent widget which is @class TmainView has to cooperate.
 * First of all - every touch is propagated from parent through @p mapTouchEvent(QTouchEvent),
 * but when @class TmainView detects that @class TguitarView was touched it has to call:
 * @p TguitarView::setTouched().
 * Also it automatically scrolls its context 
 * to always displays current fret at the beginning of the view
 * and moves view position next to currently displayed fret.
 */
class NOOTKACORE_EXPORT TguitarView : public QGraphicsView
{

public:
      /** Creates preview, it is hidden by default and has to be shown through @p displayAt(QPointF) */
  TguitarView(QGraphicsView* guitar, QGraphicsView* parent);
  virtual ~TguitarView();

  QGraphicsProxyWidget* proxy() { return m_proxy; }

      /** Depend on available screen size @p TguitarView can display
       * zoomed preview of actually touched fret or just propagate touch to guitar.
       * This method checks this necessity and sets appropriate switches:
       * @p isPreview(), preview scale (if any), fret mark
       */
  bool checkIsPreview();

  bool mapTouchEvent(QTouchEvent* te); /** Propagates touch events into view or into guitar */

      /** Returns @p TRUE when widget was visible and touch was started over it. */
  bool touchStartedFromView() { return m_touchStartedHere; }
  void setTouched() { m_touchStartedHere = true; }
  bool isPreview() { return m_isPreview; } /** @p TRUE when guitar is too small and fret preview is displayed */
  bool guitarEnabled();

protected:
  void guitarAvailChanged(bool guitEnabled);
#if defined (Q_OS_ANDROID)
  void displayAt(const QPointF& scenePos); /** Shows view at given scene position. IN PREVIEW MODE ONLY! */
  void updateMarkPosition(); /** Fret mark position depends on current fret */
  void updateContextPosition(); /** Sets value of scroll bar at the beginning position of current fret. */
  void hideEvent(QHideEvent *event);
#endif

private:
  QGraphicsView               *m_parent;
  TfingerBoard                *m_guitar;
  QGraphicsProxyWidget        *m_proxy;
  QGraphicsRectItem           *m_mark;
  quint8                       m_fret;
  bool                         m_couldBeTouch;
  bool                         m_touchStartedHere; /** When touch event was started over this preview */
  bool                         m_isPreview;

};

#endif // TGUITARVIEW_H
