/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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
  virtual void setNote(const Tnote& n, quint32 noteDataValue = 255) = 0;

      /**
       * @p noteData is extra information about note needed for some instruments (guitars, bandoneon).
       * In case of guitar it is more important than note itself 
       */
  virtual void askQuestion(const Tnote& n, quint32 noteData) = 0;

      /**
       * Returns additional note data like position on the guitar or left/right hand on bandoneon
       */
  virtual int technical() = 0;

      /**
       * Sets color of selected item (finger, button, key - depends on instrument type)
       * to @p markColor if different than transparent, or to global color of selection
       */
  virtual void markSelected(const QColor& markColor) = 0;

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
