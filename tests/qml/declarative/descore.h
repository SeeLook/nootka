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

// #include <QtQuick/QQuickItem>
#include <QtQuick/QQuickPaintedItem>


class TscoreScene;
class TscoreStaff;
class QImage;



class DeScore : public QQuickPaintedItem
{
  Q_OBJECT

  Q_PROPERTY(Eclef clef READ clef WRITE setClef NOTIFY clefChanged)

public:
  explicit DeScore(QQuickItem* parent = nullptr);
  ~DeScore();

  enum Eclef {
      Enone = 0, // clef not defined
      Etreble_G = 1, // common treble clef
      Ebass_F = 2, // bass clef
      Ealto_C = 4,
      Etreble_G_8down = 8, // treble clef with "8" digit below (guitar)
      Ebass_F_8down = 16, // bass clef with "8" digit below (bass guitar)
      Etenor_C = 32,
      EpianoStaff = 128 // exactly is not a clef
  };
  Q_ENUM(Eclef)
  Eclef clef();
  void setClef(Eclef c);

  char keySignature();
  void setKeySignature(char k);

//   QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData*) override;
  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
  void paint(QPainter* painter) override;


signals:
  void clefChanged();
  void keySignatureChanged();


protected:
  void renderScene();

  void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) override;

  void hoverMoveEvent(QHoverEvent* event) override;

private:
  TscoreScene             *m_scene;
  TscoreStaff             *m_staff;
  QImage                  *m_sceneImage;
  int                      m_staffNr;
};

#endif // DESCORE_H
