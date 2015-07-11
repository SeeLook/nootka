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

#ifndef TTOUCHPROXT_H
#define TTOUCHPROXT_H


#include <nootkacoreglobal.h>
#include <QObject>


/**
 * @class TtouchProxy class manages touch support in Nootka.
 * It has static @p touchEnabled() method that returns state of touch events
 * It tries to update appropriate switches in Nootka classes,
 * but also emits @p touchStateChanged signal when it changes.
 * RUN-TIME CHANGING TOUCH AVAILABILITY IS NOT IMPLEMENTED YET!
 */
class NOOTKACORE_EXPORT TtouchProxy : public QObject
{
  Q_OBJECT

public:
  TtouchProxy(QObject* parent = 0);

  static bool touchEnabled() { return m_touchEnabled; }
  static void setTouchEnabled(bool touch);
  static TtouchProxy* instance() { return m_instance; }

signals:
  void touchStateChanged(bool); /** Emitting when enabling of touch events changes. */

protected:
  void emitStateChanged() { emit touchStateChanged(m_touchEnabled); }

private:
  static bool                 m_touchEnabled;
  static TtouchProxy         *m_instance;
};




#endif // TTOUCHPROXT_H

