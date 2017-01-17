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

#ifndef DESCORE_H
#define DESCORE_H


#include <nootkacoreglobal.h>

#include "destaff.h"
#include <QtQuick/qquickitem.h>
#include <music/tclef.h>
#include <music/tmeter.h>


class TscoreScene;
class TscoreStaff;
class DeStaff;
class Tnote;


class NOOTKACORE_EXPORT DeScore : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Tclef::EclefType clef READ clef WRITE setClef NOTIFY clefChanged)
  Q_PROPERTY(Tmeter::Emeter meter READ meter WRITE setMeter NOTIFY meterChanged)


  friend class DeStaff;


public:
  explicit DeScore(QQuickItem* parent = nullptr);
  ~DeScore();

  Tclef::EclefType clef();
  void setClef(Tclef::EclefType c);

  Tmeter::Emeter meter();
  void setMeter(Tmeter::Emeter m);

  qint8 keySignature();
  void setKeySignature(qint8 k);

  void append(const Tnote& n);


  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;


signals:
  void clefChanged();
  void meterChanged();
  void keySignatureChanged();


protected:
  void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) override;
  void componentComplete() override;

  void hoverMoveEvent(QHoverEvent* event) override;

  void deleteKeySlot();

  TscoreStaff* staff(int id) { return m_staves[id]->staff(); }
  TscoreStaff* firstStaff() { return m_staves.first()->staff(); } /**< Exists always  */
  TscoreStaff* lastStaff() { return m_staves.last()->staff(); }


private:
  QList<DeStaff*>          m_staves;
  QQuickItem              *m_flickContent; /**<  Pointer to Flickable content item (plane of whole score) */
};

#endif // DESCORE_H
