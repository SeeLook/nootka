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
#include "tcommoninstrument.h"
#include "tfingerpos.h"


/**
 * This is static background of guitar QML component.
 * It re-paints its content only when size changes (user scales main window)
 */
class NOOTKACORE_EXPORT TguitarBg : public TcommonInstrument
{

  Q_OBJECT

  Q_PROPERTY(int fretWidth READ fretWidth NOTIFY fretWidthChanged)
  Q_PROPERTY(int stringsGap READ stringsGap NOTIFY stringsGapChanged)
  Q_PROPERTY(QPointF fingerPos READ fingerPos NOTIFY fingerPosChanged)
  Q_PROPERTY(int string READ currentString NOTIFY stringChanged)
  Q_PROPERTY(qreal xiiFret READ xiiFret NOTIFY stringsGapChanged)
  Q_PROPERTY(QRect fbRect READ fbRect NOTIFY stringsGapChanged)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)


public:
  TguitarBg(QQuickItem* parent = nullptr);

  short currentString() { return m_curStr; }
  short currentFret() { return m_curFret; }

      /**
       * Average width of fret
       */
  int fretWidth() { return m_fretWidth; }

      /**
       * Distance between strings
       */
  int stringsGap() { return m_strGap; }

  QPointF fingerPos() { return m_fingerPos; }

  void setNote(const Tnote& n) override;

  void paint(QPainter* painter) override;

  void askQuestion(const Tnote & n) override;

      /**
       * Guitar fingerboard rectangle
       */
  QRect fbRect() { return m_fbRect; }

      /**
       * Returns scene coordinates of given guitar position (between bars)
       */
  QPointF fretToPos(const TfingerPos &pos);

  qreal xiiFret() const;

  Q_INVOKABLE qreal strWidth(int str) { return m_strWidth[str]; }

  bool readOnly() const { return m_readOnly; }
  void setReadOnly(bool ro);

      /**
       * Updates fingerboard to actual settings (guitar type string/fret number, marks)
       */
  void updateGuitar();

signals:
  void fretWidthChanged();
  void stringsGapChanged();
  void fingerPosChanged();
  void stringChanged();

protected:
  void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
  void hoverMoveEvent(QHoverEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

  void paintFingerAtPoint(QPoint p);

      /**
       * Determines string width by its note pitch. Sets @p loNote & @p hiNote
       */
  void setTune();

private:
  QRect        m_fbRect; /**< Represents top left positions and size of a fingerboard */
  int          m_strGap; /**< Distance between strings */
  int          m_fretWidth; /**< Average width of fret */
  int          m_lastFret; /**< Position of the last fret (in whole widget coordinates) */
  int          m_fretsPos[24]; /**< @p fretsPos stores X positions of frets in global widget coordinates */
  short        m_curStr, m_curFret; /**< Actual position of cursor over the guitar in strings/frets coordinates */
  qreal        m_strWidth[6]; /**< Array of each string width. The width depends on fretboard height. */
  qreal        m_widthFromPitch[6]; /**< Base values from which @p m_strWidth is calculated determined from tune. */
  QColor       m_strColors[6];

  bool         m_readOnly = false;
  QPointF      m_fingerPos;
  QQuickItem  *m_fingerItems[6];
  QQuickItem  *m_stringItems[6];

};

#endif // TGUITARBG_H
