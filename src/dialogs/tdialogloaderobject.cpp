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

#include "tdialogloaderobject.h"
#include "tlevelcreatoritem.h"
#include "tlevelpreviewitem.h"
#include "tlevelselector.h"
#include "tmelodylistview.h"
#include "main/texamexecutor.h"
#include "main/texamview.h"
#include "main/ttiphandler.h"
#include "main/texamsummary.h"
#include "main/tstartexamitem.h"
#include <qtr.h>
#include <exam/texam.h>

#include <QtCore/qdir.h>
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
    qmlRegisterType<TmelodyListView>("Nootka.Dialogs", 1, 0, "TmelodyListView");

    qmlRegisterType<TexamExecutor>("Nootka.Exam", 1, 0, "Texecutor");
    qmlRegisterType<TexamView>("Nootka.Exam", 1, 0, "Tresults");
    qmlRegisterUncreatableType<TtipHandler>("Nootka.Exam", 1, 0, "TipHandler", QStringLiteral("You cannot create an instance of the TipHandler."));
    qmlRegisterType<TexamSummary>("Nootka.Exam", 1, 0, "TexamSummary");
    qmlRegisterType<TstartExamItem>("Nootka.Exam", 1, 0, "TstartExamItem");

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
    case 134217728: return QStringLiteral("restore-defaults"); // Restore defaults
    case 16777216: return QStringLiteral("help"); // Help
    case 2097152: return QStringLiteral("exit"); // Close
    case 1024: return QStringLiteral("check"); // OK
    default: return QString();
  }
}


QString TdialogLoaderObject::buttonRoleIcon(int role) {
  switch (static_cast<QDialogButtonBox::ButtonRole>(role)) {
    case QDialogButtonBox::ApplyRole: return QStringLiteral("check"); // Apply
    case QDialogButtonBox::ResetRole: return QStringLiteral("restore-defaults"); // Restore defaults
    case QDialogButtonBox::HelpRole: return QStringLiteral("help"); // Help
    case QDialogButtonBox::AcceptRole: return QStringLiteral("check"); // OK
    default: return QStringLiteral("exit");;
  }
}


void TdialogLoaderObject::openFile(const QString& fileName) {
  QFile file(fileName);
  quint32 hdr = 0;
  if (file.open(QIODevice::ReadOnly)) {
    QDataStream in(&file);
    in >> hdr; // check what file type
    auto fullPath = QDir(file.fileName()).absolutePath();
    file.close();
    if (Texam::couldBeExam(hdr)) {
        if (Texam::isExamVersion(hdr))
          emit continueExam(fullPath);
    } else if (Tlevel::couldBeLevel(hdr)) {
        if (Tlevel::isLevelVersion(hdr))
          emit openLevel(fullPath);
    } else
        qDebug() << "[TdialogLoaderObject] file" << fileName << "is not supported by Nootka";
  }
}
