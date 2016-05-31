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

#ifndef TMENUWIDGET_H
#define TMENUWIDGET_H


#include <QtWidgets/qwidget.h>


/**
 * Widget with three dots aka Android menu.
 * It paints itself and emits @p clicked() signal.
 * It can be animated by @p animate() method
 */
class TmenuWidget : public QWidget
{

  Q_OBJECT

public:
  TmenuWidget(QWidget* parent = 0);

public slots:
    /** Performs animation to mark the widget:
     * every dot blinks once */
  void animate();

signals:
  void clicked();

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);

protected slots:
  void animSlot();

private:
  bool            m_touched;
  int             m_marked; /**< Number of marked dot. 0 - first and so on, @default -1 */

};

#endif // TMENUWIDGET_H
