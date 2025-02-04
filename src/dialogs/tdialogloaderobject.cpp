/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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
#include "main/texamexecutor.h"
#include "main/texamsummary.h"
#include "main/texamview.h"
#include "main/tnootkacertificate.h"
#include "main/tpiechartitem.h"
#include "main/tstartexamitem.h"
#include "main/ttiphandler.h"
#include "tlevelcreatoritem.h"
#include "tlevelpreviewitem.h"
#include "tlevelselector.h"
#include "tmelgenitem.h"
#include "tmelodyimportitem.h"
#include "tmelodylistview.h"
#include "tmelodywrapper.h"
#include "trtmselectoritem.h"
#include "ttunerdialogitem.h"
#include "updater/tupdateitem.h"
#if !defined(Q_OS_ANDROID)
#include "charts/tbarchartdelegate.h"
#include "charts/tchartitem.h"
#include "charts/tcharttipitem.h"
#include "charts/tlinchartdelegate.h"
#endif
#include "help/tmainhelp.h"
#include "updater/tupdatechecker.h"
#include <exam/texam.h>
#include <qtr.h>
#include <tglobals.h>
#include <tpath.h>

#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qsettings.h>
#include <QtCore/qversionnumber.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtWidgets/qdialogbuttonbox.h>

bool TdialogLoaderObject::m_firstTime = true;

TdialogLoaderObject::TdialogLoaderObject(QObject *parent)
    : QObject(parent)
{
    if (m_firstTime) {
        qmlRegisterType<TlevelCreatorItem>("Nootka.Dialogs", 1, 0, "TlevelCreatorItem");
        qmlRegisterType<TlevelPreviewItem>("Nootka.Dialogs", 1, 0, "TlevelPreviewItem");
        qmlRegisterType<TlevelSelector>("Nootka.Dialogs", 1, 0, "TlevelsSelector");
        qmlRegisterType<TmelodyListView>("Nootka.Dialogs", 1, 0, "TmelodyListView");
        qmlRegisterType<TmelodyWrapper>("Nootka.Dialogs", 1, 0, "TmelodyWrapper");
        qmlRegisterType<TtunerDialogItem>("Nootka.Dialogs", 1, 0, "TtunerDialogItem");
        qmlRegisterType<TrtmSelectorItem>("Nootka.Dialogs", 1, 0, "TrtmSelectorItem");
        qmlRegisterType<TmelGenItem>("Nootka.Dialogs", 1, 0, "TmelGenItem");
        qmlRegisterType<TmelodyImportItem>("Nootka.Dialogs", 1, 0, "TmelodyImportItem");

        qmlRegisterType<TexamExecutor>("Nootka.Exam", 1, 0, "Texecutor");
        qmlRegisterType<TexamView>("Nootka.Exam", 1, 0, "Tresults");
        qmlRegisterUncreatableType<TtipHandler>("Nootka.Exam", 1, 0, "TipHandler", QStringLiteral("You cannot create an instance of the TipHandler."));
        qmlRegisterType<TexamSummary>("Nootka.Exam", 1, 0, "TexamSummary");
        qmlRegisterType<TstartExamItem>("Nootka.Exam", 1, 0, "TstartExamItem");
        qmlRegisterType<TnootkaCertificate>("Nootka.Exam", 1, 0, "CertificateItem");
        qmlRegisterType<TpieChartItem>("Nootka.Exam", 1, 0, "TpieChartItem");

        qmlRegisterType<TupdateItem>("Nootka.Update", 1, 0, "TupdateItem");

        qRegisterMetaType<Tlevel *>("Tlevel*");
        qRegisterMetaType<Texam *>("Texam*");
        qRegisterMetaType<Tmelody *>("Tmelody*");

#if !defined(Q_OS_ANDROID)
        qmlRegisterType<TchartItem>("Nootka.Charts", 1, 0, "TchartItem");
        qmlRegisterType<TchartTipItem>("Nootka.Charts", 1, 0, "TchartTipItem");
        qmlRegisterType<TlinChartDelegate>("Nootka.Charts", 1, 0, "TlinChartDelegate");
        qmlRegisterType<TbarChartDelegate>("Nootka.Charts", 1, 0, "TbarChartDelegate");
#endif
        m_firstTime = false;
    }
}

TdialogLoaderObject::~TdialogLoaderObject()
{
}

QString TdialogLoaderObject::stdButtonText(int role)
{
    switch (role) {
    case 33554432:
        return qTR("QPlatformTheme", "Apply");
    case 4194304:
        return qTR("QPlatformTheme", "Cancel");
    case 134217728:
        return qTR("QPlatformTheme", "Restore Defaults");
    case 16777216:
        return qTR("QPlatformTheme", "Help");
    case 2097152:
        return qTR("QPlatformTheme", "Close");
    case 1024:
        return qTR("QPlatformTheme", "OK");
    default:
        return QString();
    }
}

