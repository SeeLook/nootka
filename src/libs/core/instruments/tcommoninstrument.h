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

#ifndef TCOMMONINSTRUMENT_H
#define TCOMMONINSTRUMENT_H


#include "nootkacoreglobal.h"
#include "music/tnote.h"
#include <QtQuick/qquickpainteditem.h>


/**
 * Base class for all instruments
 */
class NOOTKACORE_EXPORT TcommonInstrument : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(bool active READ active NOTIFY activeChanged)
  Q_PROPERTY(Tnote note READ note WRITE setNote NOTIFY noteChanged)
  Q_PROPERTY(bool outOfScale READ outOfScale NOTIFY outOfScaleChanged)

public:
  TcommonInstrument(QQuickItem* parent = nullptr);

      /**
       * @p TRUE when mouse cursor is over
       */
  bool active() { return m_active; }

  bool outOfScale() const { return m_outOfScale; }

  Tnote note() const { return p_note; }
  virtual void setNote(const Tnote& n) = 0;

  virtual void askQuestion(const Tnote& n) = 0;

signals:
  void activeChanged();
  void noteChanged();
  void outOfScaleChanged();

protected:
  void hoverEnterEvent(QHoverEvent*) override;
  void hoverLeaveEvent(QHoverEvent*) override;

  void setOutOfScale(bool out);

  Tnote        p_note;

private:
  bool         m_active = false;
  bool         m_outOfScale = false;
};

#endif // TCOMMONINSTRUMENT_H
