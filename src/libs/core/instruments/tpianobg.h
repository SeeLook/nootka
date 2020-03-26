/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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

  Q_PROPERTY(qreal keyWidth READ keyWidth WRITE setKeyWidth NOTIFY keyWidthChanged)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)
  Q_PROPERTY(int firstOctave READ firstOctave WRITE setFirstOctave NOTIFY firstOctaveChanged)
  Q_PROPERTY(int keysNumber READ keysNumber NOTIFY keysNumberChanged)
  Q_PROPERTY(QQuickItem* selectedKey READ selectedKey WRITE setSelectedKey NOTIFY selectedKeyChanged)
  Q_PROPERTY(QQuickItem* keyHighlight READ keyHighlight WRITE setKeyHighlight)
  Q_PROPERTY(qreal margin READ margin NOTIFY keyWidthChanged)

public:
  explicit TpianoBg(QQuickItem* parent = nullptr);
  ~TpianoBg() override;

  qreal keyWidth() const { return m_keyWidth; }
  void setKeyWidth(qreal kw);

  void setNote(const Tnote& n, quint32 noteDataValue = NO_TECHNICALS) override;

  void askQuestion(const Tnote& n, quint32 noteDataValue) override;

  void highlightAnswer(const Tnote&, quint32) override {}

  int technical() override { return NO_TECHNICALS; } // Fake - piano has no extra note data

  int firstOctave() const { return static_cast<int>(m_firstOctave); }
  void setFirstOctave(int firstO);

  bool readOnly() const { return m_readOnly; }
  void setReadOnly(bool ro);

  void paint(QPainter*) override {}

  void markSelected(const QColor & markColor) override;

  Q_INVOKABLE void applyCorrect() override;

  void showNoteName() override;

  void correct(const Tnote& n, quint32 noteData) override;

  int keysNumber() const { return m_keysNumber; }
  qreal margin() const { return m_margin; }

  Q_INVOKABLE QString octaveName(int oNr) const;

      /**
       * Calculates zoom view X coordinate from clicked @p xPos of piano widget.
       * @p zoomKeyW is the width of piano key in zoomed preview
       */
  Q_INVOKABLE int zoomViewX(qreal xPos, qreal zoomKeyW);

  QQuickItem* selectedKey() { return m_selectedKey; }
  void setSelectedKey(QQuickItem* it);

  QQuickItem* keyHighlight() { return m_keyHighlight; }
  void setKeyHighlight(QQuickItem* hi);

  Q_INVOKABLE void selectKey(QQuickItem* keyItem);

  Q_INVOKABLE void setAmbitus(const Tnote& loNote, const Tnote& hiNote);

signals:
  void keyWidthChanged();
  void keysNumberChanged();
  void firstOctaveChanged();
  void selectedKeyChanged();
  void wantKeyToSelect(int k, bool isWhite);
  void readOnlyChanged();

protected:
  void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) override;

private:
  void calculateMetrics(qreal newWidth);

private:
  int                   m_keysNumber;
  qreal                 m_keyWidth;
  qreal                 m_margin;
  char                  m_firstOctave;
  bool                  m_readOnly = false;
  QQuickItem           *m_selectedKey = nullptr;
  QQuickItem           *m_keyHighlight = nullptr; /**< Key highlighting item (Rectangle) */
  Tnote                 m_loNote, m_hiNote;
};

#endif // TPIANOBG_H
