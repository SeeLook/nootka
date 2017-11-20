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

  Q_PROPERTY(QString levelHtml READ levelHtml NOTIFY levelHtmlChanged)
  Q_PROPERTY(QString instrumentGlyph READ instrumentGlyph NOTIFY instrumentGlyphChanged)

public:
  TlevelPreviewItem(QQuickItem* parent = nullptr);
  ~TlevelPreviewItem() override;

  void setLevel(Tlevel* tl);

  QString levelHtml() const { return m_levelText; }
  QString instrumentGlyph() const { return m_instrGlyph; }

signals:
  void levelHtmlChanged();
  void instrumentGlyphChanged();

private:
  QString                   m_levelText, m_instrGlyph;
};

#endif // TLEVELPREVIEWITEM_H
