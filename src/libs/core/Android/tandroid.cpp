/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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
#include <QtCore/qdebug.h>


void Tandroid::setScreenLockDisabled() {
  QAndroidJniObject activity = QtAndroid::androidActivity();
  if (activity.isValid()) {
    QAndroidJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (window.isValid()) {
      const int FLAG_KEEP_SCREEN_ON = 128;
      const int FLAG_FULLSCREEN = 1024;
//      const int FLAG_FORCE_NOT_FULLSCREEN = 2048;
      window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON | FLAG_FULLSCREEN);
      QAndroidJniEnvironment env;
      if (env->ExceptionCheck())
        env->ExceptionClear();
    }
  }
}


QString Tandroid::getExternalPath() {
  QString extPath = qgetenv("SECONDARY_STORAGE");
  if (extPath.isEmpty())
    extPath = qgetenv("EXTERNAL_STORAGE"); // return primary storage path (device internal)
  return extPath;
//  QAndroidJniObject extDirObject =
//      QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
//  QAndroidJniObject externalPath = extDirObject.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
//  QAndroidJniEnvironment env;
//  if (env->ExceptionCheck())
//    env->ExceptionClear();
//  return externalPath.toString();
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
                                jlong(QDateTime::currentMSecsSinceEpoch() + 100), pendingIntent.object());

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
//  QAndroidJniObject::callStaticMethod<void>(
//        "net/sf/nootka/Tusers", "getUserName",
//        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
//        jTitle.object<jstring>(), jMessage.object<jstring>(), jFile.object<jstring>());

  QAndroidJniObject juser = QtAndroid::androidActivity().callObjectMethod("getUser", "()Ljava/lang/String;");
  return juser.toString();
//  return "fake";
//  return QAndroidJniObject::callStaticObjectMethod<jstring>
//              ("net/sf/nootka/account",
//               "getName").toString();
}
