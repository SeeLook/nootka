/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#ifndef TMELODYPREVIEW_H
#define TMELODYPREVIEW_H


#include <nootkacoreglobal.h>
#include <QtQuick/qquickitem.h>


class Tmelody;
class TscoreObject;


/**
 * C++ part of QML item with entire melody preview.
 * It is @p TpopupDialog with @p TscoreObject inside.
 * Also displays title, composer and melody ID
 */
class NOOTKACORE_EXPORT TmelodyPreview : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString title READ title NOTIFY melodyChanged)
  Q_PROPERTY(QString composer READ composer NOTIFY melodyChanged)
  Q_PROPERTY(QVariant melody READ melody WRITE setMelody NOTIFY melodyChanged)
  Q_PROPERTY(TscoreObject* score READ score WRITE setScore)

public:
  TmelodyPreview(QQuickItem* parent = nullptr);

  QString title() const;
  QString composer() const;

  QVariant melody();
  void setMelody(QVariant v);

  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* sc) { m_score = sc; }

signals:
  void melodyChanged();

private:
  QString              m_nr;
  Tmelody             *m_melody = nullptr;
  TscoreObject        *m_score = nullptr;
};


#endif // TMELODYPREVIEW_H
