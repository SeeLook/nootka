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

#include "tmelodyimportitem.h"
#include <music/timportscore.h>
#include <music/tmelody.h>

#include <QtCore/qdebug.h>


TmelodyImportItem::TmelodyImportItem(QQuickItem *parent) :
  QQuickItem(parent)
{}


TmelodyImportItem::~TmelodyImportItem() {
  qDebug() << "[TmelodyImportItem] deleted";
  if (IMPORT_SCORE)
    IMPORT_SCORE->deleteLater();
}


QString TmelodyImportItem::title() const {
  if (IMPORT_SCORE && IMPORT_SCORE->mainMelody())
    return IMPORT_SCORE->mainMelody()->composer() + QLatin1String(" - ") + IMPORT_SCORE->mainMelody()->title();

  return QString();
}


QList<QObject*> TmelodyImportItem::partsModel() const {
  if (IMPORT_SCORE)
    return IMPORT_SCORE->model();

  return QList<QObject*>();
}
