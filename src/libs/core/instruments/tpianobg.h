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

#ifndef TPIANOBG_H
#define TPIANOBG_H


#include "tcommoninstrument.h"


/**
 * The C++ logic of piano component and its painted background
 */
class NOOTKACORE_EXPORT TpianoBg : public TcommonInstrument
{

  Q_OBJECT

  Q_PROPERTY(qreal keyWidth READ keyWidth WRITE setKeyWidth)
  Q_PROPERTY(QRectF keyRect READ keyRect NOTIFY keyRectChanged)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
  Q_PROPERTY(int firstOctave READ firstOctave WRITE setFirstOctave)

public:
  explicit TpianoBg(QQuickItem* parent = nullptr);
  ~TpianoBg() override;

  qreal keyWidth() const { return m_keyWidth; }
  void setKeyWidth(qreal kw);

  QRectF keyRect() const { return m_keyRect; }

  void setNote(const Tnote& n, quint32 noteDataValue = 255) override;

  void askQuestion(const Tnote& n, quint32 noteDataValue) override;

  void highlightAnswer(const Tnote&, quint32) override {}

  int technical() override { return 0; } // Fake - piano has no extra note data

  int firstOctave() const { return static_cast<int>(m_firstOctave); }
  void setFirstOctave(int firstO);

  bool readOnly() const { return m_readOnly; }
  void setReadOnly(bool ro);

  void paint(QPainter* painter) override;

  void markSelected(const QColor & markColor) override;

  void showNoteName() override;

  void correct(const Tnote& n, quint32 noteData) override;

signals:
  void keyRectChanged();
  void selectedRectChanged();

protected:
  void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) override;
  void hoverLeaveEvent(QHoverEvent*) override;
  void hoverMoveEvent(QHoverEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

private:
  void calculateMetrics(int newWidth);

private:
  int                   m_keysNumber;
  qreal                 m_keyWidth;
  QRectF                m_keyRect;
  int                   m_margin;
  Tnote                 m_activeNote;
  char                  m_firstOctave;
  bool                  m_readOnly = false;
};

#endif // TPIANOBG_H
