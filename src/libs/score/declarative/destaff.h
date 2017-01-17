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

#ifndef DESTAFF_H
#define DESTAFF_H


#include <nootkacoreglobal.h>
#include <QtQuick/qquickpainteditem.h>


class DeScore;
class TscoreScene;
class TscoreStaff;


/**
 * @class DeStaff renders score scene with single staff
 * into @p QPainter of Scene Graph.
 * It has no representation in QML,
 * it is managed by parent class @p DeScore
 */
class NOOTKACORE_EXPORT DeStaff : public QQuickPaintedItem
{

  Q_OBJECT

public:
  explicit DeStaff(DeScore* score, int staffNr, QQuickItem* parent = nullptr);
  ~DeStaff();

  TscoreStaff* staff() { return m_staff; }

  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
  void paint(QPainter* painter) override;

private:
  DeScore                 *m_score;
  TscoreScene             *m_scene;
  TscoreStaff             *m_staff;
};

#endif // DESTAFF_H
