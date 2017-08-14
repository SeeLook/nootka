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

#ifndef TSAXBG_H
#define TSAXBG_H


#include "nootkacoreglobal.h"
#include "music/tnote.h"
#include <QtQuick/qquickitem.h>


/**
 * @class TsaxBg defines fingering chart shapes of notes from a# (11) till c3 (49).
 * Every shape is represented by unique number obtained as a bit-mask of closed (1) or opened (0) saxophone flips.
 * 
 * It converts given @p Tnote into shape number.
 * QML calls @p flapNumber and by it current shape is obtained,
 * then corresponding note is found end @p noteChanged() signal is emitted
 */
class NOOTKACORE_EXPORT TsaxBg : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(Tnote note READ note WRITE setNote NOTIFY noteChanged)
  Q_PROPERTY(bool outOfScale READ outOfScale NOTIFY outOfScaleChanged)
  Q_PROPERTY(bool active READ active NOTIFY activeChanged)
  Q_PROPERTY(int flapNumber READ flapNumber WRITE setFlapNumber NOTIFY flapNumberChanged)
  Q_PROPERTY(int fingeringId READ fingeringId NOTIFY fingeringIdChanged)

public:
  TsaxBg(QQuickItem* parent = nullptr);
  ~TsaxBg();

  Tnote note() const { return m_note; }
  void setNote(const Tnote& n);

  bool outOfScale() const { return m_outOfScale; }
  void setOutOfScale(bool out);
      /**
       * @p TRUE when mouse cursor is over
       */
  bool active() { return m_active; }

  int flapNumber() const { return m_flapNumber; }

      /**
       * Every saxophone flaps when clicked sets this value to its number
       */
  void setFlapNumber(int fNr);

  quint32 fingeringId() const { return m_fingeringId; }


signals:
  void noteChanged();
  void activeChanged();
  void outOfScaleChanged();
  void flapNumberChanged();
  void fingeringIdChanged();


protected:
  void hoverEnterEvent(QHoverEvent*) override;
  void hoverLeaveEvent(QHoverEvent*) override;

private:
  Tnote         m_note;
  bool          m_outOfScale = false;
  bool          m_active = false;
  int           m_flapNumber;
  quint32       m_fingeringId = 0;
  quint32      *m_notesArray;
};

#endif // TSAXBG_H
