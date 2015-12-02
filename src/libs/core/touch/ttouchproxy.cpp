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

#include "ttouchproxy.h"
#include <QApplication>
#include <QDebug>


bool              TtouchProxy::m_touchEnabled = false;
TtouchProxy*      TtouchProxy::m_instance = 0;

QString TtouchProxy::touchScoreHelp() {
  return tr("To edit a note in a score:<br>"
            "Touch area about some note and move a finger - note cursor will appear to set a pitch of this note.<br>"
            "Then take the finger away - additional buttons will appear for a while.<br>"
            "You may add/remove an accidental or a note.<br>"
            "Touch and move again to continue setting the pitch.<br>"
            "To select the note - tap it quickly again.<br>"
            "Touch a clef for a while to change it.<br>"
            "To scroll the score with multiple staves - use two fingers");
}


QString TtouchProxy::touchGuitarHelp() {
  return tr("Tap a fret you want to select - zoomed preview will appear.<br>"
            "You may drag it right or left to change the fret.<br>"
            "Tap zoomed preview on desired string to set the position."
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
