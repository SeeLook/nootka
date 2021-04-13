/***************************************************************************
 *   Copyright (C) 2016-2021 by Tomasz Bojczuk                             *
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

package net.sf.nootka;

import org.qtproject.qt5.android.QtNative;
import java.lang.String;
import android.content.Intent;
import java.io.File;
import android.net.Uri;
import android.util.Log;
import android.app.Activity;
import android.os.Build;
import android.os.Build.VERSION_CODES;

/**
 * Following module requires Android Support Repository from Android extras installed (through SDK manager)
 * There is appropriate information to use it during buid process in
 * src/android/build.gradle
 */
import android.support.v4.content.FileProvider;


public class TshareExam extends Activity
{

    /**
      * @fn send(String title, String message, String filePath)
      * Displays chooser dialog to send/share exam file
      * @p title - is head text of that dialog
      * @p message - is message content when send by email was chosen.
      */
  public static void send(String title, String message, String filePath) {
    if (QtNative.activity() == null)
        return;

    Intent shareIntent = new Intent(Intent.ACTION_SEND);
    shareIntent.setType("*/*");
    shareIntent.putExtra(Intent.EXTRA_SUBJECT, "Nootka");
    shareIntent.putExtra(Intent.EXTRA_TEXT, message);
  // We can be sure that file exists - Nootka checked it before
    Uri uri;
    if (Build.VERSION.SDK_INT < 24) {
        File attachment = new File(filePath);
        uri = Uri.fromFile(attachment);
    } else {
        shareIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        File attachment = new File(filePath);
        uri = FileProvider.getUriForFile(QtNative.activity(), "net.sf.nootka.provider", attachment);
    }
    shareIntent.putExtra(Intent.EXTRA_STREAM, uri);

    try {
        QtNative.activity().startActivity(Intent.createChooser(shareIntent, title));
    } catch (Exception e) {
        String msgTag = "Send_exam_file_error";
        Log.e(msgTag, e.toString());
        e.printStackTrace();
    }
  }

}
