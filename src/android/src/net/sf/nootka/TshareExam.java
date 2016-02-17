/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                  				         *
 *   seelook@gmail.com        						                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

package net.sf.nootka;

import org.qtproject.qt5.android.QtNative;
import java.lang.String;
import android.content.Intent;
import java.io.File;
import android.net.Uri;


public class TshareExam 
{

    /** @fn send(String title, String message, String filePath)
      * Displays chooser dialog to send/share exam file
      * @p title - is head text of that dialog
      * @p message - is message content when send by email was chosen.
    */
  public static void send(String title, String message, String filePath) {
    if (QtNative.activity() == null)
        return;
    Intent shareIntent = new Intent(Intent.ACTION_SEND);
//       shareIntent.setType("application/noo");
    shareIntent.setType("*/*");
    shareIntent.putExtra(Intent.EXTRA_SUBJECT, "Nootka");
    shareIntent.putExtra(Intent.EXTRA_TEXT, message);
  // We can be sure that file exists - Nootka checked it before
    File attachment = new File(filePath);
    Uri uri = Uri.fromFile(attachment);
    shareIntent.putExtra(Intent.EXTRA_STREAM, uri);

    QtNative.activity().startActivity(Intent.createChooser(shareIntent, title));
  }

}