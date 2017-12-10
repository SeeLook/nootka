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
#include "tlevelpreviewitem.h"
#include "tlevelselector.h"
#include "main/texamexecutor.h"
#include "qtr.h"

#include <QtWidgets/qdialogbuttonbox.h>
#include <QtCore/qdebug.h>


bool TdialogLoaderObject::m_firstTime = true;


TdialogLoaderObject::TdialogLoaderObject(QObject* parent) :
  QObject(parent)
{
  if (m_firstTime) {
    qmlRegisterType<TlevelCreatorItem>("Nootka.Dialogs", 1, 0, "TlevelCreatorItem");
    qmlRegisterType<TlevelPreviewItem>("Nootka.Dialogs", 1, 0, "TlevelPreviewItem");
    qmlRegisterType<TlevelSelector>("Nootka.Dialogs", 1, 0, "TlevelsSelector");
    qmlRegisterType<TexamExecutor>("Nootka.Exam", 1, 0, "Texecutor");
    qRegisterMetaType<Tlevel*>("Tlevel*");
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
    case 134217728: return QStringLiteral("clear-score"); // Restore defaults
    case 16777216: return QStringLiteral("help"); // Help
    case 2097152: return QStringLiteral("exit"); // Close
    case 1024: return QStringLiteral("check"); // OK
    default: return QString();
  }
}


QString TdialogLoaderObject::buttonRoleIcon(int role) {
  switch (static_cast<QDialogButtonBox::ButtonRole>(role)) {
    case QDialogButtonBox::ApplyRole: return QStringLiteral("check"); // Apply
    case QDialogButtonBox::ResetRole: return QStringLiteral("clear-score"); // Restore defaults
    case QDialogButtonBox::HelpRole: return QStringLiteral("help"); // Help
    case QDialogButtonBox::AcceptRole: return QStringLiteral("check"); // OK
    default: return QStringLiteral("exit");;
  }
}

