/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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


class QScrollArea;
class QAction;
class QVBoxLayout;
class TlabelWidget;
class TtouchMenu;
class TnootkaLabel;


/**
 * Implementation of Android-like menu with possibility of scrolling context.
 * It converts @class QAction given in @p addAction() method
 * into a QLabel-s and displays them in @class QScrollArea wrapped by @class QScroller.
 * @p exec method calls @class TtouchMenu instance positioned on left-hand side.
 */
class TmaterialMenu : public QWidget
{
  Q_OBJECT

public:
  TmaterialMenu(QWidget* parent = 0);

  void addAction(QAction* a);

      /** Action invoked after clicking of header widget. */
  void setAboutAction(QAction* a);

  QAction* exec();

protected:
  virtual void paintEvent(QPaintEvent* event);
  void menuItemClicked(); /**< Slot invoked when menu item (button or header widget) is clicked */
  virtual bool eventFilter(QObject* ob, QEvent* e);

private:
  QScrollArea           *m_scrollArea;
  QVBoxLayout           *m_lay;
  TlabelWidget          *m_nootkaLabel;
  QTimer                *m_hideScrollTimer;
  QAction               *m_selectedAction, *m_aboutAction;
  bool                   m_isMoving;
  QPointer<TtouchMenu>   m_menu;
  QPoint                 m_startPos;
};

//=================================================================================
//                            class TlabelWidget
//=================================================================================
/**
 * @class TnootkaLabel wrapped by widget with the same background color.
 * Color is randomized during construction time.
 */
class TlabelWidget : public QWidget {

  Q_OBJECT

public:
  explicit TlabelWidget(QWidget* parent = 0);

  QColor color() const { return m_color; }

signals:
  void clicked();

protected:
  virtual void paintEvent(QPaintEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);

private:
  QColor                  m_color;
};


//=================================================================================
//                                 class TmenuItem
//=================================================================================

class QLabel;
class QRadioButton;

/**
 * Item represents single entry taken form QAction in menu.
 * This is label with icon (or space if not set)
 * + label with text (elided if necessary)
 * + radio widget if parent action is checkeble.
 * @class TmenuItem emits @p clicked signal.
 * It handles mouse (actually touch) moves to ignore click (tap) when finger got moved.
 */
class TmenuItem : public QWidget {

  Q_OBJECT

public:
  explicit TmenuItem(QAction* action, QWidget* parent = 0);

  virtual QSize sizeHint() const { return m_sizeHint; }

signals:
  void clicked();

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);

private:
  QSize            m_sizeHint;
  QLabel          *m_iconLabel;
  QLabel          *m_textLabel;
  QRadioButton    *m_radio;
  bool             m_pressed;
  QPoint           m_startPos;
};


#endif // TMATERIALMENU_H
