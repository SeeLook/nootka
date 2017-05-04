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

#ifndef TPIANOBG_H
#define TPIANOBG_H


#include <QtQuick/qquickpainteditem.h>
#include "music/tnote.h"


/**
 * 
 */
class TpianoBg : public QQuickPaintedItem
{

  Q_OBJECT

  Q_PROPERTY(qreal keyWidth READ keyWidth WRITE setKeyWidth)
  Q_PROPERTY(bool active READ active NOTIFY activeChanged)
  Q_PROPERTY(QRectF keyRect READ keyRect NOTIFY keyRectChanged)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
  Q_PROPERTY(Tnote note READ note WRITE setNote NOTIFY noteChanged)
  Q_PROPERTY(int firstOctave READ firstOctave WRITE setFirstOctave)

public:
  explicit TpianoBg(QQuickItem* parent = nullptr);
  ~TpianoBg() override;

  qreal keyWidth() const { return m_keyWidth; }
  void setKeyWidth(qreal kw);

  QRectF keyRect() const { return m_keyRect; }

  Tnote note() const { return m_note; }
  void setNote(const Tnote& n);

  int firstOctave() const { return static_cast<int>(m_firstOctave); }
  void setFirstOctave(int firstO);

      /**
       * @p TRUE when mouse cursor is over
       */
  bool active() const { return m_active; }

  bool readOnly() const { return m_readOnly; }
  void setReadOnly(bool ro);

  void paint(QPainter* painter) override;

signals:
  void activeChanged();
  void keyRectChanged();
  void noteChanged();

protected:
  void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) override;
  void hoverEnterEvent(QHoverEvent*) override;
  void hoverLeaveEvent(QHoverEvent*) override;
  void hoverMoveEvent(QHoverEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  void calculateMetrics(int newWidth);

private:
  int                   m_keysNumber;
  qreal                 m_keyWidth;
  bool                  m_active = false;
  QRectF                m_keyRect;
  int                   m_margin;
  Tnote                 m_note, m_activeNote;
  char                  m_firstOctave;
  bool                  m_readOnly = false;
};

#endif // TPIANOBG_H
