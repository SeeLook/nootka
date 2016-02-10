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

#ifndef TTOUCHMESSAGE_H
#define TTOUCHMESSAGE_H


#include <graphics/tgraphicstexttip.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qwidget.h>


#define tMessage TtouchMessage::instance() // shortcut for TtouchMessage::instance();

class TmovedAnim;


/**
 * This is temporary message box derived from @class TgraphicsTextTip
 * It takes message text (HTML formatted) through @p setMessage() method
 * where time of preview may be set to determine how long the message is displayed.
 * Otherwise it is not hidden until click on it.
 * By default colors are inverted against current palette (white text on black background).
 * Message tip is displayed at the bottom of main Nootka window and has the window width.
 * There is @p mainWindowOnTop() method to check is main Nootka window on the top.
 *
 * There is static @p instance() method or macro @p tMessage to access it.
 */
class TtouchMessage : public TgraphicsTextTip
{
  Q_OBJECT

public:
  TtouchMessage();

  virtual ~TtouchMessage();

      /** Displays message (HTML formatted) and hides it when @p previewTime is set */
  void setMessage(const QString& message, int previewTime = 0);

      /** There is only one instance of @class TtouchMessage handled by @class MainWindow.
      * It is created during constructor and available through @p instance() method
      * or through a macro tMessage. */
  static TtouchMessage* instance() { return m_instance; }

      /** Returns @p TRUE if there is no any dialog above main Nootka window */
  static bool mainWindowOnTop() { return qApp->activeWindow()->objectName() == QLatin1String("MainNootkaWindow"); }

protected:
  void showAnim();
  void hideAnim();

      /** All mouse events are overridden to properly handle click */
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  static TtouchMessage    *m_instance;

  TmovedAnim              *m_anim;
  QTimer                  *m_timer;
};

#endif // TTOUCHMESSAGE_H