QString TdialogLoaderObject::stdButtonIcon(int role)
{
    switch (role) {
    case 33554432:
        return QStringLiteral("check"); // Apply
    case 4194304:
        return QStringLiteral("stopExam"); // Cancel
    case 134217728:
        return QStringLiteral("restore-defaults"); // Restore defaults
    case 16777216:
        return QStringLiteral("help"); // Help
    case 2097152:
        return QStringLiteral("exit"); // Close
    case 1024:
        return QStringLiteral("check"); // OK
    default:
        return QString();
    }
}

QString TdialogLoaderObject::buttonRoleIcon(int role) const
{
    switch (static_cast<QDialogButtonBox::ButtonRole>(role)) {
    case QDialogButtonBox::ApplyRole:
        return QStringLiteral("check"); // Apply
    case QDialogButtonBox::ResetRole:
        return QStringLiteral("restore-defaults"); // Restore defaults
    case QDialogButtonBox::HelpRole:
        return QStringLiteral("help"); // Help
    case QDialogButtonBox::AcceptRole:
        return QStringLiteral("check"); // OK
    default:
        return QStringLiteral("exit");
    }
}

QColor TdialogLoaderObject::buttinRoleColor(int role) const
{
    switch (static_cast<QDialogButtonBox::ButtonRole>(role)) {
    case QDialogButtonBox::ApplyRole:
        return Qt::green; // Apply
    case QDialogButtonBox::ResetRole:
        return QColor(0, 128, 128); // Restore defaults
    case QDialogButtonBox::HelpRole:
        return QColor(250, 140, 0); // Help
    case QDialogButtonBox::AcceptRole:
        return Qt::green; // OK
    default:
        return Qt::red;
    }
}

void TdialogLoaderObject::openFile(const QString &fileName)
{
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
            else
                GLOB->warnAboutNewerVersion(fileName);
        } else if (Tlevel::couldBeLevel(hdr)) {
            if (Tlevel::isLevelVersion(hdr))
                emit openLevel(fullPath);
            else
                GLOB->warnAboutNewerVersion(fileName);
        } else
            qDebug() << "[TdialogLoaderObject] file" << fileName << "is not supported by Nootka";
    }
}

QString TdialogLoaderObject::aboutQt() const
{
    return "<h1 align=\"center\">"
        + qTR("QMessageBox", "<h3>About Qt</h3><p>This program uses Qt version %1.</p>")
              .arg(qVersion())
              .replace(QLatin1String("<p>"), QString())
              .replace(QLatin1String("</p>"), QString())
        + "</h1><br>"
        + qTR("QMessageBox",
              "<p>Qt is a C++ toolkit for cross-platform application "
              "development.</p>"
              "<p>Qt provides single-source portability across all major desktop "
              "operating systems. It is also available for embedded Linux and other "
              "embedded and mobile operating systems.</p>"
              "<p>Qt is available under three different licensing options designed "
              "to accommodate the needs of our various users.</p>"
              "<p>Qt licensed under our commercial license agreement is appropriate "
              "for development of proprietary/commercial software where you do not "
              "want to share any source code with third parties or otherwise cannot "
              "comply with the terms of the GNU LGPL version 3 or GNU LGPL version 2.1.</p>"
              "<p>Qt licensed under the GNU LGPL version 3 is appropriate for the "
              "development of Qt&nbsp;applications provided you can comply with the terms "
              "and conditions of the GNU LGPL version 3.</p>"
              "<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
              "development of Qt&nbsp;applications provided you can comply with the terms "
              "and conditions of the GNU LGPL version 2.1.</p>"
              "<p>Please see <a href=\"http://%2/\">%2</a> "
              "for an overview of Qt licensing.</p>"
              "<p>Copyright (C) %1 The Qt Company Ltd and other "
              "contributors.</p>"
              "<p>Qt and the Qt logo are trademarks of The Qt Company Ltd.</p>"
              "<p>Qt is The Qt Company Ltd product developed as an open source "
              "project. See <a href=\"http://%3/\">%3</a> for more information.</p>")
              .arg(QStringLiteral("2021"), QStringLiteral("qt.io/licensing"), QStringLiteral("qt.io"));
}

QString TdialogLoaderObject::mainHelp() const
{
    return TmainHelp::mainHelp();
}

