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

#include "ttouchproxy.h"
#include <qtr.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>


bool              TtouchProxy::m_touchEnabled = false;
TtouchProxy*      TtouchProxy::m_instance = 0;

QString TtouchProxy::touchScoreHelp() {
  return QApplication::translate("TouchHelp", "To edit a note in a score:<br>"
            "Touch area about some note and move a finger to select its pitch.<br>"
            "Then take the finger away to add/remove an accidental or a note.<br>"
            "Touch and move again to continue selecting the pitch.<br>"
            "To set the note - tap it quickly again.<br>"
            "To scroll the score with multiple staves - use two fingers.");
}


QString TtouchProxy::touchClefHelp() {
  return QApplication::translate("TouchHelp", "Touch a clef for a while to change it.");
}



QString TtouchProxy::touchGuitarHelp() {
  return QApplication::translate("TouchHelp", "Tap a fret you want to select - zoomed preview will appear.<br>"
            "You may drag it right or left to change the fret.<br>"
            "Tap desired string on the preview to set the position."
  );
}



TtouchProxy::TtouchProxy(QObject* parent) :
  QObject(parent)
{
  if (m_instance) {
    qDebug() << "TtouchProxy instance already exists!!";
    return;
  }
  m_instance = this;
}


TtouchProxy::~TtouchProxy()
{
  m_instance = 0;
}


void TtouchProxy::setTouchEnabled(bool touch) {
  if (touch != m_touchEnabled) {
    m_touchEnabled = touch;
    qApp->setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, touch);
    qApp->setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, touch);
    if (instance())
      instance()->emitStateChanged();
  }
}
