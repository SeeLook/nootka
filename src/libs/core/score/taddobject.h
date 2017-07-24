/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#ifndef TADDOBJECT_H
#define TADDOBJECT_H


#include "nootkacoreglobal.h"
#include <QtQuick/qquickitem.h>
#include <QtCore/qelapsedtimer.h>


class TscoreObject;
class QTimer;


/**
 * This is C++ part of @p NoteAdd component which is not able to handle mouse grabbing,
 * so in occasion a few logic routines are done here
 */
class NOOTKACORE_EXPORT TaddObject : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(TscoreObject* scoreObject READ scoreObject WRITE setScoreObject)
  Q_PROPERTY(bool active READ active NOTIFY activeChanged)
  Q_PROPERTY(qreal yPos READ yPos NOTIFY yPosChanged)

public:
  explicit TaddObject(QQuickItem* parent = nullptr);
  ~TaddObject();

  TscoreObject* scoreObject() { return m_scoreObject; }
  void setScoreObject(TscoreObject* sc);

  bool active() const { return m_active; }

  qreal yPos() const { return m_yPos; }

signals:
  void activeChanged();
  void yPosChanged();
  void addNote();

protected:
  void hoverEnterEvent(QHoverEvent* event) override;
  void hoverLeaveEvent(QHoverEvent*) override;
  void hoverMoveEvent(QHoverEvent* event) override;

  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private:
  TscoreObject                        *m_scoreObject = nullptr;
  bool                                 m_active = false;
  bool                                 m_hovered = false;
  QTimer                              *m_hideTimer;
  qreal                                m_yPos;
  QElapsedTimer                        m_touchElapsed;
};

#endif // TADDOBJECT_H
