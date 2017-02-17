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
#include <QtQuick/qquickpainteditem.h>


/**
 * This is static background of guitar QML component.
 * It re-paints its content only when size changes (user scales main window)
 */
class NOOTKACORE_EXPORT TguitarBg : public QQuickPaintedItem
{
  Q_OBJECT

public:
  TguitarBg(QQuickItem* parent = nullptr);

  virtual void paint(QPainter* painter);

      /**
       * Guitar fingerboard rectangle
       */
  QRect fbRect() { return m_fbRect; }

protected:
  virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private:
  QRect       m_fbRect; /**< Represents top left positions and size of a fingerboard */
  int          m_strGap; /**< Distance between strings */
  int         m_fretWidth; /**< Average width of fret */
  int         m_lastFret; /**< Position of the last fret (in whole widget coordinates) */
  int         m_fretsPos[24]; /**< @p fretsPos stores X positions of frets in global widget coordinates */

};

#endif // TGUITARBG_H
