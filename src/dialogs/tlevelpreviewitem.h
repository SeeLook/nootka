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

#ifndef TLEVELPREVIEWITEM_H
#define TLEVELPREVIEWITEM_H

#include <QtQuick/qquickitem.h>


class Tlevel;

/**
 * C++ logic for QML level preview 
 */
class TlevelPreviewItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(bool validLevel READ validLevel NOTIFY levelChanged)
  Q_PROPERTY(QString instrumentGlyph READ instrumentGlyph NOTIFY levelChanged)
  Q_PROPERTY(QString header READ header NOTIFY levelChanged)
  Q_PROPERTY(QString description READ description NOTIFY levelChanged)
  Q_PROPERTY(QString instrument READ instrument NOTIFY levelChanged)
  Q_PROPERTY(QString clef READ clef NOTIFY levelChanged)
  Q_PROPERTY(QString noteRange READ noteRange NOTIFY levelChanged)
  Q_PROPERTY(QString fretRange READ fretRange NOTIFY levelChanged)
  Q_PROPERTY(QString keyRange READ keyRange NOTIFY levelChanged)
  Q_PROPERTY(QString accidentals READ accidentals NOTIFY levelChanged)

  Q_PROPERTY(QString questions READ questions NOTIFY levelChanged)
  Q_PROPERTY(QString answers READ answers NOTIFY levelChanged)
  Q_PROPERTY(QString requireOctave READ requireOctave NOTIFY levelChanged)

public:
  TlevelPreviewItem(QQuickItem* parent = nullptr);
  ~TlevelPreviewItem() override;

  void setLevel(Tlevel* tl);

  bool validLevel() const { return m_validLevel; }
  QString instrumentGlyph() const { return m_instrGlyph; }

  QString header() const { return m_header; }
  QString description() const { return m_description; }
  QString instrument() const { return m_instrument; }
  QString clef() const { return m_clef; }
  QString noteRange() const { return m_noteRange; }
  QString fretRange() const { return m_fretRange; }
  QString keyRange() const { return m_keyRange; }
  QString accidentals() const { return m_accidentals; }

  QString questions() const { return m_questions; }
  QString answers() const { return m_answers; }
  QString requireOctave() const { return m_requireOctave; }

signals:
  void levelChanged();

private:
  bool                      m_validLevel = false;
  QString                   m_instrGlyph;
  QString                   m_header;
  QString                   m_description;
  QString                   m_instrument;
  QString                   m_clef;
  QString                   m_noteRange;
  QString                   m_fretRange;
  QString                   m_keyRange;
  QString                   m_accidentals;

  QString                   m_questions;
  QString                   m_answers;
  QString                   m_requireOctave;
};

#endif // TLEVELPREVIEWITEM_H
