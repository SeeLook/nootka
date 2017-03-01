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

#ifndef TGUITARBG_H
#define TGUITARBG_H


#include "nootkacoreglobal.h"
#include <tfingerpos.h>

#include <QtQuick/qquickpainteditem.h>


/**
 * This is static background of guitar QML component.
 * It re-paints its content only when size changes (user scales main window)
 */
class NOOTKACORE_EXPORT TguitarBg : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(bool active READ active NOTIFY activeChanged)
  Q_PROPERTY(int fretWidth READ fretWidth NOTIFY fretWidthChanged)
  Q_PROPERTY(int stringsGap READ stringsGap NOTIFY stringsGapChanged)
  Q_PROPERTY(QPointF fingerPos READ fingerPos NOTIFY fingerPosChanged)


public:
  TguitarBg(QQuickItem* parent = nullptr);

      /**
       * @p TRUE when mouse cursor is over
       */
  bool active() { return m_active; }

      /**
       * Average width of fret
       */
  int fretWidth() { return m_fretWidth; }

      /**
       * Distance between strings
       */
  int stringsGap() { return m_strGap; }

  QPointF fingerPos() { return m_fingerPos; }

  void paint(QPainter* painter) override;

      /**
       * Guitar fingerboard rectangle
       */
  QRect fbRect() { return m_fbRect; }

      /**
       * Returns scene coordinates of given guitar position (between bars)
       */
  QPointF fretToPos(const TfingerPos &pos);

signals:
  void activeChanged();
  void fretWidthChanged();
  void stringsGapChanged();
  void fingerPosChanged();

protected:
  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
  void hoverEnterEvent(QHoverEvent*) override;
  void hoverLeaveEvent(QHoverEvent*) override;
  void hoverMoveEvent(QHoverEvent* event) override;

  void paintFingerAtPoint(QPoint p);

private:
  QRect        m_fbRect; /**< Represents top left positions and size of a fingerboard */
  int          m_strGap; /**< Distance between strings */
  int          m_fretWidth; /**< Average width of fret */
  int          m_lastFret; /**< Position of the last fret (in whole widget coordinates) */
  int          m_fretsPos[24]; /**< @p fretsPos stores X positions of frets in global widget coordinates */
  short        m_curStr, m_curFret; /**< Actual position of cursor over the guitar in strings/frets coordinates */

  bool         m_active;
  QPointF      m_fingerPos;

};

#endif // TGUITARBG_H
