/***************************************************************************
 *   Copyright (C) 2015-2021 by Tomasz Bojczuk                             *
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

#include "tandroid.h"
#include <QtCore/qdatetime.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qstandardpaths.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmessagebox.h>

#include <QtCore/qdebug.h>

using namespace QNativeInterface;

void Tandroid::keepScreenOn(bool on)
{
    QAndroidApplication::runOnAndroidMainThread([on] {
        auto activity = QJniObject(QAndroidApplication::context());
        if (activity.isValid()) {
            auto window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

            if (window.isValid()) {
                const int FLAG_KEEP_SCREEN_ON = 128;
                if (on)
                    window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                else
                    window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
            }
        }
        QJniEnvironment env;
        env.checkAndClearExceptions();
    });
}

void Tandroid::disableRotation(bool disRot)
{
    int orientation = disRot ? 0 : 10; // SCREEN_ORIENTATION_LANDSCAPE or SCREEN_ORIENTATION_FULL_SENSOR
    QAndroidApplication::runOnAndroidMainThread([orientation] {
        auto activity = QJniObject(QAndroidApplication::context());
        if (activity.isValid())
            activity.callMethod<void>("setRequestedOrientation", "(I)V", orientation);
        QJniEnvironment env;
        env.checkAndClearExceptions();
    });
}

int Tandroid::getAPIlevelNr()
{
    return QAndroidApplication::sdkVersion();
}

bool Tandroid::hasWriteAccess()
{
    // if (QAndroidApplication::sdkVersion() < 23) // TODO
    return false; // true;
    // else
    // return QtAndroid::checkPermission(QStringLiteral("android.permission.WRITE_EXTERNAL_STORAGE")) == QtAndroid::PermissionResult::Granted;
}

void Tandroid::askForWriteAcces()
{
    // if (QtAndroid::androidSdkVersion() >= 23) {
    //     const QString writeID("android.permission.WRITE_EXTERNAL_STORAGE");
    //     if (QtAndroid::checkPermission(writeID) != QtAndroid::PermissionResult::Granted) {
    //         auto perms = QtAndroid::requestPermissionsSync(QStringList() << writeID);
    //         qDebug() << writeID << (perms[writeID] == QtAndroid::PermissionResult::Granted);
    //     }
    // }
}

QString Tandroid::getExternalPath()
{
    QString extPath;
    if (getAPIlevelNr() < 19) { // look for SD card only before Kitkat, otherwise it is inaccessible
        extPath = qgetenv("SECONDARY_STORAGE");
        if (!extPath.isEmpty()) {
            if (!QFileInfo(extPath).isWritable()) {
                qDebug() << "[Tandroid] No write access to secondary storage!";
                extPath.clear();
            }
        }
    }

    askForWriteAcces();

    if (extPath.isEmpty())
        extPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    //    extPath = qgetenv("EXTERNAL_STORAGE"); // return primary storage path (device internal)
    if (!QFileInfo(extPath).isWritable()) {
        qDebug() << "[Tandroid] No write access to primary storage!";
        extPath.clear();
    }
    return extPath;
}

QString Tandroid::getRunArgument()
{
    QString argument;
    auto activity = QJniObject(QAndroidApplication::context());
    if (activity.isValid()) {
        QJniObject intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
        if (intent.isValid()) {
            QJniObject data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
            if (data.isValid()) {
                QJniObject arg = data.callObjectMethod("getPath", "()Ljava/lang/String;");
                if (arg.isValid())
                    argument = arg.toString();
            }
        }
    }
    QJniEnvironment env;
    env.checkAndClearExceptions();
    return argument;
}

void Tandroid::restartNootka()
{
    //     auto activity = QtAndroid::androidActivity();
    //     auto packageManager = activity.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");
    //     auto activityIntent = packageManager.callObjectMethod("getLaunchIntentForPackage",
    //                                                           "(Ljava/lang/String;)Landroid/content/Intent;",
    //                                                           activity.callObjectMethod("getPackageName", "()Ljava/lang/String;").object());
    //     auto pendingIntent =
    //         QAndroidJniObject::callStaticObjectMethod("android/app/PendingIntent",
    //                                                   "getActivity",
    //                                                   "(Landroid/content/Context;ILandroid/content/Intent;I)Landroid/app/PendingIntent;",
    //                                                   activity.object(),
    //                                                   jint(0),
    //                                                   activityIntent.object(),
    //                                                   QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_CLEAR_TOP"));
    //     auto alarmManager =
    //         activity.callObjectMethod("getSystemService",
    //                                   "(Ljava/lang/String;)Ljava/lang/Object;",
    //                                   QAndroidJniObject::getStaticObjectField("android/content/Context", "ALARM_SERVICE", "Ljava/lang/String;").object());
    //     alarmManager.callMethod<void>("set",
    //                                   "(IJLandroid/app/PendingIntent;)V",
    //                                   QAndroidJniObject::getStaticField<jint>("android/app/AlarmManager", "RTC"),
    //                                   jlong(QDateTime::currentMSecsSinceEpoch() + 750),
    //                                   pendingIntent.object());
    //     QAndroidJniEnvironment env;
    //     if (env->ExceptionCheck())
    //         env->ExceptionClear();
    // }

    // void Tandroid::sendExam(const QString &title, const QString &message, const QString &filePath)
    // {
    //     QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
    //     QAndroidJniObject jMessage = QAndroidJniObject::fromString(message);
    //     QAndroidJniObject jFile = QAndroidJniObject::fromString(filePath);
    //     QAndroidJniObject::callStaticMethod<void>("net/sf/nootka/TshareExam",
    //                                               "send",
    //                                               "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
    //                                               jTitle.object<jstring>(),
    //                                               jMessage.object<jstring>(),
    //                                               jFile.object<jstring>());
    //     QAndroidJniEnvironment env;
    //     if (env->ExceptionCheck())
    //         env->ExceptionClear();
}

QString Tandroid::accountName()
{
    //  QString model = QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build", "MODEL").toString();
    //  QAndroidJniObject juser = QtAndroid::androidActivity().callObjectMethod("getUser", "()Ljava/lang/String;");
    //  return model;
    return QStringLiteral("Android");
    //  return QAndroidJniObject::callStaticObjectMethod<jstring>
    //              ("net/sf/nootka/account",
    //               "getName").toString();
}
