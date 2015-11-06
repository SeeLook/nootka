/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#ifndef TMATERIALMENU_H
#define TMATERIALMENU_H


#include <QtCore/qpointer.h>
#include <QtGui/qicon.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qpushbutton.h>


class QScrollArea;
class QAction;
class QVBoxLayout;
class TlabelWidget;
class TtouchMenu;


/**
 * Implementation of Android-like menu with possibility of scrolling context.
 * It converts @class QAction given in @p addAction() method
 * into a QPushButton and displays them in @class QScrollArea wrapped by @class QScroller.
 * @p exec method calls @class TtouchMenu instance positioned on left-hand side.
 */
class TmaterialMenu : public QWidget
{
  Q_OBJECT

public:
  TmaterialMenu(QWidget* parent = 0);

  void addAction(QAction* a);

  QAction* exec();

protected:
  virtual void paintEvent(QPaintEvent* event);

private:
  QScrollArea           *m_scrollArea;
  QVBoxLayout           *m_lay;
  TlabelWidget          *m_nootkaLabel;
  QTimer                *m_hideScrollTimer;
  QAction               *m_selectedAction;
  bool                   m_isMoving;
  QPointer<TtouchMenu>   m_menu;
};


/**
 * Reimplemented @class QPushButton with @p paintEvent() nicely displaying icon and text
 */
class TmenuButton : public QPushButton {

  Q_OBJECT

public:
  explicit TmenuButton(QAction* action, QWidget* parent = 0);

  virtual QSize sizeHint() const { return m_sizeHint; }

protected:
  virtual void paintEvent(QPaintEvent* e);

private:
  QSize         m_sizeHint;

};


/**
 * Just paints horizontal line with given width
 */
class TlineSpacer : public QWidget
{

public:
  explicit TlineSpacer(int lineWidth = 1, QWidget* parent = 0);

protected:
  virtual void paintEvent(QPaintEvent* e);

private:
  int           m_lineWidth;
};

#endif // TMATERIALMENU_H
