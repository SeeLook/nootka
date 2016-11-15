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

#ifndef TMENU_H
#define TMENU_H

#include <nootkacoreglobal.h>
#include <QtWidgets/qmenu.h>


class TmenuHandler;


/**
 * Due to Nootka tool bar exists inside graphics proxy widget
 * menus handled by tool buttons have no proper parent and 
 * menu position can be wrong and window decorations are not respected.
 * To workaround this @class Tmenu has static @p menuHandler().
 * This is QObject that emits @p emitShown() signal.
 * This signal is emitted from every Tmenu instance when it is displayed.
 * Then signal is handled by @class TmainView and menu position is adjusted properly.
 *
 * Constructor automatically sets menu parent to central widget of main window
 * but it has to be declared first through @p setMainWidget().
 */
class NOOTKACORE_EXPORT Tmenu : public QMenu
{

public:
  explicit Tmenu(); /** Constructor automatically sets menu parent to central widget of main window through @p setMainWidget() */

    /** Returns Nootka main window. 
    * IT HAS TO BE INITIALIZED BY @p setMainWindow() in @p MainWindow constructor.   */
  static QWidget* mainWidget() { return m_mainWidget; }
  static void setMainWidget(QWidget* mainWindgetPtr) { m_mainWidget = mainWindgetPtr; }
  
    /** This is QObject instance created with first Tmenu object.
     * It emits @p emitShown() signal when menu appears.
     * IT HAS TO BE DELETED MANUALLY by @p deleteMenuHandler(). */
  static TmenuHandler* menuHandler() { return m_menuHandler; }
  static void deleteMenuHandler();

protected:
  virtual void showEvent(QShowEvent* event);

private:
  static QWidget                  *m_statusWidget;
  static QWidget                  *m_mainWidget;
  static TmenuHandler             *m_menuHandler;
};


/**
 * Simple QObject subclass that emits @p menuShown() signal 
 * with @class Tmenu object
 * after invoke @p emitShown() method.
 */
class NOOTKACORE_EXPORT TmenuHandler : public QObject 
{

  Q_OBJECT

public:
  TmenuHandler() : QObject() {}

  void emitShown(Tmenu* m) { emit menuShown(m); }

signals:
  void menuShown(Tmenu*);

};

#endif // TMENU_H