QStringList TdialogLoaderObject::getChanges() const
{
    QFile file(Tpath::main + QLatin1String("changes"));
    QStringList chLog;
    QTextStream in;
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QStringList htmlText = in.readAll().replace(QLatin1String("  "), QLatin1String("&nbsp;&nbsp;")).split(QLatin1String("\n"));
        chLog << QLatin1String("<h1>Nootka TWO</h1>");
        for (int i = 0; i < htmlText.size(); i++) {
            if (htmlText[i].startsWith(QLatin1String("0.")) || htmlText[i].startsWith(QLatin1String("1.")) || htmlText[i].startsWith(QLatin1String("2."))) {
                htmlText[i] = QString("<font size=\"6\" color=\"%1\"><b>&nbsp;").arg(qApp->palette().highlight().color().name()) + htmlText[i]
                    + QLatin1String("</b></font>");
                chLog << htmlText[i] + QLatin1String("<br>");
            } else if (htmlText[i].contains(QLatin1String("======"))) {
                htmlText[i] = QStringLiteral("<h1>Nootka ONE</h1>");
                chLog << htmlText[i] + QLatin1String("<br>");
            } else if (htmlText[i].length() > 7) {
                if (htmlText[i].contains(QLatin1String("BUG")))
                    htmlText[i] = QStringLiteral("&nbsp;&nbsp;<u>BUG FIXES</u>");
                else if (htmlText[i].contains(QLatin1String("Under the hood")))
                    htmlText[i] = QStringLiteral("&nbsp;&nbsp;<u>Under the hood</u>");
                else if (!htmlText[i].contains(QLatin1String("&nbsp;&nbsp; - ")))
                    htmlText[i] = QLatin1String("<b>") + htmlText[i] + QLatin1String("</b>");
                chLog.last().append(htmlText[i] + QLatin1String("<br>"));
            }
        }
    }
    file.close();
    return chLog;
}

QString TdialogLoaderObject::getLicense()
{
    QFile file(Tpath::main + QLatin1String("gpl"));
    QString license;
    QTextStream in;
    if (!file.exists()) { // Debian based
        QDir d(Tpath::main);
        d.cdUp();
        file.setFileName(d.path() + QLatin1String("/doc/nootka/copyright"));
    }
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        license = in.readAll();
    }
    file.close();
    return license;
}

void TdialogLoaderObject::updateCheckInBackground()
{
    auto updater = new TupdateChecker(qApp);
    connect(updater, &TupdateChecker::updateMessage, [=](QString m) {
        if (m.isEmpty() || m.contains(QLatin1String("error")) || m.contains(QLatin1String("No need")))
            updater->deleteLater();
    });
    updater->check(true); // do not display update window when no updates found
}

/**
 * This method is invoked by user from GlobalPage,
 * so it handles @p TupdateChecker::updateSummary and @p updateMessage signals.
 */
void TdialogLoaderObject::checkForUpdates()
{
    auto updater = new TupdateChecker(qApp);
    connect(updater, &TupdateChecker::updateSummary, this, [=](const QString &v, const QString &c, TupdateRules *r) {
        // This method is called by GlobalPage and creates Popup with updates
        // so update item content just once, when TupdateItem doesn't exist yet
        bool updateItem = TupdateItem::instance() == nullptr;
        emit updateSummary(v, c, *r);
        if (TupdateItem::instance() && updateItem) {
            TupdateItem::instance()->setUpdateRules(r, v);
            TupdateItem::instance()->setChanges(c);
        }
    });
    connect(updater, &TupdateChecker::updateMessage, this, [=](QString m) {
        m_updateMessage = m;
        emit updateMessageChanged();
        if (m.isEmpty() || m.contains(QLatin1String("error")))
            updater->deleteLater();
    });
    updater->check(false); // Anyway show update window
}

bool TdialogLoaderObject::checkVersion(QObject *nootWin)
{
    // Do not show changelog page until user wants to see any 'Got It' info.
    if ((!GLOB->isSingleNote() && GLOB->gotIt(QLatin1String("noteSelected"), true)) || GLOB->gotIt(QLatin1String("soundInfo"), true)
#if defined(Q_OS_ANDROID)
        || GLOB->gotIt(QLatin1String("howToScore"), true)
#endif
    )
        return false;

    QString configVersion = GLOB->config->value(QLatin1String("version"), QString()).toString();
    auto confVerNr = QVersionNumber::fromString(configVersion);
    auto currVerNr = QVersionNumber::fromString(GLOB->version);
    if (currVerNr > confVerNr) {
        QTimer::singleShot(1500, [=] {
            if (nootWin && QString(nootWin->metaObject()->className()).contains("MainWindow_QMLTYPE")) {
                QMetaObject::invokeMethod(nootWin, "showDialog", Q_ARG(QVariant, 2));
                auto dialogLoader = qvariant_cast<QObject *>(nootWin->property("dialogLoader"));
                if (dialogLoader) {
                    auto aboutContent = qvariant_cast<QObject *>(dialogLoader->property("currentDialog"));
                    QMetaObject::invokeMethod(aboutContent, "showPage", Q_ARG(QVariant, 6));
                }
                GLOB->config->setValue(QLatin1String("version"), GLOB->version);
            }
        });
        return true;
    }

    return false;
}

bool TdialogLoaderObject::checkForSupport(QObject *nootWin)
{
    if (nootWin && QString(nootWin->metaObject()->className()).contains("MainWindow_QMLTYPE")) {
        int supportDaysPass = GLOB->config->value("General/supportDate", QDate(2012, 12, 31)).toDate().daysTo(QDate::currentDate());
        if (supportDaysPass > 7) { // display support dialog every seven days
            QTimer::singleShot(1500, [=] {
                QMetaObject::invokeMethod(nootWin, "askForSupport");
            });
            GLOB->config->setValue("General/supportDate", QDate::currentDate());
            return true;
        }
    }
    return false;
}
