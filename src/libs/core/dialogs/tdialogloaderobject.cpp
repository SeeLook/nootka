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

#include "tdialogloaderobject.h"
#include "tlevelcreatoritem.h"
#include "exam/tlevelpreviewitem.h"
#include "qtr.h"


TdialogLoaderObject::TdialogLoaderObject(QObject* parent) :
  QObject(parent)
{
  if (m_firstTime) {
    qmlRegisterType<TlevelCreatorItem>("Nootka.exam", 1, 0, "TlevelCreatorItem");
    qmlRegisterType<TlevelPreviewItem>("Nootka.exam", 1, 0, "TlevelPreviewItem");
    m_firstTime = false;
  }
}


TdialogLoaderObject::~TdialogLoaderObject() {}


QString TdialogLoaderObject::stdButtonText(int role) {
  switch (role) {
    case 33554432: return qTR("QPlatformTheme", "Apply");
    case 4194304: return qTR("QPlatformTheme", "Cancel");
    case 134217728: return qTR("QPlatformTheme", "Restore Defaults");
    case 16777216: return qTR("QPlatformTheme", "Help");
    case 2097152: return qTR("QPlatformTheme", "Close");
    case 1024: return qTR("QPlatformTheme", "OK");
    default: return QString();
  }
}


QString TdialogLoaderObject::stdButtonIcon(int role) {
  switch (role) {
    case 33554432: return QStringLiteral("check"); // Apply
    case 4194304: return QStringLiteral("record"); // Cancel
    case 134217728: return QStringLiteral("fingerpoint"); // Restore defaults
    case 16777216: return QStringLiteral("help"); // Help
    case 2097152: return QStringLiteral("exit"); // Close
    case 1024: return QStringLiteral("check"); // OK
    default: return QString();
  }
}
