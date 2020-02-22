/***************************************************************************
 *   Copyright (C) 2015-2020 by Tomasz Bojczuk                             *
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
#include <QtAndroidExtras/qandroidfunctions.h>
#include <QtAndroidExtras/qandroidjnienvironment.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qfileinfo.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>

#include <QtCore/qdebug.h>


void Tandroid::keepScreenOn(bool on) {
  QtAndroid::runOnAndroidThread([on]{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
      QAndroidJniObject window =
          activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

      if (window.isValid()) {
        const int FLAG_KEEP_SCREEN_ON = 128;
        if (on)
          window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        else
          window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
      }
    }
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck())
      env->ExceptionClear();
  });
}


void Tandroid::disableRotation(bool disRot) {
  int orientation = disRot ? 0 : 10; // SCREEN_ORIENTATION_LANDSCAPE or SCREEN_ORIENTATION_FULL_SENSOR
  QtAndroid::runOnAndroidThread([orientation]{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid())
      activity.callMethod<void>("setRequestedOrientation" , "(I)V", orientation);
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck())
      env->ExceptionClear();
  });
}


int Tandroid::getAPIlevelNr() {
  return  QtAndroid::androidSdkVersion();
}


bool Tandroid::hasWriteAccess() {
  if (QtAndroid::androidSdkVersion() < 23)
    return true;
  else
    return QtAndroid::checkPermission(QStringLiteral("android.permission.WRITE_EXTERNAL_STORAGE")) == QtAndroid::PermissionResult::Granted;
}


QString Tandroid::getExternalPath() {
  QString extPath;
  if (getAPIlevelNr() < 19) { // look for SD card only before Kitkat, otherwise it is inaccessible
    extPath = qgetenv("SECONDARY_STORAGE");
    //  QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment",
    //                                                                         "getExternalStorageDirectory", "()Ljava/io/File;");
    //  QString extPath = mediaDir.callObjectMethod("getAbsolutePath", "()Ljava/lang/String;").toString();
    if (!extPath.isEmpty()) {
      if (!QFileInfo(extPath).isWritable()) {
        qDebug() << "[Tandroid] No write access to secondary storage!";
        extPath.clear();
      }
    }
  }

  if(QtAndroid::androidSdkVersion() >= 23) {
      const QString PermissionID("android.permission.WRITE_EXTERNAL_STORAGE");
      if (QtAndroid::checkPermission(PermissionID) != QtAndroid::PermissionResult::Granted) {
          auto perms = QtAndroid::requestPermissionsSync(QStringList() << PermissionID);
          qDebug() << PermissionID << (perms[PermissionID] == QtAndroid::PermissionResult::Granted);
      }
  }

  if (extPath.isEmpty())
    extPath = qgetenv("EXTERNAL_STORAGE"); // return primary storage path (device internal)
  if (!QFileInfo(extPath).isWritable()) {
    qDebug() << "[Tandroid] No write access to primary storage!";
    extPath.clear();
  }
  return extPath;
}


QString Tandroid::getRunArgument() {
  QString argument;
  QAndroidJniObject activity = QtAndroid::androidActivity();
  if (activity.isValid()) {
    QAndroidJniObject intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
    if (intent.isValid()) {
       QAndroidJniObject data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
       if (data.isValid()) {
         QAndroidJniObject arg = data.callObjectMethod("getPath", "()Ljava/lang/String;");
         if (arg.isValid())
           argument = arg.toString();
       }
    }
  }
  QAndroidJniEnvironment env;
  if (env->ExceptionCheck())
    env->ExceptionClear();
  return argument;
}


void Tandroid::restartNootka() {
  auto activity = QtAndroid::androidActivity();
  auto packageManager = activity.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");
  auto activityIntent = packageManager.callObjectMethod("getLaunchIntentForPackage",
                                                        "(Ljava/lang/String;)Landroid/content/Intent;",
                                                        activity.callObjectMethod("getPackageName",
                                                        "()Ljava/lang/String;").object());
  auto pendingIntent = QAndroidJniObject::callStaticObjectMethod("android/app/PendingIntent", "getActivity",
                                                                 "(Landroid/content/Context;ILandroid/content/Intent;I)Landroid/app/PendingIntent;",
                                                                 activity.object(), jint(0), activityIntent.object(),
                                                                 QAndroidJniObject::getStaticField<jint>("android/content/Intent",
                                                                                                         "FLAG_ACTIVITY_CLEAR_TOP"));
  auto alarmManager = activity.callObjectMethod("getSystemService",
                                                "(Ljava/lang/String;)Ljava/lang/Object;",
                                                QAndroidJniObject::getStaticObjectField("android/content/Context",
                                                                                        "ALARM_SERVICE",
                                                                                        "Ljava/lang/String;").object());
  alarmManager.callMethod<void>("set",
                                "(IJLandroid/app/PendingIntent;)V",
                                QAndroidJniObject::getStaticField<jint>("android/app/AlarmManager", "RTC"),
                                jlong(QDateTime::currentMSecsSinceEpoch() + 750), pendingIntent.object());
  QAndroidJniEnvironment env;
  if (env->ExceptionCheck())
    env->ExceptionClear();
}


void Tandroid::sendExam(const QString& title, const QString &message, const QString& filePath) {
  QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
  QAndroidJniObject jMessage = QAndroidJniObject::fromString(message);
  QAndroidJniObject jFile = QAndroidJniObject::fromString(filePath);
  QAndroidJniObject::callStaticMethod<void>(
        "net/sf/nootka/TshareExam", "send",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
        jTitle.object<jstring>(), jMessage.object<jstring>(), jFile.object<jstring>());
  QAndroidJniEnvironment env;
  if (env->ExceptionCheck())
    env->ExceptionClear();
}


QString Tandroid::accountName() {
//  QString model = QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build", "MODEL").toString();
//  QAndroidJniObject juser = QtAndroid::androidActivity().callObjectMethod("getUser", "()Ljava/lang/String;");
//  return model;
  return QStringLiteral("Android");
//  return QAndroidJniObject::callStaticObjectMethod<jstring>
//              ("net/sf/nootka/account",
//               "getName").toString();
}
