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

#ifndef TTOUCHMENU_H
#define TTOUCHMENU_H


#include <QtWidgets/qmenu.h>


class QTimer;
class QVBoxLayout;

/**
 * Kind of @p QMenu but optimized for touch events (Android quirks)
 * and with built-in appear animation
 */
class TtouchMenu : public QMenu
{
  Q_OBJECT

public:
  TtouchMenu(QWidget *parent = 0);
  virtual ~TtouchMenu();

  static TtouchMenu* instance() { return m_instance; }

  int animDuration() { return m_animDuration; }

      /** Sets duration of moving menu animation, by default it is 400 ms.
       * When set to 0, animation is not performed. */
  void setAnimDuration(int dur) { m_animDuration = dur; }

      /** Adds given @p a QAction to menu. Performs some additional preparations. */
  void addAction(QAction* a);

      /** Overrides standard @p exec method of @p QMenu.
       * Menu is positioned at @p endPos point but when @p startPos is set,
       * an animation from it is performed */
  QAction* exec(const QPoint& endPos, const QPoint& startPos = QPoint(-1, -1));

protected:
  virtual void showEvent(QShowEvent* e); // correct initial position
  virtual bool event(QEvent *e);

protected slots:
  void animTimeOut();

private:
  int                   m_animDuration;
  QTimer               *m_animTimer;
  int                   m_step, m_count;
  QPoint                m_startPos, m_endPos, m_offset, m_touchStartPos;
  bool                  m_vertical, m_horizontal; // determine whether menu is moving vertical or horizontal (or both)
  bool                  m_swiped; /**< @p TRUE when menu is swiped by finger (movement larger than some tolerance) */
  static TtouchMenu    *m_instance;
};

#endif // TTOUCHMENU_H
