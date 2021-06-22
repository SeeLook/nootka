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

#ifndef TMELODYIMPORTITEM_H
#define TMELODYIMPORTITEM_H


#include <QtQuick/qquickitem.h>


/**
 * @todo write docs
 */
class TmelodyImportItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString title READ title NOTIFY melodyChanged)
  Q_PROPERTY(QList<QObject*> partsModel READ partsModel NOTIFY melodyChanged)

public:
  explicit TmelodyImportItem(QQuickItem* parent = nullptr);
  ~TmelodyImportItem();

  QString title() const;
  QList<QObject*> partsModel() const;

  Q_INVOKABLE void emitImport();

signals:
  void melodyChanged();

};


#endif // TMELODYIMPORTITEM_H
